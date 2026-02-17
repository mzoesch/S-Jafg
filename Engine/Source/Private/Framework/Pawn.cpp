// Copyright mzoesch. All rights reserved.

#include "Framework/Pawn.h"
#include "User/UserPreferences.h"
#include "User/Input/InputActionValue.h"
#include "Framework/PersonaController.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"

void Jafg::APawn::DefaultInit()
{
    this->SetEverTickConstructorOnlyFlag();

    if (this->RootComponent == nullptr)
    {
        this->RootComponent = this->EmplaceComponent<ASceneComponent>();
        this->RootComponent->SetTranslation(maths::zero_vector<LWorldVec3>);
    }

    return;
}

void Jafg::APawn::Tick(const f32 DeltaTime)
{
    Super::Tick(DeltaTime);
    // algo::orphan(&this->CurrentGenericTraceResults);

    // const LVector TraceStart = this->GetTranslation();
    // const LVector TraceEnd   = this->GetTranslation() + this->GetRotator().ToVector() * 5.0f;
    // this->GetWorld()->LineTraceByChannel
    // (
    //     this->CurrentGenericTraceResults, TraceStart, TraceEnd,
    //     ECollisionChannel::Static, LCollisionQueryParams({.bSingleHit = true})
    // );

    return;
}

void Jafg::APawn::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    Super::OnGarbage(Reason);
    if (this->IsPossessed())
    {
        this->OwningController->PossessPawn(nullptr, false);
    }
    return;
}

Jafg::LEye_v2 Jafg::APawn::GetEye_v2() const noexcept
{
    check( this->RootComponent )
    auto Vs{this->RootComponent->GetRelativeVectors()};
    return {
        .VertFov = this->VertFov,
        .NearFrustum = this->NearFrustum,
        .FarFrustum = this->FarFrustum,
        .Location = this->RootComponent->GetTranslation(),
        .Front = Vs.Front,
        .Up = Vs.Up,
        };
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
    check(this->_Lives())

    this->OwningController = InNew;

#if WITH_LOCAL_LAYER
    if (InNew)
    {
        this->NearFrustum = InNew->GetLocalEgo().GetVariable_FrustumNearPlane();
        this->FarFrustum = InNew->GetLocalEgo().GetVariable_FrustumFarPlane();
    }
#endif /* WITH_LOCAL_LAYER */

    return;
}
