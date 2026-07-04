#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ─── Camera ──────────────────────────────────────────────────────
// Câmera FPS.
// A lanterna (spotlight) está acoplada à câmera:
//   - posição  = camera.Position
//   - direção  = camera.Front
// ─────────────────────────────────────────────────────────────────
class Camera {
public:
    // Posição e orientação
    glm::vec3 Position  = glm::vec3(0.0f, 1.7f, 3.0f); // altura de um humano
    glm::vec3 Front     = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up        = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 Right     = glm::vec3(1.0f, 0.0f,  0.0f);
    glm::vec3 WorldUp   = glm::vec3(0.0f, 1.0f,  0.0f);

    // Euler angles
    float Yaw   = -90.0f;
    float Pitch =   0.0f;

    // Configurações
    float Speed       = 4.0f;
    float Sensitivity = 0.10f;
    float Fov         = 75.0f;

    // Lanterna
    bool  flashlightOn  = true;
    float flashInnerCut = 18.0f;
    float flashOuterCut = 25.0f;

    Camera() { updateVectors(); }
    explicit Camera(glm::vec3 pos) : Position(pos) { updateVectors(); }

    glm::mat4 getViewMatrix()       const { return glm::lookAt(Position, Position + Front, Up); }
    glm::vec3 getFlashlightPos()    const { return Position; }
    glm::vec3 getFlashlightDir()    const { return Front; }

    void moveForward (float dt) { Position += Front  * Speed * dt; }
    void moveBackward(float dt) { Position -= Front  * Speed * dt; }
    void moveLeft    (float dt) { Position -= Right  * Speed * dt; }
    void moveRight   (float dt) { Position += Right  * Speed * dt; }

    void processMouseMove(float xOffset, float yOffset) {
        Yaw   += xOffset * Sensitivity;
        Pitch  = glm::clamp(Pitch + yOffset * Sensitivity, -89.0f, 89.0f);
        updateVectors();
    }

    void toggleFlashlight() { flashlightOn = !flashlightOn; }

private:
    void updateVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
