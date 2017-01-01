#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
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
out vec4 color;

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
uniform DirLight dirLight;
uniform Material material; 
uniform vec3 viewPos;  //the position of camera

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
    /*
    vec4 tex_specular_color = texture(texture_specular1, TexCoord);
	if (tex_specular_color.x < 0.1)
    { color = texture(texture_diffuse1, TexCoord); }
	else
    { color = 0.7 * texture(texture_diffuse1, TexCoord) + 0.3 * texture(texture_specular1, TexCoord);}
	*/
	/*
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	*/

	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	color = vec4(result, 1.0);

}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    //return (ambient + diffuse + specular);
	return (ambient + diffuse);
}
