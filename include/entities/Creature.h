#pragma once
#include "entities/GameObject.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Creature : public GameObject {
public:
    Creature();

    void update(float dt) override;

    std::vector<std::unique_ptr<GameObject>> buildBody();

    void setFlashlightInfo(const glm::vec3& pos, const glm::vec3& dir, bool on);

    void attachBodyParts(const std::vector<GameObject*>& parts) { m_bodyParts = parts; }
    void setForestCenter(const glm::vec3& c) { m_forestCenter = c; }
    void setGameTime(float t) { m_gameTime = t; }
    void setPlayerPos(const glm::vec3& p) { m_playerPos = p; }

    bool isVisible() const { return m_state == VISIBLE; }
    bool isJumpscareActive() const { return m_state == JUMPSCARE; }
    bool isGameOver() const { return m_gameOver; }
    bool isLightsOut() const { return m_lightsOut; }

    static constexpr float CREATURE_HEIGHT = 3.2f;

private:
    enum State { HIDDEN, VISIBLE, JUMPSCARE };
    State m_state = HIDDEN;

    float m_gameTime = 0.0f;
    float m_currentRadius = 25.0f;
    float m_jumpscareTimer = 0.0f;
    float m_moveTimer = 0.0f;
    float m_timeWithoutLight = 0.0f;
    bool m_gameOver = false;
    bool m_lightsOut = false;

    glm::vec3 m_currentPos;
    glm::vec3 m_playerPos;
    glm::vec3 m_flashPos;
    glm::vec3 m_flashDir;
    bool m_flashOn = true;
    glm::vec3 m_forestCenter = glm::vec3(0.0f);

    std::vector<GameObject*> m_bodyParts;

    void choosePosition();
    void updateTransforms();
    bool isFlashlightOnCreature() const;
    void setPartsVisible(bool v);
};
