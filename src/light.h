/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class DirLight
{
public:

    /* Default constructor & constructor */
    DirLight(const glm::vec3& _direction = glm::vec3(-1, -1, -1),
             const glm::vec3& _ambient = glm::vec3(0.2, 0.2, 0.2),
             const glm::vec3& _diffuse = glm::vec3(0.7, 0.7, 0.7),
             const glm::vec3& _specular = glm::vec3(0.1, 0.1, 0.1))
        : direction(glm::normalize(_direction)),
          ambient(_ambient),
          diffuse(_diffuse),
          specular(_specular)
    { // Constructor
    }

    /* Bind light with shader
    ** SEE the `shader` directory: `shader.frag`, `shader.vert`
    ** Calculation of light is implemented in shader files (GLSL) */
    void bindShader(Shader shader)
    {
        shader.install();
        glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.direction"), 1, glm::value_ptr(direction));
        glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.ambient"), 1, glm::value_ptr(ambient));
        glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.diffuse"), 1, glm::value_ptr(diffuse));
        glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.specular"), 1, glm::value_ptr(specular));
        shader.uninstall();
    }

    /* Returns the private members
    ** We set the light settings private, because they are not supposed to be editted easily
    ** If they need to be editted, call the `set*` functions, which makes sure you edit them
    ** on purpose, instead of unconsciously. */
    const glm::vec3 getDirection() { return direction; }
    const glm::vec3 getAmbient() { return ambient; }
    const glm::vec3 getDiffuse() { return diffuse; }
    const glm::vec3 getSpecular() { return specular; }

    /* Reset some private members */
    void setDirection(const glm::vec3& _direction) { direction = _direction; }
    void setAmbient(const glm::vec3& _ambient) { ambient = _ambient; }
    void setDiffuse(const glm::vec3& _diffuse) { diffuse = _diffuse; }
    void setSpecular(const glm::vec3& _specular) { specular = _specular; }

private:

    /* The direction of the light
    ** No matter what kind the light has (parallel or spot)*/
    glm::vec3 direction;

    /* The ambient reflection coefficients of light */
    glm::vec3 ambient;

    /* The diffuse reflection coefficients of light */
    glm::vec3 diffuse;

    /* The specular reflection coefficients of light */
    glm::vec3 specular;
};

#endif
