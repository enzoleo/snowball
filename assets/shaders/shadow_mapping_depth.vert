#version 330 core

/* LAYOUT
** IN VEC parameters
** @param position: the position data */
layout (location = 0) in vec3 position;

/* UNIFORM
** @param lightSpaceMatrix: the matrix of light space
** @param model: the model matrix */
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}