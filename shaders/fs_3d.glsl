#version 330 core

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

const int MAX_LIGHTS = 10;
uniform samplerCube depthMaps[MAX_LIGHTS];
uniform Light lights[MAX_LIGHTS];

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;

float ShadowCalculation(vec3 fragToLight, vec3 normal, int lightIndex)
{
    // Sample depth from cube map
    float closestDepth = texture(depthMaps[lightIndex], fragToLight).r * lights[lightIndex].radius;

    // Get current linear depth
    float currentDepth = length(fragToLight);

    // Use a small bias for shadow acne prevention
    float lightDirDotNormal = dot(normalize(fragToLight), normalize(normal));
    float angleBias = clamp(0.05 * (1.0 - lightDirDotNormal), 0.0, 0.05);

    // Shadow factor calculation
    float shadow = (currentDepth - (0.5 + angleBias)) > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{    
    vec4 texColor = texture(texture_diffuse, TexCoords);
  
    float shadowFactor = 1.0;

    vec3 norm = normalize(Normal);

    for (int i = 0; i < MAX_LIGHTS; ++i) 
    {
        vec3 lightDir = normalize(lights[i].pos - FragPos);
        float distance = length(lights[i].pos - FragPos);

        // Precompute attenuation
        float attenuation = 1.0 - min(distance / lights[i].radius, 1.0);

        // Avoid calculating shadow if light is too parallel
        float normDotLightDir = dot(norm, lightDir);

        if (normDotLightDir > 0.1 && lights[i].castShadows) 
        {
            vec3 fragToLight = FragPos - lights[i].pos;
            float shadow = ShadowCalculation(fragToLight, norm, i);
            shadowFactor *= (1.0 - shadow * 0.5); // Factor for shadow darkness
        }
    }

    // Avoid excessive clamping, apply shadow if fully occluded
    shadowFactor = max(shadowFactor, 0.5);

    FragColor = texColor * shadowFactor;
}
