#include <SDL.h>
#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>

#include "tiny_obj_loader.h"
#include "scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#ifndef ROOT_DIR
#define ROOT_DIR "."
#endif

using namespace ge::gl;

const char* texVSSrc = R".(
#version 420

out vec2 vCoord;

void main(){
    if(gl_VertexID==0)gl_Position = vec4(0,0,0,1);
    if(gl_VertexID==1)gl_Position = vec4(1,0,0,1);
    if(gl_VertexID==2)gl_Position = vec4(0,1,0,1);
    if(gl_VertexID==3)gl_Position = vec4(1,1,0,1);

    vCoord = gl_Position.xy;
}
).";

const char* texFSSrc = R".(
#version 420

in vec2 vCoord;
layout(binding=0)uniform sampler2D tex;

out vec4 fColor;

void main(){
    fColor = texture(tex,vCoord);
}
).";

const char *vsSrc = R".(
#version 420

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 FragPos;
out vec3 vNormal;
out vec2 vTexcoord;

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    vNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = proj * view * vec4(FragPos, 1.0);
    vTexcoord = texcoord;
}
).";

const char* csSrc = R".(
#version 420

layout(vertices=1)out;

in vec4 vColor[];
patch out mat4 K;

void main(){
    gl_TessLevelOuter[0]=1;
    gl_TessLevelOuter[1]=64;
    gl_TessLevelOuter[2]=1;
    gl_TessLevelOuter[3]=1;
    gl_TessLevelInner[0]=1;
    gl_TessLevelInner[1]=1;

    vec4 TT[3];
    TT[0]=gl_in[0].gl_Position;
    TT[1]=gl_in[1].gl_Position;
    TT[2]=gl_in[2].gl_Position;
    float t01=length((TT[0]-TT[1]).xyz);
    float t02=length((TT[0]-TT[2]).xyz);
    float t12=length((TT[1]-TT[2]).xyz);
    float s=t01+t02+t12;
    float r=sqrt((s/2-t01)*(s/2-t02)*(s/2-t12)*s/2)*2/s;
    t01/=s;
    t02/=s;
    t12/=s;
    vec3 C=TT[0].xyz*t12+TT[1].xyz*t02+TT[2].xyz*t01;
    vec3 x=normalize(TT[0].xyz-C);
    vec3 y=normalize(TT[1].xyz-C);
    vec3 z=normalize(cross(x,y));
    y=normalize(cross(z,x));
    K=mat4(vec4(x,0)*r,vec4(y,0)*r,vec4(z,0)*r,vec4(C,1));
}
).";

const char* esSrc = R".(
#version 420

layout(isolines)in;

out vec4 eColor;
out vec2 eCoord;

#define MY_PI 3.14159265359

uniform mat4 proj = mat4(1.f);
uniform mat4 view = mat4(1.f);
uniform mat4 model = mat4(1.f);

patch in mat4 K;
void main(){
    float Angle=gl_TessCoord.x*MY_PI*2;
    vec4 PP=vec4(cos(Angle),sin(Angle),0,1);
    gl_Position=model*K*PP;
    eCoord = gl_Position.yz;
    gl_Position = proj*view*gl_Position;
    eColor = vec4(1,0,0,1);
}
).";

const char *fsSrc = R".(
#version 420
in vec3 FragPos;
in vec3 vNormal;
in vec2 vTexcoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

out vec4 fColor;

layout(binding=0) uniform sampler2D diffuseTexture;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 lightColor = vec3(0.8, 0.9, 0.9);
    vec3 objectColor = texture(diffuseTexture, vTexcoord).rgb;

    // Ambient lighting
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, vNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // Calculate shadow
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
    vec4 textureColor = texture(diffuseTexture, vTexcoord);
    fColor = textureColor * vec4(lighting, 1.0);
}
).";

GLuint createShader(GLenum type,std::string const&src){
  char const*srcs[]={
    src.c_str(),
  };
  GLuint shader = glCreateShader(type);
  glShaderSource(shader,1,srcs,nullptr);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
  if(status != GL_TRUE){
    char buffer[1000];
    glGetShaderInfoLog(shader,1000,nullptr,buffer);
    std::cerr << "ERROR: " << buffer << std::endl;
  }

  return shader;
}

GLuint createProgram(std::vector<GLuint>const&shaders){
    GLuint prg = glCreateProgram();
    for(auto const&s:shaders)
        glAttachShader(prg,s);
    glLinkProgram(prg);

    GLint status;
    glGetProgramiv(prg,GL_LINK_STATUS,&status);
    if(status != GL_TRUE){
        char buffer[1000];
        glGetProgramInfoLog(prg,1000,nullptr,buffer);
        std::cerr << "ERROR: program linking failed: " << buffer << std::endl;
    }

    return prg;
}

void bind_element_buffers(std::vector<float> vertices, std::vector<unsigned int> indices) {
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(2);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void render_scene_walls() {
    bind_element_buffers(floorVertices, floorIndices);
    bind_element_buffers(frontWallVertices, frontWallIndices);
    bind_element_buffers(leftWallVertices, leftWallIndices);
    bind_element_buffers(rightWallVertices, rightWallIndices);
    bind_element_buffers(backWallVertices, backWallIndices);
    bind_element_buffers(ceilingVertices, ceilingIndices);
    bind_element_buffers(pedestalVertices, pedestalIndices);
}

void loadOBJ(const std::string &path, const std::string &mtlBaseDir, std::vector<float> &vertices, std::vector<float> &normals, std::vector<float> &texcoords, std::vector<unsigned int> &indices, std::unordered_map<std::string, GLuint> &textures) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), mtlBaseDir.c_str())) {
        throw std::runtime_error(warn + err);
    }

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (!attrib.normals.empty()) {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            } else {
                normals.insert(normals.end(), { 0.0f, 0.0f, 0.0f });
            }

            if (!attrib.texcoords.empty()) {
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            } else {
                texcoords.insert(texcoords.end(), { 0.0f, 0.0f });
            }

            indices.push_back(indices.size());
        }
    }

    for (const auto& material : materials) {
        if (!material.diffuse_texname.empty()) {
            std::string texturePath = mtlBaseDir + "/" + material.diffuse_texname;
            int width, height, channels;
            unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);

            // std::cerr << "Loaded texture: " << texturePath << " " << width << "x" << height << "x" << channels << std::endl;

            if (data) {
                GLuint textureID;
                glCreateTextures(GL_TEXTURE_2D,1,&textureID);
                glTextureStorage2D(textureID,1,GL_RGB8,width,height);
                glPixelStorei(GL_UNPACK_ROW_LENGTH,width);
                glPixelStorei(GL_UNPACK_ALIGNMENT,1);
                glTextureSubImage2DEXT(textureID,GL_TEXTURE_2D,0,0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,data);
                glTextureParameteri(textureID,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTextureParameteri(textureID,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

                textures[material.name] = textureID;
            } else {
                std::cerr << "Failed to load texture: " << texturePath << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    std::cerr << "Hello Piano" << std::endl;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int width, height;
    SDL_Rect displayBounds;
    if (SDL_GetDisplayBounds(0, &displayBounds) != 0) {
        std::cerr << "SDL_GetDisplayBounds Error: " << SDL_GetError() << std::endl;
    } else {
        width = displayBounds.w;
        height = displayBounds.h;
    }

    auto window = SDL_CreateWindow("Piano", 0, 0, width-10, height-10, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    auto context = SDL_GL_CreateContext(window);

    bool running = true;

    ge::gl::init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Load the OBJ file
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<uint32_t> indices;
    std::unordered_map<std::string, GLuint> textures;

    try {
        loadOBJ(ROOT_DIR "/models/Simple_Piano/Joined_piano.obj", ROOT_DIR "/models/Simple_Piano", vertices, normals, texcoords, indices, textures);
        // loadOBJ(ROOT_DIR "/models/Simple_Piano/Piano.obj", ROOT_DIR "/models/Simple_Piano", vertices, normals, texcoords, indices, textures);
    } catch (const std::exception &e) {
        std::cerr << "Error loading OBJ: " << e.what() << std::endl;
        return -1;
    }
    //

    // shader program
    GLuint vs = createShader(GL_VERTEX_SHADER, vsSrc);
    // GLuint cs = createShader(GL_TESS_CONTROL_SHADER, csSrc);
    // GLuint es = createShader(GL_TESS_EVALUATION_SHADER, esSrc);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prg = createProgram({vs, fs});//cs, es,

    // texture program
    // GLuint texVS = createShader(GL_VERTEX_SHADER  ,texVSSrc);
    // GLuint texFS = createShader(GL_FRAGMENT_SHADER,texFSSrc);
    // GLuint texPRG = createProgram({texVS,texFS});

    glClearColor(0.2,0.2,0.2,1);

    GLuint iTimeLocation  = glGetUniformLocation(prg,"iTime");
    GLuint aspectLocation = glGetUniformLocation(prg,"aspect");
    GLuint modelLocation  = glGetUniformLocation(prg,"model");
    GLuint viewLocation   = glGetUniformLocation(prg,"view");
    GLuint projLocation   = glGetUniformLocation(prg,"proj");

    GLuint lightPosLocation = glGetUniformLocation(prg, "lightPos");
    GLuint viewPosLocation = glGetUniformLocation(prg, "viewPos");
    GLuint lightSpaceMatrixLocation = glGetUniformLocation(prg, "lightSpaceMatrix");
    GLuint shadowMapLocation = glGetUniformLocation(prg, "shadowMap");

    glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
    glm::vec3 viewPos(0.0f, 0.0f, 0.0f);
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    float cameraDistance = 40.f;
    float cameraXAngle = 0.31f;
    float cameraYAngle = 3.71f;

    float iTime = 0.f;

    // Model binding
    GLuint vao, vbo, nbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glVertexArrayElementBuffer(vao, ebo);
    //

    bool add_texture = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if(event.type == SDL_WINDOWEVENT){
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    width  = event.window.data1;
                    height = event.window.data2;
                    glViewport(0, 0, width, height);
                }
            }
            // controls
            if(event.type == SDL_MOUSEMOTION){
                if(event.motion.state & SDL_BUTTON_RMASK){
                    cameraYAngle += event.motion.xrel * 0.01f;
                    cameraXAngle += event.motion.yrel * 0.01f;
                }
                if(event.motion.state & SDL_BUTTON_MMASK){
                    cameraDistance += event.motion.yrel * 0.1f;
                }
                if(event.motion.state & SDL_BUTTON_LMASK){
                    lightPos.x -= event.motion.xrel * 0.1f;
                    lightPos.y -= event.motion.yrel * 0.1f;
                    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
                    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
                    lightSpaceMatrix = lightProjection * lightView;
                }
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                if(event.key.keysym.sym == SDLK_F1)
                    add_texture = !add_texture;
            }
        }

        // render
        glm::mat4 model = glm::rotate(glm::mat4(1.f),iTime*0.01f,glm::vec3(0.f,1.f,0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), float(width) / height, 0.1f, 100.0f);

        auto Ry = glm::rotate(glm::mat4(1.f),cameraYAngle,glm::vec3(0.f,1.f,0.f)); 
        auto Rx = glm::rotate(glm::mat4(1.f),cameraXAngle,glm::vec3(1.f,0.f,0.f)); 
        auto T  = glm::translate(glm::mat4(1.f),glm::vec3(0.f,0.f,-cameraDistance));
        glm::mat4 view = T*Rx*Ry;
        //

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prg);

        glProgramUniform1f(prg,iTimeLocation, iTime);
        glProgramUniform1f(prg,aspectLocation, (float)width/height);

        glProgramUniformMatrix4fv(prg, viewLocation, 1, GL_FALSE, (float*)&view);
        glProgramUniformMatrix4fv(prg, projLocation, 1, GL_FALSE, (float*)&proj);
        glProgramUniformMatrix4fv(prg, modelLocation, 1, GL_FALSE, (float*)&model);

        // renders walls, floor and ceiling from scene.h
        render_scene_walls();

        // render piano model with textures
        glBindVertexArray(vao);
        if(add_texture) {
            auto texID = 0;
            for (const auto& texture : textures) {
                glBindTextureUnit(texID, texture.second);
                texID++;
            }
        } if(!add_texture) {
            glBindTextureUnit(0, 0);
        }
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //

        // Light and shadow
        glUniform3fv(lightPosLocation, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLocation, 1, glm::value_ptr(viewPos));
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform1i(shadowMapLocation, 0);
        //

        // glUseProgram(texPRG);
       
        if(auto status = glGetError();status != GL_FALSE)
            std::cerr << status << std::endl;

        iTime += 0.1f;
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}