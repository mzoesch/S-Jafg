// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace Jafg
{

class Camera
{
public:

    explicit Camera(const LVector& InStartLocation = LVector::Zero());

    LMatrix GetViewMatrix() const;

    void ProcessKeyboard(const Camera_Movement Dir, const float DeltaTime);
    void ProcessMouseMovement(const float XOffset, const float YOffset);
    void ProcessMouseScroll(float YOffset);

    LVector  Location;
    LRotator Rotator;

    LVector  Front;
    LVector  Right;
    LVector  Up;

    float MovementSpeed    = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom             = 70.0f;

private:

    void UpdateCameraTransform();
};

} /* Namespace Jafg */
