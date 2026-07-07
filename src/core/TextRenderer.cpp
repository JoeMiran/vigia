#include "core/TextRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

static const unsigned int FONT_W = 8;
static const unsigned int FONT_H = 8;

static const unsigned char fontData[11][8] = {
    {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C}, // 0
    {0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x3E}, // 1
    {0x3C, 0x42, 0x02, 0x04, 0x18, 0x20, 0x40, 0x7E}, // 2
    {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x02, 0x42, 0x3C}, // 3
    {0x04, 0x0C, 0x14, 0x24, 0x44, 0x7E, 0x04, 0x04}, // 4
    {0x7E, 0x40, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C}, // 5
    {0x1C, 0x20, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C}, // 6
    {0x7E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x10}, // 7
    {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C}, // 8
    {0x3C, 0x42, 0x42, 0x42, 0x3E, 0x02, 0x04, 0x38}, // 9
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18}, // :
};

static int charToIndex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c == ':') return 10;
    return -1;
}

static const char* textVertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

static const char* textFragSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textTexture;
uniform vec3 textColor;
void main() {
    float alpha = texture(textTexture, TexCoord).r;
    if (alpha < 0.1) discard;
    FragColor = vec4(textColor, alpha);
}
)";

static unsigned int compileShader(GLenum type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "[TextRenderer] Shader compile error: " << info << "\n";
    }
    return shader;
}

TextRenderer::TextRenderer(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    loadShaders();
    loadFontTexture();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::cout << "[TextRenderer] Inicializado\n";
}

TextRenderer::~TextRenderer() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_texture) glDeleteTextures(1, &m_texture);
    if (m_shader) glDeleteProgram(m_shader);
}

void TextRenderer::loadShaders() {
    unsigned int vert = compileShader(GL_VERTEX_SHADER, textVertSrc);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, textFragSrc);
    m_shader = glCreateProgram();
    glAttachShader(m_shader, vert);
    glAttachShader(m_shader, frag);
    glLinkProgram(m_shader);
    int success;
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_shader, 512, nullptr, info);
        std::cerr << "[TextRenderer] Program link error: " << info << "\n";
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void TextRenderer::loadFontTexture() {
    const int COLS = 11;
    const int TEX_W = COLS * FONT_W;
    const int TEX_H = FONT_H;
    std::vector<unsigned char> pixels(TEX_W * TEX_H, 0);

    for (int ch = 0; ch < COLS; ch++) {
        for (int row = 0; row < 8; row++) {
            unsigned char byte = fontData[ch][row];
            for (int col = 0; col < 8; col++) {
                int px = ch * 8 + col;
                int py = row;
                if (byte & (1 << (7 - col))) {
                    pixels[py * TEX_W + px] = 255;
                }
            }
        }
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_W, TEX_H, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::updateProjection(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale,
                              const glm::vec3& color)
{
    if (text.empty() || !m_shader) return;

    glDisable(GL_DEPTH_TEST);

    glUseProgram(m_shader);

    glm::mat4 proj = glm::ortho(0.0f, (float)m_screenWidth, (float)m_screenHeight, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(m_shader, "textColor"), 1, &color[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(glGetUniformLocation(m_shader, "textTexture"), 0);

    glBindVertexArray(m_vao);

    const float CHAR_W = FONT_W * scale;
    const float CHAR_H = FONT_H * scale;
    const float TEX_COLS = 11.0f;

    float cursorX = x;
    for (char c : text) {
        int idx = charToIndex(c);
        if (idx < 0) {
            cursorX += CHAR_W * 0.5f;
            continue;
        }

        float u0 = (float)idx / TEX_COLS;
        float u1 = (float)(idx + 1) / TEX_COLS;
        float v0 = 0.0f;
        float v1 = 1.0f;

        float verts[] = {
            cursorX,        y,        u0, v0,
            cursorX,        y + CHAR_H, u0, v1,
            cursorX + CHAR_W, y + CHAR_H, u1, v1,
            cursorX,        y,        u0, v0,
            cursorX + CHAR_W, y + CHAR_H, u1, v1,
            cursorX + CHAR_W, y,        u1, v0,
        };

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        cursorX += CHAR_W;
    }

    glBindVertexArray(0);
    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}
