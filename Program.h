//
// Created by murchan on 08.02.2018.
//

#ifndef OPENGL_SHADERPROGRAM_H
#define OPENGL_SHADERPROGRAM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Program {
private:
    GLuint shdrProgram;
    void readFile(const GLchar *, std::string &);
    GLuint createShader(const char *, GLuint);
    void checkShaderCompilation(GLuint &);
    void checkProgramLinking(GLuint &);
public :
    void uniform3fv(std::string, glm::vec3);
    void uniformMatrix4fv(std::string, glm::mat4);
    void uniform1f(std::string, GLfloat);
    void uniform1i(std::string, GLint);
    Program(const GLchar *, const GLchar *);
    GLuint getProgram();
    void Use();
};


#endif //OPENGL_SHADERPROGRAM_H
