#include "entities/Creature.h"
#include "core/Mesh.h"
#include "core/TextureCache.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>

Creature::Creature() : GameObject("Creature") {
    active = false;
}

std::vector<std::unique_ptr<GameObject>> Creature::buildBody() {
    std::vector<std::unique_ptr<GameObject>> parts;
    TextureCache::makeColor("creature_skin", 10, 10, 12);
    TextureCache::makeColor("creature_eye", 200, 5, 5);

    auto head = std::make_unique<GameObject>("CreatureHead");
    head->mesh = std::make_unique<Mesh>(Mesh::createSphere(0.15f, 8, 8));
    head->textureName = "creature_skin";
    parts.push_back(std::move(head));

    auto body = std::make_unique<GameObject>("CreatureBody");
    body->mesh = std::make_unique<Mesh>(Mesh::createBox(0.35f, 1.0f, 0.18f));
    body->textureName = "creature_skin";
    parts.push_back(std::move(body));

    auto lArm = std::make_unique<GameObject>("CreatureLArm");
    lArm->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.03f, 1.1f, 6));
    lArm->textureName = "creature_skin";
    parts.push_back(std::move(lArm));

    auto rArm = std::make_unique<GameObject>("CreatureRArm");
    rArm->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.03f, 1.1f, 6));
    rArm->textureName = "creature_skin";
    parts.push_back(std::move(rArm));

    auto lLeg = std::make_unique<GameObject>("CreatureLLeg");
    lLeg->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.04f, 1.6f, 6));
    lLeg->textureName = "creature_skin";
    parts.push_back(std::move(lLeg));

    auto rLeg = std::make_unique<GameObject>("CreatureRLeg");
    rLeg->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.04f, 1.6f, 6));
    rLeg->textureName = "creature_skin";
    parts.push_back(std::move(rLeg));

    {
        auto eyeL = std::make_unique<GameObject>("CreatureEyeL");
        eyeL->mesh = std::make_unique<Mesh>(Mesh::createSphere(0.035f, 6, 6));
        eyeL->textureName = "creature_eye";
        parts.push_back(std::move(eyeL));
    }
    {
        auto eyeR = std::make_unique<GameObject>("CreatureEyeR");
        eyeR->mesh = std::make_unique<Mesh>(Mesh::createSphere(0.035f, 6, 6));
        eyeR->textureName = "creature_eye";
        parts.push_back(std::move(eyeR));
    }

    return parts;
}

void Creature::setFlashlightInfo(const glm::vec3& pos, const glm::vec3& dir, bool on) {
    m_flashPos = pos;
    m_flashDir = dir;
    m_flashOn = on;
}

void Creature::update(float dt) {
    if (!active) return;
    m_animTime += dt;
    updatePhase();

    switch (m_state) {
    case HIDDEN: {
        m_hiddenTimer -= dt;
        if (m_hiddenTimer <= 0.0f) {
            if (m_currentRadius <= 0.0f) {
                m_entryStart = glm::vec3(-2.6f, 0.0f, 3.0f);
                m_entryEnd = glm::vec3(0.0f, 0.0f, 1.5f);
                m_entryTimer = 0.0f;
                m_currentPos = m_entryStart;
                setPartsVisible(true);
                m_state = ENTERING;
                std::cout << "[Creature] ENTERING THE ROOM!\n";
            } else {
                choosePosition();
                m_visibleTimer = 0.0f;
                m_timeInLight = 0.0f;
                m_lightTeleportTimer = 0.0f;
                m_graceTimer = 0.5f;
                m_foundThisCycle = false;
                setPartsVisible(true);
                m_state = VISIBLE;
                std::cout << "[Creature] Appeared at radius " << m_currentRadius << "\n";
            }
        }
        updateTransforms();
        break;
    }
    case VISIBLE: {
        m_visibleTimer += dt;
        bool hit = m_flashOn && isFlashlightOnCreature();

        if (hit) {
            if (!m_foundThisCycle) {
                m_foundThisCycle = true;
                std::cout << "[Creature] Found!\n";
            }
            m_timeInLight += dt;
            m_lightTeleportTimer += dt;
            m_graceTimer = 0.5f;

            if (m_lightTeleportTimer >= 10.0f) {
                choosePosition();
                m_lightTeleportTimer = 0.0f;
            }
        } else {
            m_timeInLight = 0.0f;

            if (m_foundThisCycle) {
                m_graceTimer -= dt;
                if (m_graceTimer <= 0.0f) {
                    m_currentRadius = std::max(0.0f, m_currentRadius - m_cycleAdvance);
                    setPartsVisible(false);
                    if (m_currentRadius <= 0.0f) {
                        m_entryStart = glm::vec3(-2.6f, 0.0f, 3.0f);
                        m_entryEnd = glm::vec3(0.0f, 0.0f, 1.5f);
                        m_entryTimer = 0.0f;
                        m_currentPos = m_entryStart;
                        setPartsVisible(true);
                        m_state = ENTERING;
                        std::cout << "[Creature] REACHED THE DOOR!\n";
                    } else {
                        m_hiddenTimer = m_hiddenDuration;
                        m_state = HIDDEN;
                        std::cout << "[Creature] Gone, radius " << m_currentRadius << "\n";
                    }
                }
            } else {
                m_lightTeleportTimer = 0.0f;
                if (m_visibleTimer >= m_findTimeLimit) {
                    m_currentRadius = std::max(0.0f, m_currentRadius - 1.0f);
                    setPartsVisible(false);
                    if (m_currentRadius <= 0.0f) {
                        m_entryStart = glm::vec3(-2.6f, 0.0f, 3.0f);
                        m_entryEnd = glm::vec3(0.0f, 0.0f, 1.5f);
                        m_entryTimer = 0.0f;
                        m_currentPos = m_entryStart;
                        setPartsVisible(true);
                        m_state = ENTERING;
                        std::cout << "[Creature] REACHED THE DOOR (penalty)!\n";
                    } else {
                        m_hiddenTimer = m_hiddenDuration;
                        m_state = HIDDEN;
                        std::cout << "[Creature] Not found, radius " << m_currentRadius << "\n";
                    }
                }
            }
        }
        updateTransforms();
        break;
    }
    case ENTERING: {
        m_entryTimer += dt;
        float t = std::min(m_entryTimer / 1.5f, 1.0f);
        float smooth = t * t * (3.0f - 2.0f * t);
        m_currentPos = glm::mix(m_entryStart, m_entryEnd, smooth);

        updateTransforms();

        if (m_playerMoving || t >= 1.0f) {
            std::cout << "[Creature] Jumpscare triggered!\n";
            m_currentPos = m_entryEnd;
            updateTransforms();
            m_jumpscareTimer = 2.0f;
            m_state = JUMPSCARE;
        }
        break;
    }
    case JUMPSCARE: {
        m_jumpscareTimer -= dt;
        if (m_jumpscareTimer <= 0.0f) {
            m_gameOver = true;
        }
        break;
    }
    }
}

void Creature::updatePhase() {
    if (m_gameTime >= 90.0f) {
        m_hiddenDuration = 4.0f;
        m_findTimeLimit = 3.0f;
        m_cycleAdvance = 0.5f;
    } else {
        m_hiddenDuration = 8.0f;
        m_findTimeLimit = 4.0f;
        m_cycleAdvance = 0.3f;
    }
}

void Creature::choosePosition() {
    float ang = (float)(rand() % 6283) / 1000.0f;
    float dist = m_currentRadius;
    m_currentPos = m_forestCenter + glm::vec3(cosf(ang) * dist, 0.0f, sinf(ang) * dist);
}

void Creature::updateTransforms() {
    float t1 = m_animTime * 1.3f;
    float t2 = m_animTime * 0.7f + 1.0f;
    float sway = sinf(t1 * 0.5f) * 0.03f;

    if (m_bodyParts.size() < 8) return;

    float headBob = sinf(t2 * 2.0f) * 0.02f;
    glm::vec3 headPos = m_currentPos + glm::vec3(0.0f, 2.85f + headBob, 0.0f);

    {
        auto& p = *m_bodyParts[0];
        p.position = headPos;
        p.rotation = glm::vec3(sinf(t1 * 1.1f) * 2.0f, 0.0f, sway * 5.0f);
    }
    {
        auto& p = *m_bodyParts[1];
        float bob = sinf(t2 * 1.5f) * 0.015f;
        p.position = m_currentPos + glm::vec3(0.0f, 2.05f + bob, 0.0f);
        p.rotation = glm::vec3(sinf(t1 * 0.9f) * 1.0f, 0.0f, sway * 3.0f);
    }
    {
        auto& p = *m_bodyParts[2];
        float armSwing = sinf(t1 * 1.7f) * 8.0f + 5.0f;
        p.position = m_currentPos + glm::vec3(-0.25f, 2.0f, 0.0f);
        p.rotation = glm::vec3(armSwing + sinf(t2 * 2.3f) * 3.0f, 0.0f, sway * 8.0f);
    }
    {
        auto& p = *m_bodyParts[3];
        float armSwing = sinf(t1 * 1.7f + 3.14f) * 8.0f + 5.0f;
        p.position = m_currentPos + glm::vec3(0.25f, 2.0f, 0.0f);
        p.rotation = glm::vec3(armSwing + sinf(t2 * 2.3f + 1.0f) * 3.0f, 0.0f, -sway * 8.0f);
    }
    {
        auto& p = *m_bodyParts[4];
        float legSwing = sinf(t1 * 1.5f) * 2.0f;
        p.position = m_currentPos + glm::vec3(-0.12f, 0.8f, 0.0f);
        p.rotation = glm::vec3(legSwing + sinf(t2 * 0.5f) * 1.0f, 0.0f, sway * 2.0f);
    }
    {
        auto& p = *m_bodyParts[5];
        float legSwing = sinf(t1 * 1.5f + 3.14f) * 2.0f;
        p.position = m_currentPos + glm::vec3(0.12f, 0.8f, 0.0f);
        p.rotation = glm::vec3(legSwing + sinf(t2 * 0.5f + 1.0f) * 1.0f, 0.0f, -sway * 2.0f);
    }
    {
        auto& p = *m_bodyParts[6];
        p.position = headPos + glm::vec3(-0.09f, 0.02f, 0.13f);
        p.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    {
        auto& p = *m_bodyParts[7];
        p.position = headPos + glm::vec3(0.09f, 0.02f, 0.13f);
        p.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

bool Creature::isFlashlightOnCreature() const {
    glm::vec3 toCreature = m_currentPos - m_flashPos;
    float dist = glm::length(toCreature);
    if (dist < 0.001f) return false;
    if (dist > 35.0f) return false;
    toCreature /= dist;

    float cosAngle = glm::dot(toCreature, m_flashDir);
    float outerCut = cosf(glm::radians(25.0f));
    return cosAngle >= outerCut;
}

void Creature::setPartsVisible(bool v) {
    for (auto* part : m_bodyParts) {
        if (part) part->active = v;
    }
}
