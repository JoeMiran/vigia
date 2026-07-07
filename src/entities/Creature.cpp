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
    TextureCache::makeColor("creature_skin", 0, 0, 0);
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

    switch (m_state) {
    case HIDDEN:
        choosePosition();
        setPartsVisible(true);
        m_state = VISIBLE;
        m_moveTimer = 6.0f; // force first teleport next frame
        std::cout << "[Creature] Visible at radius " << m_currentRadius << "\n";
        updateTransforms();
        break;
    case VISIBLE: {
        bool hit = m_flashOn && isFlashlightOnCreature();

        if (hit) {
            m_timeWithoutLight = 0.0f;
        } else {
            m_timeWithoutLight += dt;
            if (m_timeWithoutLight >= 4.0f) {
                m_currentRadius = std::max(0.0f, m_currentRadius - 1.0f);
                m_timeWithoutLight = 0.0f;
                m_moveTimer = 0.0f;
                if (m_currentRadius <= 0.0f) {
                    std::cout << "[Creature] LIGHTS OUT!\n";
                    m_lightsOut = true;
                    m_jumpscareTimer = 2.0f;
                    m_state = JUMPSCARE;
                    break;
                }
                choosePosition();
                std::cout << "[Creature] Closer! Radius " << m_currentRadius << "\n";
            }
        }

        m_moveTimer += dt;
        if (m_moveTimer >= 6.0f) {
            choosePosition();
            m_moveTimer = 0.0f;
        }

        updateTransforms();
        break;
    }
    case JUMPSCARE:
        m_jumpscareTimer -= dt;
        if (m_jumpscareTimer <= 0.0f) {
            m_gameOver = true;
        }
        break;
    }
}

void Creature::choosePosition() {
    float ang = (float)(rand() % 6283) / 1000.0f;
    float dist = m_currentRadius;
    m_currentPos = m_forestCenter + glm::vec3(cosf(ang) * dist, 0.0f, sinf(ang) * dist);
}

void Creature::updateTransforms() {
    if (m_bodyParts.size() < 8) return;

    // Always face the player
    glm::vec3 toPlayer = m_playerPos - m_currentPos;
    float facingAngle = 0.0f;
    if (glm::length(toPlayer) > 0.001f) {
        facingAngle = glm::degrees(atan2(toPlayer.x, toPlayer.z));
    }

    // Rotate offset vectors by the facing angle
    float rad = glm::radians(facingAngle);
    float c = cosf(rad);
    float s = sinf(rad);
    auto rot = [c, s](float x, float z) -> glm::vec2 {
        return glm::vec2(x * c - z * s, x * s + z * c);
    };

    // Head
    auto ho = rot(0.0f, 0.0f);
    glm::vec3 headPos = m_currentPos + glm::vec3(ho.x, 2.85f, ho.y);
    m_bodyParts[0]->position = headPos;
    m_bodyParts[0]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);

    // Body
    auto bo = rot(0.0f, 0.0f);
    m_bodyParts[1]->position = m_currentPos + glm::vec3(bo.x, 2.05f, bo.y);
    m_bodyParts[1]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);

    // Left arm
    auto la = rot(-0.25f, 0.0f);
    m_bodyParts[2]->position = m_currentPos + glm::vec3(la.x, 2.0f, la.y);
    m_bodyParts[2]->rotation = glm::vec3(5.0f, facingAngle, 0.0f);

    // Right arm
    auto ra = rot(0.25f, 0.0f);
    m_bodyParts[3]->position = m_currentPos + glm::vec3(ra.x, 2.0f, ra.y);
    m_bodyParts[3]->rotation = glm::vec3(-5.0f, facingAngle, 0.0f);

    // Left leg
    auto ll = rot(-0.12f, 0.0f);
    m_bodyParts[4]->position = m_currentPos + glm::vec3(ll.x, 0.8f, ll.y);
    m_bodyParts[4]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);

    // Right leg
    auto rl = rot(0.12f, 0.0f);
    m_bodyParts[5]->position = m_currentPos + glm::vec3(rl.x, 0.8f, rl.y);
    m_bodyParts[5]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);

    // Eyes (relative to head)
    auto eL = rot(-0.09f, 0.13f);
    m_bodyParts[6]->position = headPos + glm::vec3(eL.x, 0.02f, eL.y);
    m_bodyParts[6]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);

    auto eR = rot(0.09f, 0.13f);
    m_bodyParts[7]->position = headPos + glm::vec3(eR.x, 0.02f, eR.y);
    m_bodyParts[7]->rotation = glm::vec3(0.0f, facingAngle, 0.0f);
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
    int count = 0;
    for (auto* part : m_bodyParts) {
        if (part) { part->active = v; count++; }
    }
    std::cout << "[Creature] Parts " << (v ? "visible" : "hidden") << " (" << count << " parts)\n";
}
