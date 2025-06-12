cmake_minimum_required(VERSION 3.22)
project(snowball LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug Release." FORCE)
endif()
message(STATUS "Check the build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# About required dependencies:
# - opengl >= 3.30: support GLSL shader in this project.
# - sdl >= 2.0.0: support image loading, saving, etc. about direct media.
# - assimp: needed to import models constructed externly.
# - glm: mathematics libraries for opengl.
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(src)
