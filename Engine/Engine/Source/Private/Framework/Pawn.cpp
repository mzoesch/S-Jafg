// Copyright mzoesch. All rights reserved.

#include "Framework/Pawn.h"
#include "User/Input/InputActionValue.h"
#include "Framework/PersonaController.h"
#include "Engine/Engine.h"

Jafg::APawn::APawn(LCxxObjectInitializer const& CxxObjectInitializer) : Super(CxxObjectInitializer)
{
    this->SetEverTickConstructorOnlyFlag();
    return;
}

void Jafg::APawn::Tick(const f32 DeltaTime)
{
    Super::Tick(DeltaTime);

    this->UpdateRelativeVectors();

    algo::orphan(&this->CurrentGenericTraceResults);
    // const LVector TraceStart = this->GetTranslation();
    // const LVector TraceEnd   = this->GetTranslation() + this->GetRotator().ToVector() * 5.0f;
    // this->GetWorld()->LineTraceByChannel
    // (
    //     this->CurrentGenericTraceResults, TraceStart, TraceEnd,
    //     ECollisionChannel::Static, LCollisionQueryParams({.bSingleHit = true})
    // );

    return;
}

void Jafg::APawn::EndLife()
{
    Super::EndLife();

    if (this->IsPossessed())
    {
        this->OwningController->PossessPawn(nullptr, false);
    }

    return;
}

bool Jafg::APawn::IsPossessedLocally() const noexcept
{
    return this->OwningController && this->OwningController->IsSurfaceValid();
}

Jafg::LLocalEgo* Jafg::APawn::GetLocalEgoIfPossessed() const noexcept
{
    if (this->OwningController)
    {
        return &this->OwningController->GetLocalEgo();
    }

    return nullptr;
}

void Jafg::APawn::SetOwningController(APersonaController* InNew)
{
    this->OwningController = InNew;

#if WITH_LOCAL_LAYER
    if (InNew)
    {
        this->Eye.SetOwningPawn(this);
    }
#endif /* WITH_LOCAL_LAYER */

    return;
}

void Jafg::APawn::UpdateRelativeVectors()
{
    // this->RelativeFront.X =
    //     Maths::Cos(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    // this->RelativeFront.Y =
    //     Maths::Sin(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    // this->RelativeFront.Z =
    //     Maths::Sin(Maths::ToRadians(this->GetRotator().Pitch));
    // this->RelativeFront.Normalize();

    this->RelativeRight = this->RelativeFront.Cross(LVector::UpVector).NormalizeRet().InvertRet();
    this->RelativeUp    = this->RelativeRight.Cross(this->RelativeFront).NormalizeRet().InvertRet();

    return;
}
