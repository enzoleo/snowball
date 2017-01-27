# --------------------------------------------------------------------------------------
# COMPILER settings
#
# Create new directory named `bin` in the source directory. This `bin` directory save
# all binary files, executables included. 
#
# Set the default compiler g++ to link c++ standard libraries, as we use c++ standard
# template library in this project. Besides set the compiler flag "-std=c++0x" to enable
# c++11, this is important. Consider that we must include correct headers and link all
# correct libraries accurately, set the following options carefully.
# --------------------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${SnowballRun_SOURCE_DIR}/bin)
set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++0x")
set(SRC_LIST main.cpp)

# GDB Debug supported
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Add executables into binaries directory
add_executable(SnowballRun ${SRC_LIST})

# --------------------------------------------------------------------------------------
# Find DEPENDENCIES packages
#
# OpenGL needed! (Version >= 3.30) We use GLSL shader in this simple project, so make
# sure OpenGL core 3.30 is supported. Besides, in order to support OpenGL3.3, the GLEW
# and GLFW libraries are required. They are OpenGL foundamental dependencies, thus you
# have to install them by compiling the source files or other ways under the instruction
# of official documentations. To enable CMake compilation, We recommend you download the
# source files and compile them by yourself, which ensures high accuracy and few errors.
#
# SDL (Simple DirectMedia Layer) needed! (Version >= 2.0.0) We use SDL to support image
# loading, saving and other functions about direct media. SDL2_image is another extensi-
# on library of SDL2, along with SDL2_mixer, SDL2_ttf, SDL2_net and SDL2_gfx. We also
# strongly recommend you download the source files from the official sites and compile
# them by yourself. The steps are really simple with Unix Makefiles. If you are in OSX
# or Windows, see documentations for your platform. There's one more thing needs atten-
# tion: to enable CMake finding for SDL2 extension libraries (for example, SDL2_image),
# you need to tell CMake where to find headers and linking libraries. Simply writing a
# new cmake file named `FindSDL2_image.cmake` is possible.
#
# ASSIMP library is needed to import models constructed externly. This library is compl-
# ete. Install ASSIMP from the official site and simply use its CMake files to include
# headers and link libraries.
#
# GLM is mathematics libraries for OpenGL. It is written in HPP files, which means you
# don't need to compile the source files first and then link the libraries, just simply
# include the headers is OK. See the official documentations about GLM usage! Attention:
# Generate the CMake-config files and copy them to default cmake paths, or tell CMake
# where to find the glm-config cmake file by hand.  
# --------------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(glm REQUIRED)

# Include directories of dependicies
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_image_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
)

# Could not find the OpenGL and GLUT packages
#if (NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT SDL2_FOUND OR NOT ASSIMP_FOUND OR NOT glm_FOUND)
#    message(FATAL_ERROR "Package not found!")
#endif(NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT SDL2_FOUND OR NOT ASSIMP_FOUND OR NOT glm_FOUND)

target_link_libraries(
    SnowballRun
    glfw
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${ASSIMP_LIBRARIES}
)
