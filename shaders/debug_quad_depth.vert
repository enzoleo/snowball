#version 330 core

/* LAYOUT
** IN VEC parameters
** @param position: the position data 
** @param texCoords: the coordinates of texture */
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

/* OUT VEC
** @param TexCoords: The texture coordinates */
out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 1.0f);
    TexCoords = texCoords;
}