#version 330 core

/* IN VEC
** @param UV: the uv coordinates calculated by hand
** Interpolated values from the vertex shaders */
in vec2 UV;

/* OUT VEC4
** @param color: ouput color data */
out vec4 color;

/* UNIFORM
** @param texture_sampler: The texture */
uniform sampler2D texture_sampler;

void main()
{
    // Output color (color of the texture at the specified UV)
    // Here the snowflake has white and half-transparent color
    color = texture(texture_sampler, UV) * vec4(1.0, 1.0, 1.0, 0.7);
}