#include "core/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertPath, const char* fragPath) {
    load(vertPath, fragPath);
}

Shader::~Shader() {
    if (ID) glDeleteProgram(ID);
}

void Shader::load(const char* vertPath, const char* fragPath) {
    std::string vertSrc = readFile(vertPath);
    std::string fragSrc = readFile(fragPath);

    unsigned int vert = compile(GL_VERTEX_SHADER, vertSrc, "VERTEX");
    unsigned int frag = compile(GL_FRAGMENT_SHADER, fragSrc, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool v) const {
    glUniform1i(loc(name), (int)v);
}

void Shader::setInt(const std::string& name, int v) const {
    glUniform1i(loc(name), v);
}

void Shader::setFloat(const std::string& name, float v) const {
    glUniform1f(loc(name), v);
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const {
    glUniform2fv(loc(name), 1, &v[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(loc(name), 1, &v[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const {
    glUniform4fv(loc(name), 1, &v[0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& v) const {
    glUniformMatrix3fv(loc(name), 1, GL_FALSE, &v[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& v) const {
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, &v[0][0]);
}

std::string Shader::readFile(const char* path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Shader] Erro ao abrir " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compile(GLenum type, const std::string& src, const char* typeName) const {
    unsigned int shader = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);
    checkErrors(shader, typeName);
    return shader;
}

void Shader::checkErrors(unsigned int obj, const char* type) const {
    int success;
    char info[1024];
    if (std::string(type) == "PROGRAM") {
        glGetProgramiv(obj, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(obj, 1024, nullptr, info);
            std::cerr << "[Shader] ERRO LINK (" << type << "): " << info << "\n";
        }
    } else {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(obj, 1024, nullptr, info);
            std::cerr << "[Shader] ERRO COMPILE (" << type << "): " << info << "\n";
        }
    }
}

int Shader::loc(const std::string& name) const {
    return glGetUniformLocation(ID, name.c_str());
}
