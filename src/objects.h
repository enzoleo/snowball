/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <math.h>
#include <time.h>
#include <deque>
#include <vector>
#include "texture.h"
#include "shader.h"

/* BASE CLASS: Ball */
class Ball
{
public:

    /* Default constructor & constructor*/
    Ball(GLdouble _radius = 0,
         GLint _slices = 40,
         GLint _stacks = 40)
        : radius(_radius), 
          slices(_slices), 
          stacks(_stacks)
    {
        genVertCord();
        genUVCord();
    }

    /* Return private members
    ** Read the value of the @radius, @slices and @stacks */
    const GLdouble getRadius() { return radius; }
    const GLint getSlices() { return slices; }
    const GLint getStacks() { return stacks; }

    /* Reset some private members
    ** Regenerate the coordinates of vertices if you reset these members */
    void setRadius(const GLfloat _radius) { radius = _radius; genVertCord(); genUVCord(); }
    void setSlices(const GLint _slices) { slices = _slices; genVertCord(); genUVCord(); }
    void setStacks(const GLint _stacks) { stacks = _stacks; genVertCord(); genUVCord(); }
    
    /* Reset all private members */
    void reset(GLdouble _radius,
               GLint _slices,
               GLint _stacks)
    {
        radius = _radius;
        slices = _slices;
        stacks = _stacks;
        genVertCord();
        genUVCord();
    }

    /* Bind vertex data buffers
    ** Must run this function before drawing
    ** ATTENTION:
    **      You must initialize GLEW before calling this function! 
    **      In other words, call this function after `GlewInit()`. */
    void bindBuffer()
    {
        // Generate vertex arrays and buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &vert_VBO);
        glGenBuffers(1, &normal_VBO);
        glGenBuffers(1, &uv_VBO);
        
        // Bind buffer data (@vert_VBO and @uv_VBO)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vert_VBO);
        glBufferData(GL_ARRAY_BUFFER, slices * stacks * 6 * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
        glBufferData(GL_ARRAY_BUFFER, slices * stacks * 6 * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
        glBufferData(GL_ARRAY_BUFFER, slices * stacks * 4 * sizeof(GLfloat), uvcord.data(), GL_STATIC_DRAW);
    }

    /* Draw the ball (DEFAULT) */
    void draw()
    {
        // Set the vertex attribute pointers
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // Bind buffer data: @vert_VBO, @normal_VBO and @uv_VBO
        glBindBuffer(GL_ARRAY_BUFFER, vert_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBindVertexArray(0);

        // Draw this ball
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, slices * stacks * 2);
        glBindVertexArray(0);

        // Disable vertex attribute array
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    /* Draw the ball with shader */
    void draw(Shader shader)
    {
        // Install the shader
        shader.install();
        draw();
        shader.uninstall();
    }

protected:

    /* PRIVATE MEMBER:
    ** Generate vertex coordinates */
    void genVertCord()
    {
        // A fragment is determined by its two endpoints (6 coordinates)
        // The number of vertices is determined by @slices and @stacks
        vertex.reserve(slices * stacks * 6);

        // Calculate the coordinates
        for(int i = 0; i < stacks; i++)
        {
            // The current and the next latitude (TOTAL: @stacks)
            GLfloat cur_latitude = M_PI * (-0.5f + (GLfloat)i * 1.0f / stacks);
            GLfloat next_latitude = M_PI * (-0.5f + (GLfloat)(i + 1) * 1.0f / stacks);

            for(int j = 0; j < slices; j++)
            {
                // The current longitude
                GLfloat longitude = 2.0f * M_PI * (GLfloat)j / slices;

                // The start point of the current fragment
                vertex.push_back(cos(cur_latitude) * sin(longitude));
                vertex.push_back(sin(cur_latitude));
                vertex.push_back(cos(cur_latitude) * cos(longitude));

                // The end point of the current fragment
                vertex.push_back(cos(next_latitude) * sin(longitude));
                vertex.push_back(sin(next_latitude));
                vertex.push_back(cos(next_latitude) * cos(longitude));
            }
        }
    }

    /* PRIVATE MEMBER:
    ** Generate UV coordinates */
    void genUVCord()
    {
        // Notice that the texture image is 2D
        // Each fragment need two 20-points (4 coordinates)
        uvcord.reserve(slices * stacks * 4);
        for(int i = 0; i < stacks; i++)
        {
            for(int j = 0; j < slices; j++)
            {
                uvcord.push_back((GLfloat)j * 1.0f / slices);
                uvcord.push_back((GLfloat)i * 1.0f / stacks);
                uvcord.push_back((GLfloat)j * 1.0f / slices);
                uvcord.push_back((GLfloat)(i + 1) * 1.0f / stacks);
            }
        }
    }

    /* The radius of the ball 
    ** Actually in function `glutSolidSphere (glutWireSphere)`
    ** ATTENTION: The type of RADIUS is `GLdouble` */
    GLdouble radius;

    /* The private members `slices` and `stacks` are used in OpenGL functions which indend
    ** to draw this ball. In fact, we want a ball to be SLIPPY enough. */
    /* The number of subdivisions around the Z axis (similar to lines of longitude). */
    GLint slices;

    /* The number of subdivisions along the Z axis (similar to lines of latitude). */
    GLint stacks;

    /* The vertex coordinates and UV coordinates vector.
    ** Used when drawing this ball.
    ** We use std::vector<> instead of glm::vec3, because we need to bind
    ** vertex buffer with array. `.data()` in std::vector<> provided.*/
    std::vector<GLfloat> vertex, uvcord;

    /* The VAO, VBOs of this ball */
    GLuint VAO, vert_VBO, uv_VBO, normal_VBO;
};

/* CLASS: Snow ball - Our protagonist! */
class SnowBall : public Ball
{
public:

    /* Default constructor & Constructor 
    ** PHYSICS MODEL:
    ** The variable @accelerator and @speed are different from what you learn in physics. 
    ** These variables are related to parameter `TIME`. However, in OpenGL we draw our
    ** scene for each `FRAME`. */
    SnowBall(GLdouble _radius = 0,
             GLint _slices = 40,
             GLint _stacks = 40,
             GLfloat _rotAngle = 0.0f,
             GLfloat _speed = 12.0f, 
             GLfloat _accelerator = 1.2f,
             GLfloat _meltSpeed = 0.01f)
        : Ball(_radius, _slices, _stacks),
          rotAngle(_rotAngle),
          speed(_speed),
          accelerator(_accelerator),
          meltSpeed(_meltSpeed)
    {
        curPosition.y = radius;
        curPosition.z = radius * 20.0f / 5;
    }

    /* Return private members
    ** Read the value of the @radius, @slices and @stacks */
    const glm::vec3 getCurPosition() { return curPosition; }
    const GLfloat getSpeed() { return speed; }
    const GLfloat getAccelerator() { return accelerator; }
    const GLfloat getRotAngle() { return rotAngle; }
    const GLfloat getRotSpeed() { return speed / glm::radians(radius); }
    const GLfloat getMeltSpeed() { return meltSpeed; }

    /* Modify the value of private members
    ** Notice that the radius of snow ball is always being changing.
    ** Use this function to reset the private member. */
    void setSpeed(const GLfloat _speed) { speed = _speed; }
    void setAccelerator(const GLfloat _accelerator) { accelerator = _accelerator; }
    void setRotAngle(const GLfloat _rotAngle) { rotAngle = _rotAngle; }
    void setMeltSpeed(const GLfloat _meltSpeed) { meltSpeed = _meltSpeed; }
    void setPositionX(const GLfloat _x) { curPosition.x = _x; }
    void setPositionY(const GLfloat _y) { curPosition.y = _y; }
    void setPositionZ(const GLfloat _z) { curPosition.z = _z; }
    
    /* Reset current position completely */
    void resetPosition(const GLfloat _x,
                       const GLfloat _y,
                       const GLfloat _z)
    {
        curPosition.x = _x;
        curPosition.y = _y;
        curPosition.z = _z;
    }

    /* Load or reload the texture of snow ball */
    void loadTexture(const char* image_path)
    {
        texture.reload(image_path);
    }

    /* Get the texture of snow ball */
    Texture getTexture()
    { // Return private member
        return texture;
    }

private:

    /* The rotation angle of each snowball */
    GLfloat rotAngle;

    /* The current position of the snowball
    ** WHY do we need this struct?
    ** Actually it is really important. In order to detect collision, update the barriers'
    ** deque and calculate the player's score, it is important for us to know where the
    ** snow ball actually is. */
    glm::vec3 curPosition;

    /* The current speed of this snowball
    ** PHYSICS MODEL. 
    ** Associated variable: @accelerator */
    GLfloat speed;

    /* The current accelerated velocity 
    ** PHYSICS MODEL.
    ** Associated variable: @speed */
    GLfloat accelerator;

    /* The meltSpeed of Snowball
    ** The snowball is always being melting! */
    GLfloat meltSpeed;

    /* The sphere texture of this ball
    ** You can load different textures onto the sphere.
    ** DEFAULT: our simple snowball texture. */
    Texture texture;
};

/* Queue structure of barriers' position */
typedef std::deque<int> barrierDeque;

/* CLASS: barriers */
class Barriers
{
public:
    // Iterator type redefinition
    typedef barrierDeque::iterator iterator;

    /* Default constructor & Constructor*/
    Barriers(GLfloat _baseline = 0.8f, 
             GLfloat _rotSpeed = 0.0f,
             GLfloat _spacing = 30.0f,
             GLint _rowSize = 20)
        : baseline(_baseline), 
          rotSpeed(_rotSpeed),
          spacing(_spacing),
          rowSize(_rowSize)
    {
    }

    /* Return private members
    ** Read the value of the following members:
    ** @baseline, @rotAngle, @rotSpeed, @spacing, @rowSize, @deque */
    const GLfloat getBaseline() { return baseline; }
    const GLfloat getRotSpeed() { return rotSpeed; }
    const GLfloat getSpacing() { return spacing; }
    const GLint getRowSize() { return rowSize; }
    const barrierDeque getDeque() { return deque; }

    /* Modify the value of private members */
    void setBaseline(const GLfloat _baseline) { baseline = _baseline; }
    void setSpacing(const GLfloat _spacing) { spacing = _spacing; }

    /* Notice that the value of @rotAngle is [0.0f, 360.0f) */
    void setRotSpeed(const GLfloat _rotSpeed)
    {
        rotSpeed = _rotSpeed;
        while(rotSpeed >= 360.0f) rotSpeed -= 360.0f;
        while(rotSpeed < 0.0f) rotSpeed += 360.0f;
    }

    /* DEFAULT: Initialize the barrier queue */
    void initDeque()
    {
        // If the queue size not equals zero
        if (deque.size())
        { // EXIT program here!
            printf("INIT ERROR: The barrier queue exists! Exit.\n");
            exit(0);
        }

        // Get random numbers
        int safeLane;
        srand(time(0));
        for (int loop = 0; loop < rowSize; loop++)
        { // Push these random numbers into our barrier queue
            safeLane = rand() % 3 - 1;
            deque.push_back(safeLane);
        }
    }

    /* PARAMETERIZED: Initialize the barrier queue */
    void initDeque(int* array)
    {
        // If the queue size not equals zero
        if (deque.size())
        { // EXIT program here!
            printf("INIT ERROR: The barrier queue exists! Exit.\n");
            exit(0);
        }

        for (int loop = 0; loop < rowSize; loop++)
        { // Push these random numbers into our barrier queue
            deque.push_back(array[loop]);
        }
    }

    /* UPDATE dequeue data
    ** When the snow ball passes one barrier row, the dequeue data should be updated */
    void updateDeque()
    {
        // Push back a new random number
        int safeLane = rand() % 3 - 1;
        deque.pop_front();
        deque.push_back(safeLane);
    }

    /* Output the deque data to the console */
    void outputDeque()
    {
        // Initialize iterator
        iterator iter = deque.begin();
        for (; iter != deque.end(); iter++)
            printf("%d ", *iter);
        printf("\n");
    }

private:

    /* The vertical location of barriers */
    GLfloat baseline;

    /* The rotation speed of each barrier
    ** DEFAULT: 10.0f
    ** Set this member to 0.0f to disable rotation
    ** The value of rotation angle is [0.0f, 360.0f) */
    GLfloat rotSpeed;

    /* The spacing between two barrier rows */
    GLfloat spacing;

    /* The number of barrier rows (size of @deque) */
    GLint rowSize;

    /* Barriers' position dequeue */
    barrierDeque deque;
};

#endif
