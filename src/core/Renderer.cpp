#include "core/Renderer.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include "core/Game.h"
#include "core/Mesh.h"
#include "systems/SceneManager.h"
#include "entities/Player.h"
#include "entities/GameObject.h"
#include <iostream>
#include <vector>
#include "core/TextureCache.h"

static unsigned int createVAO(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices) {
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return VAO;
}

static void buildCubeMesh(unsigned int& vao, int& count) {
    std::vector<Vertex> verts = {
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
    };

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    vao = createVAO(verts, indices);
    count = (int)indices.size();
}

static void buildPlaneMesh(unsigned int& vao, int& count) {
    std::vector<Vertex> verts = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };

    std::vector<unsigned int> indices = {0,1,2, 2,3,0};

    vao = createVAO(verts, indices);
    count = (int)indices.size();
}

Renderer::Renderer(int width, int height)
    : m_width(width), m_height(height)
{
    m_sceneShader = std::make_unique<Shader>("shaders/scene_vertex.glsl", "shaders/scene_fragment.glsl");

    buildCubeMesh(m_cubeVAO, m_cubeCount);
    buildPlaneMesh(m_planeVAO, m_planeCount);

    glGenTextures(1, &m_defaultTexture);
    glBindTexture(GL_TEXTURE_2D, m_defaultTexture);
    unsigned char whitePixel[] = {255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "[Renderer] Inicializado\n";
}

Renderer::~Renderer() {
    if (m_cubeVAO) glDeleteVertexArrays(1, &m_cubeVAO);
    if (m_planeVAO) glDeleteVertexArrays(1, &m_planeVAO);
    if (m_defaultTexture) glDeleteTextures(1, &m_defaultTexture);
}

void Renderer::render(SceneManager& scene) {
    renderScene(scene, *m_sceneShader);
}

void Renderer::renderScene(SceneManager& scene, Shader& shader) {
    const auto& game = Game::get();
    const auto& camera = scene.getPlayer().camera;

    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov),
                                            (float)m_width / (float)m_height, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", camera.getViewMatrix());
    shader.setVec3("viewPos", camera.Position);

    setupLighting(shader, scene);

    glActiveTexture(GL_TEXTURE0);

    for (const auto& obj : scene.getObjects()) {
        if (!obj->active) continue;

        std::cout << "[DEBUG] Rendering: " << obj->name << std::endl;

        if (!obj->textureName.empty())
            glBindTexture(GL_TEXTURE_2D, TextureCache::get(obj->textureName));
        else
            glBindTexture(GL_TEXTURE_2D, m_defaultTexture);
        shader.setInt("texture_diffuse", 0);

        glm::mat4 model = obj->getModelMatrix();
        shader.setMat4("model", model);

        if (obj->model) {
            shader.setVec2("uvScale", glm::vec2(1.0f));
            std::cout << "[DEBUG] model->draw(" << (obj->textureName.empty() && obj->model->hasPerMeshTextures()) << ")" << std::endl;
            obj->model->draw(obj->textureName.empty() && obj->model->hasPerMeshTextures());
            continue;
        }

        if (obj->mesh && obj->mesh->valid()) {
            shader.setVec2("uvScale", glm::vec2(1.0f));
            obj->mesh->draw();
            continue;
        }

        unsigned int vao = (obj->name == "Floor") ? m_planeVAO : m_cubeVAO;
        int count = (obj->name == "Floor") ? m_planeCount : m_cubeCount;

        // ← tiling reduzido pela metade (estava 6x6=36 repetições, agora 3x3)
        float s = glm::max(obj->scale.x, glm::max(obj->scale.y, obj->scale.z));
        shader.setVec2("uvScale", glm::vec2(s / 2.0f));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
}

void Renderer::setupLighting(Shader& shader, SceneManager& scene) {
    const auto& player = scene.getPlayer();
    const auto& camera = player.camera;

    shader.setVec3("flashlight.position",  camera.getFlashlightPos());
    shader.setVec3("flashlight.direction", camera.getFlashlightDir());
    shader.setFloat("flashlight.innerCutOff", glm::cos(glm::radians(camera.flashInnerCut)));
    shader.setFloat("flashlight.outerCutOff", glm::cos(glm::radians(camera.flashOuterCut)));
    shader.setFloat("flashlight.constant",  1.0f);
    shader.setFloat("flashlight.linear",    0.09f);
    shader.setFloat("flashlight.quadratic", 0.032f);
    shader.setVec3("flashlight.ambient",  glm::vec3(0.0f));
    shader.setVec3("flashlight.diffuse",  glm::vec3(0.9f, 0.88f, 0.75f));
    shader.setVec3("flashlight.specular", glm::vec3(0.4f));
    shader.setBool("flashlight.on", camera.flashlightOn);

    glm::vec3 lampPositions[4] = {
        glm::vec3(-1.2f, 2.4f, -1.0f),
        glm::vec3(-1.2f, 2.4f,  1.0f),
        glm::vec3( 1.2f, 2.4f, -1.0f),
        glm::vec3( 1.2f, 2.4f,  1.0f)
    };
    shader.setInt("pointLightCount", 4);
    for (int i = 0; i < 4; i++) {
        std::string prefix = "pointLights[" + std::to_string(i) + "]";
        shader.setVec3 (prefix + ".position",  lampPositions[i]);
        shader.setVec3 (prefix + ".color",     glm::vec3(1.0f, 0.97f, 0.88f));
        shader.setFloat(prefix + ".constant",  1.0f);
        shader.setFloat(prefix + ".linear",    0.27f);
        shader.setFloat(prefix + ".quadratic", 0.12f);
    }

    shader.setVec3 ("ambientLight.color",     glm::vec3(0.03f, 0.04f, 0.08f));
    shader.setFloat("ambientLight.intensity", 0.08f);

    shader.setFloat("fogDensity", 0.045f);
    shader.setVec3 ("fogColor",   glm::vec3(0.01f, 0.015f, 0.04f));
}