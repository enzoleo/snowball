/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "objects.h"
#include "model.h"
#include "camera.h"
#include "light.h"

/*****************************************************************
** The foundamental texture declaration
** @param texture_grass: The ground texture rendering grassland
** @param texture_sky: The sky texture rendering bluesky
** ---------------------------------------------------------------
** You can change these textures by yourself.
** Remember put your texture images into `textures` directory! 
******************************************************************/

//class Model;  //forward declaration
//class Camera;
//class Shader;

GLFWwindow* window;

Texture texture_grass;
Texture texture_sky;
Texture texture_mud;
Texture texture_snow;


/* Construct a snow ball object */
SnowBall snowball(0.6f, 20, 20);
// Ball and model declaration
Ball barrier_ball(1.0f, 20, 20);

/* Default color using namespace */
namespace Color
{
    // Basic color defined by RGBA value
    const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    const float pureGold[] = { 0.85f, 0.96f, 0.15f, 1.0f };
    const float rostyBrown[] = { 0.75f, 0.56f, 0.56f, 1.0f };
    const float darkGray[] = { 0.20f, 0.20f, 0.20f, 1.0f };
    const float blueViolet[] = { 0.54f, 0.17f, 0.89f, 1.0f };
    const float steelBlue[] = { 0.27f, 0.51f, 0.71f, 1.0f };
    const float fireBrick[] = { 0.70f, 0.13f, 0.13f, 1.0f };
} // NAMESPACE END

/* The global variables of parallel light */
float lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightDirection[] = { 1.0f, 1.0f, 1.0f, 0.0f };

/* Spot light class definition */
class Spotlight
{
public:
    Spotlight() : angle(45.0f)
    { // Constructor
        color = new float[4];
        position = new float[4];
        direction = new float[3];
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
        position[0] = 0.0f; position[1] = 0.0f; position[2] = 2.0f; position[3] = 1.0f;
        direction[0] = 0.0f; direction[1] = 0.0f; direction[2] = 1.0f;
    }

    ~Spotlight()
    { // Destructor
        delete[] color;
        delete[] position;
        delete[] direction;
    }

    // The color of spotlight
    float* color;
    // The position of spotlight
    float* position;
    // The direction of spotlight
    float* direction;
    // The light angle of spotlight
    float angle;
};

// Add a spotlight to the scene
Spotlight spotlight;
// Add barriers to the scene
Barriers barriers;

int score = 0;

bool barriersUp = false;

float deltaMove = 0;
bool bTemp = true;

bool update_deque_waiting_flag = false;
bool update_deque_processing_flag = false;

GLfloat speed = 0.001f;

Model grass;

//Camera camera(glm::vec3(0,0,5));
Camera camera(
    0.0f, 5.0f, 20.0f,
    0.0f, 1.0f, 0.0f,
    -90.0f, 6.0f
);
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Shader shader;
GLuint screenWidth = 800, screenHeight = 600;

GLuint proj_loc;
GLuint view_loc;
GLuint model_loc;
GLuint texture_loc;

DirLight dirlight0;

// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_vertex_buffer_cube[] = {
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

// Two UV coordinatesfor each vertex. They were created withe Blender.
static const GLfloat g_uv_buffer_cube[] = {
    0.000059f, 1.0f - 0.000004f,
    0.000103f, 1.0f - 0.336048f,
    0.335973f, 1.0f - 0.335903f,
    1.000023f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.336024f, 1.0f - 0.671877f,
    0.667969f, 1.0f - 0.671889f,
    1.000023f, 1.0f - 0.000013f,
    0.668104f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.000059f, 1.0f - 0.000004f,
    0.335973f, 1.0f - 0.335903f,
    0.336098f, 1.0f - 0.000071f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.336024f, 1.0f - 0.671877f,
    1.000004f, 1.0f - 0.671847f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.668104f, 1.0f - 0.000013f,
    0.335973f, 1.0f - 0.335903f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.668104f, 1.0f - 0.000013f,
    0.336098f, 1.0f - 0.000071f,
    0.000103f, 1.0f - 0.336048f,
    0.000004f, 1.0f - 0.671870f,
    0.336024f, 1.0f - 0.671877f,
    0.000103f, 1.0f - 0.336048f,
    0.336024f, 1.0f - 0.671877f,
    0.335973f, 1.0f - 0.335903f,
    0.667969f, 1.0f - 0.671889f,
    1.000004f, 1.0f - 0.671847f,
    0.667979f, 1.0f - 0.335851f
};

//this cube center at (0,0,0), the length of each edge is 2
class Cube
{
public:
    GLuint VAO;
    GLuint VBO[2];

    Cube()
    {
        glGenVertexArrays(1, &VAO);  //must initialise GLEW first!
        glGenBuffers(2, &VBO[0]);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_cube), g_vertex_buffer_cube, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_cube), g_uv_buffer_cube, GL_STATIC_DRAW);
        
        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
        
        // Vertex UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
        
        glBindVertexArray(0);   
    }

    void Draw(Shader shader) {
        shader.install();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles
        glBindVertexArray(0);
    }

};

static GLfloat path_buffer[] = {
    // Positions                    // Texture Coords
    300.0f, 0.0f, 10000.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top Right
    300.0f, 0.0f, -10000.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
    -300.0f, 0.0f, -10000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
    -300.0f, 0.0f, 10000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // Top Left 
};

static GLuint g_indices[] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

class Square
{
public:
    GLuint VAO, VBO, EBO;
    GLfloat* buffer;
    
    Square(GLfloat *vert_buffer)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 128, vert_buffer, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

        // TexCoord attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

        glBindVertexArray(0); // Unbind VAO
    }

    void draw(Shader shader) {
        shader.install();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader.uninstall();
    }
};

#endif
