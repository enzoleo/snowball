/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _LIGHT_HPP_
#define _LIGHT_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

class Light {
 public:
  /* Default constructor & Constructor */
  Light(const glm::vec3& _direction = glm::vec3(-1, -1, -1),
        const glm::vec3& _ambient = glm::vec3(0.4, 0.4, 0.4),
        const glm::vec3& _diffuse = glm::vec3(0.5, 0.5, 0.5),
        const glm::vec3& _specular = glm::vec3(0.1, 0.1, 0.1))
      : direction(glm::normalize(_direction)),
        ambient(_ambient),
        diffuse(_diffuse),
        specular(_specular) {  // Do nothing here
  }

  /* Returns the private members
  ** We set the light settings private, because they are not supposed to be editted easily
  ** If they need to be editted, call the `set*` functions, which makes sure you edit them
  ** on purpose, instead of unconsciously. */
  glm::vec3 getDirection() { return direction; }
  glm::vec3 getAmbient() { return ambient; }
  glm::vec3 getDiffuse() { return diffuse; }
  glm::vec3 getSpecular() { return specular; }

  /* Reset some private members */
  void setDirection(const glm::vec3& _direction) { direction = _direction; }
  void setAmbient(const glm::vec3& _ambient) { ambient = _ambient; }
  void setDiffuse(const glm::vec3& _diffuse) { diffuse = _diffuse; }
  void setSpecular(const glm::vec3& _specular) { specular = _specular; }

  /* Function binds light shader */
  void bindShader(Shader shader) {
    shader.install();
    shader.setUniform3fv("light.direction", direction);
    shader.setUniform3fv("light.ambient", ambient);
    shader.setUniform3fv("light.diffuse", diffuse);
    shader.setUniform3fv("light.specular", specular);
    shader.uninstall();
  }

 private:
  /* PRIVATE MEMBER
  ** The direction of light (no matter the kind of light) */
  glm::vec3 direction;

  /* PRIVATE MEMBER
  ** The ambient vector of light */
  glm::vec3 ambient;

  /* PRIVATE MEMBER
  ** The diffuse vector of light */
  glm::vec3 diffuse;

  /* PRIVATE MEMBER
  ** The specular vector of light */
  glm::vec3 specular;
};

#endif
