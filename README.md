# SDL engine

A small C++17 hobby project that renders a hard-coded coloured triangle in a
resizable window. Includes a simple scene container and vector math helpers.

## Build and run

Requires CMake 3.15+, a C++17 compiler, and SDL3 3.2+ development files.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 2
./build/run
```

If CMake cannot find SDL3, add `-DCMAKE_PREFIX_PATH=/path/to/SDL3` to the first
command. Close the window to exit.

## Rendering

Uses SDL_GPU with Metal or Vulkan and requires a compatible GPU and driver.
Startup errors are reported to the terminal. There is no depth buffer or scene
loading yet.

Edit `SDLInterface::BuildGeometry()` in `sdl_interface.cpp` to change the scene.
Coordinates run from -1 to 1, with positive y pointing up. The scene stretches
with the window.

Shader assets are copied into `shaders/` beside the executable during the build.
Keep that directory with the executable if you move it. Both Metal source and
precompiled SPIR-V are included; no shader compiler is needed to run the project.
After editing GLSL, regenerate the SPIR-V with `glslangValidator`:

```sh
glslangValidator -V --target-env vulkan1.0 -o shaders/triangle.vert.spv shaders/triangle.vert
glslangValidator -V --target-env vulkan1.0 -o shaders/triangle.frag.spv shaders/triangle.frag
```

Keep `shaders/triangle.metal` in sync with the GLSL shaders.

## Current Output 
<img width="960" height="522" alt="Screenshot 2026-10-17 at 16 11 10" src="https://github.com/user-attachments/assets/3377b74b-acf7-4507-b27e-053a3591609e" />

