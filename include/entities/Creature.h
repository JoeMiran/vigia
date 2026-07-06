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
    void setPlayerMoving(bool v) { m_playerMoving = v; }
    void setPlayerPos(const glm::vec3& p) { m_playerPos = p; }

    bool isVisible() const { return m_state == VISIBLE; }
    bool isAtDoor() const { return m_state == ENTERING; }
    bool isJumpscareActive() const { return m_state == JUMPSCARE; }
    bool isGameOver() const { return m_gameOver; }

    static constexpr float CREATURE_HEIGHT = 3.2f;

private:
    enum State { HIDDEN, VISIBLE, ENTERING, JUMPSCARE };
    State m_state = HIDDEN;

    float m_gameTime = 0.0f;
    float m_hiddenTimer = 0.0f;
    float m_visibleTimer = 0.0f;
    float m_timeInLight = 0.0f;
    float m_lightTeleportTimer = 0.0f;
    float m_graceTimer = 0.5f;
    float m_currentRadius = 25.0f;
    float m_findTimeLimit = 4.0f;
    float m_hiddenDuration = 8.0f;
    float m_jumpscareTimer = 0.0f;
    float m_cycleAdvance = 0.3f;
    bool m_foundThisCycle = false;
    bool m_playerMoving = false;
    bool m_gameOver = false;

    glm::vec3 m_currentPos;
    glm::vec3 m_flashPos;
    glm::vec3 m_flashDir;
    glm::vec3 m_playerPos;
    bool m_flashOn = true;
    glm::vec3 m_forestCenter = glm::vec3(0.0f);

    std::vector<GameObject*> m_bodyParts;
    float m_animTime = 0.0f;

    // Room entry animation
    float m_entryTimer = 0.0f;
    glm::vec3 m_entryStart;
    glm::vec3 m_entryEnd;

    void choosePosition();
    void updateTransforms();
    bool isFlashlightOnCreature() const;
    void setPartsVisible(bool v);
    void updatePhase();
};
