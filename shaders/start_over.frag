#version 330 core

/* IN VEC
** @param UV: the uv coordinates calculated by hand
** Interpolated values from the vertex shaders */
in vec2 UV;

/* OUT VEC4
** @param color: ouput color data */
out vec4 color;

/* UNIFORM
** @param texture_sampler: The texture
** @param LifeLevel: The current life level of player */
uniform sampler2D texture_sampler;
uniform float lifeLevel;

void main()
{
    // Output color (color of the texture at the specified UV)
    color = texture(texture_sampler, UV);
}