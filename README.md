# snowball
[![Build Status](https://travis-ci.org/enzoleo/SnowballRun.svg?branch=master)](https://travis-ci.com/enzoleo/snowball.svg)
[![License](https://img.shields.io/github/license/enzoleo/SnowballRun.svg)](https://raw.githubusercontent.com/enzoleo/snowball/master/LICENSE)
### *Archive*

*This repository is archived now.*

## Brief introduction
This is a simple game developed by **OpenGL3.3**. You control a snowball and drive it home, which means, you need to elude barriers to avoid dying and accumulate other snowballs to prolong your life. The snowball is always being melting! In other words, the radius of your snowball, which represents your life level, is always being decreasing. Merely elude barriers, or you will be dead immediately, and do not forget to accumulate snowballs at the same time. When seeing a house, you win the game.

## Dependencies
+ [**ASSIMP**](https://github.com/assimp/assimp): Open Asset Import Library (assimp)
> Official Open Asset Import Library Repository. Loads 40+ 3D file formats into one unified and clean data structure. [http://www.assimp.org](http://www.assimp.org) 

+ [**SDL**](https://www.libsdl.org/): Simple DirectMedia Layer (**VERSION >= 2.0.0**)
> Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D. [https://www.libsdl.org/](https://www.libsdl.org/)

**OpenGL3.3**: *GLM*, *GLEW* and *GLFW* required.

+ [**GLM**](https://github.com/g-truc/glm): OpenGL Mathematics (GLM)
> OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications. [https://glm.g-truc.net](https://glm.g-truc.net)  

+ [**GLEW**](https://github.com/nigels-com/glew): The OpenGL Extension Wrangler Library
> GLEW - The OpenGL Extension Wrangler Library
[http://glew.sourceforge.net/](http://glew.sourceforge.net/)  
[https://github.com/nigels-com/glew](https://github.com/nigels-com/glew)  

+ [**GLFW**](https://github.com/glfw/glfw): A multi-platform library for OpenGL, window and input 
> GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development. It provides a simple, platform-independent API for creating windows, contexts and surfaces, reading input, handling events, etc. [http://www.glfw.org/](http://www.glfw.org/)

## Download
This project is tested on `Linux` and `Windows` platforms. [Download](https://codeload.github.com/perryleo/FinalProject/zip/master) the source files and compile them manually (make sure the **dependencies** mentioned above have been built). CMake is strongly recommended for you! See the official site of [CMake](https://cmake.org) to learn more details. 

[**CMake**](https://cmake.org): An open-source, cross-platform family of tools designed to build, test and package software.
> CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice. [https://cmake.org](https://cmake.org)  

[Download](https://cmake.org/download/) **CMake** (choose the suitable version from the official site) and install it. Then use your CMake to compile source files. The executables will be generated into the `bin/` directory.

### Linux
Open the home directory of this poject in terminal. Create a new directory named `build/` and build CMake files in this directory. Below is an example of how the game can be compiled on Linux.

```
mkdir build
cd build
cmake ..
make
````
