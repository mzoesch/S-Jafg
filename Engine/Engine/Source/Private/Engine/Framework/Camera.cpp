// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Camera.h"

Jafg::Camera::Camera(glm::vec3 position, float yaw, float pitch)
: MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    Yaw      = yaw;
    Pitch    = pitch;

    updateCameraVectors();
}

Jafg::Camera::Camera(float posX, float posY, float posZ, float yaw, float pitch)
: MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Jafg::Camera::GetViewMatrix()
{
    glm::vec3 LookAtLocation = Position;
    // LookAtLocation *= glm::vec3(-1, -1, 1);
    // LookAtLocation = glm::vec3(LookAtLocation.y, LookAtLocation.x, LookAtLocation.z);

    return glm::lookAt(LookAtLocation, LookAtLocation + LocalFront, LocalUp);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Jafg::Camera::ProcessKeyboard(const Camera_Movement Dir, const float DeltaTime)
{
    const float Vel = MovementSpeed * DeltaTime;

    // const glm::vec3 Forward = glm::vec3(LVector::ForwardVector.X, LVector::ForwardVector.Y, LVector::ForwardVector.Z);
    // const glm::vec3 Right = glm::vec3(LVector::RightVector.X, LVector::RightVector.Y, LVector::RightVector.Z);
    const glm::vec3 Up = glm::vec3(TVector<float>::UpVector.X, LVector::UpVector.Y, LVector::UpVector.Z);

    const glm::vec3 Forward = LocalFront;
    const glm::vec3 Right   = LocalRight;

    if (Dir == FORWARD)
    {
        Position += Forward * Vel;
    }
    else if (Dir == BACKWARD)
    {
        Position -= Forward * Vel;
    }
    else if (Dir == LEFT)
    {
        Position -= Right * Vel;
    }
    else if (Dir == RIGHT)
    {
        Position += Right * Vel;
    }
    else if (Dir == UP)
    {
        Position += Up * Vel;
    }
    else if (Dir == DOWN)
    {
        Position -= Up * Vel;
    }

    return;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Jafg::Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= -MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Jafg::Camera::ProcessMouseScroll(const float YOffset)
{
    MovementSpeed += YOffset;
    if (MovementSpeed < 0)
        MovementSpeed = 0;
    if (MovementSpeed > 50)
        MovementSpeed = 50;

    // std::cout << "Camera speed: " << MovementSpeed << '\n';
    // std::cout.flush();
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Jafg::Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    LocalFront = glm::normalize(front);

    LocalFront = glm::vec3(LocalFront.x, LocalFront.z, LocalFront.y);

    // also re-calculate the Right and Up vector

    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm::vec3 WorldUp = glm::vec3(LVector::UpVector.X, LVector::UpVector.Y, LVector::UpVector.Z);
    LocalRight = glm::normalize(glm::cross(LocalFront, WorldUp));

    LocalUp = glm::normalize(glm::cross(LocalRight, LocalFront));
}
