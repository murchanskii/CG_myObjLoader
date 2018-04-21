#version 330 core

in vec3 fNormals;
in vec3 fPosition;
in vec3 fLightPos;
in vec2 fTexCoords;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material mtl;
uniform Light sun;
//uniform vec3 viewPos;
uniform sampler2D texture;
uniform bool texture_is_bound;
//uniform sampler2D normalMap;

void main() {
    vec3 viewPos = vec3(0.0f, 0.0f, -3.0f);
    vec3 lightColor = vec3(1.0f);
    vec3 objectColor = vec3(0.847f, 0.067f, 0.431f);

    vec3 ambient = lightColor * mtl.ambient;

    vec3 norm = normalize(fNormals);
    vec3 lightDir = normalize(-sun.direction);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = lightColor * (diff * mtl.diffuse);

    vec3 viewDir = normalize(viewPos - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.shininess);
    vec3 specular = lightColor * (spec * mtl.specular);

    vec4 color;

    if (texture_is_bound)
        color = texture(texture, fTexCoords);
    else
        color = vec4(1.0f);

    gl_FragColor = color * vec4((ambient + diffuse + specular), 1.0f);
}
