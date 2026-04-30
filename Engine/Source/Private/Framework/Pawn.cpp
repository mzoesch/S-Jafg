// Copyright mzoesch. All rights reserved.

#include "Framework/Pawn.h"
#include "User/UserPreferences.h"
#include "User/Input/InputActionValue.h"
#include "Framework/PersonaController.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"
#include "Components/PawnComponent.h"

#if JAFG_DO_CHECKS
void Jafg::APawnComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    if (this->GetOwningActor().IsA<APawn>() == false)
    {
        LOG_FATAL(LogUserInput, "Class [{}] requires to be attached a APawn but is on [{}].",
            this->GetNameAsString(),
            this->GetOwningActor().GetNameAsString()
        )
    }

    return;
}
#endif /* JAFG_DO_CHECKS */

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

void Jafg::APawn::Tick(const f32 Dt)
{
    Super::Tick(Dt);
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

void Jafg::APawn::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);
    if (this->IsOwningControllerValid())
    {
        this->GetOwningControllerChecked()->PossessPawn(nullptr, true);
    }
    return;
}

Jafg::LEye_v2 Jafg::APawn::GetEye() const noexcept
{
    check(this->RootComponent)
    return {
        .VertFov = this->VertFov,
        .NearFrustum = this->NearFrustum,
        .FarFrustum = this->FarFrustum,
        .Translation = this->RootComponent->GetTranslation(),
        .Front = this->RootComponent->GetRotator() * maths::forward_vector<LWorldVec3>,
        /* Maybe hard lock this to maths::up_vector? */
        .Up = this->RootComponent->GetRotator() * maths::up_vector<LWorldVec3>,
        };
}

bool Jafg::APawn::IsPossessedLocally() const noexcept
{
    return this->IsOwningControllerValid() && this->GetOwningControllerChecked()->IsOwningNodeValid();
}

Jafg::LLocalEgo* Jafg::APawn::GetLocalEgoIfPossessed() const noexcept
{
    if (this->OwningController)
    {
        return &this->OwningController->GetMutableLocalEgo();
    }

    return nullptr;
}

void Jafg::APawn::_SetOwningController(APersonaController* New)
{
    check(this->_Lives())
    check(this->OwningController != New)

    this->OwningController = New;

#if WITH_LOCAL_LAYER
    if (New)
    {
        this->NearFrustum = New->GetLocalEgo().GetVariable_FrustumNearPlane();
        this->FarFrustum = New->GetLocalEgo().GetVariable_FrustumFarPlane();
    }
#endif /* WITH_LOCAL_LAYER */

    for (auto const& Comp : this->GetComponents())
    {
        if (auto* PawnComp{Comp->As<APawnComponent>()})
        {
            PawnComp->OnNewPersonaController(this->OwningController);
        }
    }

    return;
}
