#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Light
{
    vec3 pos;
    vec3 color;
    float intensity;
    float radius;
    bool castShadows;
};

uniform sampler2D texture_diffuse;

const int MAX_LIGHTS = 10;
uniform samplerCube depthMap[MAX_LIGHTS];
uniform Light light[MAX_LIGHTS];

uniform int numLights;

uniform vec3 viewPos;

float ShadowCalculation(vec3 fragPos, int textIndex)
{
    // Use the light's radius as the far plane for shadow calculation
    float far_plane = light[textIndex].radius;
    
    vec3 fragToLight = fragPos - light[textIndex].pos;
    float closestDepth = texture(depthMap[textIndex], fragToLight).r;
    // Scale the depth value by the light's radius
    closestDepth *= far_plane;
    
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;        
    
    return shadow;
}

void main()
{          
    vec3 color = texture(texture_diffuse, TexCoords).rgb;
    vec3 normal = normalize(Normal);

    vec3 lighting = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < numLights; ++i)
    {
        vec3 ambient = 0.3 * light[i].color;
        // diffuse
        vec3 lightDir = normalize(light[i].pos - FragPos);
        float diff = max(dot(lightDir, normal), 0.0) * light[i].intensity;
        vec3 diffuse = diff * light[i].color;
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = 0.2;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        vec3 specular = spec * light[i].color;    
        // calculate shadow
        float shadow = ShadowCalculation(FragPos, i);
        lighting += (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    }
    
    FragColor = vec4(lighting, 1.0);
}
