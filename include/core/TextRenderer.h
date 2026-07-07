#pragma once
#include <string>
#include <glm/glm.hpp>

class TextRenderer {
public:
    TextRenderer(int screenWidth, int screenHeight);
    ~TextRenderer();

    void renderText(const std::string& text, float x, float y, float scale,
                    const glm::vec3& color = glm::vec3(1.0f, 0.2f, 0.2f));

    void updateProjection(int screenWidth, int screenHeight);

private:
    void loadShaders();
    void loadFontTexture();

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_texture = 0;
    unsigned int m_shader = 0;
    int m_screenWidth;
    int m_screenHeight;
};
