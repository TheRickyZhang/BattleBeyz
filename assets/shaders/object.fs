#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 VertexColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D texture1;
uniform vec3 viewPos;

void main()
{
    // Lighting calculations
    float ambientStrength = 0.7;
    vec3 ambient = ambientStrength * lightColor;

    float diffStrength = 0.5;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0) * diffStrength;
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 lighting = ambient + diffuse + specular;

    // Texture sampling
    vec4 texColor = texture(texture1, TexCoords);

    // Combine texture color with vertex color
    vec3 colorEffect = texColor.rgb * VertexColor;

    // Apply lighting to the combined color effect
    vec3 result = lighting * colorEffect;

    FragColor = vec4(result, 1.0);
}
