# All-Purpose Graphics Playground (APGP) iPASS Shadowing

Render pass diagram and accompanying pseudocode are available in the assets folder.

Relevant Files are as follows:

# Header/Source Files

## scene.cpp
Where surface and light entities are added into the scene. Entities must be created, then populated with relevant components.

## input_system.cpp
All controls and inputs are registered and defined in this file. 
Move camera with wasd, look around with mouse.
Move the current object with arrow keys, holding shift to move forwards and back rather than up and down.
'u' changes the current object to control, 'i' turns off and on shadowing, 'o' changes how we calculate tess factors from the compute shader to c++ code, and 'p' enables/disables a wireframe of the coarse surface approximation used to generate shadows.

## patch.cpp
Contains functions used when iPASS algorithm is calcultated without the compute shader.

## render_system.cpp
Second most relevant source file. Handles the general render loop.
Clear() clears the screen and depth buffer.
UpdateProjection() updates the matrices related to the primary camera and viewing frustrum.
RenderSurfaceLighting() clears the light's depth texture, writes the MVPs of each surface relative to the light, then calls WriteDepthBuffer() in surface_renderer.cpp.
RenderSurfaces() calls a function to update the patch tess levels (UpdatePatchTessLevels() when done through the compute file, UpdateTessNonCompute() otherwise), and then calls RenderSurface() in surface_renderer.cpp.

## surface_renderer.cpp
Most relevant source file. Handles the rendering of surfaces.
WriteDepthBuffer() writes coarse surface shadows to the depth buffer.
UpdatePatchTessLevels() dispatches the iPASS compute shader to inform proper tessellation levels for each patch.
RenderSurface() renders the surface.
UpdateTessNonCompute() uses the afformentioned c++ code to compute tessellation factors.

## universal_vars.h/.cpp
Used to easily pass variables across files.

# Shader Files

## patch_tess_pass.cs.glsl
Responsible for two things:
1. Computing the tessellation factors for each patch during the tess pass
2. Feeding an EBO the proper indexing to triangulate the control vertices for coarse shadowing

## patch_shadow.vs/.fs.glsl
Fills the depth buffer with coarse surface data to be used for shadowing.

## visualize_tiles.vs/.fs.glsl
Rudimentary shader for the purpose of viewing the coarse mesh versions of surfaces used for shadowing.

## render_pass.vs/.tes/.tcs/.fs.glsl
Main render loop for surfaces. Tess Engine applies tess factor levels determined by patch_tess_pass, and frag shader applies depth buffer for shadowing.

# Json Files

## graphics_config.json
Allows certian gl/glfw settings to be done without recompilation.

## program.json
Tells program what json files to look for when getting program data.

## shaders.json
Defines all shader program names and their files for use in c++ code.

## windows.json
Defines window qualities such as resolution, title, display mode, etc..

# Surface Files

## *.srf

A custom spec based on .obj. v x y z defines vertex position, vt x y defines texture coordinate, and cv v1/vt1 v2/vt2 v3/vt3 v4/vt4 defines one tile of a patch. Each set of four cv lines defines a surface patch. Everything is zero-indexed.     

## Contributions

This is a personal repository, and thus any updates to the codebase will come directly from me. Anyone is free to fork and use the program for their own fun but all code is subject to change at my discretion.

## Author

### **Liam D. McKenna** 
Graduate student at the University of Florida predominantly studying computer graphics.

## Licenses

This project uses the following third-party libraries:

### GLEW (OpenGL Extension Wrangler Library)
- **License**: [Modified BSD License](https://opensource.org/licenses/BSD-3-Clause)
- **Copyright**: (c) 2002-2019, Nigel Stewart.
- **Summary**: Redistribution and use in source and binary forms, with or without modification, are permitted provided that the license conditions are met. See the GLEW license for more details.

### GLFW (Graphics Library Framework)
- **License**: [zlib/libpng License](https://opensource.org/licenses/Zlib)
- **Copyright**: (c) 2006-2023 Camilla Berglund and contributors.
- **Summary**: This software is provided 'as-is,' with no warranty. Redistribution and use in source and binary forms, with or without modification, are permitted. Attribution is appreciated but not required.

### OpenGL
- **License**: Khronos Group's [OpenGL Specification License](https://www.khronos.org/legal/)
- **Copyright**: Khronos Group and contributing organizations.
- **Summary**: The OpenGL API and specification are free to use. Implementation details depend on the GPU driver/vendor.

### GLM (OpenGL Mathematics Library)
- **License**: [MIT License](https://opensource.org/licenses/MIT)
- **Copyright**: (c) 2005-2023 G-Truc Creation.
- **Summary**: Permission is granted to use, modify, and distribute this library for any purpose, provided the original copyright and license are retained.

### nlohmann JSON
- **License**: [MIT License](https://opensource.org/licenses/MIT)
- **Copyright**: (c) 2013-2023 Niels Lohmann.
- **Summary**: Permission is granted to use, modify, and distribute this library for any purpose, provided the original copyright and license are retained.

### iPASS / Pixel-Accurate Rendering
- **License**: GNU General Public License v3.0
- **Copyright**: (c) 2017–2023, Yeo, Bin, Peters
- **Source**: Derived from the iPASS system described in Efficient Pixel-Accurate Rendering of Curved Surfaces ([paper link] (https://www.cise.ufl.edu/research/SurfLab/papers/1109reyes.pdf)) and associated code release.
- **Summary**: Portions of this project, including compute shaders and tessellation logic, are adapted under the terms of the GNU GPL v3.0. See the full license in [LICENSE_GPL.txt] for details.

---

For detailed licensing information, see the respective library's documentation or license file.
