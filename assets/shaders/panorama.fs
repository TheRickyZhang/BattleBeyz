#version 330 core
out vec4 FragColor;
in vec3 TexCoords;

uniform sampler2D backgroundTexture;

void main()
{
    vec3 normalizedCoords = normalize(TexCoords); // Normalize the coordinates
    float theta = atan(normalizedCoords.z, normalizedCoords.x); // Horizontal angle
    float phi = acos(normalizedCoords.y); // Vertical angle

    vec2 sphericalCoords;
    sphericalCoords.x = (theta + 3.14159265) / (2.0 * 3.14159265); // Map from [-π, π] to [0, 1]
    sphericalCoords.y = phi / 3.14159265; // Map from [0, π] to [0, 1]

    FragColor = texture(backgroundTexture, sphericalCoords);
}
