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
    float falloff;  // Added falloff property
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
    float far_plane = light[textIndex].radius;
    vec3 fragToLight = fragPos - light[textIndex].pos;
    
    float closestDepth = texture(depthMap[textIndex], fragToLight).r;
    closestDepth *= far_plane;
    
    float currentDepth = length(fragToLight);
    float bias = max(0.05 * (1.0 - dot(Normal, fragToLight)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{          
    vec3 color = texture(texture_diffuse, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lighting = vec3(0.0);

    for (int i = 0; i < numLights; ++i)
    {
        vec3 ambient = 0.3 * light[i].color;
        
        // Calculate distance from fragment to light
        float distance = length(FragPos - light[i].pos);
        
        // Calculate smooth attenuation using falloff
        float attenuation = 1.0 - smoothstep(
            light[i].radius - light[i].falloff,
            light[i].radius,
            distance
        );
        
        // Diffuse calculation
        vec3 lightDir = normalize(light[i].pos - FragPos);
        float diff = max(dot(lightDir, normal), 0.0) * light[i].intensity;
        vec3 diffuse = diff * light[i].color;
        
        // Specular calculation
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        vec3 specular = spec * light[i].color;
        
        // Shadow calculation
        float shadow = ShadowCalculation(FragPos, i);
        
        // Combine all components with attenuation
        vec3 result = (ambient + 
                      (1.0 - shadow) * 
                      attenuation * 
                      (diffuse + specular)) * color;
        
        lighting += result;
    }
    
    FragColor = vec4(lighting, 1.0);
}
