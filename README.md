# 3D Graphics Engine (WIP)

A lightweight C++ playground for learning modern 3D graphics programming.

The goal is to keep the core codebase minimal so the focus stays on:
- raw vertex-buffer manipulation (no scene graph magic)
- shader experimentation
- rapid iteration on small geometry samples

Everything here is deliberately hand-rolled to stay close to the GPU.


## Demos (V1)

### Current demo screenshots
![screenshot](GitResources/Screenshot%202025-05-08%20215414.png)

### Example: building geometry by hand
```cpp
void Bunny::CreateShape()
{
    mVertices.clear();

    // ears
    mVertices.push_back({ { 0.00f,  0.30f, 0.0f }, Colors::Brown });
    mVertices.push_back({ { 0.30f,  0.70f, 0.0f }, Colors::Brown });
    mVertices.push_back({ { 0.10f,  0.15f, 0.0f }, Colors::Brown });

    // ... (heads, legs, body, etc.) ...
}
```


## Quick Tour

### Real-time primitives
The engine starts with the fundamentals: lighting, shading, and depth testing for:
- sphere
- cube
- rectangular prism
- pyramid

![3D primitives rendered in real time (sphere, cube, rectangular prism, pyramid)](GitResources/3dShapez.png)

### Live debug UI
Press Tab (or your configured hotkey) to open an ImGui-powered debug menu.
Use it to tweak values and inspect the scene while the engine is running.

![In-engine ImGui debug menu with lighting, material, and performance controls](GitResources/DebugMenu.png)

### Everything working together
An integration demo showing primitives, debug UI, input, and the render loop running together.

![Combined scene showing primitives and debug UI running simultaneously](GitResources/EverythingTogether.png)


## Feature Highlights (V2)

Replace the placeholders below with your real images and gifs once exported.

### 1) Lighting with material maps
Lighting pipeline supports:
- diffuse (albedo)
- specular
- normal
- shadow maps

![V2 lighting maps](GitResources/lighting_maps_placeholder.png)

### 2) Lighting effects: cel shading
A stylized cel shader for toon-like lighting and banded shading.

![V2 cel shader](GitResources/cel_shader_placeholder.png)

### 3) Model loader and render groups
Loads external models and organizes draw calls using render groups.

![V2 model loader](GitResources/model_loader_placeholder.png)

### 4) Post-processing effects
Post-processing stack with effects such as:
- monochrome
- chromatic aberration
- blur

![V2 post processing](GitResources/post_processing_placeholder.png)

### 5) Shadows using a depth map
Shadowing implemented via a depth map pass.

![V2 depth map shadow](GitResources/V2/depthmap_shadow_placeholder.png)

### 6) Procedural opal gemstone shader (custom)
A custom shader that procedurally renders opal-like gemstones.

Opal shader demos (4 gifs):
![Opal shader demo 01](GitResources/opal_01.gif)
![Opal shader demo 02](GitResources/opal_02.gif)
![Opal shader demo 03](GitResources/opal_03.gif)
![Opal shader demo 04](GitResources/opal_04.gif)
