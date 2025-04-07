#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 0.0, 1.0));
    TexCoord = aTexCoord;
    gl_Position = projection * vec4(FragPos, 1.0);
}
