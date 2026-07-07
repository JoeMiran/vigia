#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

// Forward declarations
class Renderer;
class InputManager;
class SceneManager;
class AudioManager;

// ─── Estados do jogo ──────────────────────────────────────────────
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// ─── Classe principal ─────────────────────────────────────────────
class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int width, int height);
    void run();
    void shutdown();

    // Getters globais (acesso via Game::get())
    static Game& get() { return *s_instance; }
    GLFWwindow*   getWindow()   const { return m_window; }
    int           getWidth()    const { return m_width; }
    int           getHeight()   const { return m_height; }
    float         getDeltaTime()const { return m_deltaTime; }
    GameState     getState()    const { return m_state; }
    void          setState(GameState s) { m_state = s; }

private:
    void processInput();
    void update();
    void render();

    static Game* s_instance;

    GLFWwindow* m_window   = nullptr;
    int         m_width    = 1280;
    int         m_height   = 720;
    float       m_deltaTime = 0.0f;
    float       m_lastFrame = 0.0f;
    GameState   m_state    = GameState::MENU;

    std::unique_ptr<Renderer>     m_renderer;
    std::unique_ptr<InputManager> m_input;
    std::unique_ptr<SceneManager> m_scene;
    std::unique_ptr<AudioManager> m_audio;
};
