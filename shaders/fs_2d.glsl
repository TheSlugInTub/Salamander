#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec4 color;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    vec4 finalColor = texColor * color;

    // Output the final color
    FragColor = finalColor;
}
