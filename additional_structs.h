//
// Created by Murchan on 23/04/2018.
//

#ifndef CG_LAB_ADDITIONAL_STRUCTS_H
#define CG_LAB_ADDITIONAL_STRUCTS_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shininess;

    Material() : ambient(0.4f),
                 diffuse(0.5f),
                 specular(0.5f),
                 shininess(0.8f)
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

struct Shape {
    GLuint vb_id; // vertex buffer id
    GLint num_triangles;
    GLint material_id;
};

static bool file_exists(const std::string &abs_filename) {
    FILE* fp = fopen(abs_filename.c_str(), "rb");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

static bool is_obj_file(const std::string &abs_filename) {

}

#endif //CG_LAB_ADDITIONAL_STRUCTS_H
