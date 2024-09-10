#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D backgroundTexture;
uniform float time;

void main()
{
    vec2 uv = TexCoords;
    uv.x += time * 0.03;
    uv.y += time * 0.03;
    // 0.5 is for faded background
    FragColor = texture(backgroundTexture, uv) * 0.5;
}
