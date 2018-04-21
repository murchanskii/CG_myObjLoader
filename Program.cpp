//
// Created by murchan on 08.02.2018.
//

#include "Program.h"

void Program::uniform1i(std::string var_name, GLint data) {
    GLint location = glGetUniformLocation(shdrProgram, var_name.c_str());
    glUniform1i(location, data);
}

void Program::uniform1f(std::string var_name, GLfloat data) {
    GLint location = glGetUniformLocation(shdrProgram, var_name.c_str());
    glUniform1f(location, data);
}

void Program::uniform3fv(std::string var_name, glm::vec3 data) {
    GLint location = glGetUniformLocation(shdrProgram, var_name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(data));
}

void Program::uniformMatrix4fv(std::string var_name, glm::mat4 data) {
    GLint location = glGetUniformLocation(getProgram(), var_name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
}

GLuint Program::getProgram() {
    return shdrProgram;
}

void Program::Use() {
    glUseProgram(shdrProgram);
}

Program::Program(const GLchar *vertexPath, const GLchar *fragmentPath) {
    std::string vCode, fCode;
    unsigned int n_shdrs = 2;
    readFile(vertexPath, vCode);
    readFile(fragmentPath, fCode);
    GLuint shaders[n_shdrs];
    shaders[0] = createShader(vCode.c_str(), GL_VERTEX_SHADER);
    shaders[1] = createShader(fCode.c_str(), GL_FRAGMENT_SHADER);

    shdrProgram = glCreateProgram();
    glBindAttribLocation(shdrProgram, 0, "pos");
    glBindAttribLocation(shdrProgram, 1, "color");
    for (int i = 0; i < n_shdrs; i++)
        glAttachShader(shdrProgram, shaders[i]);
    glLinkProgram(shdrProgram);
    checkProgramLinking(shdrProgram);
    for (int i = 0; i < n_shdrs; i++)
        glDeleteShader(shaders[i]);
}

void Program::readFile(const GLchar *filePath, std::string &fileCode) {
    std::ifstream shdrFile;
    try {
        shdrFile.open(filePath);
        std::stringstream shdrStream;
        shdrStream << shdrFile.rdbuf();
        shdrFile.close();
        fileCode = shdrStream.str();
    }
    catch(std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}

GLuint Program::createShader(const char *shader_code, GLuint shader_type) {
    GLuint shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_code, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader);
    return shader;
}

void Program::checkShaderCompilation(GLuint &shader) {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" <<
                  infoLog << std::endl;
    }
    else
        std::cout << "SUCCESS:: Shader has been compiled" << std::endl;
}

void Program::checkProgramLinking(GLuint &program) {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" <<
                  infoLog << std::endl;
    }
    else
        std::cout << "SUCCESS:: Program has been linked" << std::endl;
}