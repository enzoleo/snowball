#version 330 core

/* LAYOUT
** IN VEC parameters
** @param position: the position data 
** @param normal: the normals of vertices
** @param texCoords: the coordinates of texture */
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

/* OUT VEC
** @param FragPos: the fragment position
** @param Normal: the normals of vertices
** @param TexCoords: the texture coordinates
** @param FragPosLightSpace: The light space of given fragment position */
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

/* UNIFORM
** @param model: the model matrix
** @param view: the view matrix
** @param projection: the projection matrix
** @param lightSpaceMatrix: the matrix of light space */
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}