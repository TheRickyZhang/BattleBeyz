#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos; // Directly pass the normalized coordinates
    gl_Position = vec4(aPos, 1.0); // Output position to the clip space
}
