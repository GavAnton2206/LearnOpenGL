#version __VERSION__

#define PI 3.14159265359

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    sampler2D diffuse;
    sampler2D specularMap;
    sampler2D emission;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
  
    vec3 ambient; // BP
    vec3 diffuse;
    vec3 specular;

    vec3 color; // PBR
    float intensity;
};  

struct PointLight {    
    vec3 position;
    
    float constant; // BP
    float linear;
    float quadratic;  

    vec3 ambient; // BP
    vec3 diffuse;
    vec3 specular;

    vec3 color; // PBR
    float intensity;
};

struct SpotLight {
    vec3  position;
    vec3  direction;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 color; // PBR
    float intensity;
    float radius;
};  

uniform vec2 scaleUV;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 1
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
  
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

out vec4 FragColor;

float DistributionGGX(vec3 N, vec3 H, float a);
float GeometrySchlickGGX(float NdotV, float k);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

vec3 BRDF_PBR(vec3 N, vec3 V, vec3 L, vec3 H, vec3 radiance)
{
    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float G = GeometrySmith(N, V, L, k);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcDirLight(vec3 N, vec3 V) 
{
    vec3 L = normalize(-dirLight.direction);
    vec3 H = normalize(V + L);

    vec3 radiance = dirLight.intensity * dirLight.color;

    return BRDF_PBR(N, V, L, H, radiance);
}  

vec3 CalcPointLight(vec3 N, vec3 V, int i)
{
    vec3 L = normalize(pointLights[i].position - FragPos);
    vec3 H = normalize(V + L);

    float distance = length(pointLights[i].position - FragPos);
    float attenuation = 1.0 / (distance * distance);

    vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

    return BRDF_PBR(N, V, L, H, radiance);
} 

vec3 CalcSpotLight(vec3 N, vec3 V, int i)
{    
    vec3 L = normalize(spotLights[i].position - FragPos);
    vec3 H = normalize(V + L);

    float distance = length(spotLights[i].position - FragPos);
    float attenuation = 1.0 / (distance * distance);

    float theta = dot(L, normalize(-spotLights[i].direction));
    float epsilon = spotLights[i].cutOff  - spotLights[i].outerCutOff;
    float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

    vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensity;

    return BRDF_PBR(N, V, L, H, radiance);
} 


float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L0 = vec3(0.0);

    L0 += CalcDirLight(N, V);

    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        L0 += CalcPointLight(N, V, i);
    }

    for(int i = 0; i < NR_SPOT_LIGHTS; ++i)
    {
        L0 += CalcSpotLight(N, V, i);
    }

    vec3 ambient = vec3(0.03) * material.albedo * material.ao;
    vec3 color = ambient + L0;

    
    FragColor = vec4(color, 1.0);
}