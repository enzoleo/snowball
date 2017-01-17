#version 330 core

/* LAYOUT
** IN VEC parameters
** @param squareVertices: the vertices data 
** @param squareUVs: the UV coordinates of vertices
** @param particle_positions_worldspace: the position of particles */
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec2 squareUVs; 
layout(location = 2) in vec3 particle_positions_worldspace;

/* OUT VEC
** @param UV: the UV coordinates */
out vec2 UV;

/* UNIFORM
** @param CameraRight_worldspace: the right direction of camera
** @param CameraUp_worldspace: the up direction of camera
** @param view: the view matrix
** @param projection: the projection matrix */
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Compute the position in world space
    vec3 vertexPosition_worldspace = particle_positions_worldspace
                                   + CameraRight_worldspace * squareVertices.x
                                   + CameraUp_worldspace * squareVertices.y;
    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);

    // Output UV coordinates
    UV = squareUVs;
}

