# Real Engine

I practice OpenGL here, and maybe someday it will develop into something big.
For now, only GLTF models are supported.

![Preview](preview.png)

---

## Build Instructions

### Compiler
This project is built with **GCC 15.2.1**. From time to time I fix compilation for the MSVC compiler.

### Requirements
- CMake >= 3.25
- GCC >= 13 (or Clang >= 15)

### Build Steps
1. Clone the repository and install dependencies (see CMakeLists.txt for details).
2. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```
3. Configure the project:
   ```sh
   cmake ..
   ```
4. Build the executable:
   ```sh
   make -j
   ```
5. Run the engine:
   ```sh
   ./Real_Engine
   ```
