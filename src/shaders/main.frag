#version 330 core

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float kd, ks, shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;
out vec4 color;

uniform DirLight dirLight;
uniform Material material; 
uniform vec3 viewPos;  //the position of camera
uniform sampler2D shadowMap;
uniform sampler2D snowMap;
uniform float factor;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
float CalcShadowDirLight(DirLight light, vec4 FragPosLightSpace, vec3 normal);

float shadow = 0.0;  //global variable

void main()
{    
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	CalcShadowDirLight(dirLight, FragPosLightSpace, norm);     
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
    color = vec4(result, 1.0);
	
}

float CalcShadowDirLight(DirLight light, vec4 FragPosLightSpace, vec3 normal)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	vec3 lightDir = normalize(-light.direction);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.0005);
	//float bias = 0.005 * tan(acos(clamp(dot(normal, lightDir), 0, 1)));
	//float bias = 0;
    // PCF, or percentage-closer filtering
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
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
	//vec3 specular = light.specular * spec * vec3(texture(material.diffuse1, TexCoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoord));
	
	//shadow = 0.0;  //for debugging
    return ((1 - material.kd - material.ks) * ambient 
	   + (1-shadow) * (material.kd * diffuse + material.ks * specular));
	

	//following code is for debugging
	//return vec3(texture(material.diffuse1, TexCoord));	
	//return (ambient + (1-shadow) * (diffuse + specular));
	//return diffuse;    
	//return specular;
	//return ambient;
	//return light.specular;
	//return vec3(0.1) + vec3(0.4) + vec3(1);
	//return specular + vec3(1000.0);
	//return diffuse + specular + ambient;
	//return vec3(material.kd);
	//return vec3(material.ks);
	//return vec3(material.shininess);
	//return vec3(spec);
	//return vec3(-1);
	//return vec3(pow(0, 0));
	//return vec3((dot(viewDir, reflectDir)));
}