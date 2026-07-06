#include "entities/Creature.h"
#include "core/Mesh.h"
#include "core/TextureCache.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

Creature::Creature() : GameObject("Creature") {
    active = false; // not rendered directly, parts are rendered
}

std::vector<std::unique_ptr<GameObject>> Creature::buildBody() {
    std::vector<std::unique_ptr<GameObject>> parts;
    TextureCache::makeColor("creature_skin", 255, 255, 255);

    // Head: small sphere at top
    auto head = std::make_unique<GameObject>("CreatureHead");
    head->mesh = std::make_unique<Mesh>(Mesh::createSphere(0.15f, 8, 8));
    head->textureName = "creature_skin";
    parts.push_back(std::move(head));

    // Body/torso: thin tall box
    auto body = std::make_unique<GameObject>("CreatureBody");
    body->mesh = std::make_unique<Mesh>(Mesh::createBox(0.35f, 1.0f, 0.18f));
    body->textureName = "creature_skin";
    parts.push_back(std::move(body));

    // Left arm: thin cylinder from shoulder down
    auto lArm = std::make_unique<GameObject>("CreatureLArm");
    lArm->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.03f, 1.1f, 6));
    lArm->textureName = "creature_skin";
    parts.push_back(std::move(lArm));

    // Right arm
    auto rArm = std::make_unique<GameObject>("CreatureRArm");
    rArm->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.03f, 1.1f, 6));
    rArm->textureName = "creature_skin";
    parts.push_back(std::move(rArm));

    // Left leg: very long thin cylinder
    auto lLeg = std::make_unique<GameObject>("CreatureLLeg");
    lLeg->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.04f, 1.6f, 6));
    lLeg->textureName = "creature_skin";
    parts.push_back(std::move(lLeg));

    // Right leg
    auto rLeg = std::make_unique<GameObject>("CreatureRLeg");
    rLeg->mesh = std::make_unique<Mesh>(Mesh::createCylinder(0.04f, 1.6f, 6));
    rLeg->textureName = "creature_skin";
    parts.push_back(std::move(rLeg));

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

    switch (m_state) {
    case HIDDEN: {
        m_hiddenTimer -= dt;
        if (m_hiddenTimer <= 0.0f) {
            choosePosition();
            m_visibleTimer = 0.0f;
            m_timeInLight = 0.0f;
            m_wasEverLit = false;
            m_proximityPenaltyApplied = false;
            setPartsVisible(true);
            m_state = VISIBLE;
        }
        updateTransforms();
        break;
    }
    case VISIBLE: {
        m_visibleTimer += dt;
        bool hit = m_flashOn && isFlashlightOnCreature();

        if (hit) {
            if (!m_wasEverLit) {
                m_wasEverLit = true;
                if (m_visibleTimer >= 5.0f && !m_proximityPenaltyApplied) {
                    advanceProximity();
                    m_proximityPenaltyApplied = true;
                }
            }
            m_timeInLight += dt;
            if (m_timeInLight >= 10.0f) {
                setPartsVisible(false);
                m_disappearTimer = 5.0f;
                m_proximityLevel++;
                m_state = DISAPPEARED;
            }
        } else {
            m_timeInLight = 0.0f;
            if (m_visibleTimer >= 30.0f) {
                setPartsVisible(false);
                m_disappearTimer = 5.0f;
                m_state = DISAPPEARED;
            }
        }
        updateTransforms();
        break;
    }
    case STUNNED: {
        // Kept for compatibility, but merged into VISIBLE logic
        break;
    }
    case DISAPPEARED: {
        m_disappearTimer -= dt;
        if (m_disappearTimer <= 0.0f) {
            m_hiddenTimer = 8.0f + (float)(rand() % 7000) / 1000.0f;
            m_state = HIDDEN;
        }
        updateTransforms();
        break;
    }
    }
}

void Creature::choosePosition() {
    // Pick random position in forest tree rings
    // Rings: { minDist, maxDist, count }
    float ringData[][2] = {
        {7.5f, 9.0f},
        {10.0f, 12.0f},
        {14.0f, 16.0f},
        {18.0f, 20.0f},
        {23.0f, 25.0f},
        {29.0f, 32.0f},
    };
    // Proximity level reduces which rings are available
    int ringIdx = std::min(m_proximityLevel, 3);
    // Add random jitter within level
    ringIdx = std::max(0, ringIdx + (rand() % 3 - 1));
    ringIdx = std::min(ringIdx, 4);

    float minD = ringData[ringIdx][0];
    float maxD = ringData[ringIdx][1];

    float ang = (float)(rand() % 6283) / 1000.0f;
    float dist = minD + (float)(rand() % 10000) / 10000.0f * (maxD - minD);

    m_currentPos = m_forestCenter + glm::vec3(cosf(ang) * dist, 0.0f, sinf(ang) * dist);

    std::cout << "[Creature] Spawned at (" << m_currentPos.x << ", " << m_currentPos.z
              << "), level=" << m_proximityLevel << "\n";
}

void Creature::updateTransforms() {
    // Animate with two sine waves for unnatural jitter
    float t1 = m_animTime * 1.3f;
    float t2 = m_animTime * 0.7f + 1.0f;

    // Overall body sway
    float sway = sinf(t1 * 0.5f) * 0.03f;

    if (m_bodyParts.size() < 6) return;

    // Body parts indices: 0=head, 1=body, 2=lArm, 3=rArm, 4=lLeg, 5=rLeg
    // Head
    {
        auto& p = *m_bodyParts[0];
        float bob = sinf(t2 * 2.0f) * 0.02f;
        p.position = m_currentPos + glm::vec3(0.0f, 2.85f + bob, 0.0f);
        p.rotation = glm::vec3(sinf(t1 * 1.1f) * 2.0f, 0.0f, sway * 5.0f);
    }
    // Body
    {
        auto& p = *m_bodyParts[1];
        float bob = sinf(t2 * 1.5f) * 0.015f;
        p.position = m_currentPos + glm::vec3(0.0f, 2.05f + bob, 0.0f);
        p.rotation = glm::vec3(sinf(t1 * 0.9f) * 1.0f, 0.0f, sway * 3.0f);
    }
    // Left arm
    {
        auto& p = *m_bodyParts[2];
        float armSwing = sinf(t1 * 1.7f) * 8.0f + 5.0f;
        p.position = m_currentPos + glm::vec3(-0.25f, 2.0f, 0.0f);
        p.rotation = glm::vec3(armSwing + sinf(t2 * 2.3f) * 3.0f, 0.0f, sway * 8.0f);
    }
    // Right arm
    {
        auto& p = *m_bodyParts[3];
        float armSwing = sinf(t1 * 1.7f + 3.14f) * 8.0f + 5.0f;
        p.position = m_currentPos + glm::vec3(0.25f, 2.0f, 0.0f);
        p.rotation = glm::vec3(armSwing + sinf(t2 * 2.3f + 1.0f) * 3.0f, 0.0f, -sway * 8.0f);
    }
    // Left leg
    {
        auto& p = *m_bodyParts[4];
        float legSwing = sinf(t1 * 1.5f) * 2.0f;
        p.position = m_currentPos + glm::vec3(-0.12f, 0.8f, 0.0f);
        p.rotation = glm::vec3(legSwing + sinf(t2 * 0.5f) * 1.0f, 0.0f, sway * 2.0f);
    }
    // Right leg
    {
        auto& p = *m_bodyParts[5];
        float legSwing = sinf(t1 * 1.5f + 3.14f) * 2.0f;
        p.position = m_currentPos + glm::vec3(0.12f, 0.8f, 0.0f);
        p.rotation = glm::vec3(legSwing + sinf(t2 * 0.5f + 1.0f) * 1.0f, 0.0f, -sway * 2.0f);
    }
}

bool Creature::isFlashlightOnCreature() const {
    glm::vec3 toCreature = m_currentPos - m_flashPos;
    float dist = glm::length(toCreature);
    if (dist < 0.001f) return false;
    // Max flashlight range ~35 units
    if (dist > 35.0f) return false;
    toCreature /= dist;

    float cosAngle = glm::dot(toCreature, m_flashDir);
    float outerCut = cosf(glm::radians(25.0f)); // flashOuterCut
    return cosAngle >= outerCut;
}

void Creature::advanceProximity() {
    m_proximityLevel++;
    std::cout << "[Creature] Proximity increased to " << m_proximityLevel << "\n";
    // Move closer immediately
    float ang = atan2f(m_currentPos.z - m_forestCenter.z, m_currentPos.x - m_forestCenter.x);
    float currentDist = glm::length(glm::vec2(m_currentPos.x - m_forestCenter.x, m_currentPos.z - m_forestCenter.z));
    float newDist = currentDist * 0.7f;
    newDist = std::max(newDist, 3.5f);
    m_currentPos = m_forestCenter + glm::vec3(cosf(ang) * newDist, 0.0f, sinf(ang) * newDist);
}

void Creature::setPartsVisible(bool v) {
    for (auto* part : m_bodyParts) {
        if (part) part->active = v;
    }
}
