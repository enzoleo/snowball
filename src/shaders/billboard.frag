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
    
    // Hardcoded life level
    // To campute the color, use a simple algorithm
    if (UV.x < lifeLevel - 0.04 && UV.y > 0.3 && UV.y < 0.7 && UV.x > 0.04)
    {
        color = vec4(1.0 - lifeLevel, lifeLevel, 0.0, 1.0);
    }
}