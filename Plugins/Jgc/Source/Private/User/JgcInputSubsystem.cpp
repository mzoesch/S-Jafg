// Copyright mzoesch. All rights reserved.

#include "User/JgcInputSubsystem.h"
#include "User/CoreSubsystems/CoreInputSubsystem.h"
#include "User/Frontend/Osd/DebugScreen.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>();

    Jafg::LUserInputRegistry& Registry{ this->GetLocalEgo().GetUserInputRegistry() };

    {
        Jafg::LUserInputContext* Context{ Registry.GetContextByNameChecked(Name_UicInOmni) };

        // Action: OpenDebugScreen
        Context->MapAction(
            &Registry,
            {Name_UsrInToggleDebugScreen, "Toggle Debug Screen", Jafg::EInputActionCategory::Boolean},
            "",
            Jafg::EKeys::F3,
            Jafg::EInputActionTrigger::Triggered,
            {},
            [](Jafg::LViewport& Viewport, Jafg::LInputActionValue&)
            {
                if (auto* Widget{ Viewport.GetTopLevelWidgetByClass<WDebugScreen>() })
                {
                    Widget->RemoveFromParent();
                }
                else
                {
                    Viewport.AddWidget<WDebugScreen>();
                }
            });
    }

    return;
}
