# All-Purpose Graphics Playground (APGP)

APGP serves as my custom environment to test and implement various aspects of real-time 3D graphics rendering. As a repository, it serves as a personal exercise in making a publicly-accessible codebase that can easily run in a system-agnostic manner, along with simply being an archive of my efforts.

## Getting Started

### Dependencies

APGP requires a handful of external libraries to function. Some of these (specifically all header-only libraries) are included within the repo. Other than those, it is recommended you use the package manager [vckpg](https://github.com/microsoft/vcpkg) to install **GLFW3** and **GLEW**.

```bash
vcpkg install glfw3
vcpkg install glew
```

### Installation [TODO]

Firstly, clone the repository to your desired location.

```powershell
git clone https://github.com/liamckenna/APGP.git
cd APGP
```

then blah blah blah

## Usage [TODO]

Build and see the explanation of the json format below.

## Repository Breakdown

### build

Properly building the folder will create the executable **APGP.exe** which will launch the program. This executable should be located in the **build** folder after a successful compilation.

### data

Opening the **data** folder, you'll find a handful of nested directories that work together to describe the scene being generated when APGP is run.

### data/jsons

The **jsons** folder contains a handful of json files by default. **program.json** tells our program the names of the user and scene jsons, respectively. By default, APGP loads **user.json** for the user data and **scene.json** for the scene because program.json directs it their way. APGP expects a standard format from these json files, which can be seen in the files loaded by default. 

The benefit of this approach is that rebuilding is **not required** to change what APGP loads. By following the formats described by user.json and scene.json, you can create your own json files for APGP to load without having to hard-code any of the data.

### data/jsons/scenes

This folder should contain any json files that describe the **scene** you want APGP to load. 

Scene descriptions should first detail the use of any given shaders or meshes (.obj files) that you want to load. Camera, light, and object data should all be described within this json, along with the default material you want meshes to use in the case they are not directly given a material.

See **scene.json** for a demonstration of the formatting APGP expects from a scene description.

### data/jsons/users

This folder should contain any json files that describe the **user** you want APGP to load. 

User data is a catch-all for the following information: OpenGL, GLFW, and GLEW-specific settings, window descriptions, and APGP-specific settings.

See **user.json** for a demonstration of the formatting APGP expects from a user description.

### data/materials

The **materials** directory holds all .mtl files requested by the objects held in the objects folder. These are loaded as-needed, from any objects that request them.

### data/objects

The **objects** folder holds all .obj files requested by the scene.

### include

The **include** directory holds all header files used by APGP. There are a handful of header files that either don't yet have or simply don't require source counterparts. 

### libs

**libs** contains all the external libraries that are included within APGP. As stated prior, these consist of the header-only libraries **glm** for vector and matrix calculations, **nlohmann's json parser** for, well, parsing jsons, and **stb_image** for texture implementation.

### src

The **src** folder contains all of the cpp source files I have created for APGP. All of these source files have complementary header files in the **include** folder with the exception of **main.cpp**.


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

---

For detailed licensing information, see the respective library's documentation or license file.
