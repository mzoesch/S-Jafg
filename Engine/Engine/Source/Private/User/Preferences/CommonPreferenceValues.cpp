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
    return Lal::SprintF("{}", Value);
}

LString Jafg::LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent(const double Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_ERROR(LogPreferences, "Value is not in range of [0, 1]. Found: {}.", Value)
        return "ERROR";
    }

    return Lal::SprintF("{}%", static_cast<i32>(Maths::Round(LAL_TO_PERCENT * Value)));
}

void Jafg::LPreferenceValue_Scalar::BuildDefault(const LPreference* Self, WParentBase* Target)
{
    const LPreferenceValue_Scalar* This = static_cast<const LPreferenceValue_Scalar*>(Self);

    WParentBase* Container;

    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().size() % 2 == 0 ? 1.8 : 1));

    NewNodeCtx(Target, WHRegion).SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .MinDesiredSize({0_pt, 10})
        .Type(ERegionBrush::Box)
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNodeCtx(Target, WTextBox)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBoxBrush::Body())
            .Content(This->GetDisplayName())
        +
        NewNodeCtx(Target, WTextBox)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBoxBrush::Body())
            .TextAlign(ETextAlign::Right)
            .Content(This->GetFormattedText())
    ];

    Target->AddChild(Container);

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

    const LInputAction* Action = GEngine->GetLocalEgo().GetUserInput().GetActionByNameChecked(Self->GetName());
    const TArray<TUnique<LUserInputContext>>& Contexts = GEngine->GetLocalEgo().GetUserInput().GetRegisteredContexts();

    NewNodeCtx(Target, WVRegion).SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .MinDesiredSize({0_pt, 10})
        .Type(ERegionBrush::Box)
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNodeCtx(Target, WHRegion)
            .Anchor(EAnchor::Fill)
        [
            NewNodeCtx(Target, WTextBox)
                .Brush(LTextBoxBrush::Body())
                .Content(Action->GetDisplayName())
            +
            NewNodeCtx(Target, WTextBox)
                .Anchor(EAnchor::HFill)
                .Brush(LTextBoxBrush::Body())
                .Content(LexToString(Action->GetCategory()))
                .TextAlign(ETextAlign::Right)
        ]
    ]
    ;

    for (auto& Context: Contexts)
    {
        const LInputMappedAction* MappedAction = Context->FindMappedAction(Action);
        if (MappedAction == nullptr)
        {
            continue;
        }

        if (MappedAction->Triggers.empty())
        {
            LOG_WARNING(LogPreferences, "Mapped action [{}] has no triggers.", Action->GetName())
            continue;
        }

        auto AddKeys = [](WParentBase* InTarget, const TArray<LKey>& InKeys) -> void
        {
            for (const LKey& Key: InKeys)
            {
                WNode* KeyNode;
                NewNodeCtx(InTarget, WTextBox).SaveTo(&KeyNode)
                    .Brush(LTextBoxBrush::Body())
                    .Content(LexToString(Key))
                ;

                InTarget->AddChild(KeyNode);

                continue;
            }

            return;
        };

        if (MappedAction->Triggers.size() == 1)
        {
            WHRegion* ContextContainer;
            NewNodeCtx(Target, WHRegion).SaveTo(&ContextContainer)
                .Anchor(EAnchor::Fill)
                .Padding({20.0f, 0.0f, 0.0f, 0.0f})
            [
                NewNodeCtx(Target, WTextBox)
                    .Brush(LTextBoxBrush::Body())
                    .Content(Context->GetDisplayName())
                +
                NewNodeCtx(Target, WSpacer)
                    .Anchor(EAnchor::HFill)
            ]
            ;

            const LInputTrigger& Trigger = MappedAction->Triggers[0];
            check( Trigger.Name.empty() )

            AddKeys(ContextContainer, Trigger.Keys);

            Container->AddChild(ContextContainer);

            continue;
        }

        WVRegion* ContextContainer;
        NewNodeCtx(Target, WVRegion).SaveTo(&ContextContainer)
            .Anchor(EAnchor::Fill)
            .Padding({20.0f, 0.0f, 0.0f, 0.0f})
        [
            NewNodeCtx(Target, WTextBox)
                .Brush(LTextBoxBrush::Body())
                .Content(Context->GetDisplayName())
        ]
        ;

        for (const LInputTrigger& Trigger: MappedAction->Triggers)
        {
            WHRegion* TriggerContainer;
            NewNodeCtx(Target, WHRegion).SaveTo(&TriggerContainer)
                .Anchor(EAnchor::Fill)
                .Padding({40.0f, 0.0f, 0.0f, 0.0f})
            [
                NewNodeCtx(Target, WTextBox)
                    .Brush(LTextBoxBrush::Body())
                    .Content(Trigger.Name)
                +
                NewNodeCtx(Target, WSpacer)
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
