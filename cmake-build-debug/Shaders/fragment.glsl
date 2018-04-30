#version 330 core

in vec3 fNormals;
in vec3 fPosition;
in vec3 fLightPos;
in vec2 fTexCoords;

struct GlobalIlluminance {
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
    float dissolve;
};

#define NR_POINT_LIGHTS 2
uniform GlobalIlluminance sun[NR_POINT_LIGHTS];

uniform Material mtl;
uniform sampler2D texture;
uniform bool texture_is_bound;

vec3 CalcGlobalLight(GlobalIlluminance light, vec3 normal, vec3 viewDir) {
    vec3 lightColor = vec3(1.0f);
    vec3 lightDir = normalize(-light.direction);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(lightDir, reflectDir), 0.0f), mtl.shininess);

    vec3 color;
        if (texture_is_bound)
            color = vec3(texture(texture, fTexCoords));
        else
            color = mtl.diffuse + mtl.specular;

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular) * color * lightColor;
}

void main() {
    vec3 viewPos = vec3(0.0f, 0.0f, -3.0f);

    /*vec3 color;
    if (texture_is_bound)
        color = vec3(texture(texture, fTexCoords));
    else
        color = vec3(1.0f);

    vec3 ambient = lightColor * mtl.ambient * color;*/

    vec3 norm = normalize(fNormals);
    /*vec3 lightDir = normalize(-sun.direction);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = lightColor * mtl.diffuse * diff * color;*/

    vec3 viewDir = normalize(viewPos - fPosition);
    /*vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.shininess);
    vec3 specular = lightColor * mtl.specular * spec * mtl.specular;*/

    vec3 result;
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += CalcGlobalLight(sun[i], norm, viewDir);
    gl_FragColor = vec4(result, mtl.dissolve);
}
