#pragma once
#include <GL/glew.h>
#include "objects.h"
#include "light.hpp"
#include "terrain.h"

/* forward declaration */
class Camera;
class Texture;
class Model;
class ParticleSystem;
class ShadowMap;
class Billboard;

// Window
GLFWwindow* window;
GLuint window_width, window_height;

// Shaders
Shader main_shader;
Shader particle_shader;
Shader depth_shader;
Shader debug_depth_shader;
Shader billboard_shader;
Shader go_shader;
Shader win_shader;

// Camera
/*Camera camera(
    glm::vec3(0.0f, 5.0f, 20.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -90.0f, 0.0f
);*/

Camera camera(
    glm::vec3(0.0f, 15.0f, 25.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -90.0f, 37.0f
);


// Key, mouse callbacks related 
bool keys[1024];
GLfloat lastX = window_width * 0.5, lastY = window_height * 0.5;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


// For calculating ms/frame, fps
int num_frames = 0;
GLfloat lastTime = 0.0f;
GLboolean bGameOver = false;
GLboolean bWin = false;
GLboolean game_process_flag = false;


// Textures
Texture texture_grass;
Texture texture_mud;
Texture texture_sky;
Texture texture_earth;
Texture texture_snow;
Texture texture_snowflake;
Texture texture_red;
Texture texture_white;
Texture texture_desert;
Texture texture_sbb;
Texture texture_billboard;
Texture texture_rock;
Texture texture_wood;
Texture texture_gameover;
Texture texture_win;

// Primitives, Models, etc.
Cube cube(0.7, 0.1, 30);
Square square(0.5, 0.1, 30);
Square path(0.9, 0.0, 10);
Ball ball(1.0, 40, 40, 0.0, 0.7, 50);
Model grass;
Model wood;
Model tree;
Model house;
Model snowhouse;
Model stone;
SnowBall snowball(1.0f, 20, 20, 1.0, 0.0, 10);
Ball barrier_ball(1.0f, 20, 20, 1.0, 0.0, 10);
Cube barrier_cube(1.0, 0.0, 10);

// Light
Light light0;
glm::vec3 lightPos;
glm::vec3 lightDir;


// Game related
Barriers barriers;

int score = 0;

bool barriersUp = false;

float deltaMove = 0;

bool bTemp = true;

bool update_deque_waiting_flag = false;
bool update_deque_processing_flag = false;

GLfloat speed = 0.001f;
GLfloat currentX, currentY, currentZ;

// For moving the generator's position 
GLfloat offset_z = 0;  


// Terrains
Terrain mini_terrain("../assets/terrains/test.bmp");


// Particle system
ParticleSystem *ps;


// Shadow Map
ShadowMap *sm;
const GLuint shadow_map_width = 1024, shadow_map_height = 1024;


// Billboard
Billboard* billboard;
Billboard* gameover;
Billboard* winning;

// Screenshot
int num_screenshots = 0;


// For texture changing, see main.frag 
//GLfloat pastTime = 0.0f;
GLfloat factor = 0.0f;


// For changing scenes and stages
GLfloat dist_total = 0.0f;
GLfloat dist_delta = 0.0f;  // dist_delta < 100

// For transforming models like grass, trees, etc.
GLuint num_grass = 20;
std::vector<glm::mat4> grassModelMatsA;  //transformation matrices for part A
std::vector<glm::mat4> grassModelMatsB;  //transformation matrices for part B

GLuint num_tree = 4;
std::vector<glm::mat4> treeModelMatsA;
std::vector<glm::mat4> treeModelMatsB;

glm::mat4 pathModelMatA;
glm::mat4 pathModelMatB;

Texture* pathTexA;
Texture* pathTexB;

GLuint num_terrain = 2;
std::vector<glm::mat4> terrainModelMatsA;
std::vector<glm::mat4> terrainModelMatsB;


bool updateA = true;

bool drawPlantA = true;
bool drawPlantB = true;
bool drawTerrainA = false;
bool drawTerrainB = false;
bool drawSnow = false;
bool drawSnowHouse = false;

bool startMovePS = false;

GLuint stageA = 1;
GLuint stageB = 1;
