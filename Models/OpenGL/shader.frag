#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial);

void main()
{
    vec3 diffuseMaterial = texture(material.diffuse, TexCoords).rgb;
    vec3 specularMaterial = texture(material.specular, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, diffuseMaterial, specularMaterial);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, viewDir, diffuseMaterial, specularMaterial);

    result += CalcSpotLight(spotLight, norm, viewDir, diffuseMaterial, specularMaterial);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);

    vec3 ambient = light.ambient * diffuseMaterial;
    vec3 diffuse = light.diffuse * diff * diffuseMaterial;
    vec3 specular = light.specular * spec * specularMaterial;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * diffuseMaterial;
    vec3 diffuse = light.diffuse * diff * diffuseMaterial;
    vec3 specular = light.specular * spec * specularMaterial;

    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuseMaterial, vec3 specularMaterial)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * diffuseMaterial;
    vec3 diffuse = light.diffuse * diff * diffuseMaterial;
    vec3 specular = light.specular * spec * specularMaterial;

    ambient *= attenuation; 
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}