#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 shadowMatrix; // One face at a time

out vec4 FragPos; // World space position

void main()
{
    FragPos = model * vec4(aPos, 1.0);
    gl_Position = shadowMatrix * FragPos;
}
