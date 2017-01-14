#version 330 core

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec2 squareUVs; 
layout(location = 2) in vec3 particle_positions_worldspace;

out vec2 UV;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float size = 0.4;
	vec3 vertexPosition_worldspace = 
		particle_positions_worldspace
		+ CameraRight_worldspace * squareVertices.x * size
		+ CameraUp_worldspace * squareVertices.y * size;

	gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);

	UV = squareUVs;
}

