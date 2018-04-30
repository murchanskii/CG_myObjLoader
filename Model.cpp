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

void Model::create_model(std::vector<std::vector<GLfloat>> &vertices,
                         std::vector<std::vector<GLfloat>> &normals,
                         std::vector<std::vector<GLfloat>> &texcoords,
                         std::vector<tinyobj::material_t> &materials,
                         std::vector<GLint> &material_ids) {
    for (GLuint d = 0; d < 3; ++d)
        center_model_by_dimension(d, vertices);

    GLuint num_shapes = vertices.size();
    GLfloat max_elements[num_shapes];
    for (GLuint s = 0; s < num_shapes; ++s) {
        long long int max_element_id = std::distance(vertices[s].begin(),
                                 std::max_element(vertices[s].begin(),
                                          vertices[s].end(),
                                          [](GLfloat a, GLfloat b) {
                                              return (std::abs(a) < std::abs(b));
                                          }));
        max_elements[s] = vertices[s][max_element_id];
    }
    GLfloat *maxptr = std::max_element(max_elements, max_elements + num_shapes);
    GLfloat max = *maxptr;
    for (GLuint s = 0; s < num_shapes; ++s) {
        std::transform(vertices[s].begin(), vertices[s].end(), vertices[s].begin(),
                       [max](GLfloat elem) { return elem / max; });
        Mesh mesh = Mesh(vertices[s], normals[s], texcoords[s]);
        _meshes.emplace_back(mesh);
        if (material_ids[s] >= 0)
            load_texture(_meshes[s], materials[material_ids[s]]);
    }
}

void Model::center_model_by_dimension(GLuint &first_idx,
                                      std::vector<std::vector<GLfloat>> &vertices) {
    GLuint num_shapes = vertices.size();

    GLfloat min, max;
    min = max = vertices[0][first_idx];
    for (GLuint s = 0; s < num_shapes; ++s) {
        for (GLuint i = first_idx; i < vertices[s].size(); i += 3) {
            min = std::min(min, vertices[s][i]);
            max = std::max(max, vertices[s][i]);
        }
    }
    GLfloat diff = (max + min) * 0.5f;
    for (GLuint s = 0; s < num_shapes; ++s) {
        for (GLuint i = first_idx; i < vertices[s].size(); i += 3) {
            vertices[s][i] -= diff;
        }
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
        materials.emplace_back(tinyobj::material_t());
        mtl_default = true;
    }

    std::vector<GLfloat> vertices[shapes.size()];
    std::vector<GLfloat> normals[shapes.size()];
    std::vector<GLfloat> texcoords[shapes.size()];
    GLint material_ids[shapes.size()];

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
        GLint index = mtl_default ? 0 : shapes[s].mesh.material_ids[0];
        material_ids[s] = index;
    }
    std::vector<std::vector<GLfloat>> all_vertices(vertices, vertices + shapes.size());
    std::vector<std::vector<GLfloat>> all_normals(normals, normals + shapes.size());
    std::vector<std::vector<GLfloat>> all_texcoords(texcoords, texcoords + shapes.size());
    std::vector<GLint> all_material_ids(material_ids, material_ids + shapes.size());
    create_model(all_vertices, all_normals, all_texcoords, materials, all_material_ids);
}

void Model::load_texture(Mesh &mesh, tinyobj::material_t &material) {
    if (material.name.length() > 0) {
        mesh.set_material(material.ambient,
                          material.diffuse,
                          material.specular,
                          material.shininess,
                          material.illum,
                          material.dissolve);
    }
    GLint tex_w, tex_h, nr_channels;
    std::string filename = _directory_to_obj + material.diffuse_texname;

     unsigned char *tex_image = stbi_load(filename.c_str(),
                                   &tex_w, &tex_h, &nr_channels, 0);

    GLfloat border_color[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    mesh.set_texture(tex_image, tex_w, tex_h, nr_channels);
}