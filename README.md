# 3D-Graphics Engine (WIP)

A lightweight C++ playground for learning modern **3D graphics programming**.  
The goal is to keep the core codebase minimal so we can focus on:

* raw vertex-buffer manipulation (no scene graph magic),
* shader experimentation, and
* rapid iteration on tiny geometry samples.

Everything you see here is deliberately “hand-rolled” to stay close to the GPU.

---

## Current demo

![screenshot](GitResources/Screenshot%202025-05-08%20215414.png)
![screenshot](GitResources/Screenshot%202025-05-08%20215529.png)
![screenshot](GitResources/Screenshot%202025-05-08%20215454.png)

```cpp
void Bunny::CreateShape()
{
    mVertices.clear();

    // ——— ears ———
    mVertices.push_back({ { 0.00f,  0.30f, 0.0f }, Colors::Brown });
    mVertices.push_back({ { 0.30f,  0.70f, 0.0f }, Colors::Brown });
    mVertices.push_back({ { 0.10f,  0.15f, 0.0f }, Colors::Brown });

    // … (heads, legs, body, etc. – 18 more triangles) …
}

```
## Quick Tour

###  Real-Time Primitives
Shapez 3D starts with the fundamentals: watch it light, shade, and depth-test a **sphere, cube, rectangular prism, and pyramid** in real time.  
It’s a minimal scene—but every major subsystem (camera, shader pipeline, PBR materials) is already live.

![3D primitives rendered in real time (sphere, cube, rectangular prism, pyramid)](GitResources/3dShapez.png)

---

###  Live Debug UI
Tap <kbd>Tab</kbd> (or your favourite hotkey) to summon an **ImGui-powered debug menu**.  
Here the user can edit the fuctions and inspect models real time.

![In-engine ImGui debug menu with lighting, material, and performance controls](GitResources/DebugMenu.png)

---

###  Everything Working Together
Below is a full integration demo—**primitives + debug UI + input + render loop**—all happily co-existing.  
If it renders here, it’s ready for you to extend with your own meshes and shadersc.

![Combined scene showing primitives and debug UI running simultaneously](GitResources/EverythingTogether.png)

---

## V2 Features

### Lights
Standard Blinn-Phong lighting model implementation supporting diffuse, specular, and ambient components. Normals are mapped from tangent space to world space using a TBN (Tangent-Bitangent-Normal) matrix derived from the geometry.

![Lights Placeholder](Screenshot 2025-12-31 145941.png)

### Light Effects
Featuring a stylized **Cel Shader** that quantizes lighting into discrete bands. It features a 3-band diffuse step function and binary specular highlights. Outlines are generated using a dot-product edge detection method compared against the view vector.

![Cell Shader Placeholder](Screenshot 2025-12-31 152923.png)

### Model Loader and Render Group
A robust `Model` class that handles mesh data and material properties, managed by a `RenderGroup` system that batches render calls and handles hierarchical transformations.

![Model Loader Placeholder](Screenshot 2025-12-31 153945.png)

### Post Processing Effects
A configurable post-processing pipeline featuring:
*   **Monochrome**: Converts the scene to grayscale using luminance weighting.
*   **Chromatic Aberration**: Simulates lens distortion by offsetting red and blue channels radially.
*   **Blur**: Implements a 3x3 box blur or motion blur kernel for depth-of-field and motion effects.

![Post Processing Placeholder](Screenshot 2025-12-31 154642.png)

### Shadows
Dynamic shadow mapping using a depth-only pass from the light's perspective. The depth map is sampled during the main render pass to determine occlusion.

![Shadows Placeholder](Screenshot 2025-12-31 154808.png)

### Custom Shader
A procedurally generated **Opal Gemstone** shader featuring:
*   **Volumetric Depth**: Simulates milky interior depth using a facing-ratio approximation.
*   **Internal Structure**: Fractal Voronoi noise to create crystal domains.
*   **Iridescence**: View-dependent color shifting ("Schiller effect") using tangent-space diffraction grating simulation controlled by the view angle and crystal facet orientation.

![Opal Gif 1](Animation4.gif)
![Opal Gif 2](Animation3.gif)
![Opal Gif 3](Animation2.gif)
![Opal Gif 4](Animation1.gif)
![Opal Gif ](Animation.gif)
