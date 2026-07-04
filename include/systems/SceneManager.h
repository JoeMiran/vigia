#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "entities/GameObject.h"
#include "entities/Player.h"

class SceneManager {
public:
    SceneManager();

    void loadScene(const std::string& name);
    void update(float dt);

    Player& getPlayer() { return *m_player; }
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const { return m_objects; }

    void addObject(std::unique_ptr<GameObject> obj);

private:
    void buildGuaritaScene();
    GameObject& make(std::unique_ptr<GameObject> obj);

    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<GameObject>> m_objects;
    std::string m_currentScene;
};
