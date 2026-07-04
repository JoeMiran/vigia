#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

// ─── InputManager ────────────────────────────────────────────────
// Centraliza teclado e mouse.
// Use isKeyPressed()     → tecla pressionada neste frame
//     isKeyJustPressed() → só no primeiro frame
//     getMouseDelta()    → movimento do mouse
// ─────────────────────────────────────────────────────────────────
class InputManager {
public:
    InputManager(GLFWwindow* window);

    void update(); // chamado a cada frame

    bool isKeyPressed(int key)     const;
    bool isKeyJustPressed(int key) const;
    bool isKeyJustReleased(int key)const;

    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    glm::vec2 getMousePos()   const { return m_mousePos; }
    float     getScrollDelta()const { return m_scrollDelta; }

    static InputManager& get() { return *s_instance; }

    // Callbacks (registrados no GLFW)
    static void mouseMoveCallback  (GLFWwindow*, double x, double y);
    static void scrollCallback     (GLFWwindow*, double, double y);

private:
    GLFWwindow* m_window;

    std::unordered_map<int, bool> m_currentKeys;
    std::unordered_map<int, bool> m_previousKeys;

    glm::vec2 m_mousePos     = {0, 0};
    glm::vec2 m_lastMousePos = {0, 0};
    glm::vec2 m_mouseDelta   = {0, 0};
    float     m_scrollDelta  = 0.0f;

    static InputManager* s_instance;
};
