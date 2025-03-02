// Copyright mzoesch. All rights reserved.

#include "Engine/Framework/Eye.h"
#include "Engine/Framework/Pawn.h"

Jafg::LMatrix Jafg::LEye::GetViewMatrix() const
{
    this->UpdateRelativeVectors();
    return Maths::MakeViewMatrix(this->Owner->GetTranslation(),
        this->Owner->GetTranslation() + this->RelativeFront, this->RelativeUp);
}

void Jafg::LEye::UpdateRelativeVectors() const
{
    const LRotator Rotator = this->Owner->GetRotator();

    this->RelativeFront.X =
    Maths::Cos(Maths::ToRadians(Rotator.Yaw)) * Maths::Cos(Maths::ToRadians(Rotator.Pitch));
    this->RelativeFront.Y =
        Maths::Sin(Maths::ToRadians(Rotator.Yaw)) * Maths::Cos(Maths::ToRadians(Rotator.Pitch));
    this->RelativeFront.Z =
        Maths::Sin(Maths::ToRadians(Rotator.Pitch));
    this->RelativeFront.Normalize();

    this->RelativeRight = this->RelativeFront.Cross(LVector::UpVector).NormalizeRet().InvertRet();
    this->RelativeUp    = this->RelativeRight.Cross(this->RelativeFront).NormalizeRet().InvertRet();

    return;
}
