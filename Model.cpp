//
// Created by Murchan on 20/04/2018.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "Model.h"

std::string Model::get_base_directory(const std::string &filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

Model::Model(std::string path) {
    load_model(std::move(path));
    std::cout << "SUCCESS:: Model has been loaded" << std::endl;
}

void Model::draw(Program &shader_program, MVP_matrix &position) {
    for (auto &mesh : _meshes) {
        mesh.draw(shader_program, position);
    }
}

void Model::load_model(std::string path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool mtl_default = false;

    _directory_to_obj = get_base_directory(path);
    if (_directory_to_obj.empty()) {
        _directory_to_obj = ".";
    }
#ifdef _WIN32
    _directory_to_obj += "\\";
#else
    _directory_to_obj += "/";
#endif

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                                path.c_str(), _directory_to_obj.c_str());
    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    if (materials.empty()) {
        tinyobj::material_t material;
        tinyobj::InitMaterial(&material);
        materials.emplace_back(material);
        mtl_default = true;
    }

    std::vector<GLfloat> vertices[shapes.size()];
    std::vector<GLfloat> normals[shapes.size()];
    std::vector<GLfloat> texcoords[shapes.size()];

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; ++v) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                if (idx.vertex_index != -1) {
                    vertices[s].emplace_back(attrib.vertices[3 * idx.vertex_index + 0]);
                    vertices[s].emplace_back(attrib.vertices[3 * idx.vertex_index + 1]);
                    vertices[s].emplace_back(attrib.vertices[3 * idx.vertex_index + 2]);
                }
                if (idx.normal_index != -1) {
                    normals[s].emplace_back(attrib.normals[3 * idx.normal_index + 0]);
                    normals[s].emplace_back(attrib.normals[3 * idx.normal_index + 1]);
                    normals[s].emplace_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                if (idx.texcoord_index != -1) {
                    texcoords[s].emplace_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texcoords[s].emplace_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
            index_offset += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }
        if (mtl_default) {
            Mesh mesh(Mesh(vertices[s], normals[s], texcoords[s]));
            _meshes.emplace_back(mesh);
        }
        else {
            Material material = get_materials(materials[s]);
            Mesh mesh(Mesh(vertices[s], normals[s], texcoords[s], material));
            _meshes.emplace_back(mesh);
        }
        load_texture(_meshes[s], materials[s]);
    }
}

void Model::load_texture(Mesh &mesh, tinyobj::material_t &material) {
    if (material.diffuse_texname.empty()) {
        mesh.set_texture();
        return;
    }
    GLint tex_w, tex_h, nr_channels;
    std::string filename = _directory_to_obj + material.diffuse_texname;

    GLubyte *tex_image = stbi_load(filename.c_str(),
                                   &tex_w, &tex_h, &nr_channels, 0);

    GLfloat border_color[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    mesh.set_texture(tex_image, tex_w, tex_h, nr_channels);
}

Material Model::get_materials(tinyobj::material_t &material) {
    Material result;
    result.shininess = material.shininess;
    for (GLuint i = 0; i < 3; ++i) {
        result.ambient[i] = material.ambient[i];
        result.diffuse[i] = material.diffuse[i];
        result.specular[i] = material.specular[i];
    }
    return result;
}