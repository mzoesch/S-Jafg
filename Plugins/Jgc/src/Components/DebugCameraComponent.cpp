// Copyright mzoesch. All rights reserved.

#include "Components/DebugCameraComponent.h"
#include "Components/SceneComponent.h"
#include "Framework/LackeyForward.h"
#include "Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionValue.h"

bool Jgc::ADebugCameraComponent::ActivateUserInputContext() const noexcept
{
    if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<Jafg::ELackey::Local>()})
        {
            bool b1{Lackey->GetUserInput().ActivateContext(Jafg::LUserInputTag::AsTagChecked("RhiDebug"))};
            bool b2{Lackey->GetUserInput().ActivateContext(Jafg::LUserInputTag::AsTagChecked("DebugCamera"))};
            return b1 && b2;
        }
    }
    return false;
}

void Jgc::ADebugCameraComponent::OnMove(Jafg::LInputActionValue const& Value)
{
    if (auto* Sc{this->GetOwningActor().GetComponent<Jafg::ASceneComponent>()})
    {
        LWorldVec3 Front{Sc->GetRotator() * maths::forward_vector<LVec3F>};

        auto Value3D{Value.GetAxis3DValue() * 10.0f};

        Value3D *= this->GetWorld().GetDeltaTime();

        LWorldVec3 Delta{maths::zero_vector<LWorldVec3>};
        Delta += Front * Value3D.x;
        Delta += glm::normalize(glm::cross(Front, maths::up_vector<LWorldVec3>)) * Value3D.y;
        Delta += maths::up_vector<LWorldVec3> * Value3D.z;
        Sc->AddTranslation(Delta);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwningActor().GetNameAsString()
            )
    }

    return;
}

void Jgc::ADebugCameraComponent::OnRotate(Jafg::LInputActionValue const& Value)
{
    if (auto* Sc{this->GetOwningActor().GetComponent<Jafg::ASceneComponent>()})
    {
        auto Value2D{Value.GetAxis2DValue() * this->Sensitivity};

        /* Yaw */
        Sc->AddRotator(maths::angle_axis(-glm::radians(Value2D.x), maths::up_vector<LWorldVec3>), Jafg::ESceneSweep::Teleport);

        /* Pitch */
        LWorldReal Pitch{maths::clamp(this->CachedPitch + glm::radians(Value2D.y), glm::radians(-89.9f), glm::radians(89.9f))};
        Sc->AddRotator(maths::angle_axis(-(this->CachedPitch - Pitch), Sc->GetRotator() * maths::right_vector<LWorldVec3>), Jafg::ESceneSweep::Teleport);

        Sc->SetRotator(maths::normalize(Sc->GetRotator()), Jafg::ESceneSweep::Teleport);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a ::Jafg::SceneComponent.",
            this->GetNameAsString(),
            this->GetOwningActor().GetNameAsString()
            )
    }

    return;
}
