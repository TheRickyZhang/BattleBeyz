#version 330 core

out vec4 FragColor;

in vec3 FragPos;         // Fragment position in world space
in vec3 Normal;          // Normal vector at the fragment
in vec2 TexCoords;       // Texture coordinates
in vec3 VertexColor;     // Vertex color passed from the vertex shader

// Global
uniform sampler2D texture1;                      // Texture sampler
uniform vec3 viewPos;                            // Position of the camera/viewer

uniform int lightType = 0;                       // 0: Directional light, 1: Point light (TBD 2: Spotlight)
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);   // Color of the light source
uniform vec3 lightPos = vec3(0.0, -1.0, 0.0);    // (point light pos or directional light dir)

// Per Object
uniform vec3 tint = vec3(-1.0, -1.0, -1.0);         // Tint color for additional coloring
// uniform vec2 textureScale = vec2(1.0, 1.0);      // Scale for texture repetition

void main()
{
    // Lighting strengths
    float ambientStrength = 0.8;                // Everything
    float diffStrength = 0.3;                   // In direction of light
    float specularStrength = 0.1;               // Shiny/metallic reflection

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Default values
    vec3 norm = normalize(Normal);              // Ensure normal is normalized
    vec3 lightDir;                              // Direction of the light
    float attenuation = 1.0;                    // Default attenuation (used only for point lights)

    // Light type handling
    if (lightType == 0) {
        // Directional light: Use lightPos as a normalized direction
        lightDir = normalize(lightPos);
    } else if (lightType == 1) {
        // Point light: Use lightPos as a world-space position
        lightDir = normalize(lightPos - FragPos);
        float distance = length(lightPos - FragPos); // Distance to the light source
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance); // Attenuation formula
    }

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0) * diffStrength;
    vec3 diffuse = attenuation * diff * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos); // Direction from fragment to the camera
    vec3 reflectDir = reflect(-lightDir, norm);  // Reflection vector for specular highlights
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Specular component (higher power = sharper highlights)
    vec3 specular = attenuation * specularStrength * spec * lightColor;

    // Combine ambient, diffuse, and specular lighting
    vec3 lighting = ambient + diffuse + specular;

    vec4 texColor = texture(texture1, TexCoords);

    // Texture, vertex, and tinting can all affect the final color
    vec3 colorEffect = texColor.rgb * VertexColor * tint;

    // Apply lighting to the combined color effect
    vec3 result = lighting * colorEffect;

    FragColor = vec4(result, 1.0); // Output the final fragment color

    // Future Spotlight Logic (Commented Out)
    // vec3 spotlightDir;        // Direction of the spotlight
    // float spotlightCutoff;    // Cosine of the spotlight cutoff angle
    // float theta = dot(lightDir, normalize(spotlightDir));
    // if (theta > spotlightCutoff) {
    //     // Inside the spotlight cone
    // } else {
    //     // Outside the spotlight cone
    // }
}
