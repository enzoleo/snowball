# Compilation settings
set(EXECUTABLE_OUTPUT_PATH ${SnowballRun_SOURCE_DIR}/bin)
set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++0x")
set(SRC_LIST main.cpp)

# GDB Debug supported
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Add executables
add_executable(SnowballRun ${SRC_LIST})

# Find dependencies packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(glm REQUIRED)

# Include directories of dependicies
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
)

# Could not find the OpenGL and GLUT packages
if (NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT SDL2_FOUND OR NOT ASSIMP_FOUND OR NOT glm_FOUND)
    message(FATAL_ERROR "Package not found!")
endif(NOT OpenGL_FOUND OR NOT GLEW_FOUND OR NOT glfw3_FOUND OR NOT SDL2_FOUND OR NOT ASSIMP_FOUND OR NOT glm_FOUND)

target_link_libraries(
    SnowballRun
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${ASSIMP_LIBRARIES}
)
