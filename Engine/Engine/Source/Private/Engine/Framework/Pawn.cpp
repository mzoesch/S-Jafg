// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Framework/Pawn.h"
#include "User/Input/InputActionValue.h"

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

void Jafg::APawn::OnOngoingMovementInput(LInputActionValue& InValue)
{
    LVector TranslationDelta = LVector::Zero();
    TranslationDelta += this->RelativeFront * (InValue.Get<LVector3>().X * this->MovementSpeed);
    TranslationDelta += this->RelativeRight * (InValue.Get<LVector3>().Y * this->MovementSpeed);
    TranslationDelta += LVector::UpVector   * (InValue.Get<LVector3>().Z * this->MovementSpeed);

    this->AddTranslation(TranslationDelta);

    return;
}

void Jafg::APawn::OnOngoingRotationInput(LInputActionValue& InValue)
{
    // if (this->bFirstMouseCallback)
    // {
    //     this->LastMouseX = InValue.Get<LVector2>().X;
    //     this->LastMouseY = InValue.Get<LVector2>().Y;
    //     this->bFirstMouseCallback = false;
    // }
    //
    // const double XOffset = static_cast<double>(InValue.Get<LVector2>().X) - this->LastMouseX;
    // const double YOffset = this->LastMouseY - static_cast<double>(InValue.Get<LVector2>().Y);
    //
    // this->LastMouseX = InValue.Get<LVector2>().X;
    // this->LastMouseY = InValue.Get<LVector2>().Y;

    this->AddRotator(LRotator(
        InValue.Get<LVector2>().X * this->MouseSensitivity,
        InValue.Get<LVector2>().Y * this->MouseSensitivity,
        0.0f
    ));

    this->GetMutableRotator().ConstrainAxis(ERotatorAxis::Pitch, 89.9f);
    this->GetMutableRotator().NormalizeRotation();
    check( this->GetRotator().Pitch >= -89.9f && this->GetRotator().Pitch <= 89.9f )
    check( this->GetRotator().Yaw >= -180.0f && this->GetRotator().Yaw <= 180.0f )

    this->UpdateRelativeVectors();

    return;
}

void Jafg::APawn::OnOngoingVelocityChange(LInputActionValue& InValue)
{
    this->MovementSpeed += InValue.Get<float>();

    if (this->MovementSpeed < 0)
    {
        this->MovementSpeed = 0;
    }
    if (this->MovementSpeed > 200)
    {
        this->MovementSpeed = 200;
    }

    return;
}

void Jafg::APawn::OnOngoingPrimaryInput(LInputActionValue& InValue)
{
    check( this->GetWorld() )

    TdhArray<LHitResult> Hits;
    if (this->GetWorld()->LineTraceByChannel(
        Hits,
        this->GetTranslation(),
        this->GetTranslation() + this->GetRotator().ToVector() * 5.0f,
        ECollisionChannel::Static,
        LCollisionQueryParams()
    ) == false)
    {
        LOG_WARNING(LogUserInput, "Hit nothing.")
        return;
    }

    for (const LHitResult& Hit : Hits)
    {
        LOG_WARNING(LogUserInput, "Hit at {} -> {}.", Hit.GlobalWorldLocation.ToString(), Hit.Actor->GetTranslation().ToString())
    }

    return;
}

bool Jafg::APawn::TraceFromEyeByChannel(
    TdhArray<LHitResult>& OutHits,
    const float DistanceInMeters,
    const ECollisionChannel::Type Channel,
    const LCollisionQueryParams& Params
) const
{
    return false;
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
