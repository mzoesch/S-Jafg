// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Camera.h"

Jafg::Camera::Camera(const LVector& InStartLocation)
{
    this->Location = InStartLocation;
    this->Rotator  = LRotator::ZeroRotator;
    this->UpdateCameraTransform();
    return;
}

Jafg::LMatrix Jafg::Camera::GetViewMatrix() const
{
    return Maths::MakeViewMatrix(this->Location, this->Location + this->Front, this->Up);

    // glm::mat4 Result;
    // for (int32 i = 0; i < 4; i++)
    // {
    //     for (int32 j = 0; j < 4; j++)
    //     {
    //         Result[i][j] = Mat.Matrix[i][j];
    //     }
    // }
    //
    // return Result;
}

void Jafg::Camera::ProcessKeyboard(const Camera_Movement Dir, const float DeltaTime)
{
    const float Vel = this->MovementSpeed * DeltaTime;

    if (Dir == FORWARD)
    {
        this->Location += this->Front * Vel;
    }
    else if (Dir == BACKWARD)
    {
        this->Location -= this->Front * Vel;
    }
    else if (Dir == LEFT)
    {
        this->Location -= this->Right * Vel;
    }
    else if (Dir == RIGHT)
    {
        this->Location += this->Right * Vel;
    }
    else if (Dir == UP)
    {
        this->Location += LVector::UpVector * Vel;
    }
    else if (Dir == DOWN)
    {
        this->Location -= LVector::UpVector * Vel;
    }

    return;
}

void Jafg::Camera::ProcessMouseMovement(float XOffset, float YOffset)
{
    XOffset *= this->MouseSensitivity;
    YOffset *= this->MouseSensitivity;

    this->Rotator.Yaw   += XOffset;
    this->Rotator.Pitch += YOffset;

    this->Rotator.ConstrainAxis(ERotatorAxis::Pitch, 89.9f);
    this->Rotator.NormalizeRotation();
    check( this->Rotator.Pitch >= -89.9f && this->Rotator.Pitch <= 89.9f )
    check( this->Rotator.Yaw >= -180.0f && this->Rotator.Yaw <= 180.0f )

    this->UpdateCameraTransform();

    return;
}

void Jafg::Camera::ProcessMouseScroll(const float YOffset)
{
    this->MovementSpeed += YOffset;

    if (this->MovementSpeed < 0)
    {
        this->MovementSpeed = 0;
    }
    if (this->MovementSpeed > 50)
    {
        this->MovementSpeed = 50;
    }

    return;
}

void Jafg::Camera::UpdateCameraTransform()
{
    this->Front.X = Maths::Cos(Maths::ToRadians(this->Rotator.Yaw)) * Maths::Cos(Maths::ToRadians(this->Rotator.Pitch));
    this->Front.Y = Maths::Sin(Maths::ToRadians(this->Rotator.Yaw)) * Maths::Cos(Maths::ToRadians(this->Rotator.Pitch));
    this->Front.Z = Maths::Sin(Maths::ToRadians(this->Rotator.Pitch));
    this->Front.Normalize();

    this->Right = this->Front.Cross(LVector::UpVector).NormalizeRet().InvertRet();
    this->Up    = this->Right.Cross(this->Front).NormalizeRet().InvertRet();

    return;
}
