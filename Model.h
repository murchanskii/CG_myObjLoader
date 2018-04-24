//
// Created by Murchan on 20/04/2018.
//

#ifndef OPENGL_MODEL_H
#define OPENGL_MODEL_H

#include "Mesh.h"

#include "tiny_obj_loader.h"

class Model {
private:
    std::vector<Mesh> _meshes;
    std::string _directory_to_obj;

    void load_texture(Mesh &mesh, tinyobj::material_t &material);
    void load_model(std::string path);
    std::string get_base_directory(const std::string &filepath);
    void create_model(std::vector<std::vector<GLfloat>> &vertices,
                      std::vector<std::vector<GLfloat>> &normals,
                      std::vector<std::vector<GLfloat>> &texcoords,
                      std::vector<tinyobj::material_t> &materials,
                      std::vector<GLint> &material_ids);
    void center_model_by_dimension(GLuint &first_idx,
                                   std::vector<std::vector<GLfloat>> &vertices);
public:
    MVP_matrix mvp_matrix;

    Model(std::string path);
    void draw(Program &shader_program, MVP_matrix &position);
};

#endif //OPENGL_MODEL_H

