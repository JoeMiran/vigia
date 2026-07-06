#include "core/Game.h"
#include "core/Renderer.h"
#include "core/InputManager.h"
#include "systems/SceneManager.h"
#include <iostream>

Game* Game::s_instance = nullptr;

Game::Game() {
    s_instance = this;
}

Game::~Game() {
    shutdown();
}

bool Game::init(const std::string& title, int width, int height) {
    m_width = width; m_height = height;

    // ── GLFW ──────────────────────────────────────────────────────
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "[Game] Falha ao criar janela\n";
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync

    // ── GLAD ──────────────────────────────────────────────────────
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Game] Falha ao inicializar GLAD\n";
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    // ── Subsistemas ───────────────────────────────────────────────
    m_input    = std::make_unique<InputManager>(m_window);
    m_renderer = std::make_unique<Renderer>(width, height);
    m_scene    = std::make_unique<SceneManager>();
    m_scene->loadScene("night_shift"); // cena inicial

    m_state = GameState::PLAYING;
    std::cout << "[Game] Iniciado com sucesso\n";
    return true;
}

void Game::run() {
    while (!glfwWindowShouldClose(m_window)) {
        float now = (float)glfwGetTime();
        m_deltaTime = now - m_lastFrame;
        m_lastFrame = now;

        processInput();
        update();
        render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::processInput() {
    m_input->update();
    if (m_input->isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window, true);
    if (m_input->isKeyJustPressed(GLFW_KEY_P))
        m_state = (m_state == GameState::PAUSED) ? GameState::PLAYING : GameState::PAUSED;
}

void Game::update() {
    if (m_state == GameState::PLAYING) {
        m_scene->update(m_deltaTime);
        if (m_scene->isGameOver()) {
            m_state = GameState::GAME_OVER;
            std::cout << "[Game] GAME OVER\n";
        }
    }
}

void Game::render() {
    m_renderer->render(*m_scene);
}

void Game::shutdown() {
    glfwTerminate();
}
