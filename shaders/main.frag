#version 330 core

/* STRUCTS: Scene layout */
/* MATERIAL: The variables about diffuse, specular and shininess.
**     All members here are related to light settings */
struct Material
{
    // Sampler variables
    sampler2D diffuse1;
    sampler2D specular1;

    // The coefficients of diffuse, specular and shininess
    float kd, ks, shininess;
}; 

/* DirLight: The variables about light settings
**     Only parallel light is added.
**     The variables used are @direction and other material settings */
struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

/***************
** IN VEC
** @param FragPos: The position of fragments
** @param Normal: The normal of vertices
** @param TexCoord: The coordinates of textures 
** @param FragPosLightSpace: The light space of given fragment position 
******************************/
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

/* OUT VEC
** @param color: output the color vector */
out vec4 color;

/****************
** UNIFORM
** @param dirLight: The direction of parallel light
** @param material: The material setting
** @param viewPos: The view vector
** @param shadowMap: The shadow map pf the current scene
** @param snowMap: The snow map (will be mixed after snow)
** @param factor: The mixed factor
****************/
uniform DirLight dirLight;
uniform Material material; 
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform sampler2D snowMap;
uniform float factor;

/* Function prototypes
** Compute light direction and shadow light direction */
vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir);
float ComputeShadowDirLight(DirLight light, vec4 FragPosLightSpace, vec3 normal);

/* GLOBAL variable (SHADOW) */
float shadow = 0.0;

void main()
{
    // Compute shadow light direction
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    ComputeShadowDirLight(dirLight, FragPosLightSpace, norm);     
    vec3 result = ComputeDirLight(dirLight, norm, viewDir);
    
    // Output color
    color = vec4(result, 1.0);
}

/**************
** Calcculates the shadow light direction
** Given parameters:
** @param light: The parallel light
** @param FragPosLightSpace: The light space of given fragment position
** @param normal: The normals of vertices
**************/
float ComputeShadowDirLight(DirLight light, vec4 FragPosLightSpace, vec3 normal)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Declare and compute depth
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(-light.direction);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.0005);

    // Compute shadow results
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0) shadow = 0.0;

    // Return the shadow result
    return shadow;
}

/**************
** Calculates the color when using a directional light.
** Given parameters:
** @param light: The parallel light
** @param normal: The normals of vertices
** @param vieDir: The current view direction vector 
**************/
vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Combine results
    vec3 ambient = light.ambient * vec3((1 - factor) * texture(material.diffuse1, TexCoord) + factor * texture(snowMap, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3((1 - factor) * texture(material.diffuse1, TexCoord) + factor * texture(snowMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(1.0);

    return ((1 - material.kd - material.ks) * ambient 
          + (1 - shadow) * (material.kd * diffuse + material.ks * specular));
}