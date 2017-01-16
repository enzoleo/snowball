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
#include <iostream>
#include <glm/glm.hpp>
#include "texture.h"
#include "shader.h"

/* CLASS: Object (base class) */
class Object
{
public:

    /* Default constructor & Constructor */
    Object(const GLfloat& _kd = 1.0f,
           const GLfloat& _ks = 0.0f,
           const GLfloat& _shininess = 10.0f)
        : kd(_kd),
          ks(_ks),
          shininess(_shininess) 
    { 
        if (kd + ks > 1.0) {
            std::cout << "kd + ks > 1.0, please reenter these parameters" << std::endl;
            kd = 1.0;
            ks = 0.0;
        }
    }

    virtual void draw(Shader shader) = 0;

    GLfloat getKd() const { return kd; }
    GLfloat getKs() const { return ks; }
    GLfloat getShininess() const { return shininess; }
    glm::mat4 getModelMatrix() const { return model2world; }
    Texture* getTexture() const { return texture_ptr; }
    virtual void setModelMatrix(const glm::mat4& m) { model2world = m; }
    void setTexture(Texture* _texture) 
    { 
        if (!texture_ptr) texture_ptr = new Texture();
        texture_ptr = _texture; 
    }
    
protected:
    GLuint VAO;  //vertex array object
    glm::mat4 model2world;  //this matrix transforms the object from model space to world space
    Texture* texture_ptr;  //texture(s) 
    GLfloat kd, ks, shininess;
};

static const GLfloat g_vertices_square[] = {
    // Positions, Normals, Texture Coordinates
    0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f,  1.0f, 1.0f, // Top Right
    0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f,  1.0f, 0.0f, // Bottom Right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // Bottom Left
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f,   0.0f, 1.0f  // Top Left 
};

static const GLuint g_indices_square[] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
};

//this square's center is at (0,0,0), the length of each edge is 1
//this square is at plane xOy
class Square : public Object
{
public:
    Square() {};
    Square(const GLfloat& _kd, const GLfloat& _ks, const GLfloat& _shininess) :Object(_kd, _ks, _shininess) {}

    void setup()
    {
        GLuint VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices_square), g_vertices_square, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices_square), g_indices_square, GL_STATIC_DRAW);
        // link vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(Shader shader) {
        shader.install();
        glBindVertexArray(VAO);
        shader.setUniformMatrix4fv("model", model2world);
        if (texture_ptr != NULL) {
            texture_ptr->bind(texture_ptr->getUnit());
            shader.setUniform1i("material.diffuse1", texture_ptr->getUnit());
        }
        shader.setUniform1f("material.kd", kd);
        shader.setUniform1f("material.ks", ks);
        shader.setUniform1f("material.shininess", shininess);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        texture_ptr->unbind();
        glBindVertexArray(0);
        // Disable vertex attribute array
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        shader.uninstall();
    }
};

static const GLfloat g_vertices_cube[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
};

//this cube's center is at (0,0,0), the length of each edge is 1
class Cube : public Object
{
public:
    Cube(const glm::mat4& m = glm::mat4()) 
    {
        mat = m; model2world = mat; 
    }
    Cube(const GLfloat& _kd, const GLfloat& _ks, const GLfloat& _shininess, const glm::mat4& m = glm::mat4()) :Object(_kd, _ks, _shininess) 
    {
        mat = m; model2world = mat; 
    }
    void setup() {
        GLuint VBO;
        glGenVertexArrays(1, &VAO);  
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices_cube), g_vertices_cube, GL_STATIC_DRAW);
        // link vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(Shader shader) {
        shader.install();
        glBindVertexArray(VAO);
        shader.setUniformMatrix4fv("model", model2world);
        if (texture_ptr != NULL) {
            texture_ptr->bind(texture_ptr->getUnit());
            shader.setUniform1i("material.diffuse1", texture_ptr->getUnit());
        }
        shader.setUniform1f("material.kd", kd);
        shader.setUniform1f("material.ks", ks);
        shader.setUniform1f("material.shininess", shininess);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 12 triangles
        texture_ptr->unbind();
        glBindVertexArray(0);
        // Disable vertex attribute array
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        shader.uninstall();
    }

    void setModelMatrix(const glm::mat4& m) { model2world = m * mat; }

    void setInitModelMatrix(const glm::mat4& m) { mat = m; }

private:
    glm::mat4 mat;
};

/* CLASS: Ball */
class Ball : public Object
{
public:
    /* Default constructor */
    Ball(GLfloat _radius = 1,
        GLint _slices = 40,
        GLint _stacks = 40,
        GLfloat _kd = 1.0,
        GLfloat _ks = 0.0,
        GLfloat _shininess = 0.0)
        : Object(_kd, _ks, _shininess),
        radius(_radius),
        slices(_slices),
        stacks(_stacks)
    {
        genVertCord();
        genUVCord();
    }

    /* Bind vertex data buffers
    ** Must run this function before drawing
    ** ATTENTION:
    **      You must initialize GLEW before calling this function!
    **      In other words, call this function after `GlewInit()`. */
    void setup()
    {
        // Generate vertex arrays and buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &vert_VBO);
        glGenBuffers(1, &normal_VBO);
        glGenBuffers(1, &uv_VBO);

        // Bind buffer data (@vert_VBO and @uv_VBO)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vert_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
        glBufferData(GL_ARRAY_BUFFER, uvcord.size() * sizeof(GLfloat), uvcord.data(), GL_STATIC_DRAW);
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

    /* draw the ball (DEFAULT) */
    void draw(Shader shader)
    {
        shader.install();
        glBindVertexArray(VAO);
        shader.setUniformMatrix4fv("model", model2world);
        if (texture_ptr != NULL) {
            texture_ptr->bind(texture_ptr->getUnit());
            shader.setUniform1i("material.diffuse1", texture_ptr->getUnit());
        }
        shader.setUniform1f("material.kd", kd);
        shader.setUniform1f("material.ks", ks);
        shader.setUniform1f("material.shininess", shininess);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (slices+1) * stacks * 2);
        texture_ptr->unbind();
        glBindVertexArray(0);
        // Disable vertex attribute array
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        shader.uninstall();
    }

    /* draw the ball with shader */
    //void draw(Shader shader)
    //{
    //  // Install the shader
    //  shader.install();
    //  draw();
    //  shader.uninstall();
    //}

protected:

    /* PRIVATE MEMBER:
    ** Generate vertex coordinates */
    void genVertCord()
    {
        // A fragment is determined by its two endpoints (6 coordinates)
        // The number of vertices is determined by @slices and @stacks
        vertex.reserve((slices + 1) * stacks * 6);

        // Calculate the coordinates
        for (int i = 0; i < stacks; i++)
        {
            // The current and the next latitude (TOTAL: @stacks)
            GLfloat cur_latitude = M_PI * (-0.5f + (GLfloat)i / stacks);
            GLfloat next_latitude = M_PI * (-0.5f + (GLfloat)(i + 1)  / stacks);

            for (int j = 0; j < slices + 1; j++)
            {
                // The current longitude
                GLfloat longitude = 2.0f * M_PI * (GLfloat)j / slices;

                // The start point of the current fragment
                vertex.push_back(radius * cos(cur_latitude) * sin(longitude));
                vertex.push_back(radius * sin(cur_latitude));
                vertex.push_back(radius * cos(cur_latitude) * cos(longitude));

                // The end point of the current fragment
                vertex.push_back(radius * cos(next_latitude) * sin(longitude));
                vertex.push_back(radius * sin(next_latitude));
                vertex.push_back(radius * cos(next_latitude) * cos(longitude));
            }
        }
    }

    /* PRIVATE MEMBER:
    ** Generate UV coordinates */
    void genUVCord()
    {
        // Notice that the texture image is 2D
        // Each fragment need two 20-points (4 coordinates)
        uvcord.reserve((slices + 1)* stacks * 4);
        for (int i = 0; i < stacks; i++)
        {
            for (int j = 0; j < slices + 1; j++)
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

    /* The VBOs of this ball */
    GLuint vert_VBO, uv_VBO, normal_VBO;
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
    SnowBall(GLfloat _radius = 1,
        GLint _slices = 40,
        GLint _stacks = 40,
        GLfloat _kd = 1.0,
        GLfloat _ks = 0.0,
        GLfloat _shininess = 10,
        GLfloat _rotAngle = 0.0f,
        GLfloat _speed = 24.0f,
        GLfloat _accelerator = 0.0f,
        GLfloat _meltSpeed = 0.01f)
        : Ball(_radius, _slices, _stacks, _kd, _ks, _shininess),
        rotAngle(_rotAngle),
        speed(_speed),
        accelerator(_accelerator),
        meltSpeed(_meltSpeed)
    {
        curPosition.y = radius;
        curPosition.z = radius * 20.0f / 3 -20;
    }

    /* Return private members
    ** Read the value of the @radius, @slices and @stacks */
    const glm::vec3 getCurPosition() { return curPosition; }
    const GLfloat getSpeed() { return speed; }
    const GLfloat getAccelerator() { return accelerator; }
    const GLfloat getRotAngle() { return rotAngle; }
    const GLfloat getRotSpeed() { return (180.0f * speed) / (3.14 * radius); }
    const GLfloat getMeltSpeed() { return meltSpeed; }
    /* Modify the value of private members
    ** Notice that the radius of snow ball is always being changing.
    ** Use this function to reset the private member. */
    void setSpeed(const GLfloat _speed) { speed = _speed; }
    void setAccelerator(const GLfloat _accelerator) { accelerator = _accelerator; }
    void setRotAngle(const GLfloat _rotAngle) { rotAngle = _rotAngle; }
    void setPositionX(const GLfloat _x) { curPosition.x = _x; }
    void setPositionY(const GLfloat _y) { curPosition.y = _y; }
    void setPositionZ(const GLfloat _z) { curPosition.z = _z; }
    void setMeltSpeed(const GLfloat _meltSpeed) { meltSpeed = _meltSpeed; }
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
    //void loadTexture(const char* image_path)
    //{
    //  texture.reload(image_path);
    //}

    /* Get the texture of snow ball */
    //Texture getTexture()
    //{ // Return private member
    //  return texture;
    //}

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
    Barriers(GLfloat _baseline = 1.0f,
        GLfloat _rotSpeed = 0.0f,
        GLfloat _spacing = 30.0f,
        GLint _rowSize = 20,
        GLuint _num_barrier_types = 1)
        : baseline(_baseline),
        rotSpeed(_rotSpeed),
        spacing(_spacing),
        rowSize(_rowSize),
        num_barrier_types(_num_barrier_types)
    {
        // Default settings 
        //for (GLuint i = 0; i < 2 * rowSize; ++i) {
        //  barrier_types.push_back(0);
        //}
        //barrier_objs.reserve(num_barrier_types);
        //for (GLuint i = 0; i < num_barrier_types; ++i) {
        //  barrier_objs.push_back(0);
        //}
        //model2worlds.reserve(2 * rowSize);
        //for (GLuint i = 0; i < 2 * rowSize; ++i) {
        //  model2worlds.push_back(glm::mat4());
        //}
        init();
    }

    /* Return private members
    ** Read the value of the following members:
    ** @baseline, @rotAngle, @rotSpeed, @spacing, @rowSize, @deque */
    const GLfloat getBaseline() { return baseline; }
    const GLfloat getRotSpeed() { return rotSpeed; }
    const GLfloat getSpacing() { return spacing; }
    const GLint getRowSize() { return rowSize; }
    const barrierDeque getDeque() { return deque; }
    const std::deque<GLuint> getBarTypes() { return barrier_types; }

    /* Modify the value of private members */
    void setBaseline(const GLfloat _baseline) { baseline = _baseline; }
    void setSpacing(const GLfloat _spacing) { spacing = _spacing; }

    /* Notice that the value of @rotAngle is [0.0f, 360.0f) */
    void setRotSpeed(const GLfloat _rotSpeed)
    {
        rotSpeed = _rotSpeed;
        while (rotSpeed >= 360.0f) rotSpeed -= 360.0f;
        while (rotSpeed < 0.0f) rotSpeed += 360.0f;
    }

    void setNumBarTypes(const GLuint& n) 
    { 
        if (num_barrier_types < n) {
            for (GLuint i = num_barrier_types; i < n; ++i) {
                barrier_objs.push_back(0);
            }
        }

        num_barrier_types = n;

        for (int i = 0; i < 2 * rowSize; ++i) {
            barrier_types[i] = rand() % num_barrier_types;
        }

    }

    /* DEFAULT: Initialize the barrier queue */
    void init()
    {
        // If the queue size not equals zero
        if (deque.size())
        { // EXIT program here!
            printf("INIT ERROR: The barrier queue exists! Exit.\n");
            exit(0);
        }

        // Get random numbers
        int safeLane;
        for (int loop = 0; loop < rowSize; loop++)
        { // Push these random numbers into our barrier queue
            safeLane = rand() % 3 - 1;
            deque.push_back(safeLane);
        }

        for (GLuint i = 0; i < 2 * rowSize; ++i) {
            barrier_types.push_back(rand() % num_barrier_types);
        }

        barrier_objs.reserve(num_barrier_types);
        for (GLuint i = 0; i < num_barrier_types; ++i) {
            barrier_objs.push_back(0);
        }
        
        GLfloat barrierLoop = -spacing;
        barrierDeque::iterator iter = deque.begin();
        glm::mat4 model;
        for (; iter != deque.end(); iter++)
        {
            for (int pos = -1; pos < 2; pos++)
            {
                if (*iter != pos)
                {
                    // Draw barrier at each unsafe position
                    model = glm::translate(glm::mat4(), glm::vec3(pos * 3.0f, 0.0f, barrierLoop));
                    model = glm::translate(model, glm::vec3(0.0f, baseline, 0.0f));
                    model = glm::rotate(model, rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
                    model2worlds.push_back(model);
                }
            }
            barrierLoop -= spacing;
        }

        begin_z = -spacing;
        std::cout << "begin z = " << begin_z << std::endl;

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
    void update()
    {
        
        // Push back a new random number
        int safeLane = rand() % 3 - 1;
        deque.pop_front();
        deque.push_back(safeLane);

        GLuint type0, type1;
        type0 = rand() % num_barrier_types;
        type1 = rand() % num_barrier_types;
        barrier_types.pop_front();
        barrier_types.push_back(type0);
        //std::cout << "first change " << std::endl;
        //outputTypes();
        barrier_types.pop_front();
        barrier_types.push_back(type1);
        //std::cout << "second change " << std::endl;
        std::cout << "after update types " << std::endl;
        outputTypes();

        glm::mat4 model;
        for (GLint i = -1; i < 2; ++i) {
            if (i != safeLane) {
                model = glm::translate(glm::mat4(), glm::vec3(i * 3.0f, 0.0f, begin_z - spacing * rowSize));
                model = glm::translate(model, glm::vec3(0.0f, baseline, 0.0f));
                model = glm::rotate(model, rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
                std::cout << "z of new barrier: " << begin_z - spacing * rowSize << std::endl;
                model2worlds.pop_front();
                model2worlds.push_back(model);
            }
        }

        begin_z -= spacing;
        std::cout << "begin z = " << begin_z << std::endl;
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

    void outputTypes()
    {
        for (GLuint i = 0; i < rowSize * 2; i = i + 2) {
            std::cout << "row " << i/2 << ": " << barrier_types[i] << ", " << barrier_types[i + 1] << std::endl;
        }
    }

    void draw(Shader shader) 
    {
        for (int i = 0; i < 2 * rowSize; ++i) {
            barrier_objs[barrier_types[i]]->setModelMatrix(model2worlds[i]);
            barrier_objs[barrier_types[i]]->draw(shader);
        }
    }

    void setModelMatrix(const GLuint& i, const glm::mat4& mat) {
        if (i < model2worlds.size()) {
            model2worlds[i] = mat;
        }
    }

    void setBarrierType(const GLuint& i, const GLuint& t) {
        if (i < barrier_types.size()) {
            barrier_types[i] = t;
        }
    }

    void setBarrierObj(const GLuint& i, Object& obj) {
        barrier_objs[i] = &obj;
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

    /* Model matrices for barriers 
    ** Size of this vector equals 2 * rowSize */
    //std::vector<glm::mat4> model2worlds;
    std::deque<glm::mat4> model2worlds;

    /* Barrier types 
    ** example: 0 for ball, 1 for cube */
    //std::vector<GLuint> barrier_types;
    std::deque<GLuint> barrier_types;


    /* The number of barrier types */
    GLuint num_barrier_types;

    /* The barriers contained, i.e. ball, cube, etc. */
    std::vector<Object*> barrier_objs;

    GLfloat begin_z;

};

#endif
