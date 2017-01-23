# Compilation settings
set(EXECUTABLE_OUTPUT_PATH ${HELLO_SOURCE_DIR}/bin)
set(CMAKE_C_COMPILER g++)
set(SRC_LIST main.cpp)

# GDB Debug supported
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(hello ${SRC_LIST})

# Find dependencies packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(libSOIL REQUIRED)

# Include directories of dependicies
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${libSOIL_INCLUDE_DIRS}
)

# Could not find the OpenGL and GLUT packages
if (NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT libSOIL_FOUND)
    message(FATAL_ERROR "Package not found!")
endif(NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT libSOIL_FOUND)

