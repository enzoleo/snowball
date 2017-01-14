#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class DirLight
{
public:
	DirLight(const glm::vec3& _direction = glm::vec3(-1, -1, -1),
		const glm::vec3& _ambient = glm::vec3(0.4, 0.4, 0.4),
		const glm::vec3& _diffuse = glm::vec3(0.5, 0.5, 0.5),
		const glm::vec3& _specular = glm::vec3(0.1, 0.1, 0.1))
		:direction(glm::normalize(_direction)), ambient(_ambient), diffuse(_diffuse), specular(_specular)
	{}

	void bindShader(Shader shader) {
		shader.install();
		glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.direction"), 1, glm::value_ptr(direction));
		glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.ambient"), 1, glm::value_ptr(ambient));
		glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.diffuse"), 1, glm::value_ptr(diffuse));
		glUniform3fv(glGetUniformLocation(shader.getProgram(), "dirLight.specular"), 1, glm::value_ptr(specular));
		shader.uninstall();
	}

	glm::vec3 getDirection() { return direction; }
	glm::vec3 getAmbient() { return ambient; }
	glm::vec3 getDiffuse() { return diffuse; }
	glm::vec3 getSpecular() { return specular; }

	void setDirection(const glm::vec3& _direction) { direction = _direction; }
	void setAmbient(const glm::vec3& _ambient) { ambient = _ambient; }
	void setDiffuse(const glm::vec3& _diffuse) { diffuse = _diffuse; }
	void setSpecular(const glm::vec3& _specular) { specular = _specular; }


private:
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

//target is related to the snowball's position
//dir is the light's direction
//length is the distance between the light position and target
static void calcLightPos(const glm::vec3& target, const glm::vec3& dir, const GLfloat& length, glm::vec3& lightPos) {
	lightPos = target + length * (-dir);
}