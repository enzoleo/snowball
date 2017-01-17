/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "camera.h"
#include "shader.hpp"
#include "texture.h"

#define RANDOM_MIN_MAX(min, max) (min + ((float)rand() / (float)RAND_MAX) * (max - min))

extern GLfloat offset_z;

/* STRUCT: Particle base */
struct ParticleBase
{
    /* Default constructor & Constructor */
    ParticleBase(GLfloat _drag_coef = 1000.0f,
                 GLfloat _size = 0.2f)
        : drag_coef(_drag_coef),
          size(_size)
    { // Do nothing here
    }

    /* PUBLIC MEMBER
    ** A factor used in drag computation */
    GLfloat drag_coef;

    /* PUBLIC MEMBER
    ** The size of snowflake (Actually the edge length of cube or square) */
    GLfloat size;
};

/* STRUCT: Particle structure */
class Particle : public ParticleBase
{
public:
    /* Default constructor & Constructor */
    Particle(GLfloat _drag_coef = 1000.0f,
             GLfloat _size = 0.2f,
             const glm::vec3& _position = glm::vec3(0.0f, 0.0f, 0.0f),
             const glm::vec3& _velocity = glm::vec3(0.0f, 0.0f, 0.0f))
        : ParticleBase(_drag_coef, _size),
          position(_position),
          velocity(_velocity),
          gravity(glm::vec3(0.0f, -9.80665f, 0.0f)),
          drag(glm::vec3(0.0f, 0.0f, 0.0f)),
          life(-1.0f)
    { // Do nothing here
    }

    /*****************
    ** FUNCTION: Compute the drag accelerate speed.
    ** Using the drag equation. Learn more information, see:
    **     `https://en.wikipedia.org/wiki/Drag_equation`
    ** The equation is: `F_D = 0.5 * \pho[air] * v^2 * C_D * A`.
    ** The density of air is considered to be 0.001293 kg/m^3. And if we see the
    ** snowflake as a simple cude with edge length equaling to @size. The the actual
    ** drag accelerate speed is `a_D = 0.5 * \pho[air] * v^2 * k * @size^2 / @size^3`,
    ** where `k = C_D / \pho` is a constant parameter merely about the cube. Thus, we
    ** can compute `a_D = 0.000647f * @drag_coef * v^2 / @size` simply.
    ******************/
    void updateDrag()
    { // Using the drag equation to compute approximately
        GLfloat drag_acc = 0.000647f * drag_coef * glm::dot(velocity, velocity) / size;

        // Do not do normalization if the velocity is zero vector!
        if (velocity != glm::vec3(0.0f, 0.0f, 0.0f))
            drag = -drag_acc * glm::normalize(velocity);
    }

    /* PUBLIC MEMBER
    ** The position of particle */
    glm::vec3 position;

    /* PUBLIC MEMBER
    ** remaining life of the particle. if < 0 : dead and unused. */
    GLfloat life;

    /* PUBLIC MEMBER
    ** The velocity of particle */
    glm::vec3 velocity;

    /* PUBLIC MEMBER
    ** @param gravity: The gravity constant (USUALLY we see this as a constant vector).
    ** @param drag: The drag accelerator caused by air. Actually this is a physical model, but we
    **     have to see the snowflake as a simple mass cube, considering the complexity if we intend
    **     to render the snowflake scene like a real natural phenomenon. */
    const glm::vec3 gravity;
    glm::vec3 drag;
};

/* CLASS: Particle system */
class ParticleSystem : public ParticleBase
{
public:

    /* Default constructor & Constructor */
    ParticleSystem(const Shader& _shader,
                   const Texture& _texture,
                   const glm::vec3& _position_generator,
                   GLuint _generate_speed,
                   GLfloat _range_x,
                   GLfloat _range_z,
                   GLfloat _drag_coef = 200.0f,
                   GLfloat _size = 0.2f)
        : shader(_shader),
          texture(_texture),
          generate_speed(_generate_speed),
          position_generator(_position_generator),
          range_x(_range_x),
          range_z(_range_z),
          ParticleBase(_drag_coef, _size)
    { // Do initialization
        init();
    }

    /* Returns the private members
    ** We set the particle system settings private, because they are not supposed to be
    ** editted easily. If they need to be editted, call the `set*` functions, which makes
    ** sure you edit them on purpose, instead of unconsciously. */
    const GLfloat getTotalNum() { return total_num; }
    const GLfloat getGenSpeed() { return generate_speed; }
    const GLuint getRangeX() { return range_x; }
    const GLuint getRangeZ() { return range_z; }
    const GLfloat getPeriod() { return period; }

    /* Set some private numbers */
    void setGenSpeed(const GLfloat _generate_speed) { generate_speed = _generate_speed; }
    void setRangeX(const GLfloat _range_x) { range_x = _range_x; }
    void setRangeZ(const GLfloat _range_z) { range_z = _range_z; }
    void setGeneratorPos(const glm::vec3& pos) { position_generator = pos; }

    /* This function does UPDATE operations
    ** @param dt: the time passed (between 2 frames) */
    void update(const GLfloat& dt)
    {
        // Count the number of living particles after dt
        GLuint count = 0;
        GLuint num_new_particles = dt * generate_speed;

        // Create new particles 
        for (GLuint i = 0; i < num_new_particles; ++i)
        {
            GLuint unusedParticle = getfirstDeadParticle();
            respawn(particles[unusedParticle]);
        }
        // update all particles
        for (GLuint i = 0; i < total_num; i++)
        {
            // For each particle, decrease life
            Particle &particle = particles[i];
            particle.life -= dt;

            // Update particles if the chosen particle is still alive
            if (particle.life > 0.0f)
            {
                // Compute drag of the current particle
                particle.updateDrag();

                // Use the mechanical model!
                particle.velocity += (particle.gravity + particle.drag) * dt;
                particle.position += particle.velocity * dt + glm::vec3(0, 0, -offset_z);;
                // Fill our particle_position_data
                particle_position_data[3 * count + 0] = particle.position.x;
                particle_position_data[3 * count + 1] = particle.position.y;
                particle_position_data[3 * count + 2] = particle.position.z;
                count++;
            }
        }
        // Update the number of alive particles
        total_num_live = count;
    }

    void draw(Camera camera, GLuint texture_unit)
    {
        shader.install();
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_particle_position);
        glBufferData(GL_ARRAY_BUFFER, total_num * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, total_num_live * 3 * sizeof(GLfloat), particle_position_data);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        texture.bind(texture_unit);
        shader.setUniform1i("texture_sampler", texture_unit);
        glm::mat4 view = camera.getViewMat();
        shader.setUniform3f("CameraRight_worldspace", glm::vec3(view[0][0], view[1][0], view[2][0]));
        shader.setUniform3f("CameraUp_worldspace", glm::vec3(view[0][1], view[1][1], view[2][1]));
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, total_num_live);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
        shader.uninstall();
    }

private:

    /* PRIVATE MEMBER
    ** Initiate all parameters in particle system */
    void init()
    {
        // Compute the period of this system and the total number of particles
        period = 4.0f + drag_coef / 400.0f;
        total_num = period * generate_speed;

        // Create total_num default particle instances
        particles.reserve(total_num);
        for (GLuint i = 0; i < total_num; i++)
            particles.push_back(Particle(drag_coef, size));

        // Set the alive number = 0
        total_num_live = 0;
        GLfloat particle_quad[] =
        {
            // The position and texture coordinates
            0.0f, size, 0.0f, 0.0f, 1.0f,
            size, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, size, 0.0f, 0.0f, 1.0f,
            size, size, 0.0f, 1.0f, 1.0f,
            size, 0.0f, 0.0f, 1.0f, 0.0f
        };
        particle_position_data = new GLfloat[3 * total_num];

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_quad);
        glGenBuffers(1, &VBO_particle_position);
        glBindVertexArray(VAO);

        // The data of VBO_quad is shared by every particle (instancing technique!)
        glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

        // The data of VBO_particle_position will be updated every frame
        glBindBuffer(GL_ARRAY_BUFFER, VBO_particle_position);
        glBufferData(GL_ARRAY_BUFFER, total_num * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_particle_position);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        // ----------------------------------------------------------------------------------
        // Particles vertices : always reuse the same 4 vertices -> 0
        // Particles uvs : always reuse the same 4 uvs  -> 0
        // Positions of particle centers : one per quad -> 1
        // ----------------------------------------------------------------------------------
        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 1);
        glBindVertexArray(0);
    }

    /* PRIVATE MEMBER
    ** Find first dead particle */
    GLuint getfirstDeadParticle()
    {
        // First search from last used particle, this will usually return almost instantly
        // Using iterator encapsulated in the STL vector
        GLuint index = last_particle;
        std::vector<Particle>::iterator iter = particles.begin() + last_particle;
        for (; iter != particles.end(); iter++)
        {
            if (iter->life <= 0.0f)
            { // If the iterator finds one dead particle
                last_particle = index;
                return index;
            }
            index++;
        }
        // Do a completely linear search if cannot find index
        iter = particles.begin();
        index = 0;
        for (; iter != particles.begin() + last_particle; iter++)
        {
            if (iter->life <= 0.0f)
            { // If the iterator finds one dead particle
                last_particle = index;
                return index;
            }
            index++;
        }

        // All particles are taken, override the first one
        // (note that if it repeatedly hits this case, more particles should be reserved)
        last_particle = 0;
        return 0;
    }

    /* PRIVATE MEMBER
    ** Respawn a certain number of particles from (almost) a plane
    ** Use macro RANDOM_MIN_MAX to generate random number from (min, max) */
    void respawn(Particle &particle)
    {
        // Renew the position and velocity of this particle
        particle.position.x = position_generator.x + RANDOM_MIN_MAX(-range_x, range_x);
        particle.position.y = position_generator.y + RANDOM_MIN_MAX(-10, 0);
        particle.position.z = position_generator.z + RANDOM_MIN_MAX(-range_z, range_z);
        particle.velocity = glm::vec3(0.0f);

        // Renew the life of this particle (notice the disturbing term)
        // The foundamental life is related to the drag coefficient
        particle.life = period;
    }

    /* PRIVATE MEMBER
    ** The vector records all particles */
    std::vector<Particle> particles;

    /* PRIVATE MEMBER
    ** Positions of particle centers every frame (updated every frame) */
    GLfloat* particle_position_data;

    /* PRIVATE MEMBERS
    ** @param total_num: The maximum number of particles
    ** @param generate_speed: The number of new particles generated for a second
    ** @param total_num_live: The number of living particles */
    GLuint total_num, generate_speed, total_num_live;

    /* PRIVATE MEMBER
    ** The position of the generator */
    glm::vec3 position_generator;

    /* PRIVATE MEMBERS
    ** The range within which we respawn particles */
    GLfloat range_x, range_z;

    /* PRIVATE MEMBER
    ** A specific shader for rendering particles */
    Shader shader;

    /* PRIVATE MEMBER
    ** The texture we use to attach to each particle */
    Texture texture;

    /* PRIVATE MEMBERS
    ** The VAO and VBOs of the particle system */
    GLuint VAO, VBO_quad, VBO_particle_position;

    /* PRIVATE MEMBER
    ** Stores the index of the last particle used.
    ** For quick access to next dead particle. */
    GLuint last_particle;

    /* PRIVATE MEMBER
    ** The period of particle generation */
    GLfloat period;

};

#endif