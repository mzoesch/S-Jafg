// Copyright mzoesch. All rights reserved.

#include "Components/DebugCameraComponent.h"
#include "Components/SceneComponent.h"

void Jgc::ADebugCameraComponent::OnAttach(Jafg::AActor& InOwner)
{
    Super::OnAttach(InOwner);

    if (this->GetOwner().IsA<Jafg::APawn>() == false)
    {
        LOG_FATAL(LogUserInput, "Class [{}] requires to be attached a APawn but is on [{}].",
            this->GetNameAsString(),
            this->GetOwner().GetNameAsString()
            )
    }

    Jafg::APawn& Pawn{*Jafg::StaticCastChecked<Jafg::APawn>(&this->GetOwner())};

    if (auto* Ctrl{Pawn.GetOwningController()})
    {
        if (auto* Surface{Ctrl->GetSurface()})
        {
            Surface->GetUserInput().ActivateContext(Jafg::LUserInputTag::AsTagChecked("DebugCamera"));
        }
    }

    return;
}

void Jgc::ADebugCameraComponent::OnMove(Jafg::LInputActionValue const& Value)
{
    if (auto* Scene{this->GetOwner().GetComponent<Jafg::ASceneComponent>()})
    {
        auto Value3D{Value.GetAxis3DValue()};
        auto Vs{Scene->GetRelativeVectors()};

        // if:
        //  fwd: -z
        //  up: +y
        //  right: +x
        glm::vec3 front;
        front.x = -cos(glm::radians(Scene->TempRot.x)) * sin(glm::radians(Scene->TempRot.y));
        front.y = sin(glm::radians(Scene->TempRot.x));
        front.z = cos(glm::radians(Scene->TempRot.x)) * cos(glm::radians(Scene->TempRot.y));
        front   = glm::normalize(front);

        LVec3F Translation{ maths::zero_vector<LVec3F> };

        // fwd back
        Translation += front * Value3D.x;
        // left right
        Translation += glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))) * Value3D.y;
        // up down
        Translation += glm::vec3{0,-1,0} // TODO: Make this an option for the user. (or Vs.Up)
            * Value3D.z;

        // TODO: Make this an option for the user.
        // Translation += Vs.Up * Value3D.Z;
        // Translation += maths::up_vector<LVec3F> * Value3D.z;
        // Translation.z += Value3D.z;

        Scene->TempPos += Translation;

        glm::vec3 front_jafg;
        front_jafg.y = -cos(Scene->TempRot.x) * sin(Scene->TempRot.y);
        front_jafg.z = sin(Scene->TempRot.x);
        front_jafg.x = -cos(Scene->TempRot.x) * cos(Scene->TempRot.y);
        front_jafg   = glm::normalize(front_jafg);
        LVec3F Translation_jafg{ maths::zero_vector<LVec3F> };
        Translation_jafg += front_jafg * -Value3D.x;
        Translation_jafg += glm::normalize(glm::cross(front_jafg, glm::vec3(0.0f, 0.0f, 1.0f))) * Value3D.y;
        Translation_jafg += glm::vec3{0,0,1} * Value3D.z;

        Scene->TempPos_jafg += Translation_jafg;
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwner().GetNameAsString()
            )
    }


    return;
}

void Jgc::ADebugCameraComponent::OnRotate(Jafg::LInputActionValue const& Value)
{
    if (auto* Scene{this->GetOwner().GetComponent<Jafg::ASceneComponent>()})
    {
        f32 sensitivity = 0.1f; // radians per pixel
        auto delta{Value.GetAxis2DValue()};
        Scene->TempRot += maths::radians(LWorldVec3(-delta.y, delta.x, 0.0)) * sensitivity;
        maths::pitch_l(&Scene->TempRot) = maths::clamp(maths::pitch(Scene->TempRot), maths::radians(-89.9f), maths::radians(89.9f));
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwner().GetNameAsString()
            )
    }

    return;
}
