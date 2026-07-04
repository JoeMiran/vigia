#pragma once
#include <memory>

class SceneManager;
class Shader;

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void render(SceneManager& scene);

private:
    void renderScene(SceneManager& scene, Shader& shader);
    void setupLighting(Shader& shader, SceneManager& scene);

    int m_width, m_height;

    unsigned int m_cubeVAO = 0;
    int          m_cubeCount = 0;
    unsigned int m_planeVAO = 0;
    int          m_planeCount = 0;
    unsigned int m_defaultTexture = 0;

    std::unique_ptr<Shader> m_sceneShader;
    std::unique_ptr<Shader> m_lightShader;
};
