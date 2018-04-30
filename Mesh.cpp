//
// Created by Murchan on 20/04/2018.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"

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

    setupMesh();
}

void Mesh::set_material(GLfloat ambient[3], GLfloat diffuse[3],
                        GLfloat specular[3], GLfloat &shininess,
                        GLint &illum, GLfloat &dissolve) {
    texture_bound = false;
    _material.illum = illum;
    _material.dissolve = dissolve;
    switch (_material.illum) {
        case 2:
            _material.specular = glm::vec3(specular[0], specular[1], specular[2]);
            _material.shininess = shininess;
        case 1:
        default:
            _material.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
        case 0:
            _material.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
            break;
    }
}

void Mesh::set_texture(GLubyte *tex_image, GLint &tex_w,
                       GLint tex_h, GLint nr_channels) {
    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _texture);
    if (tex_image) {
        texture_bound = true;
        if (nr_channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_image);
        else if (nr_channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_image);
        else  {
            stbi_image_free(tex_image);
            texture_bound = false;
            std::cout << "WARNING:: Something wrong with nr_channels" << std::endl;
            return;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(tex_image);
        std::cout << "SUCCESS:: Texture has been loaded" << std::endl;
    } else {
        texture_bound = false;
        std::cout << "WARNING:: No texture was provided" << std::endl;
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
    if (_material.dissolve < 1.0f)
        glDisable(GL_DEPTH_TEST);
    shader_program.Use();

    shader_program.uniformMatrix4fv("model", position.model);
    shader_program.uniformMatrix4fv("view", position.view);
    shader_program.uniformMatrix4fv("projection", position.projection);

    shader_program.uniform3fv("mtl.ambient", _material.ambient);
    shader_program.uniform3fv("mtl.diffuse", _material.diffuse);
    shader_program.uniform3fv("mtl.specular", _material.specular);
    shader_program.uniform1f("mtl.shininess", _material.shininess);
    shader_program.uniform1f("mtl.dissolve", _material.dissolve);
    shader_program.uniform1f("texture_is_bound", texture_bound);

    shader_program.uniform3fv("sun[0].direction", glm::vec3(0.0f, 5.0f, 5.0f));
    shader_program.uniform3fv("sun[0].ambient", glm::vec3(0.2f));
    shader_program.uniform3fv("sun[0].diffuse", glm::vec3(0.4f));
    shader_program.uniform3fv("sun[0].specular", glm::vec3(0.2f));
    shader_program.uniform3fv("sun[1].direction", glm::vec3(0.0f, -5.0f, -5.0f));
    shader_program.uniform3fv("sun[1].ambient", glm::vec3(0.2f));
    shader_program.uniform3fv("sun[1].diffuse", glm::vec3(0.4f));
    shader_program.uniform3fv("sun[1].specular", glm::vec3(0.2f));

    if (texture_bound)
        glBindTexture(GL_TEXTURE_2D, _texture);
    glBindVertexArray(_VAO);
    GLsizei vertices_count = _vertices.size()/3;
    glDrawArrays(GL_TRIANGLES, 0, vertices_count);
    glBindVertexArray(0);
    if (texture_bound)
        glDisable(GL_TEXTURE_2D);

    if (_material.dissolve < 1.0f)
        glEnable(GL_DEPTH_TEST);
}