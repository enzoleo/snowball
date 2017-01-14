#version 330 core

/* IN VEC
** @param Texcoords: the coodinates of textures */
in vec2 TexCoords;

/* OUT VEC
** @param color: output color data */
out vec4 color;

/* UNIFORM SAMPLER
** @param depthMap: the shadow depth map */
uniform sampler2D depthMap;

void main()
{
    // Compute the depth value
    float depthValue = texture(depthMap, TexCoords).r;
    
    // Orthographic (output color data)
    color = vec4(vec3(depthValue), 1.0);
}