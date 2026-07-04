#pragma once
#include "entities/GameObject.h"
#include "core/Camera.h"

class Player : public GameObject {
public:
    Camera camera;

    float stamina    = 100.0f;
    float sanity     = 100.0f;
    bool  isRunning  = false;
    bool  isAlive    = true;

    bool  isSitting  = false;

    Player();

    void update(float dt) override;
    void takeSanityDamage(float amount);

private:
    void handleMovement(float dt);
    void handleFlashlight();
    void handleInteraction();
    void updateStamina(float dt);
};
