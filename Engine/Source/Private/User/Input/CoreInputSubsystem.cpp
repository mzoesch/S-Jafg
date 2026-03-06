// Copyright mzoesch. All rights reserved.

#include "User/Input/CoreInputSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/Frontend.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/InputAction.h"
#include "Engine/Engine.h"

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    LUserInputRegistry& Registry{this->GetLocalEgo().GetUserInputRegistry()};

    if (LUserInputContext* Context{Registry.RegisterContext({LUserInputTag::ToTag("RhiDebug"), "Rhi Debug"})})
    {
        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("ToggleRhiPolygonMode"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F1, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                if (auto& Prefs{GetMutableSingleton<JUserPreferences>()}; Prefs.PolygonMode == EPolygonMode::Fill)
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to wireframe.")
                    Prefs.PolygonMode = EPolygonMode::Wireframe;
                }
                else
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to fill.")
                    Prefs.PolygonMode = EPolygonMode::Fill;
                }
                return {};
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("ClearMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F9, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Clearing mesh material preference.")
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference.reset();
                return {};
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetNormalMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F5, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Normal].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Normal");
                return {};
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetUVMeshMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F6, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.UV].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.UV");
                return {};
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetTangentMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F7, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Tangent].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Tangent");
                return {};
            });

        Context->MapAction(&Registry,
            {LUserInputTag::ToTag("SetHandednessMaterialPreference"), EInputActionCategory::Boolean},
            TArray<LInputTrigger>{}.reflexive_emplace_back(LInputTrigger{EKeys::F8, EInputActionTrigger::Triggered}),
            [](LViewport&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [Jafg.Mesh.Handedness].")
                check(GEngine)
                GetMutableSingleton<JUserPreferences>().MeshMaterialPreference =
                    GEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Mesh.Handedness");
                return {};
            });
    }

    return;
}
