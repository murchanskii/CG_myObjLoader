//
// Created by Murchan on 20/04/2018.
//

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Program.h"

#include "stb_image.h"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shininess;

    Material() : ambient(0.2f),
                 diffuse(0.8f),
                 specular(0.5f),
                 shininess(8.0f)
    { }

    Material(glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, GLfloat shine) :
            ambient(amb), diffuse(dif), specular(spec), shininess(shine)
    { }
};

struct MVP_matrix {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    MVP_matrix() : model(1.0f),
                   view(1.0f),
                   projection(1.0f)
    { }

    MVP_matrix(glm::mat4 m, glm::mat4 v, glm::mat4 p) :
            model(m), view(v), projection(p)
    { }
};

class Mesh {
private:
    std::vector<GLfloat> _vertices;
    std::vector<GLfloat> _normals;
    std::vector<GLfloat> _tex_coords;
    Material _material;
    GLuint _VAO;
    GLuint _texture;
    bool texture_bound;

    void normalize_vertices();
    void setupMesh();
    glm::vec3 calculate_surface_normal(const glm::vec3 &p1,
                                       const glm::vec3 &p2,
                                       const glm::vec3 &p3);
    void set_surface_normal();
public:
    Mesh(const std::vector<GLfloat> &vertices,
         const std::vector<GLfloat> &normals,
         const std::vector<GLfloat> &tex_coords,
         Material material);
    Mesh(const std::vector<GLfloat> &vertices,
         const std::vector<GLfloat> &normals,
         const std::vector<GLfloat> &tex_coords);
    void set_texture();
    void set_texture(GLubyte *tex_image, GLint &tex_w,
                     GLint tex_h, GLint nr_channels);
    void draw(Program &shader_program, MVP_matrix &position);
};


#endif //OPENGL_MESH_H
