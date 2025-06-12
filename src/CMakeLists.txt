add_executable(snowballrun main.cpp)
target_include_directories(
  snowballrun PRIVATE
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(
  snowballrun PRIVATE
  OpenGL::GL
  glfw
  GLEW::GLEW
  glm::glm
  assimp::assimp
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
)
