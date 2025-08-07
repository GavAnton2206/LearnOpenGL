#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

  
out vec4 FragColor;
  
uniform Material material;
uniform Light light; 
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 objectSpecular = vec3(texture(material.specular, TexCoords));
    vec3 specular = light.specular * spec * (objectSpecular);  

    vec3 emission = 3.0 * vec3(texture(material.emission, TexCoords)) * max((vec3(0.6f) - diff), 0.0);  

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}