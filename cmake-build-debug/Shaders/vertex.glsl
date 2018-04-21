#version 330 core

in vec3 pos;
in vec3 normals;
in vec2 texCoords;

out vec3 fNormals;
out vec3 fPosition;
out vec3 fLightPos;
out vec2 fTexCoords;

//uniform vec3 lightPos;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() {
    vec3 lightPos = vec3(0.5f, 0.0f, 1.0f);
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    fPosition = vec3(view * model * vec4(pos, 1.0f));
    fNormals = mat3(transpose(inverse(view * model))) * normals;
    fLightPos = vec3(view * vec4(lightPos, 1.0f));
    fTexCoords = vec2(texCoords.x, 1.0 - texCoords.y);
}
