#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Color;

uniform sampler2D texture1;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool useTexture;
    vec3 color;
};

uniform Material material;

void main() {
    vec3 defaultAmbient = vec3(0.2, 0.2, 0.2); // Default ambient color
    vec3 defaultDiffuse = vec3(0.5, 0.5, 0.5); // Default diffuse color
    vec3 defaultSpecular = vec3(1.0, 1.0, 1.0); // Default specular color
    float defaultShininess = 32.0; // Default shininess

    vec3 ambientColor = material.ambient;
    vec3 diffuseColor = material.diffuse;
    vec3 specularColor = material.specular;
    float shininessValue = material.shininess;
    vec3 materialColor = material.color; // Use material color

    // Assign default values if necessary
    if (ambientColor == vec3(0.0, 0.0, 0.0)) {
        ambientColor = defaultAmbient;
    }
    if (diffuseColor == vec3(0.0, 0.0, 0.0)) {
        diffuseColor = defaultDiffuse;
    }
    if (specularColor == vec3(0.0, 0.0, 0.0)) {
        specularColor = defaultSpecular;
    }
    if (shininessValue == 0.0) {
        shininessValue = defaultShininess;
    }

    vec3 textureColor = vec3(1.0);
    if (material.useTexture) {
        textureColor = vec3(texture(texture1, TexCoords));
    }

    vec3 ambient = ambientColor * textureColor * materialColor * 0.1; // Use material color and reduce ambient intensity

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseColor * diff * textureColor * materialColor * 0.5; // Use material color and reduce diffuse intensity

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
    vec3 specular = specularColor * spec * textureColor * materialColor * 0.3; // Use material color and reduce specular intensity

    vec3 result = ambient + diffuse + specular;

    // Clamp the result to avoid overly bright spots
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 1.0);
}
