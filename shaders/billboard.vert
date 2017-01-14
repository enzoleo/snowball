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
    // The position of center of the billboard
    // Make it always at the top-right corner
    vec3 position_center = vec3(0.95f - billboardLen / 2.0f, 0.95f - billboardWidth / 2.0f, 0.0f);

    // Compute the vertex data of the billboard
    vec3 position = position_center
                  + vec3(squareVertices.x * billboardLen,
                         squareVertices.y * billboardWidth,
                         0.0f); 
    gl_Position = vec4(position, 1.0f);

    // The UV coordinates of vertices
    UV = squareVertices.xy + vec2(0.5, 0.5);
}