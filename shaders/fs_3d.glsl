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

uniform sampler2D texture_diffuse;

const int MAX_LIGHTS = 10;
uniform samplerCube depthMap[MAX_LIGHTS];
uniform Light light[MAX_LIGHTS];

uniform int numLights;

float ShadowCalculation(vec3 fragPos, int index)
{
    float far_plane = 25.0f;

    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - light[index].pos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap[index], fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= 25.0f;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{          
    vec3 viewPos = vec3(1.0, 1.0, 1.0);

    vec3 color = texture(texture_diffuse, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;

    vec3 lighting = ambient;

    for (int i = 0; i < numLights; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(light[i].pos - FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * lightColor;
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = 0.0;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        vec3 specular = spec * lightColor;    
        // calculate shadow
        float shadow = ShadowCalculation(FragPos, i);
        lighting += (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    }
    
    FragColor = vec4(lighting, 1.0);
}
