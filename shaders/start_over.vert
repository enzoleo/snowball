#version 330 core

/* IN
** @param squareVertices: Input vertex data */
layout(location = 0) in vec3 squareVertices;

/* OUT VEC
** @param UV: the uv coordinates calculated by hand
** Interpolated values from the vertex shaders */
out vec2 UV;

/* UNIFORM VEC
** @param billboardLen: the length of the billboard
** @param billboardWidth: the width of the billboard */
uniform float billboardLen;
uniform float billboardWidth;

void main()
{
    // Compute the vertex data of the billboard
    vec3 position = vec3(squareVertices.x * billboardLen,
                         squareVertices.y * billboardWidth,
                         0.0f);
    gl_Position = vec4(position, 1.0f);

    // The UV coordinates of vertices
    UV.x = 0.5 + squareVertices.x;
    UV.y = 0.5 - squareVertices.y;
}