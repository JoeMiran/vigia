#pragma once
#include <memory>
#include <string>

class SceneManager;
class Shader;
class TextRenderer;

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void render(SceneManager& scene);
    void renderTimer(SceneManager& scene, float gameTime);

private:
    void renderScene(SceneManager& scene, Shader& shader);
    void setupLighting(Shader& shader, SceneManager& scene);
    void renderJumpscare();
    unsigned int createJumpscareTexture();

    int m_width, m_height;

    unsigned int m_cubeVAO = 0;
    int          m_cubeCount = 0;
    unsigned int m_planeVAO = 0;
    int          m_planeCount = 0;
    unsigned int m_defaultTexture = 0;

    unsigned int m_jumpscareTexture = 0;
    unsigned int m_fullscreenVAO = 0;

    std::unique_ptr<Shader> m_sceneShader;
    std::unique_ptr<Shader> m_lightShader;
    std::unique_ptr<Shader> m_screenShader;

    std::unique_ptr<TextRenderer> m_textRenderer;
};
