#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

uniform mat4 model;
uniform vec3 objectColor;
uniform mat4 projection;
uniform bool useObjectColor;
uniform mat4 view;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 VertexColor;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    VertexColor = useObjectColor ? objectColor : aColor;  // Use objectColor vs use colors embedded in the vertex data.
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
