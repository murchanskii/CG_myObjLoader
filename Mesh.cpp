//
// Created by Murchan on 20/04/2018.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"

Mesh::Mesh(const std::vector<GLfloat> &vertices,
           const std::vector<GLfloat> &normals,
           const std::vector<GLfloat> &tex_coords,
           Material material) {
    _material = material;
    _vertices = vertices;
    _tex_coords = tex_coords;
    if (normals.empty())
        set_surface_normal();
    else
        _normals = normals;

    normalize_vertices();
    setupMesh();
}

Mesh::Mesh(const std::vector<GLfloat> &vertices,
           const std::vector<GLfloat> &normals,
           const std::vector<GLfloat> &tex_coords) {
    _material = Material();
    _vertices = vertices;
    _tex_coords = tex_coords;
    if (normals.empty())
        set_surface_normal();
    else
        _normals = normals;

    normalize_vertices();
    setupMesh();
}

void Mesh::set_texture() {
    texture_bound = false;
}

void Mesh::set_texture(GLubyte *tex_image, GLint &tex_w,
                       GLint tex_h, GLint nr_channels) {
    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _texture);
    if (tex_image) {
        texture_bound = true;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(tex_image);
        std::cout << "SUCCESS:: Texture has been loaded" << std::endl;
    } else {
        texture_bound = false;
        std::cout << "ERROR:: Failed to load texture" << std::endl;
    }
    glDisable(GL_TEXTURE_2D);
    glBindVertexArray(0);
}

void Mesh::set_surface_normal() {
    for (GLuint i = 0; i < _vertices.size(); i += 9) {
        glm::vec3 normal(1.0f), p1(1.0f), p2(1.0f), p3(1.0f);
        // p1
        p1[0] = _vertices[i + 0];
        p1[1] = _vertices[i + 1];
        p1[2] = _vertices[i + 2];
        // p2
        p2[0] = _vertices[i + 3];
        p2[1] = _vertices[i + 4];
        p2[2] = _vertices[i + 5];
        // p3
        p3[0] = _vertices[i + 6];
        p3[1] = _vertices[i + 7];
        p3[2] = _vertices[i + 8];
        normal = calculate_surface_normal(p1, p2, p3);
        for (GLuint j = 0; j < 3; ++j) {
            _normals.emplace_back(normal.x);
            _normals.emplace_back(normal.y);
            _normals.emplace_back(normal.z);
        }
    }
}

glm::vec3 Mesh::calculate_surface_normal(const glm::vec3 &p1,
                                         const glm::vec3 &p2,
                                         const glm::vec3 &p3) {
    glm::vec3 u = p2 - p1;
    glm::vec3 v = p3 - p1;
    glm::vec3 normal(1.0f);
    normal.x = u.y * v.z - u.z * v.y;
    normal.y = u.z * v.x - u.x * v.z;
    normal.z = u.x * v.y - u.y * v.x;
    return normal;
}

void Mesh::normalize_vertices() {
    long long int max_element_id = std::distance(_vertices.begin(),
                                         std::max_element(_vertices.begin(),
                                                          _vertices.end(),
                                                          [](GLfloat a, GLfloat b) {
                                                              return (std::abs(a) < std::abs(b));
                                                          }));
    GLfloat max_element = std::abs(_vertices[max_element_id]);
    std::transform(_vertices.begin(), _vertices.end(), _vertices.begin(),
                   [max_element] (GLfloat elem) { return elem / max_element; });
    for (GLint i = 1; i < _vertices.size(); i += 3) {
        _vertices[i] -= 0.5f;
    }
}

void Mesh::setupMesh() {
    texture_bound = false;
    GLuint vertexVBO, textureVBO, normalVBO;

    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &normalVBO);
    glGenVertexArrays(1, &_VAO);
    glGenTextures(1, &_texture);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat),
                 _vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glDisable(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(GLfloat),
                 _normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glDisable(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, _tex_coords.size() * sizeof(GLfloat),
                 _tex_coords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glDisable(GL_ARRAY_BUFFER);

    glBindVertexArray(0);
}

void Mesh::draw(Program &shader_program, MVP_matrix &position) {
    shader_program.Use();

    shader_program.uniformMatrix4fv("model", position.model);
    shader_program.uniformMatrix4fv("view", position.view);
    shader_program.uniformMatrix4fv("projection", position.projection);
    shader_program.uniform3fv("mtl.ambient", _material.ambient);
    shader_program.uniform3fv("mtl.diffuse", _material.diffuse);
    shader_program.uniform3fv("mtl.specular", _material.specular);
    shader_program.uniform1f("mtl.shininess", _material.shininess);
    shader_program.uniform1f("texture_is_bound", texture_bound);
    shader_program.uniform3fv("sun.direction", glm::vec3(-0.2f, -1.0f, -0.3f));

    if (texture_bound)
        glBindTexture(GL_TEXTURE_2D, _texture);
    glBindVertexArray(_VAO);
    GLsizei vertices_count = _vertices.size()/3;
    glDrawArrays(GL_TRIANGLES, 0, vertices_count);
    glBindVertexArray(0);
    if (texture_bound)
        glDisable(GL_TEXTURE_2D);
}