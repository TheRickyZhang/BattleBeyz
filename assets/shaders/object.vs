#version 330 core
#pragma optimize(off)

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

// Global
uniform mat4 projection;
uniform mat4 view;

// Per Object
uniform mat4 model;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    VertexColor = aColor;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
