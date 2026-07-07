#include "core/Renderer.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include "core/Game.h"
#include "core/Mesh.h"
#include "core/TextRenderer.h"
#include "systems/SceneManager.h"
#include "entities/Player.h"
#include "entities/GameObject.h"
#include "entities/Creature.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
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
    m_screenShader = std::make_unique<Shader>("shaders/screen_vertex.glsl", "shaders/screen_fragment.glsl");

    buildCubeMesh(m_cubeVAO, m_cubeCount);
    buildPlaneMesh(m_planeVAO, m_planeCount);

    glGenTextures(1, &m_defaultTexture);
    glBindTexture(GL_TEXTURE_2D, m_defaultTexture);
    unsigned char whitePixel[] = {255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_jumpscareTexture = createJumpscareTexture();

    // Full-screen quad VAO
    float quadVerts[] = {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
    };
    glGenVertexArrays(1, &m_fullscreenVAO);
    unsigned int fsVBO;
    glGenBuffers(1, &fsVBO);
    glBindVertexArray(m_fullscreenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    m_textRenderer = std::make_unique<TextRenderer>(m_width, m_height);

    std::cout << "[Renderer] Inicializado\n";
}

Renderer::~Renderer() {
    if (m_cubeVAO) glDeleteVertexArrays(1, &m_cubeVAO);
    if (m_planeVAO) glDeleteVertexArrays(1, &m_planeVAO);
    if (m_defaultTexture) glDeleteTextures(1, &m_defaultTexture);
    if (m_jumpscareTexture) glDeleteTextures(1, &m_jumpscareTexture);
    if (m_fullscreenVAO) glDeleteVertexArrays(1, &m_fullscreenVAO);
}

void Renderer::render(SceneManager& scene) {
    if (scene.isGameOver()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    if (scene.isLightsOut()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        renderScene(scene, *m_sceneShader);
    }

    if (scene.getCreature().isJumpscareActive()) {
        renderJumpscare();
    }

    renderTimer(scene, scene.getGameTime());
}

void Renderer::renderTimer(SceneManager& scene, float gameTime) {
    if (!m_textRenderer) return;
    if (scene.isLightsOut()) return;

    float totalGameMinutes = 180.0f + gameTime;

    int hours = (int)(totalGameMinutes / 60.0f);
    int mins = (int)totalGameMinutes % 60;

    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":"
       << std::setw(2) << std::setfill('0') << mins;

    float scale = 6.0f;
    float textW = 5.0f * 8.0f * scale;
    float x = (m_width - textW) * 0.5f;
    float y = 10.0f;
    m_textRenderer->renderText(ss.str(), x, y, scale, glm::vec3(1.0f, 0.1f, 0.05f));
}

void Renderer::renderScene(SceneManager& scene, Shader& shader) {
    const auto& game = Game::get();
    const auto& camera = scene.getPlayer().camera;

    float gameTime = scene.getGameTime();
    float dayFactor = glm::clamp(gameTime / 300.0f, 0.0f, 1.0f);

    glm::vec3 nightAmbient(0.01f, 0.01f, 0.02f);
    glm::vec3 dayAmbient(0.15f, 0.18f, 0.22f);
    glm::vec3 ambientColor = glm::mix(nightAmbient, dayAmbient, dayFactor * dayFactor);

    glm::vec3 nightFog(0.003f, 0.005f, 0.015f);
    glm::vec3 dayFog(0.2f, 0.2f, 0.2f);
    glm::vec3 fogColor = glm::mix(nightFog, dayFog, dayFactor);

    float fogDensity = glm::mix(0.065f, 0.015f, dayFactor);

    glm::vec3 nightClear(0.005f, 0.005f, 0.015f);
    glm::vec3 dayClear(0.2f, 0.22f, 0.25f);
    glm::vec3 clearColor = glm::mix(nightClear, dayClear, dayFactor);

    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov),
                                            (float)m_width / (float)m_height, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", camera.getViewMatrix());
    shader.setVec3("viewPos", camera.Position);

    shader.setVec3("ambientLight.color", ambientColor);
    shader.setFloat("ambientLight.intensity", glm::mix(0.03f, 0.3f, dayFactor * dayFactor));
    shader.setFloat("fogDensity", fogDensity);
    shader.setVec3("fogColor", fogColor);

    setupLighting(shader, scene);

    glActiveTexture(GL_TEXTURE0);

    for (const auto& obj : scene.getObjects()) {
        if (!obj->active) continue;

        if (!obj->textureName.empty())
            glBindTexture(GL_TEXTURE_2D, TextureCache::get(obj->textureName));
        else
            glBindTexture(GL_TEXTURE_2D, m_defaultTexture);
        shader.setInt("texture_diffuse", 0);

        glm::mat4 model = obj->getModelMatrix();
        shader.setMat4("model", model);

        if (obj->model) {
            shader.setVec2("uvScale", glm::vec2(1.0f));
            obj->model->draw(obj->textureName.empty() && obj->model->hasPerMeshTextures());
            continue;
        }

        if (obj->mesh && obj->mesh->valid()) {
            shader.setVec2("uvScale", glm::vec2(1.0f));
            obj->mesh->draw();
            continue;
        }

        unsigned int vao = (obj->name == "Floor" || obj->name == "GroundOutside") ? m_planeVAO : m_cubeVAO;
        int count = (obj->name == "Floor" || obj->name == "GroundOutside") ? m_planeCount : m_cubeCount;

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
    shader.setFloat("flashlight.linear",    0.025f);
    shader.setFloat("flashlight.quadratic", 0.008f);
    shader.setVec3("flashlight.ambient",  glm::vec3(0.0f));
    shader.setVec3("flashlight.diffuse",  glm::vec3(1.6f, 1.55f, 1.3f));
    shader.setVec3("flashlight.specular", glm::vec3(0.6f));
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
}

void Renderer::renderJumpscare() {
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_screenShader || !m_screenShader->ID) return;
    m_screenShader->use();
    m_screenShader->setInt("screenTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_jumpscareTexture);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(m_fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    std::cout << "[Renderer] Jumpscare rendered (shader ID=" << m_screenShader->ID << ")\n";
}

unsigned int Renderer::createJumpscareTexture() {
    const int W = 256, H = 256;
    std::vector<unsigned char> pixels(W * H * 4);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float nx = (float)x / W * 2.0f - 1.0f;
            float ny = (float)y / H * 2.0f - 1.0f;

            unsigned char r = 0, g = 0, b = 0;

            // Large white eyes with black pupils
            float eyeL = glm::length(glm::vec2(nx + 0.35f, ny + 0.05f));
            float eyeR = glm::length(glm::vec2(nx - 0.35f, ny + 0.05f));
            bool inEye = eyeL < 0.18f || eyeR < 0.18f;
            float rd = eyeL < 0.18f ? eyeL : eyeR;

            if (inEye && rd < 0.07f) {
                // pupil — bright white
                r = 255; g = 255; b = 255;
            } else if (inEye && rd < 0.18f) {
                // sclera — white with red veins
                float t = (rd - 0.07f) / 0.11f;
                r = (unsigned char)(200 + 55 * (1 - t));
                g = (unsigned char)(30 * (1 - t));
                b = (unsigned char)(30 * (1 - t));
            }

            // Wide open mouth (dark hole with teeth outline)
            float mx = nx;
            float my = ny + 0.45f;
            float mouthDist = glm::length(glm::vec2(mx * 1.3f, my));
            if (mouthDist < 0.25f) {
                if (mouthDist > 0.20f) {
                    // teeth outline — white
                    r = 255; g = 255; b = 255;
                } else {
                    // mouth interior — black
                    r = 0; g = 0; b = 0;
                }
            }

            // Dark red glow behind everything
            if (!inEye && mouthDist >= 0.25f) {
                float glow = 0.3f + 0.2f * (1.0f - glm::length(glm::vec2(nx, ny)));
                r = (unsigned char)(glow * 180);
                g = (unsigned char)(glow * 10);
                b = (unsigned char)(glow * 10);
            }

            int idx = (y * W + x) * 4;
            pixels[idx + 0] = r;
            pixels[idx + 1] = g;
            pixels[idx + 2] = b;
            pixels[idx + 3] = 255;
        }
    }

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "[Renderer] Jumpscare texture created (bright version)\n";
    return tex;
}
