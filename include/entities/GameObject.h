#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>
#include "core/Mesh.h"
#include "core/Model.h"

// ─── GameObject ──────────────────────────────────────────────────
class GameObject {
public:
    std::string textureName;
    std::string name = "GameObject";
    bool        active = true;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);

    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Model> model;

    GameObject() = default;
    explicit GameObject(const std::string& name) : name(name) {}
    virtual ~GameObject() = default;

    virtual void update(float dt) {}

    glm::mat4 getModelMatrix() const {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0,1,0));
        m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1,0,0));
        m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0,0,1));
        m = glm::scale(m, scale);
        return m;
    }
};
