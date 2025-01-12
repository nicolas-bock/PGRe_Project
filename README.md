# PGRe_Project
## Piano 3D Renderer

This project is a 3D rendering of a grand piano implemented in C++ using **OpenGL**. The renderer loads a 3D model of a piano, applies textures, and displays it in a custom environment with proper lighting and material properties. The project showcases fundamental computer graphics concepts such as texture mapping, lighting, and shader programming.

---

## Features

- **3D Model Rendering**: Renders a grand piano model from an `.obj` file.
- **Texture Mapping**: Applies textures to the piano using a texture atlas.
- **Custom Shaders**: Implements vertex and fragment shaders for proper lighting and texture sampling.
- **Lighting Effects**: Realistic lighting using the Phong shading model.
- **Camera Movement**: Allows user-controlled navigation around the model.
- **Environment Mapping**: Simulates the surrounding space with basic background effects.

---

## Requirements

- **C++ Compiler**: A modern compiler that supports C++11 or later.
- **OpenGL**: Version 3.3 or later.
- **CMake** & **Make**: For managing the build process.
- **Libraries**:
  - **SDL**: For creating the window, managing OpenGL context and handling input events.
  - **tinyobjloader**: For loading the .obj 3D model file and the .mtl file.
  - **glm**: For mathematical operations like matrices and vectors.
  - **stb_image**: For loading texture files.

---

## Installation
### Steps
1. **Clone the Repository**
   ```bash
   git clone https://github.com/nicolas-bock/PGRe_Project
   cd PGRe_Project
   ```

2. **Create ``build`` file**
   ```bash
   mkdir build
   cd build
   ```

3. **Run CMake to generate a buildsystem**
   ```bash
   cmake ..
   ```

5. **Build and run the project using the make build system**
   ```bash
   make -j8 && make run
   ```

---

## Controls
Use your mouse to move the scene:
* Press and hold the ``Right-Button`` to move the scene in the x and y axis (all around the piano).
* Press and hold the ``Scroll-Button`` to get closer to or further from the center of the scene.
* Press and hold the ``Left-Button`` to move the light source, up and down, or left and right, by moving the mouse in the same directions.
* Press ``Escape`` to close the window.
* Press ``F1`` to add or remove the textures/colors.
* Press ``F2`` to switch the light on/off.
* Press ``F3`` to switch to phantom mode.
* Press ``F4`` to add a fresnel effect.