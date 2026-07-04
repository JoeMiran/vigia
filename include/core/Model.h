#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <cfloat>

class Mesh;

class Model {
public:
    explicit Model(const std::string& path);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    void draw(bool usePerMesh = false) const;
    bool hasPerMeshTextures() const { return !m_meshTextures.empty(); }
    glm::vec3 getMinBounds() const { return m_min; }
    glm::vec3 getMaxBounds() const { return m_max; }
    glm::vec3 getSize()      const { return m_max - m_min; }

private:
    void load(const std::string& path);

    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<glm::vec3> m_meshKd;
    std::vector<unsigned int> m_meshTextures;

    glm::vec3 m_min{ FLT_MAX,  FLT_MAX,  FLT_MAX};
    glm::vec3 m_max{-FLT_MAX, -FLT_MAX, -FLT_MAX};
};
