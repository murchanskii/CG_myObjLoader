//
// Created by Murchan on 20/04/2018.
//

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H

#include "additional_structs.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Program.h"

#include "stb_image.h"

class Mesh {
private:
    std::vector<GLfloat> _vertices;
    std::vector<GLfloat> _normals;
    std::vector<GLfloat> _tex_coords;
    Material _material;
    GLuint _VAO;
    GLuint _texture;
    bool texture_bound;

    void setupMesh();
    glm::vec3 calculate_surface_normal(const glm::vec3 &p1,
                                       const glm::vec3 &p2,
                                       const glm::vec3 &p3);
    void set_surface_normal();
public:
    Mesh(const std::vector<GLfloat> &vertices,
         const std::vector<GLfloat> &normals,
         const std::vector<GLfloat> &tex_coords);
    void set_material(GLfloat ambient[3], GLfloat diffuse[3],
                      GLfloat specular[3], GLfloat &shininess,
                      GLint &illum, GLfloat &dissolve);
    void set_texture(GLubyte *tex_image, GLint &tex_w,
                     GLint tex_h, GLint nr_channels);
    void draw(Program &shader_program, MVP_matrix &position);
};


#endif //OPENGL_MESH_H
