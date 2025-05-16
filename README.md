# 🐇 3D-Graphics Engine (WIP)

A lightweight C++ playground for learning modern **3D graphics programming**.  
The goal is to keep the core codebase minimal so we can focus on:

* raw vertex-buffer manipulation (no scene graph magic),
* shader experimentation, and
* rapid iteration on tiny geometry samples.

Everything you see here is deliberately “hand-rolled” to stay close to the GPU.

---

## ✨ Current micro-demo

![screenshot](GitResources/Screenshot%202025-05-08%20215529.png)

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


