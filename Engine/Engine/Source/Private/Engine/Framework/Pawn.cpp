// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Framework/Pawn.h"

void Jafg::APawn::DeclareNewPossessor(APersonaController* InNewController)
{
    this->OwningController = InNewController;


#if WITH_LOCAL_LAYER
    if (InNewController)
    {
        this->Eye = new LEye(this);
    }
#endif /* WITH_LOCAL_LAYER */

    return;
}

void Jafg::APawn::ProcessKeyboard(const Camera_Movement Dir, const float DeltaTime)
{
    const float Vel = this->MovementSpeed * DeltaTime;

    LVector TranslationDelta = LVector::Zero();
    if (Dir == FORWARD)
    {
        TranslationDelta += this->RelativeFront * Vel;
    }
    else if (Dir == BACKWARD)
    {
        TranslationDelta -= this->RelativeFront * Vel;
    }
    else if (Dir == LEFT)
    {
        TranslationDelta -= this->RelativeRight * Vel;
    }
    else if (Dir == RIGHT)
    {
        TranslationDelta += this->RelativeRight * Vel;
    }
    else if (Dir == UP)
    {
        TranslationDelta += LVector::UpVector * Vel;
    }
    else if (Dir == DOWN)
    {
        TranslationDelta -= LVector::UpVector * Vel;
    }

    this->AddTranslation(TranslationDelta);

    return;
}

void Jafg::APawn::ProcessMouseMovement(float XOffset, float YOffset)
{
    XOffset *= this->MouseSensitivity;
    YOffset *= this->MouseSensitivity;

    this->AddRotator(LRotator(YOffset, XOffset, 0.0f));

    this->GetMutableRotator().ConstrainAxis(ERotatorAxis::Pitch, 89.9f);
    this->GetMutableRotator().NormalizeRotation();
    check( this->GetRotator().Pitch >= -89.9f && this->GetRotator().Pitch <= 89.9f )
    check( this->GetRotator().Yaw >= -180.0f && this->GetRotator().Yaw <= 180.0f )

    this->UpdateRelativeVectors();

    return;
}

void Jafg::APawn::ProcessMouseScroll(const float YOffset)
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

void Jafg::APawn::UpdateRelativeVectors()
{
    this->RelativeFront.X =
        Maths::Cos(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Y =
        Maths::Sin(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Z =
        Maths::Sin(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Normalize();

    this->RelativeRight = this->RelativeFront.Cross(LVector::UpVector).NormalizeRet().InvertRet();
    this->RelativeUp    = this->RelativeRight.Cross(this->RelativeFront).NormalizeRet().InvertRet();

    return;
}
