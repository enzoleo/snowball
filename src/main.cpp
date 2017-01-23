#include <iostream>
#include <time.h>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.h"
#include "texture.h"
#include "model.h"
#include "util.h"
#include "hmap_generator.h"
#include "terrain.h"
#include "particle_system.h"
#include "shadow_map.h"
#include "billboard.h"

#define FULL_SCREEN_MODE

#ifdef  _WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "SOIL.lib")
#pragma comment(lib, "assimp.lib")
#endif 

/* Function prototypes for moving the camera and changing viewing direction */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void move_func();

/* Function to do screen shot */
void screenshot()
{
    // Declare a string (the name of the screenshot)
    // ATTENTION: here the screenshots are always PNG files
    std::string filename = "../screenshots/screenshot" + std::to_string(num_screenshots) + ".png";

    // We need 4 bytes for RGBA pixels
    unsigned char* pixels = new unsigned char[window_width * window_height * 4];
    glReadPixels(0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    // ----------------------------------------------------------------------
    // Function `glReadPixels` reads the given rectangle from bottom-left to
    // top-right, which means that our screenshot is upside down. So we must
    // reverse the image pointer by hand.
    // Attention:
    //     For an image, the double-loop below may cost plenty of time. Thus
    //     remember to use it carefully.
    // ----------------------------------------------------------------------
    for (int y = 0; y < window_height / 2; y++)
    {
        const int swapY = window_height - y - 1;
        for (int x = 0; x < window_width; x++)
        {
            const int offset = 4 * (x + y * window_width);
            const int swapOffset = 4 * (x + swapY * window_width);

            // Swap R, G and B of the 2 pixels
            std::swap<unsigned char>(pixels[offset + 0], pixels[swapOffset + 2]);
            std::swap<unsigned char>(pixels[offset + 1], pixels[swapOffset + 1]);
            std::swap<unsigned char>(pixels[offset + 2], pixels[swapOffset + 0]);
            std::swap<unsigned char>(pixels[offset + 3], pixels[swapOffset + 3]);
        }
    }

    // Create a SDL surface by computed pointer
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        pixels, window_width, window_height, 32,
        window_width * 4, 0, 0, 0, 0
    );
    // Save screenshot surface as PNG file
    IMG_SavePNG(surface, filename.c_str());
    SDL_FreeSurface(surface);
    delete [] pixels;
}

/* GLOBAL: INITIATION
** The function to initiate GL */
void initGL()
{
    // GLFW initiation (VERSION declared)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE); 

    // ------------------------------------------------------------------------------
    // Get the screem width and height to open fullscreen mode
    // Create a new GLFW window (name it `Snowball Game`)
    // If you do not like fullscreen mode, undef the macro FULL_SCREEN_MODE
    // ------------------------------------------------------------------------------
#ifdef FULL_SCREEN_MODE
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    window_width = mode->width;
    window_height = mode->height;
    window = glfwCreateWindow(window_width, window_height, "SnowballRun", glfwGetPrimaryMonitor(), nullptr);
#else
    window_width = 800;
    window_height = 600;
    window = glfwCreateWindow(window_width, window_height, "SnowballRun", nullptr, nullptr);
#endif
    glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers
    // GLEW initiation is really important
    glewExperimental = GL_TRUE;
    glewInit();

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, window_width, window_height);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void initScene()
{
    // Set random seed for running particle system, setting barriers, etc.
    srand(time(0));  

    // Load textures
    texture_grass.reload("../assets/textures/grass.jpg", TEXTURE_JPG); texture_grass.setUnit(1);
    texture_mud.reload("../assets/textures/mud.jpg", TEXTURE_JPG); texture_mud.setUnit(2);
    texture_snow.reload("../assets/textures/snow.jpg", TEXTURE_JPG); texture_snow.setUnit(3);
    texture_snowflake.reload("../assets/textures/snowflake.png", TEXTURE_PNG); texture_snowflake.setUnit(4);
    texture_white.reload("../assets/textures/white.jpg", TEXTURE_JPG); texture_white.setUnit(5);
    texture_billboard.reload("../assets/textures/billboard.png", TEXTURE_PNG); texture_billboard.setUnit(6);
    texture_wood.reload("../assets/textures/container.jpg", TEXTURE_JPG); texture_wood.setUnit(7);
    texture_gameover.reload("../assets/textures/gameover.png", TEXTURE_PNG); texture_gameover.setUnit(8);
    texture_win.reload("../assets/textures/win.png", TEXTURE_PNG); texture_win.setUnit(9);
    texture_sbb.reload("../assets/textures/snowball_barrier.jpg", TEXTURE_JPG); texture_sbb.setUnit(10);

    // Load Shaders
    main_shader.reload("../assets/shaders/main.vert", "../assets/shaders/main.frag");
    main_shader.setFuncType(NORMAL);
    depth_shader.reload("../assets/shaders/shadow_mapping_depth.vert", "../assets/shaders/shadow_mapping_depth.frag");
    depth_shader.setFuncType(DEPTH);
    particle_shader.reload("../assets/shaders/particle_system.vert", "../assets/shaders/particle_system.frag");
    particle_shader.setFuncType(PARTICLE);
    billboard_shader.reload("../assets/shaders/billboard.vert", "../assets/shaders/billboard.frag");
    billboard_shader.setFuncType(BILLBOARD);
    go_shader.reload("../assets/shaders/start_over.vert", "../assets/shaders/start_over.frag");
    go_shader.setFuncType(BILLBOARD);
    win_shader.reload("../assets/shaders/start_over.vert", "../assets/shaders/start_over.frag");
    win_shader.setFuncType(BILLBOARD);

    // Set light
    lightDir = light0.getDirection();
    lightPos = snowball.getCurPosition() + glm::vec3(0.0f, 4.0f, 15.0f) - 20.0f * lightDir;
    light0.bindShader(main_shader);

    // Setup objects
    mini_terrain.setup();
    square.setup();
    path.setup();
    snowball.setup();
    barrier_ball.setup();
    barrier_cube.setup();

    // Load models
    grass.load_from_files("../assets/models/grass/grass.obj", "../assets/models/grass/grass.png");
    tree.load_from_file("../assets/models/tree/tree.3ds");
    snowhouse.load_from_file("../assets/models/snow_house/SnowCoveredCottageOBJ.obj");

    // Initialize particle system, shadow map and others
    ps = new ParticleSystem(particle_shader, texture_snowflake, glm::vec3(0, 30.0f, -2050), 1000, 50, 50);
    sm = new ShadowMap(shadow_map_width, shadow_map_height);
    billboard = new Billboard(billboard_shader, texture_billboard);
    gameover = new Billboard(go_shader, texture_gameover);
    winning = new Billboard(win_shader, texture_win);

    // Bind shadow map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sm->getDepthMap());
    main_shader.install();
    main_shader.setUniform1i("shadowMap", 0);
    main_shader.uninstall();

    // Bind snow map
    texture_white.bind(texture_white.getUnit());
    main_shader.install();
    main_shader.setUniform1i("snowMap", texture_white.getUnit());
    main_shader.uninstall();

    // Initialize transformation matrices for objects
    glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(0, 0, -100));
    {
        grassModelMatsA.reserve(num_grass);
        grassModelMatsB.reserve(num_grass);
        for (GLint i = 0; i < num_grass / 2; i++)
        {
            int temp = -10 * i;
            glm::mat4 model;
            model = glm::translate(glm::mat4(), glm::vec3(-7, 0, temp));
            grassModelMatsA.push_back(model);
            grassModelMatsB.push_back(trans * model);
            model = glm::translate(glm::mat4(), glm::vec3(7, 0, temp));
            grassModelMatsA.push_back(model);
            grassModelMatsB.push_back(trans * model);
        }

        treeModelMatsA.reserve(num_tree);
        treeModelMatsB.reserve(num_tree);
        for (GLint i = 0; i < num_tree / 2; i++)
        {
            int temp = -50 * i;
            glm::mat4 model;
            model = glm::translate(glm::mat4(), glm::vec3(-10, 0, temp));
#ifdef _WIN32
            model = glm::rotate(model, (GLfloat)M_PI_2, glm::vec3(-1.0f, 0.0f, 0.0f));
#else
            model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
#endif
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            treeModelMatsA.push_back(model);
            treeModelMatsB.push_back(trans * model);
            model = glm::translate(glm::mat4(), glm::vec3(10, 0, temp));
#ifdef _WIN32
            model = glm::rotate(model, (GLfloat)M_PI_2, glm::vec3(-1.0f, 0.0f, 0.0f));
#else
            model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
#endif
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            treeModelMatsA.push_back(model);
            treeModelMatsB.push_back(trans * model);
        }
    }
    // path
    {
        glm::mat4 model;
        model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -50.0f));
#ifdef _WIN32
        model = glm::rotate(model, (GLfloat)M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
#else
        model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); //the angle's unit is radian!!!
#endif        
        model = glm::scale(model, glm::vec3(200.0f, 100.0f, 1.0f));
        pathModelMatA = model;
        pathModelMatB = trans * model;
    }
    // terrains
    {
        terrainModelMatsA.reserve(num_terrain);
        terrainModelMatsB.reserve(num_terrain);
        glm::mat4 model;
        model = glm::translate(glm::mat4(), glm::vec3(-107.0f, 0.0f, -900.0f));  //left
        terrainModelMatsA.push_back(model);
        terrainModelMatsB.push_back(trans * model);
        model = glm::translate(glm::mat4(), glm::vec3(7.0f, 0.0f, -900.0f));  //right
        terrainModelMatsA.push_back(model);
        terrainModelMatsB.push_back(trans * model);
    }
    // snow house
    {
        glm::mat4 model;
        model = glm::translate(glm::mat4(), glm::vec3(20.0f, 0.0f, -3014.0f));
#ifdef _WIN32
        model = glm::rotate(model, (GLfloat)M_PI_2, glm::vec3(0, 1, 0));
#else
        model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(0, 1, 0));
#endif
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        snowhouse.setModelMatrix(model);
    }
    // barrier cube
    {
        glm::mat4 temp = glm::scale(glm::mat4(), glm::vec3(2, 2, 2));
        barrier_cube.setInitModelMatrix(temp);
    }

    // Initialize textures for objects
    pathTexA = &texture_grass;
    pathTexB = &texture_grass;
    snowball.setTexture(&texture_snow);
    barrier_ball.setTexture(&texture_sbb);
    barrier_cube.setTexture(&texture_wood);
    mini_terrain.setTexture(&texture_mud);
    
    // Initialize barriers
    barriers.setNumBarTypes(2);
    barriers.setBarrierObj(0, barrier_ball);
    barriers.setBarrierObj(1, barrier_cube);
}

/* update camera settings, light settings, objects settings, etc. */
void updateScene()
{
    if (dist_total > 900 && stageA == 1) //change scene A
    {
        stageA++;
        pathTexA = &texture_mud;
        drawPlantA = false;  
        drawTerrainA = true;
    }
    if (dist_total > 1000 && stageB == 1) //change scene B
    {
        stageB++;
        pathTexB = &texture_mud;
        drawPlantB = false;
        drawTerrainB = true;
    }
    if (dist_total > 1900 && stageA == 2)
    {
        stageA++;
        drawSnow = true;
    }
    if (dist_total > 2000 && stageB == 2)
    {
        stageB++;
        startMovePS = true;
    }
    if (dist_total > 2500 && drawTerrainA == true) 
    { 
        drawTerrainA = false;
    }
    if (dist_total > 2600 && drawTerrainB == true)
    {
        drawTerrainB = false;
    }
    if (dist_total > 2700) 
    {
        drawSnowHouse = true;
    }

    // Get the current position of the snow ball
    currentX = snowball.getCurPosition().x;
    currentY = snowball.getCurPosition().y;
    currentZ = snowball.getCurPosition().z;

    if (currentZ < -3000.0f)
    {
        bWin = true;
    }

    // Update camera
    if (game_process_flag)
    {
        glm::vec3 cameraPos(currentX, camera.getPosition().y, currentZ + 16.0f);
        camera.setPosition(cameraPos);
    }
    glm::mat4 projection = glm::perspective(camera.getFovy(), (float)window_width / (float)window_height, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMat();

    // Update light settings
    if (lightDir.x > 1) lightDir.x -= 0.05 * deltaTime;
    if (lightDir.x < -1) lightDir.x += 0.05 * deltaTime;
    light0.setDirection(lightDir);
    light0.bindShader(main_shader);

    // Re-compute the light position vector
    lightPos = snowball.getCurPosition() + glm::vec3(0.0f, 4.0f, 15.0f) - 20.0f * light0.getDirection();
    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
    lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f);
    lightView = glm::lookAt(lightPos,
                            snowball.getCurPosition() + glm::vec3(0.0f, 4.0f, 15.0f),
                            glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    // Set uniforms for our shaders
    main_shader.install();
    main_shader.setUniformMatrix4fv("projection", projection);
    main_shader.setUniformMatrix4fv("view", view);
    main_shader.setUniform3f("viewPos", camera.getPosition());
    main_shader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
    depth_shader.install();
    depth_shader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
    particle_shader.install();
    particle_shader.setUniformMatrix4fv("projection", projection);
    particle_shader.setUniformMatrix4fv("view", view);
    particle_shader.uninstall();

    // Update particle system
    if (drawSnow)
    {
        if (startMovePS) 
            ps->setGeneratorPos(glm::vec3(0, 30, currentZ));        
        ps->update(deltaTime);

        if (factor < 0.9f)
        { // For changing texture
            factor += 0.05 * deltaTime;
            main_shader.install();
            main_shader.setUniform1f("factor", factor);
            main_shader.uninstall();
        }
    }
    
    // Update our scene when dist_delta > 100
    if (dist_delta > 100)
    {
        glm::mat4 temp = glm::translate(glm::mat4(), glm::vec3(0, 0, -200));
        if (updateA)  // update Scene A
        {
            if (drawPlantA)
            {
                for (int i = 0; i < num_grass; ++i)
                    grassModelMatsA[i] = temp * grassModelMatsA[i];
                for (int i = 0; i < num_tree; ++i)
                    treeModelMatsA[i] = temp * treeModelMatsA[i];
            }
            if (drawTerrainA)
            {
                for (int i = 0; i < num_terrain; ++i)
                    terrainModelMatsA[i] = temp * terrainModelMatsA[i];
            }
            
            pathModelMatA = temp * pathModelMatA;
            updateA = !updateA;
        }
        else  //update Scene B 
        {
            if (drawPlantB)
            {
                for (int i = 0; i < num_grass; ++i)
                    grassModelMatsB[i] = temp * grassModelMatsB[i];
                for (int i = 0; i < num_tree; ++i)
                    treeModelMatsB[i] = temp * treeModelMatsB[i];
            }
            if (drawTerrainB)
            {
                for (int i = 0; i < num_terrain; ++i)
                    terrainModelMatsB[i] = temp * terrainModelMatsB[i];
            }
            
            pathModelMatB = temp * pathModelMatB;
            updateA = !updateA;
        }
        dist_delta -= 100;
    }

    // Update snowball and barriers
    // Increase rotation angle
    {
        GLfloat rotAngle = snowball.getRotAngle();
        GLfloat rotSpeed = snowball.getRotSpeed();
        snowball.setRotAngle(rotAngle + rotSpeed * deltaTime);
    }
    if (bTemp && game_process_flag)
    {
        GLfloat speed = snowball.getSpeed();
        GLfloat accelerator = snowball.getAccelerator();
        snowball.setPositionZ(currentZ - speed * deltaTime);
        snowball.setPositionY(snowball.getRadius());
        snowball.setSpeed(speed + accelerator * deltaTime);
        snowball.setRadius(snowball.getRadius() - snowball.getMeltSpeed() * deltaTime);

        offset_z = speed * deltaTime;
        dist_total += offset_z;
        dist_delta += offset_z;
    }
    else
    {
        offset_z = 0;
    }

    // Get barriers' parameters
    barrierDeque deque = barriers.getDeque();
    std::deque<GLuint> bar_types = barriers.getBarTypes();
    GLfloat spacing = barriers.getSpacing();
    GLfloat barrierPositionZ = -(score + 1) * spacing;

    // ------------------------------------------------------------------------------
    // The x-coordinate of safe position in each barrier row
    // We have to update deque data if the score exceeds half the size of
    // the original deque size (The size is fixed!)
    // ------------------------------------------------------------------------------
    GLfloat safePositionX;
    std::vector<GLfloat> foodPositionX; // size can be zero, one and two
    GLfloat left = 2, right = 2;  //no specific reason for choosing 2 
    {
        if (score < deque.size() / 2) {
            safePositionX = 3.0f * deque[score];
            if (deque[score] == -1) {
                left = 0; right = 1;
            }
            if (deque[score] == 0) {
                left = -1; right = 1;
            }
            if (deque[score] == 1) {
                left = -1; right = 0;
            }
            
            // check food
            if (bar_types[2 * score] == 0) foodPositionX.push_back(3.0f * left);
            if (bar_types[2 * score + 1] == 0) foodPositionX.push_back(3.0f * right);
        }
        else
        {
            GLuint temp = deque.size() / 2 - 1;  //important!
            safePositionX = 3.0f * deque[temp];
            if (deque[temp] == -1) {
                left = 0; right = 1;
            }
            if (deque[temp] == 0) {
                left = -1; right = 1;
            }
            if (deque[temp] == 1) {
                left = -1; right = 0;
            }

            // check food
            if (bar_types[deque.size() - 2] == 0) foodPositionX.push_back(3.0f * left);
            if (bar_types[deque.size() - 1] == 0) foodPositionX.push_back(3.0f * right);
        }
    }

    // Collision detection
    if (currentZ < barrierPositionZ + 3.0f &&
        currentZ > barrierPositionZ + 1.0f)
    {   
        if (foodPositionX.size() == 0) // No food!
        {
            if (currentX < safePositionX - 1.2f ||
                currentX > safePositionX + 1.2f ||
                snowball.getRadius() < 0.05f)
            {
                printf("Game over!\n");
                printf("Your score: %d\n", score);
                bTemp = !bTemp;
                bGameOver = !bGameOver;
            }
        }
        else if (foodPositionX.size() == 1)  // 1 food
        {
            //eat this food
            if (abs(currentX - foodPositionX[0]) < 1.0f) 
            {
                
                snowball.setRadius(snowball.getRadius() + 0.01);
            }
            else if (currentX < safePositionX - 1.2f ||
                currentX > safePositionX + 1.2f ||
                snowball.getRadius() < 0.05f)
            {
                printf("Game over!\n");
                printf("Your score: %d\n", score);
                bTemp = !bTemp;
                bGameOver = !bGameOver;
            }
        }
        else // 2 food
        {
            if (currentX < safePositionX - 1.2f ||
                currentX > safePositionX + 1.2f) 
            {
               
                snowball.setRadius(snowball.getRadius() + 0.01);
            }
        }

        // Bonus if passed one row
        score++;

        // No need to update queue if the score is two low
        if (score >= deque.size() / 2) {
           
            barriers.update();
        }
    }

    // Get baseline and rotation angle of barriers
    GLfloat baseline = barriers.getBaseline();
    GLfloat rotSpeed = barriers.getRotSpeed();

    // Increase rotation angle
    barriers.setRotSpeed(rotSpeed + 0.01f);

    // Update billboard(life)
    float LifeLevel = snowball.getRadius();
    billboard_shader.install();
    billboard_shader.setUniform1f("LifeLevel", LifeLevel);
    billboard_shader.setUniform1f("billboardLen", 0.6f);
    billboard_shader.setUniform1f("billboardWidth", 0.075f);
    billboard_shader.uninstall();

    go_shader.install();
    go_shader.setUniform1f("billboardLen", 0.001f * gameover->getWidth());
    go_shader.setUniform1f("billboardWidth", 0.001f * gameover->getHeight());
    go_shader.uninstall();

    win_shader.install();
    win_shader.setUniform1f("billboardLen", 0.001f * winning->getWidth());
    win_shader.setUniform1f("billboardWidth", 0.001f * winning->getHeight());
    win_shader.uninstall();
}

// Note that renderScene() will be called for every iteration! 
// One for depth buffer(shadow map), one for color buffer(display screen)
// So do not update any parameters in this function!
void renderScene(Shader shader)
{   
    glm::mat4 model;

    // render scene A
    {
        if (drawPlantA) {
            for (int i = 0; i < num_grass; ++i) {
                grass.setModelMatrix(grassModelMatsA[i]);
                grass.draw(shader);
            }

            for (int i = 0; i < num_tree; ++i) {
                tree.setModelMatrix(treeModelMatsA[i]);
                tree.draw(shader);
            }
        }
        if (drawTerrainA) {
            for (int i = 0; i < num_terrain; ++i) {
                mini_terrain.setModelMatrix(terrainModelMatsA[i]);
                mini_terrain.draw(shader);
            }
        }

        path.setModelMatrix(pathModelMatA);
        path.setTexture(pathTexA);
        path.draw(shader);
    }

    // render scene B
    {
        if (drawPlantB) {
            for (int i = 0; i < num_grass; ++i) {
                grass.setModelMatrix(grassModelMatsB[i]);
                grass.draw(shader);
            }

            for (int i = 0; i < num_tree; ++i) {
                tree.setModelMatrix(treeModelMatsB[i]);
                tree.draw(shader);
            }
        }
        if (drawTerrainB) {
            for (int i = 0; i < num_terrain; ++i) {
                mini_terrain.setModelMatrix(terrainModelMatsB[i]);
                mini_terrain.draw(shader);
            }
        }

        path.setModelMatrix(pathModelMatB);
        path.setTexture(pathTexB);
        path.draw(shader);
    }

    if (drawSnowHouse) {
        snowhouse.draw(shader);
    }

    // Draw snowball
    GLfloat radius = snowball.getRadius();
    model = glm::translate(glm::mat4(), glm::vec3(currentX, currentY, currentZ));
#ifdef _WIN32
    model = glm::rotate(model, deg2rad(snowball.getRotAngle()), glm::vec3(-1.0f, 0.0f, 0.0f));
#else
    model = glm::rotate(model, snowball.getRotAngle(), glm::vec3(-1.0f, 0.0f, 0.0f));
#endif
    model = glm::scale(model, glm::vec3(radius, radius, radius));
    snowball.setModelMatrix(model);
    snowball.draw(shader);
    barriers.draw(shader);

    // Particle System
    if (drawSnow) {
        ps->draw(camera, texture_snowflake.getUnit());
    }
    

    // Billboard
    if (shader.getFuncType() != DEPTH)
    {
        float lifeLevel = snowball.getRadius();
        billboard_shader.install();
        billboard_shader.setUniform1f("lifeLevel", lifeLevel);
        billboard_shader.uninstall();
        billboard->draw(camera, texture_billboard.getUnit());

        if(bGameOver)
            gameover->draw(camera, texture_gameover.getUnit());
        else if (bWin)
            winning->draw(camera, texture_win.getUnit());
    }
}

int main()
{
    initGL();
    initScene();
    
    lastFrame = glfwGetTime();
    lastTime = glfwGetTime();
    GLfloat camera_start_pos = snowball.getCurPosition().z + 16.0f;
    
    // --------------------------------------------------------------
    // START ANIMATION
    // Really simple animation - move camera.
    // --------------------------------------------------------------
    while (!glfwWindowShouldClose(window) && !game_process_flag)
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        num_frames++;
        GLfloat dt = currentFrame - lastTime;

        // Check and call events
        glfwPollEvents();
        updateScene();
        sm->bind();
        renderScene(depth_shader);
        sm->unbind();

        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        renderScene(main_shader);

        game_process_flag = camera.move(
            glm::vec3(0.0f, 5.0f, camera_start_pos),
            -90.0f, 0.0f, 0.1f, 0.1f, dt);

        // This line must be here! Or the screen will flash!
        glfwSwapBuffers(window);
    }

    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        num_frames++;
        GLfloat dt = currentFrame - lastTime;

        // Check and call events
        glfwPollEvents();
        move_func(); 
        updateScene();

        sm->bind();
        renderScene(depth_shader);
        sm->unbind();

        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        renderScene(main_shader);

        // This line must be here! Or the screen will flash!
        glfwSwapBuffers(window);
    }

    PerlinNoiseGenerator pg;
    pg.generate("ok.png");

    glfwTerminate();
    return 0;
}

/* Moves/alters the camera positions based on user input */
void move_func()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.translate(deltaTime, CAMERA_FORWARD);
    if (keys[GLFW_KEY_S])
        camera.translate(deltaTime, CAMERA_BACKWARD);
    if (keys[GLFW_KEY_A])
        camera.translate(deltaTime, CAMERA_LEFT);
    if (keys[GLFW_KEY_D])
        camera.translate(deltaTime, CAMERA_RIGHT);
    if (keys[GLFW_KEY_LEFT])
        if (snowball.getCurPosition().x > -3.0f)
            snowball.setPositionX(snowball.getCurPosition().x - 0.2f);
    if (keys[GLFW_KEY_RIGHT])
        if (snowball.getCurPosition().x < 3.0f)
            snowball.setPositionX(snowball.getCurPosition().x + 0.2f);
}

/* Is called whenever a key is pressed/released via GLFW */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        bTemp = !bTemp;
        if (bGameOver)
            bGameOver = !bGameOver;
    }

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        screenshot();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.rotate(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.zoom(yoffset);
}
