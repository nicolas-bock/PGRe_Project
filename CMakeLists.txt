cmake_minimum_required(VERSION 3.15)

project(Piano_Project VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

SET(CMAKE_CXX_STANDARD 17)

option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(libs/SDL-release-2.30.8)
# add_subdirectory(libs/assimp-5.4.3)
add_subdirectory(libs/geGL)
add_subdirectory(libs/glm-0.9.9.8)
# add_subdirectory(libs/tinyobjloader-release)
# add_subdirectory(libs/glfw-3.4/glfw-3.4)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} 
  src/tiny_obj_loader.cpp
  src/tiny_obj_loader.h
  # libs/glew-2.1.0/include/GL/glew.h
  # libs/glad/include/glad/glad.h
  # libs/glad/src/glad.c
  # src/tiny_gltf.cpp
  # src/tiny_gltf.h
  src/scene.h
  # src/config.h
  # src/config.cpp
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  OpenGL::GL
  SDL2::SDL2
  SDL2::SDL2main
  geGL::geGL
  # assimp::assimp
  # GLEW::GLEW
  glm
  # glfw
  )


target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC libs)

add_custom_target(run ./${PROJECT_NAME})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

