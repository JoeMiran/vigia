#include "core/InputManager.h"
#include <algorithm>

InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager(GLFWwindow* window)
    : m_window(window)
{
    s_instance = this;

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(w));
        if (!self) return;
        self->m_currentKeys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    });

    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    m_mousePos = glm::vec2((float)x, (float)y);
    m_lastMousePos = m_mousePos;
}

void InputManager::update() {
    m_previousKeys = m_currentKeys;

    m_mouseDelta = m_mousePos - m_lastMousePos;
    m_lastMousePos = m_mousePos;

    m_scrollDelta = 0.0f;
}

bool InputManager::isKeyPressed(int key) const {
    auto it = m_currentKeys.find(key);
    return it != m_currentKeys.end() && it->second;
}

bool InputManager::isKeyJustPressed(int key) const {
    bool cur = false;
    bool prev = false;
    {
        auto it = m_currentKeys.find(key);
        if (it != m_currentKeys.end()) cur = it->second;
    }
    {
        auto it = m_previousKeys.find(key);
        if (it != m_previousKeys.end()) prev = it->second;
    }
    return cur && !prev;
}

bool InputManager::isKeyJustReleased(int key) const {
    bool cur = false;
    bool prev = false;
    {
        auto it = m_currentKeys.find(key);
        if (it != m_currentKeys.end()) cur = it->second;
    }
    {
        auto it = m_previousKeys.find(key);
        if (it != m_previousKeys.end()) prev = it->second;
    }
    return !cur && prev;
}

void InputManager::mouseMoveCallback(GLFWwindow* window, double x, double y) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;
    self->m_mousePos = glm::vec2((float)x, (float)y);
}

void InputManager::scrollCallback(GLFWwindow* window, double, double y) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;
    self->m_scrollDelta = (float)y;
}
