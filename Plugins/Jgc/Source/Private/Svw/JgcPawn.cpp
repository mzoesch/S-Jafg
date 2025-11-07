// Copyright mzoesch. All rights reserved.

#include "Svw/JgcPawn.h"
#include "Core/JgcNames.h"
#include "User/Input/InputActionValue.h"

void Jgc::AJgcPawn::BeginLifeCDR()
{
    Super::BeginLifeCDR();

    Jafg::LUserInputRegistry& Registry{ this->GetLocalEgo().GetUserInputRegistry() };

    if (auto* Ctx{ Registry.RegisterContext({Name_UicNaiveFly, "Naive Fly"}) })
    {
        Ctx->MapAction(
            &Registry,
            {Name_UsrinNaiveMove, "Naive Move", Jafg::EInputActionCategory::Axis3D},
            TArray<Jafg::LInputTrigger>{}
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Forward",
                    Jafg::EKeys::W,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Left",
                    Jafg::EKeys::A,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeySwizzleModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Backwards",
                    Jafg::EKeys::S,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Right",
                    Jafg::EKeys::D,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeySwizzleModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Up",
                    Jafg::EKeys::E,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                })
                .reflexive_emplace_back(Jafg::LInputTrigger
                {
                    "Down",
                    Jafg::EKeys::Q,
                    Jafg::EInputActionTrigger::Ongoing,
                    Jafg::LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyNegateModifier>())
                        .reflexive_emplace_back(Jafg::MakeModifier<Jafg::LInputActionMappedKeyDeltaTimeModifier>())
                }),
                AJgcPawn::OnOngoingNaiveMove
            );
    }
    else
    {
        LOG_ERROR(LogUserInput, "Failed to register naive fly input context.")
    }

    return;
}

void Jgc::AJgcPawn::SetOwningController(Jafg::APersonaController* InNew)
{
    if (this->IsPossessedLocally())
    {
        this->GetOwningControllerChecked()->GetSurfaceChecked()->GetUserInput().DeactivateContext(Name_UicNaiveFly);
    }

    Super::SetOwningController(InNew);

    if (this->IsPossessedLocally())
    {
        this->GetOwningControllerChecked()->GetSurfaceChecked()->GetUserInput().ActivateContext(Name_UicNaiveFly);
    }

    return;
}

void Jgc::AJgcPawn::OnOngoingNaiveMove(Jafg::LViewport& Viewport, Jafg::LInputActionValue& Value)
{
    if (auto* Controller{ Viewport.GetSurface().GetController() })
    {
        if (auto* Pawn{ Controller->GetPawn() })
        {
            if (auto* JgcPawn{ Pawn->As<AJgcPawn>() })
            {
                LVector Delta;
                Delta += JgcPawn->GetRelativeFront() * (Value.Get<LVector3>().X * JgcPawn->GetMovementSpeed());
                Delta += JgcPawn->GetRelativeRight() * (Value.Get<LVector3>().Y * JgcPawn->GetMovementSpeed());
                Delta += LVector::UpVector           * (Value.Get<LVector3>().Z * JgcPawn->GetMovementSpeed());

                JgcPawn->AddTranslation(Delta);
            }
        }
    }

    return;
}
