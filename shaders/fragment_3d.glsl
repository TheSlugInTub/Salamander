#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;

void main()
{    
    vec4 texColor = texture(texture_diffuse, TexCoords);

    FragColor = vec4(texColor.rgb, texColor.a);
}
