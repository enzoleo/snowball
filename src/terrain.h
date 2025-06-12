/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <math.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <print>
#include <string>
#include <vector>

#include "hmap_generator.h"
#include "objects.h"

#define _TERRAIN_NORMAL_SAVE_
#define _TERRAIN_HEIGHT_SMOOTH_

/* STATIC FUNCTION
** Computes the altitude coordinates in an arbitrary triangle
** @param vertex1, vertex2, vertex3 are vertices of the triangle.
** @param (x, y, z) must lay on the triangle
** Then we use the value of x, z to return altitude y. */
static GLfloat genAltitudeCoord(const glm::vec3& vertex1,
                                const glm::vec3& vertex2,
                                const glm::vec3& vertex3,
                                const GLfloat& x, const GLfloat& z);

/* CLASS: terrain */
class Terrain : public Object {
 public:
  /* Default constructor & constructor */
  Terrain(const char* heightMapPath,
          GLfloat _size = 100.0f,
          GLfloat _peak = 40.0f)
      : size(_size),
        peak(_peak) {
    // model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
    generate(heightMapPath);
  }

  /* Default destructor */
  ~Terrain() {
    // Delete height data pointer
    for (GLuint _row = 0; _row < cells; _row++) delete heights[_row];
    delete[] heights;
  }

  /* Returns the private members
  ** We set the terrain settings private, because they are not supposed to be
  ** editted easily. If they need to be editted, call the `set*` functions, which makes
  ** sure you edit them on purpose, instead of unconsciously. */
  const GLint getCells() { return cells; }
  const GLfloat getSize() { return size; }
  const GLfloat getPeak() { return peak; }

  /* DRAW function */
  void draw(Shader shader) {
    shader.install();
    glBindVertexArray(VAO);
    shader.setUniformMatrix4fv("model", model2world);

    // Bind the texture (if not NULL)
    if (texture_ptr) {
      texture_ptr->bind(texture_ptr->getUnit());
      shader.setUniform1i("material.diffuse1", texture_ptr->getUnit());
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    shader.uninstall();
  }

  /* IMPORTANT PUBLIC FUNCTION
  ** This function sets up all variables needed in drawing.
  ** Bind buffer data and get ready. */
  void setup() {
    // Generate vertex arrays and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vert_VBO);
    glGenBuffers(1, &normal_VBO);
    glGenBuffers(1, &uv_VBO);
    glGenBuffers(1, &EBO);

    // Bind vertex array
    glBindVertexArray(VAO);

    // Bind vertex VBO buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vert_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Bind normals VBO buffer data
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Bind UV VBO buffer data
    glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Bind EBO buffer data (index)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
  }

  /* PUBLIC FUNCTION
  ** This function computes the altitude (approximate height) with
  ** given x-z coordinates in world coordinate system. */
  GLfloat getAltitude(const GLfloat& xterrain,
                      const GLfloat& zterrain) {
    // Get the coordinates and size of each cell
    // @xterrain and @zterrain must be in [0, size)
    GLfloat cell_size = size / ((GLfloat)cells - 1);

    // Calculate grid position
    GLuint hgrid = (GLuint)(zterrain / cell_size);
    GLuint wgrid = (GLuint)(xterrain / cell_size);
    if (hgrid < 0 || hgrid >= cells - 1 ||
        wgrid < 0 || wgrid >= cells - 1) {  // Wrong position
      std::print(stderr, "ERROR: Illegal parameter.\n");
      std::print(stderr, "In function `GLfloat getAltitude(const GLfloat&, const GLfloat&)`.\n");
      exit(_VECTOR_ILLEGAL_SIZE_);
    }

    // Notice that the @xCoord and @zCoord are in [0, 1)
    GLfloat xCoord = fmod(xterrain, cell_size) / cell_size;
    GLfloat zCoord = fmod(zterrain, cell_size) / cell_size;

    // Compute altitude!
    GLfloat altitude = (xCoord < zCoord) ? genAltitudeCoord(glm::vec3(0.0f, heights[hgrid][wgrid], 0.0f),
                                                            glm::vec3(1.0f, heights[hgrid + 1][wgrid + 1], 1.0f),
                                                            glm::vec3(0.0f, heights[hgrid + 1][wgrid], 1.0f),
                                                            xCoord, zCoord)
                                         : genAltitudeCoord(glm::vec3(0.0f, heights[hgrid][wgrid], 0.0f),
                                                            glm::vec3(1.0f, heights[hgrid][wgrid + 1], 0.0f),
                                                            glm::vec3(1.0f, heights[hgrid + 1][wgrid + 1], 1.0f),
                                                            xCoord, zCoord);

    return altitude;
  }

 private:
  /* PRIVATE MEMBER
  ** Generates all parameters needed. */
  void generate(const char* heightMapPath) {
    // Load heightmap from file
    SDL_Surface* surface = IMG_Load(heightMapPath);

    try {  // Assignments
      heights = new GLfloat*[cells];
      for (GLuint hloop = 0; hloop < cells; hloop++) {  // New pointer space
        heights[hloop] = new GLfloat[cells];
      }
    } catch (const std::bad_alloc& err) {  // Allocation failed (catch exception)
      std::print(stderr, "Allocation failed.\n");
      exit(_ALLOCATION_FAILED_);
    }

    // Rescale the heights read from height map
    readHeightMapData(reinterpret_cast<unsigned char*>(surface->pixels));
    SDL_FreeSurface(surface);

    // --------------------------------------------------------------------------------------
    // Compute vertices(positions), normals and texture coordinates
    computeBufferObjects();

    // Computation Ended! Bind VAO, VBO and EBO before draw this terrain
    // --------------------------------------------------------------------------------------
    // If the maco _TERRAIN_NORMAL_SMOOTH_ defined, then tell the compiler the following part needs to
    // be compiled. Thus, if you need to smoothing the normals (actually the normals computed above are
    // absolutely approximate values), define this macro before!
#ifdef _TERRAIN_NORMAL_SMOOTH_
    smoothingNormals();
#endif
#ifdef _TERRAIN_NORMAL_SAVE_
    saveNormalMap("../assets/terrains/normal_map.png");
    std::print("Normal Map Saved Successfully!\n");
#endif
  }

  /* PRIVATE MEMBER
  ** This function can smoothing normals approximately.
  ** But use it carefully, because it may destruct your normal data.
  ** If you are certain this function is needed, define macro _TERRAIN_NORMAL_SMOOTH_. */
  void smoothingNormals() {
    for (GLuint hloop = 0; hloop < cells; hloop++) {  // Traversal -> all pixels
      for (GLuint wloop = 0; wloop < cells; wloop++) {
        // Get location of 8 pixels around
        GLint _cur_cell = hloop * cells + wloop;
        // Get the normals of 8 pixels around
        glm::vec3 normal_right = (wloop < cells - 1) ? normals[_cur_cell + 1] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_toprt = (hloop > 0 && wloop < cells - 1) ? normals[_cur_cell - cells + 1] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_top = (hloop > 0) ? normals[_cur_cell - cells] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_toplt = (hloop > 0 && wloop > 0) ? normals[_cur_cell - cells - 1] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_left = (wloop > 0) ? normals[_cur_cell - 1] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_btmlt = (hloop < cells - 1 && wloop > 0) ? normals[_cur_cell + cells - 1] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_btm = (hloop < cells - 1) ? normals[_cur_cell + cells] : glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 normal_btmrt = (hloop < cells - 1 && wloop < cells - 1) ? normals[_cur_cell + cells + 1] : glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 _temp_vec = glm::normalize(normal_right + normal_left + normal_top + normal_btm + normal_toprt + normal_btmlt + normal_toplt + normal_btmrt);
        // Compute the new normal vector
        normals[_cur_cell] = glm::normalize(normal_smooth * _temp_vec + (1 - normal_smooth) * normals[_cur_cell]);
      }
    }
  }

  /* PRIVETE MEMBER
  ** The function to calculate heights data from image */
  void readHeightMapData(unsigned char* data) {
    for (GLuint hloop = 0; hloop < cells; hloop++) {    // Width traversal
      for (GLuint wloop = 0; wloop < cells; wloop++) {  // Height traversal
        GLfloat height_data = peak * (GLfloat)data[3 * (hloop * cells + wloop)] / 255;
        heights[hloop][wloop] = data ? height_data : 0.0f;
      }
    }
    // Attention: The @height_data above onlv has 256 possible values
    // Smooth interpolation between heights
#ifdef _TERRAIN_HEIGHT_SMOOTH_
    smoothing(0.1f);
#endif
  }

  /* PRIVATE MEMBER
  ** Computes all buffer objects
  ** @param cell_size: means the length of each edge of a cell */
  void computeBufferObjects() {
    // Get the size of each cell
    GLfloat cell_size = size / ((GLfloat)cells - 1);

    // The left-bottom corner of the terrain square
    glm::vec2 position(0.0f, 0.0f);
    for (GLuint hloop = 0; hloop < cells; hloop++) {  // Traversal -> all pixels
      for (GLuint wloop = 0; wloop < cells; wloop++) {
        // Compute vertices, uv and normals
        glm::vec3 vertex(position.x, heights[hloop][wloop], position.y);
        glm::vec2 uv(position.x / size, position.y / size);
        glm::vec3 normal = computeNormals(hloop, wloop);

        // Push all data into STL vector
        vertices.push_back(vertex);
        texCoords.push_back(uv);
        normals.push_back(normal);
        position.x += cell_size;

        if (hloop < cells - 1 && wloop < cells - 1) {
          // Compute indices in EBO binding
          GLint bottom_left = hloop * cells + wloop;
          GLint bottom_right = bottom_left + 1;
          GLint top_left = bottom_left + cells;
          GLint top_right = top_left + 1;

          indices.push_back(bottom_left);
          indices.push_back(bottom_right);
          indices.push_back(top_right);
          indices.push_back(bottom_left);
          indices.push_back(top_right);
          indices.push_back(top_left);
        }
      }
      position.x = 0.0f;
      position.y += cell_size;
    }
  }

  /* PRIVATE MEMBER
  ** Smoothing the height data. Use it Carefully.
  ** @param alpha: The smooth factor. This value is in [0, 1]. The smaller this value
  **     is, the smoother our terrain will be. */
  void smoothing(GLfloat alpha) {
    for (GLuint hloop = 0; hloop < cells; hloop++) {    // Traversal -> all pixels
      for (GLuint wloop = 0; wloop < cells; wloop++) {  // Determine whether the pixel is on border
        GLboolean border_flag = false;
        GLboolean vertex_flag = false;
        if (hloop == 0 || hloop == cells - 1) {  // On left or right border
          border_flag = true;
          if (wloop == 0 || wloop == cells - 1) {
            vertex_flag = true;
            border_flag = false;
          }
        }  // On top or bottom border
        else if (wloop == 0 || wloop == cells - 1)
          border_flag = true;

        // Get the height of 8 pixels around
        GLfloat _height_right = (wloop < cells - 1) ? heights[hloop][wloop + 1] : 0.0f;
        GLfloat _height_toprt = (hloop > 0 && wloop < cells - 1) ? heights[hloop - 1][wloop + 1] : 0.0f;
        GLfloat _height_top = (hloop > 0) ? heights[hloop - 1][wloop] : 0.0f;
        GLfloat _height_toplt = (hloop > 0 && wloop > 0) ? heights[hloop - 1][wloop - 1] : 0.0f;
        GLfloat _height_left = (wloop > 0) ? heights[hloop][wloop - 1] : 0.0f;
        GLfloat _height_btmlt = (hloop < cells - 1 && wloop > 0) ? heights[hloop + 1][wloop - 1] : 0.0f;
        GLfloat _height_btm = (hloop < cells - 1) ? heights[hloop + 1][wloop] : 0.0f;
        GLfloat _height_btmrt = (hloop < cells - 1 && wloop < cells - 1) ? heights[hloop + 1][wloop + 1] : 0.0f;

        // Compute the new normal vector
        GLfloat _add_height = _height_right + _height_left + _height_toprt + _height_btmlt + _height_top + _height_btm + _height_toplt + _height_btmrt;

        if (vertex_flag)  // The pixel is one of the vertices
          heights[hloop][wloop] = 0.75f * alpha * heights[hloop][wloop] + 0.25f * (1.0f - alpha) * _add_height;
        else if (border_flag)  // The pixel is on border but not a vertex
          heights[hloop][wloop] = (5.0f / 6) * alpha * heights[hloop][wloop] + (1.0f / 6) * (1.0f - alpha) * _add_height;
        else  // The pixel is inside the height map
          heights[hloop][wloop] = 0.875f * alpha * heights[hloop][wloop] + 0.125f * (1 - alpha) * _add_height;
      }
    }
  }

  /* PRIVATE MEMBER
  ** Computes the normal vectors with given vertex position */
  glm::vec3 computeNormals(GLuint hpos, GLuint wpos) {
    // Get the size of each cell
    GLfloat cell_size = size / ((GLfloat)cells - 1);

    // ------------------------------------------------------------------------------------------
    // Declare the 6 vectors around the pixel
    // Attention: In our EBO binding, each vertex is in 6 triangles, so compute normals of these
    //     triangles, and weighted-sum all of it by their areas (actually the length of vector
    //     cross). Do normalization, and the result is the approximate normal vector.
    // ------------------------------------------------------------------------------------------
    glm::vec3 right_vec = (wpos < cells - 1) ? glm::vec3(cell_size, heights[hpos][wpos + 1] - heights[hpos][wpos], 0.0f)
                                             : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 top_vec = (hpos > 1) ? glm::vec3(0.0f, heights[hpos - 1][wpos] - heights[hpos][wpos], -cell_size)
                                   : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 toplt_vec = (hpos > 1 && wpos > 1) ? glm::vec3(-cell_size, heights[hpos - 1][wpos - 1] - heights[hpos][wpos], -cell_size)
                                                 : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 left_vec = (wpos > 1) ? glm::vec3(-cell_size, heights[hpos][wpos - 1] - heights[hpos][wpos], 0.0f)
                                    : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 btm_vec = (hpos < cells - 1) ? glm::vec3(0.0f, heights[hpos + 1][wpos] - heights[hpos][wpos], cell_size)
                                           : glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 btmrt_vec = (hpos < cells - 1 && wpos < cells - 1) ? glm::vec3(cell_size, heights[hpos + 1][wpos + 1] - heights[hpos][wpos], cell_size)
                                                                 : glm::vec3(0.0f, 0.0f, 0.0f);

    // The lines above determine whether the vertex is a vertex or on border
    // Compute the approximate normal vector of the chosen vertex
    glm::vec3 normal_vec = glm::cross(right_vec, top_vec) + glm::cross(top_vec, toplt_vec) + glm::cross(toplt_vec, left_vec) + glm::cross(left_vec, btm_vec) + glm::cross(btm_vec, btmrt_vec) + glm::cross(btmrt_vec, right_vec);

    return glm::normalize(normal_vec);
  }

  /* PRIVATE MEMBER
  ** The function to save normal map */
  GLint saveNormalMap(const std::string& filename) {
    if (normals.size() != cells * cells) {  // Check the normal vector size
      std::print(stderr, "ERROR: The size of normals: {}", normals.size());
      exit(_VECTOR_ILLEGAL_SIZE_);
    }

    // Normal vector -> RGB value
    unsigned char* normal_map = new unsigned char[cells * cells * 3];
    for (GLuint hloop = 0; hloop < cells; hloop++) {
      for (GLuint wloop = 0; wloop < cells; wloop++) {
        normal_map[3 * (hloop * cells + wloop) + 2] = (normals[hloop * cells + wloop].x) * 255;
        normal_map[3 * (hloop * cells + wloop) + 1] = (normals[hloop * cells + wloop].y) * 255;
        normal_map[3 * (hloop * cells + wloop)] = (normals[hloop * cells + wloop].z) * 255;
      }
    }

    // Save the normal map data (PNG file)
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        normal_map, cells, cells, 24,
        cells * 3, 0, 0, 0, 0);

    // Save height map image
    IMG_SavePNG(surface, filename.c_str());
    SDL_FreeSurface(surface);

    // Delete normal map temporary pointer
    delete[] normal_map;
    return 0;
  }

  /* PRIVATE MEMBER
  ** The number of cells on each edge (the same in each edge) */
  GLuint cells = 256;

  /* PRIVATE MEMBER
  ** @param vertices: The vector of vertices
  ** @param normals: The normal vectors of vertices (correspondingly) */
  std::vector<glm::vec3> vertices, normals;

  /* PRIVATE MEMBER
  ** The 2d-coordinates of texture */
  std::vector<glm::vec2> texCoords;

  /* PRIVATE MEMBER
  ** The indices used in EBO */
  std::vector<GLuint> indices;

  /* PRIVATE MEMBER
  ** The pointer (GLfloat**) points to heights data! (at each grid point)
  ** This member is quite important because we need heights data to render scene */
  ImagePointer heights;

  /* PRIVATE MEMBER
  ** The length of the terrain square */
  GLfloat size;

  /* PRIVATE MEMBER
  ** The max height (peak) of the terrain */
  GLfloat peak;

  /* PRIVATE MEMBER
  ** The smooth factor of normals (if needed) */
  GLfloat normal_smooth;

  /* PRIVATE MEMBER
  ** The VAO, VBOs of this ball */
  GLuint vert_VBO, normal_VBO, uv_VBO, EBO;
};

/* The functions do computation
** Used in function `GLfloat getAltitude(const GLfloat&, const GLfloat&)`
** to get the altitude of terrain using given coordinates. */
static GLfloat genAltitudeCoord(const glm::vec3& vertex1,
                                const glm::vec3& vertex2,
                                const glm::vec3& vertex3,
                                const GLfloat& x, const GLfloat& z) {
  // Computes edge vectors and cross them
  glm::vec3 vec1_to_2 = vertex2 - vertex1;
  glm::vec3 vec1_to_3 = vertex3 - vertex1;
  glm::vec3 vec_cross = glm::cross(vec1_to_2, vec1_to_3);

  // Computes inner prodect
  GLfloat product = -glm::dot(vec_cross, vertex1);
  return -(vec_cross.x * x + vec_cross.z * z + product) / vec_cross.y;
}

#endif
