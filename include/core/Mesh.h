#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    bool valid() const { return m_vao != 0; }

    static Mesh createBox(float w, float h, float d);
    static Mesh createCylinder(float radius, float height, int segments = 16);
    static Mesh createTaperedCylinder(float radiusBot, float radiusTop, float height, int segments = 16);
    static Mesh createTree(float height, float trunkRadius, float crownRadius, int segments = 8);
    static Mesh createSphere(float radius, int latBands = 12, int lonBands = 12);

private:
    unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
    int m_indexCount = 0;
};
