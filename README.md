# MA's Graphics Explorations

A sandbox for exploring rasterized and ray traced rendering techniques. Made using MSVC, GLFW, and ImGui.

## OpenGL Renderer

![Renderer Layout](OpenGL/res/textures/raster0.png)

Features:

- Triangle mesh loading
- Phong lighting with optional diffuse and specular textures
- Shadowmaps for area, point, and spot lights
- Environment maps
- Environment reflections
- Tessellation shaders
  - Displacement maps
- Geometry shaders
  - Wireframes

## CPU Path Tracer

![Showcase Render](RayTracer/res/images/showcase0.png)

Features:

- Importance sampling
- Bounding Volume Hierarchies (BVH)
  - Axis Aligned Bounding Boxes (AABBs)
- Procedural textures
  - Checkerboard
  - Perlin
  - Marble
- Constant density volumes
- Triangle meshes, spheres, planes, cubes
- Arbitrary matrix transformations (including non-uniform scaling)
- Environment maps
- Pinhole and thin lens cameras

---

Resources used:

- [The Cherno's OpenGL series](https://youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&si=32JwEvKfUVJ4TE7f)
- [Cem Yuksel's Course on Interactive Computer Graphics](https://youtube.com/playlist?list=PLplnkTzzqsZS3R5DjmCQsqupu43oS9CFN&si=N93uHdDh6B19PljD)
- [Ray Tracing in One Weekend by Peter Shirley](https://raytracing.github.io/)
- [Physically Based Rendering: From Theory to Implementation](https://pbr-book.org/)
- [ImGui Docking Branch](https://github.com/ocornut/imgui/tree/docking)
