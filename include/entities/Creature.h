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

    bool isVisible() const { return m_state == VISIBLE || m_state == STUNNED; }

    static constexpr float CREATURE_HEIGHT = 3.2f;

private:
    enum State { HIDDEN, VISIBLE, STUNNED, DISAPPEARED };
    State m_state = HIDDEN;

    float m_hiddenTimer = 0.0f;
    float m_visibleTimer = 0.0f;
    float m_timeInLight = 0.0f;
    float m_disappearTimer = 0.0f;
    bool m_wasEverLit = false;
    bool m_proximityPenaltyApplied = false;
    int m_proximityLevel = 0;

    glm::vec3 m_currentPos;
    glm::vec3 m_flashPos;
    glm::vec3 m_flashDir;
    bool m_flashOn = true;
    glm::vec3 m_forestCenter = glm::vec3(0.0f);

    std::vector<GameObject*> m_bodyParts;
    float m_animTime = 0.0f;

    void choosePosition();
    void updateTransforms();
    bool isFlashlightOnCreature() const;
    void advanceProximity();
    void setPartsVisible(bool v);
};
