/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#include <iostream>
#include "global.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

//Shader shader;
/* initialise glfw and glew 
** and gl settings */
int initGL()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(screenWidth, screenHeight, "Snowball Game", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide our mouse in the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glViewport(0, 0, screenWidth, screenHeight);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

/* initialise our scene, including
** texture loading,
** model loading, 
** and shader loading */
void initScene()
{
    //Load textures
    texture_grass.reload("textures/grass.jpg");
    texture_mud.reload("textures/mud.jpg");
    texture_snow.reload("textures/snow.jpg");
    snowball.loadTexture("textures/snow.jpg");

    shader.reload("./shaders/shader.vert", "./shaders/shader.frag");
	dirlight0.bindShader(shader);
    
    proj_loc = glGetUniformLocation(shader.getProgram(), "projection");
    view_loc = glGetUniformLocation(shader.getProgram(), "view");
    model_loc = glGetUniformLocation(shader.getProgram(), "model");
    texture_loc = glGetUniformLocation(shader.getProgram(), "material.diffuse");
}

/* The function to draw barriers */
void drawBarriers()
{
    // Get baseline and rotation angle of barriers
    GLfloat baseline = barriers.getBaseline();
    GLfloat rotSpeed = barriers.getRotSpeed();

    // Notice that our model is a simple sphere
    //cur_model = glm::translate(cur_model, glm::vec3(0.0f, baseline, 0.0f));
    //cur_model = glm::rotate(cur_model, rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    //cur_model = glm::scale(cur_model, glm::vec3(1.0f, 1.0f, 1.0f));
    //glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(cur_model));
    barrier_ball.draw(shader);

    if (barriersUp) barriers.setBaseline(baseline - 0.0005f);
    else barriers.setBaseline(baseline + 0.0005f);

    if (baseline < 1.0f) barriersUp = false;
    else if (baseline > 2.0f) barriersUp = true;

    // Increase rotation angle
    barriers.setRotSpeed(rotSpeed + 0.01f);
}

void renderScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the current position of the snow ball
    GLfloat currentX = snowball.getCurPosition().x;
    GLfloat currentY = snowball.getCurPosition().y;
    GLfloat currentZ = snowball.getCurPosition().z;

    glm::vec3 cameraPos(currentX, 5.0f, currentZ + 16.0f);
    camera.setPosition(cameraPos);

    Square pathSquare(path_buffer);

    glm::mat4 projection = glm::perspective(camera.getFovy(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMat();
    glm::mat4 model;
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.install();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_mud.getID());
    glUniform1i(texture_loc, 0);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    pathSquare.draw(shader);

    shader.install();
    GLdouble radius = snowball.getRadius();
    model = glm::translate(glm::mat4(), glm::vec3(currentX, currentY, currentZ));
    model = glm::rotate(model, snowball.getRotAngle(), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(radius, radius, radius));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_snow.getID());
    glUniform1i(texture_loc, 1);
    snowball.bindBuffer();
    snowball.draw(shader);

    barrier_ball.bindBuffer();

    // Increase rotation speed
    GLfloat rotAngle = snowball.getRotAngle();
    GLfloat rotSpeed = snowball.getRotSpeed();
    snowball.setRotAngle(rotAngle + rotSpeed * deltaTime);

    //-----------------------------------------------------------------------------------

    if (bTemp)
    {
        GLfloat speed = snowball.getSpeed();
        GLfloat accelerator = snowball.getAccelerator();
        snowball.setPositionZ(currentZ - speed * deltaTime);
        snowball.setSpeed(speed + accelerator * deltaTime);
        snowball.setRadius(snowball.getRadius() - snowball.getMeltSpeed() * deltaTime);
    }

    // Get barriers' parameters
    barrierDeque deque = barriers.getDeque();
    GLfloat spacing = barriers.getSpacing();
    GLfloat barrierPositionZ = -(score + 1) * spacing;

    /* The x-coordinate of safe position in each barrier row
    ** We have to update deque data if the score exceeds half the size of
    ** the original deque size (The size is fixed!) */
    GLfloat safePositionX;
    if (score < deque.size() / 2)
        safePositionX = 3.0f * deque[score];
    else safePositionX = 3.0f * deque[deque.size() / 2];

    // This parameter is used in the deque traversal
    // @param barrierLoop records the current z-coordinate of the barrier rows
    GLfloat barrierLoop;
    if (score >= deque.size() / 2)
        barrierLoop = barrierPositionZ + (deque.size() / 2) * spacing;
    else barrierLoop = -spacing;

    // Collision detection
    if (currentZ < barrierPositionZ + 1.0f &&
        currentZ > barrierPositionZ - 1.0f)
    {
        // No need to update queue if the score is two low
        if (score >= deque.size() / 2)
            barriers.updateDeque();

        // Collision between snowball and one barrier
        if (currentX < safePositionX - 1.2f ||
            currentX > safePositionX + 1.2f ||
            snowball.getRadius() < 0.05f)
        {
            printf("Game over!\n");
            printf("Your score: %d\n", score);
            bTemp = !bTemp;
        }

        // Bonus if passed one row
        score++;
    }

    // Declare an iterator traverse in deque (NO iterator in std::queue)
    // You can also do traversal in array form. Learn about C++ Verision >= 11
    barrierDeque::iterator iter = deque.begin();
    for (; iter != deque.end(); iter++)
    {
        for (int pos = -1; pos < 2; pos++)
        {
            if (*iter != pos)
            {
                shader.install();

                // Draw barrier at each unsafe position
                GLdouble barrier_rad = barrier_ball.getRadius();
                model = glm::translate(glm::mat4(), glm::vec3(pos * 3.0f, 0.0f, barrierLoop));
                model = glm::translate(model, glm::vec3(0.0f, barriers.getBaseline(), 0.0f));
                model = glm::rotate(model, barriers.getRotSpeed(), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(barrier_rad, barrier_rad, barrier_rad));
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
                drawBarriers();
            }
        }
        barrierLoop -= spacing;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(void)
{
    initGL();
    initScene();
    barriers.initDeque();

    while (!glfwWindowShouldClose(window)) 
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        renderScene();
    }

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Get the current position of the snow ball
    GLfloat currentX = snowball.getCurPosition().x;
    GLfloat currentZ = snowball.getCurPosition().z;

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

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        bTemp = !bTemp;

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
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

#pragma endregion
