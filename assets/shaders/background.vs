#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform float wrapFactor; // Number of repetitions

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords.x = aTexCoords.x * wrapFactor;
    TexCoords.y = (1.0 - aTexCoords.y) * wrapFactor;
}
