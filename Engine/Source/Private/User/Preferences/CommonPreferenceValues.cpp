// Copyright mzoesch. All rights reserved.

#include "User/Preferences/CommonPreferenceValues.h"
#include "Widgets/HRegion.h"
#include "Widgets/ParentBase.h"
#include "Widgets/TextBox.h"
#include "Widgets/Region.h"
#include "Engine/Engine.h"
#include "Widgets/Spacer.h"
#include "Widgets/VRegion.h"

void Jafg::LPreferenceValue_Scalar::StoreInitial()
{
}

void Jafg::LPreferenceValue_Scalar::ResetToDefault()
{
}

void Jafg::LPreferenceValue_Scalar::ResetToInitial()
{
}

LString Jafg::LPreferenceValue_Scalar::Fmt_Raw(const double Value)
{
    return Jafg::SprintF("{}", Value);
}

LString Jafg::LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent(const double Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_ERROR(LogPreferences, "Value is not in range of [0, 1]. Found: {}.", Value)
        return "ERROR";
    }

    return Jafg::SprintF("{}%", static_cast<i32>(maths::round(maths::to_percent_d * Value)));
}

void Jafg::LPreferenceValue_Scalar::BuildDefault(const LPreference* Self, WParentBase* Target)
{
    const LPreferenceValue_Scalar* This = static_cast<const LPreferenceValue_Scalar*>(Self);


    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().size() % 2 == 0 ? 1.8 : 1));

    BeginStyling(*Target).Root<WHRegion>()
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .MinDesiredSize({0_pt, 10})
        .Type(ERegionBrush::Box)
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewStaticNodeVp(Target->GetViewport(), WTextBox)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBoxBrush::Body())
            .Content(This->GetDisplayName())
        +
        NewStaticNodeVp(Target->GetViewport(), WTextBox)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBoxBrush::Body())
            .TextAlign(ETextAlign::Right)
            .Content(This->GetFormattedText())
    ];

    return;
}

void Jafg::LPreferenceValue_InputAction::StoreInitial()
{
    jassertNoEntry()
}

void Jafg::LPreferenceValue_InputAction::ResetToDefault()
{
    jassertNoEntry()
}

void Jafg::LPreferenceValue_InputAction::ResetToInitial()
{
    jassertNoEntry()
}

void Jafg::LPreferenceValue_InputAction::BuildDefault(const LPreference* Self, WParentBase* Target)
{
    if (GEngine == nullptr)
    {
        LOG_WARNING(LogPreferences, "Engine is invalid.")
        return;
    }

    check( GEngine->GetLocalEgo().IsDecommissioned() == false )
    check( Self->GetName().IsSet() )

    WParentBase* Container;

    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().size() % 2 == 0 ? 1.8 : 1));

    auto* Action = GEngine->GetLocalEgo().GetUserInputRegistry().GetActionByNameChecked(Self->GetDisplayName());
    auto& Contexts = GEngine->GetLocalEgo().GetUserInputRegistry().GetRegisteredContexts();

    BeginStyling(*Target).Root<WVRegion>().SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .MinDesiredSize({0_pt, 10})
        .Type(ERegionBrush::Box)
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewStaticNodeVp(Target->GetViewport(), WHRegion)
            .Anchor(EAnchor::Fill)
        [
            NewStaticNodeVp(Target->GetViewport(), WTextBox)
                .Brush(LTextBoxBrush::Body())
                .Content(Action->GetDisplayName())
            +
            NewStaticNodeVp(Target->GetViewport(), WTextBox)
                .Anchor(EAnchor::HFill)
                .Brush(LTextBoxBrush::Body())
                .Content(LexToString(Action->GetCategory()))
                .TextAlign(ETextAlign::Right)
        ]
    ]
    ;

    for (auto& Context : Contexts)
    {
        const LInputMappedAction* MappedAction = Context.FindMappedAction(Action->GetTag());
        if (MappedAction == nullptr)
        {
            continue;
        }

        if (MappedAction->Triggers.empty())
        {
            LOG_WARNING(LogPreferences, "Mapped action [{}] has no triggers.", Action->GetTag())
            continue;
        }

        auto AddKeys = [](WParentBase* InTarget, const TArray<LKey>& InKeys) -> void
        {
            for (const LKey& Key: InKeys)
            {
                BeginStyling(*InTarget).Root<WTextBox>()
                    .Brush(LTextBoxBrush::Body())
                    .Content(LexToString(Key))
                    ;
            }
        };

        if (MappedAction->Triggers.size() == 1)
        {
            WHRegion* ContextContainer;
            BeginStyling(*Target).Root<WHRegion>().SaveTo(&ContextContainer)
                .Anchor(EAnchor::Fill)
                .Padding({20.0f, 0.0f, 0.0f, 0.0f})
            [
                NewStaticNodeVp(Target->GetViewport(), WTextBox)
                    .Brush(LTextBoxBrush::Body())
                    .Content(Context.GetDisplayName())
                +
                NewStaticNodeVp(Target->GetViewport(), WSpacer)
                    .Anchor(EAnchor::HFill)
            ];

            const LInputTrigger& Trigger = MappedAction->Triggers[0];
            check( Trigger.Name.empty() )

            AddKeys(ContextContainer, Trigger.Keys);

            Container->AddChild(ContextContainer);

            continue;
        }

        WVRegion* ContextContainer;
        BeginStyling(*Target).Root<WVRegion>().SaveTo(&ContextContainer)
            .Anchor(EAnchor::Fill)
            .Padding({20.0f, 0.0f, 0.0f, 0.0f})
        [
            NewStaticNodeVp(Target->GetViewport(), WTextBox)
                .Brush(LTextBoxBrush::Body())
                .Content(Context.GetDisplayName())
        ]
        ;

        for (const LInputTrigger& Trigger: MappedAction->Triggers)
        {
            WHRegion* TriggerContainer;
            BeginStyling(*Target).Root<WHRegion>().SaveTo(&TriggerContainer)
                .Anchor(EAnchor::Fill)
                .Padding({40.0f, 0.0f, 0.0f, 0.0f})
            [
                NewStaticNodeVp(Target->GetViewport(), WTextBox)
                    .Brush(LTextBoxBrush::Body())
                    .Content(Trigger.Name)
                +
                NewStaticNodeVp(Target->GetViewport(), WSpacer)
                    .Anchor(EAnchor::HFill)
            ]
            ;

            AddKeys(TriggerContainer, Trigger.Keys);

            ContextContainer->AddChild(TriggerContainer);

            continue;
        }

        Container->AddChild(ContextContainer);
        continue;
    }

    Target->AddChild(Container);

    return;
}
