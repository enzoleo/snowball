#include <iostream>
#include <time.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "model.h"
#include "global.h"
#include "hmap_generator.h"
#include "terrain.h"
#include "particle_system.h"
#include "shadow_map.h"
#include "billboard.h"

/* Function prototypes for moving the camera and changing viewing direction */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void move_func();

/* Function to do screen shot */
void screenshot()
{
    // Declare a string (the name of the screenshot)
    // ATTENTION: here the screenshots are always BMP files
    std::string filename = "./screenshots/screenshot" + std::to_string(num_screenshots) + ".bmp";

    // Use SOIL library to export image file
    GLint save_result = SOIL_save_screenshot
    (
        filename.c_str(),
        SOIL_SAVE_TYPE_BMP,
        0, 0,
        screen_width,
        screen_height
    );
    num_screenshots++;
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

    // Create a new GLFW window (name it `Snowball Game`)
    window = glfwCreateWindow(screen_width, screen_height, "Snowball Game", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers
    // GLEW initiation is really important
    glewExperimental = GL_TRUE;
    glewInit();

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide our mouse in the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, screen_width, screen_height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void initScene()
{
    // Set random seed for running particle system, setting barriers, etc.
    srand(time(0));  

    // Load textures
    texture_grass.reload("textures/grass.jpg"); texture_grass.setUnit(1);
    texture_rock.reload("textures/rock.jpg"); texture_rock.setUnit(2);
    texture_mud.reload("textures/mud.jpg"); texture_mud.setUnit(3);
    texture_snow.reload("textures/snow.jpg"); texture_snow.setUnit(4);
    texture_snowflake.reload("textures/snowflake.png"); texture_snowflake.setUnit(5);
    texture_white.reload("textures/white.jpg"); texture_white.setUnit(6);
    texture_billboard.reload("textures/billboard.png"); texture_billboard.setUnit(7);
    texture_wood.reload("textures/wood.jpg"); texture_wood.setUnit(8);
    texture_gameover.reload("textures/gameover.jpg"); texture_gameover.setUnit(9);
    texture_win.reload("textures/win.png"); texture_win.setUnit(10);
    texture_sbb.reload("textures/snowball_barrier.jpg"); texture_sbb.setUnit(11);

    // Load Shaders
    main_shader.reload("./shaders/main.vs", "./shaders/main.frag");
    main_shader.setFuncType(NORMAL);
    depth_shader.reload("./shaders/shadow_mapping_depth.vs", "./shaders/shadow_mapping_depth.frag");
    depth_shader.setFuncType(DEPTH);
    particle_shader.reload("./shaders/particle_system.vs", "./shaders/particle_system.frag");
    particle_shader.setFuncType(PARTICLE);
    debug_depth_shader.reload("./shaders/debug_quad_depth.vs", "./shaders/debug_quad_depth.frag");
    debug_depth_shader.setFuncType(DEBUG);
    billboard_shader.reload("./shaders/billboard.vert", "./shaders/billboard.frag");
    billboard_shader.setFuncType(BILLBOARD);
    go_shader.reload("./shaders/start_over.vert", "./shaders/start_over.frag");
    go_shader.setFuncType(BILLBOARD);
    win_shader.reload("./shaders/start_over.vert", "./shaders/start_over.frag");
    win_shader.setFuncType(BILLBOARD);


    // Set light
    calcLightPos(snowball.getCurPosition() + glm::vec3(0.0f, 4.0f, 15.0f), dirlight0.getDirection(), 20, lightPos);
    lightDir = dirlight0.getDirection();
    dirlight0.bindShader(main_shader);

    // Setup objects
    mini_terrain.setup();
    square.setup();
    path.setup();
    //cube.setup();
    //ball.setup();
    snowball.setup();
    barrier_ball.setup();
    barrier_cube.setup();
    //prism3.setup();

    // Load models
    grass.loadFromFiles("./models/grass/grass.obj", "./models/grass/grass.png");
    tree.loadFromFile("./models/tree/tree.3ds");
    snowhouse.loadFromFile("./models/snow_house/Snow covered CottageOBJ.obj");
    //stone.loadFromFile("./models/stone/STONE.3ds");

    // Initialize particle system
    ps = new ParticleSystem(particle_shader, texture_snowflake, glm::vec3(0, 30.0f, -2050), 1000, 50, 50);
    // Initialize shadow map
    sm = new ShadowMap(shadow_map_width, shadow_map_height);  //not need to be screen_width, screen_height
    // Initialize billboard
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
    // plants(grass, trees)
    {
        grassModelMatsA.reserve(num_grass);
        grassModelMatsB.reserve(num_grass);
        for (GLint i = 0; i < num_grass / 2; ++i)
        {
            int temp = -10 * i;
            glm::mat4 model;
            model = glm::translate(glm::mat4(), glm::vec3(-7, 0, temp));  //left
            grassModelMatsA.push_back(model);
            grassModelMatsB.push_back(trans * model);
            model = glm::translate(glm::mat4(), glm::vec3(7, 0, temp));  //right
            grassModelMatsA.push_back(model);
            grassModelMatsB.push_back(trans * model);
        }

        treeModelMatsA.reserve(num_tree);
        treeModelMatsB.reserve(num_tree);
        for (GLint i = 0; i < num_tree / 2; ++i) {
            int temp = -50 * i;
            glm::mat4 model;
            model = glm::translate(glm::mat4(), glm::vec3(-10, 0, temp));  //left
            model = glm::rotate(model, (float)90.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            treeModelMatsA.push_back(model);
            treeModelMatsB.push_back(trans * model);
            model = glm::translate(glm::mat4(), glm::vec3(10, 0, temp));  //right
            model = glm::rotate(model, (float)90.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            treeModelMatsA.push_back(model);
            treeModelMatsB.push_back(trans * model);
        }
    }
    // path
    {
        glm::mat4 model;
        model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -50.0f));
        model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); //the angle's unit is radian!!!
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
        model = glm::rotate(model, (GLfloat)90.0f, glm::vec3(0, 1, 0));
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
    // Update stage
    // stage 1: grass land
    // stage 2: desert
    // stage 3: iceworld
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

    //std::cout << "current z: " << currentZ << std::endl;

    // Update camera
    glm::vec3 cameraPos(currentX, 5.0f, currentZ + 16.0f);
    camera.setPosition(cameraPos);
    glm::mat4 projection = glm::perspective(camera.getFovy(), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMat();

    // Update light
    if (lightDir.x > 1) lightDir.x -= 0.05 * deltaTime;
    if (lightDir.x < -1) lightDir.x += 0.05 * deltaTime;
    dirlight0.setDirection(lightDir);
    dirlight0.bindShader(main_shader);
    glm::vec3 target = snowball.getCurPosition() + glm::vec3(0.0f, 4.0f, 15.0f);
    calcLightPos(target, dirlight0.getDirection(), 20, lightPos);
    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
    lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f);
    lightView = glm::lookAt(lightPos, target, glm::vec3(0.0, 1.0, 0.0));
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
    if (drawSnow) {
        if (startMovePS) {
            ps->setGeneratorPos(glm::vec3(0, 30, currentZ));
        }
        
        ps->update(deltaTime);

        // For changing texture
        if (factor < 0.9f) {
            factor += 0.05 * deltaTime;
            main_shader.install();
            main_shader.setUniform1f("factor", factor);
            main_shader.uninstall();
        }
    }
    

    // Update our scene when dist_delta > 100
    if (dist_delta > 100) {
        glm::mat4 temp = glm::translate(glm::mat4(), glm::vec3(0, 0, -200));
        if (updateA)  // update Scene A
        {
            //std::cout << "update scene A " << std::endl;
            if (drawPlantA) {
                for (int i = 0; i < num_grass; ++i) {
                    grassModelMatsA[i] = temp * grassModelMatsA[i];
                }
                for (int i = 0; i < num_tree; ++i) {
                    treeModelMatsA[i] = temp * treeModelMatsA[i];
                }
            }
            if (drawTerrainA) {
                for (int i = 0; i < num_terrain; ++i) {
                    terrainModelMatsA[i] = temp * terrainModelMatsA[i];
                }
            }
            
            pathModelMatA = temp * pathModelMatA;
            updateA = !updateA;
        }
        else  //update Scene B 
        {
            //std::cout << "update scene B " << std::endl;
            if (drawPlantB) {
                for (int i = 0; i < num_grass; ++i) {
                    grassModelMatsB[i] = temp * grassModelMatsB[i];
                }
                for (int i = 0; i < num_tree; ++i) {
                    treeModelMatsB[i] = temp * treeModelMatsB[i];
                }
            }
            if (drawTerrainB) {
                for (int i = 0; i < num_terrain; ++i) {
                    terrainModelMatsB[i] = temp * terrainModelMatsB[i];
                }
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
        snowball.setRotAngle(rotAngle + rotSpeed * deltaTime);  //update
    }
    if (bTemp)
    {
        GLfloat speed = snowball.getSpeed();
        GLfloat accelerator = snowball.getAccelerator();
        snowball.setPositionZ(currentZ - speed * deltaTime);  //update
        snowball.setPositionY(snowball.getRadius());
        snowball.setSpeed(speed + accelerator * deltaTime);  //update
        snowball.setRadius(snowball.getRadius() - snowball.getMeltSpeed() * deltaTime);  //update

        offset_z = speed * deltaTime;
        dist_total += offset_z;
        dist_delta += offset_z;

        // For debugging
        //std::cout << "total distance = " << dist_total << std::endl;
        //std::cout << "delta distance = " << dist_delta << std::endl;
    }
    else {
        offset_z = 0;
    }

    // Get barriers' parameters
    barrierDeque deque = barriers.getDeque();
    std::deque<GLuint> bar_types = barriers.getBarTypes();
    GLfloat spacing = barriers.getSpacing();
    GLfloat barrierPositionZ = -(score + 1) * spacing;

    /* The x-coordinate of safe position in each barrier row
    ** We have to update deque data if the score exceeds half the size of
    ** the original deque size (The size is fixed!) */
    GLfloat safePositionX;
    std::vector<GLfloat> foodPositionX; // size can be zero, one and two
    GLfloat left = 2, right = 2;  //no specific reason for choosing 2 
    //if (currentZ < barrierPositionZ + 1.0f &&
    //  currentZ > barrierPositionZ - 1.0f) 
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
                std::cout << "Eat Food! (1 food)" << std::endl;
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
                std::cout << "Eat Food! (2 food) " << std::endl;
                snowball.setRadius(snowball.getRadius() + 0.01);
            }
        }

        // Bonus if passed one row
        score++;

        // No need to update queue if the score is two low
        if (score >= deque.size() / 2) {
            std::cout << "update when score is " << score << std::endl;
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
    billboard_shader.uninstall();

    go_shader.install();
    go_shader.setUniform1f("billboardLen", 0.6f);
    go_shader.setUniform1f("billboardWidth", 0.3375f);
    go_shader.uninstall();

    win_shader.install();
    win_shader.setUniform1f("billboardLen", 0.6f);
    win_shader.setUniform1f("billboardWidth", 0.6f);
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
    model = glm::rotate(model, snowball.getRotAngle(), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(radius, radius, radius));
    snowball.setModelMatrix(model);
    snowball.draw(shader);
    barriers.draw(shader);

    // Particle System
    if (drawSnow) {
        ps->draw(camera, texture_snowflake.getUnit());
    }
    

    // Billboard
    if (shader.getFuncType() != DEPTH) {
        float billboardLen = 0.6f;
        //float lifeLevel = sin(glfwGetTime())*0.5f + 0.5f;
        float lifeLevel = snowball.getRadius();
        billboard_shader.install();
        billboard_shader.setUniform1f("lifeLevel", lifeLevel);
        billboard_shader.setUniform1f("billboardLen", billboardLen);
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

        sm->Bind();
        renderScene(depth_shader);
        sm->Unbind();

        glViewport(0, 0, screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        renderScene(main_shader);

        // This line must be here! Or the screen will flash!
        glfwSwapBuffers(window);
    }

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
