#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D backgroundTexture;
uniform float time;

void main()
{
    // if (gl_FragCoord.x > 400.0) {
    //     FragColor = vec4(1.0, 1.0, 0.0, 1.0);  // Yellow if x > 400
    // } else {
    //     FragColor = vec4(0.0, 0.0, 1.0, 1.0);  // Blue if x <= 400
    // }

    FragColor = vec4(gl_FragCoord.x / 800.0, gl_FragCoord.y / 600.0, 0.0, 1.0);
    // Scrolling effect. TODO: Add movement speed as a parameter as well
    vec2 uv = TexCoords;
    uv.x += time * 0.03;
    uv.y += time * 0.03;

    // set 0.5 for faded background
    FragColor = texture(backgroundTexture, uv) * 0.5;
}
