#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// ─── Shader ──────────────────────────────────────────────────────
// Carrega, compila e linka shaders GLSL.
// Fornece helpers para setar uniforms.
// ─────────────────────────────────────────────────────────────────
class Shader {
public:
    unsigned int ID = 0;

    Shader() = default;
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();

    void load(const char* vertPath, const char* fragPath);
    void use() const;

    // Uniforms
    void setBool (const std::string& name, bool  v)             const;
    void setInt  (const std::string& name, int   v)             const;
    void setFloat(const std::string& name, float v)             const;
    void setVec2 (const std::string& name, const glm::vec2& v)  const;
    void setVec3 (const std::string& name, const glm::vec3& v)  const;
    void setVec4 (const std::string& name, const glm::vec4& v)  const;
    void setMat3 (const std::string& name, const glm::mat3& v)  const;
    void setMat4 (const std::string& name, const glm::mat4& v)  const;

private:
    std::string  readFile(const char* path) const;
    unsigned int compile(GLenum type, const std::string& src, const char* typeName) const;
    void         checkErrors(unsigned int obj, const char* type) const;
    int          loc(const std::string& name) const;
};
