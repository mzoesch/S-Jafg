// Copyright mzoesch. All rights reserved.

#include "Framework/Eye.h"
#include "Framework/Pawn.h"
#include "User/LocalEgo.h"

void Jafg::LEye::SetOwner(APawn* InOwner) noexcept
{
    this->Owner = InOwner;

    if (this->Owner == nullptr)
    {
        return;
    }

    if (const LLocalEgo* Ego = this->GetOwner()->GetPossessedEgo(); Ego)
    {
        this->NearFrustum = Ego->GetVariable_FrustumNearPlane();
        this->FarFrustum  = Ego->GetVariable_FrustumFarPlane();
    }

    return;
}

void Jafg::LEye::UpdateViewMatrix()
{
    this->UpdateRelativeVectors();

    Maths::MakeViewMatrixInline(
        &this->CachedViewMatrix,
        this->Owner->GetTranslation(),
        this->Owner->GetTranslation() + this->RelativeFront,
        this->RelativeUp
    );

    return;
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
