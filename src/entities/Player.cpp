#include "entities/Player.h"
#include "core/Game.h"
#include "core/InputManager.h"
#include <algorithm>
#include <glm/glm.hpp>

static const glm::vec3 CHAIR_POS(0.5f, 0.0f, -0.3f);
static const float SIT_EYE = 1.0f;
static const float STAND_EYE = 1.7f;
static const float SIT_DIST = 1.0f;

Player::Player() : GameObject("Player") {
    camera = Camera(glm::vec3(0.0f, 1.7f, 3.0f));
}

void Player::update(float dt) {
    if (!isAlive) return;
    handleInteraction();
    if (!isSitting) {
        handleMovement(dt);
        handleFlashlight();
        updateStamina(dt);
    }
    position = camera.Position;
}

void Player::handleInteraction() {
    auto& input = InputManager::get();
    static bool prevE = false;
    bool curE = input.isKeyPressed(GLFW_KEY_E);

    if (curE && !prevE) {
        if (isSitting) {
            isSitting = false;
            camera.Position.y = STAND_EYE;
        } else {
            glm::vec3 diff = camera.Position - CHAIR_POS;
            diff.y = 0.0f;
            float dist = glm::dot(diff, diff);
            if (dist < SIT_DIST * SIT_DIST) {
                isSitting = true;
                camera.Position.y = SIT_EYE;
            }
        }
    }
    prevE = curE;
}

void Player::handleMovement(float dt) {
    auto& input = InputManager::get();

    isRunning = input.isKeyPressed(GLFW_KEY_LEFT_SHIFT) && stamina > 0.0f;
    camera.Speed = isRunning ? 8.0f : 4.0f;

    if (input.isKeyPressed(GLFW_KEY_W)) camera.moveForward(dt);
    if (input.isKeyPressed(GLFW_KEY_S)) camera.moveBackward(dt);
    if (input.isKeyPressed(GLFW_KEY_A)) camera.moveLeft(dt);
    if (input.isKeyPressed(GLFW_KEY_D)) camera.moveRight(dt);

    camera.Position.x = glm::clamp(camera.Position.x, -2.7f, 2.7f);
    camera.Position.z = glm::clamp(camera.Position.z, -2.7f, 2.7f);
    camera.Position.y = STAND_EYE;

    glm::vec2 delta = input.getMouseDelta();
    camera.processMouseMove(delta.x, -delta.y);
}

void Player::handleFlashlight() {
    auto& input = InputManager::get();
    if (input.isKeyJustPressed(GLFW_KEY_F))
        camera.toggleFlashlight();
}

void Player::updateStamina(float dt) {
    if (isRunning)
        stamina = std::max(0.0f, stamina - 20.0f * dt);
    else
        stamina = std::min(100.0f, stamina + 8.0f * dt);
}

void Player::takeSanityDamage(float amount) {
    sanity = std::max(0.0f, sanity - amount);
}
