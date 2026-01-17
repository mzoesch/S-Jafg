// Copyright mzoesch. All rights reserved.

#include "Components/DebugCameraComponent.h"
#include "Components/SceneComponent.h"

void Jgc::JDebugCameraComponent::OnAttach(Jafg::AActor* InOwner)
{
    Super::OnAttach(InOwner);

    if (this->GetOwner()->IsA<Jafg::APawn>() == false)
    {
        LOG_FATAL(LogUserInput, "Class [{}] requires to be attached a APawn but is on [{}].",
            this->GetNameAsString(),
            this->GetOwner()->GetNameAsString()
            )
    }

    Jafg::APawn* Pawn{Jafg::StaticCastChecked<Jafg::APawn>(this->GetOwner())};

    if (auto* Ctrl{Pawn->GetOwningController()})
    {
        if (auto* Surface{Ctrl->GetSurface()})
        {
            Surface->GetUserInput().ActivateContext(Jafg::LUserInputTag::AsTagChecked("DebugCamera"));
        }
    }

    return;
}

void Jgc::JDebugCameraComponent::OnMove(Jafg::LInputActionValue const& Value)
{
    if (auto* Scene{this->GetOwner()->GetComponent<Jafg::JSceneComponent>()})
    {
        auto Value3D{Value.GetAxis3DValue()};
        auto Vs{Scene->GetRelativeVectors()};

        LVector3F Translation;
        Translation += Vs.Front * Value3D.X;
        Translation += Vs.Right * Value3D.Y;

        // TODO: Make this an option for the user.
        // Translation += Vs.Up * Value3D.Z;
        Translation += LVector3F::UpVector * Value3D.Z;

        Scene->AddTranslation(Translation);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwner()->GetNameAsString()
            )
    }

    return;
}

void Jgc::JDebugCameraComponent::OnRotate(Jafg::LInputActionValue const& Value)
{
    if (auto* Scene{this->GetOwner()->GetComponent<Jafg::JSceneComponent>()})
    {
        auto Value2D{Value.GetAxis2DValue()};

        auto Rotator{Scene->GetRotator()};
        Rotator += LRotator{
              Value2D.X * this->Sensitivity
            , Value2D.Y * this->Sensitivity
            , 0.0f};
        // Rotator *= this->Sensitivity;
        Rotator.ConstrainAxis(Lal::ERotatorAxis::Pitch, 89.9f);
        Rotator.NormalizeRotation();
        check( Rotator.Pitch >= -89.9f && Rotator.Pitch <= 89.9f )
        check( Rotator.Yaw >= -180.0f && Rotator.Yaw <= 180.0f )

        Scene->SetRotator(Rotator);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwner()->GetNameAsString()
            )
    }

    return;
}
