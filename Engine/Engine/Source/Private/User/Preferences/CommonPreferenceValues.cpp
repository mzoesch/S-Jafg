// Copyright mzoesch. All rights reserved.

#include "User/Preferences/CommonPreferenceValues.h"
#include "Widgets/HRegion.h"
#include "Widgets/ParentBase.h"
#include "Widgets/TextBlock.h"
#include "Widgets/Region.h"
#include "Engine/Engine.h"

void Jafg::LPreferenceValue_Scalar::StoreInitial()
{
}

void Jafg::LPreferenceValue_Scalar::ResetToDefault()
{
}

void Jafg::LPreferenceValue_Scalar::ResetToInitial()
{
}

Jafg::LString Jafg::LPreferenceValue_Scalar::Fmt_Raw(const double Value)
{
    return LString::SprintF("{}", Value);
}

Jafg::LString Jafg::LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent(const double Value)
{
    if (Value < 0.0 || Value > 1.0)
    {
        LOG_ERROR(LogPreferences, "Value is not in range of [0, 1]. Found: {}.", Value)
        return "ERROR";
    }

    return LString::SprintF("{}%", static_cast<i32>(Maths::Round(JAFG_TO_PERCENT * Value)));
}

void Jafg::LPreferenceValue_Scalar::BuildDefault(const LPreference* Self, WParentBase* Target)
{
    const LPreferenceValue_Scalar* This = static_cast<const LPreferenceValue_Scalar*>(Self);

    WParentBase* Container;

    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().GetSize() % 2 == 0 ? 1.8 : 1));

    NewNodeCtx(Target, WHRegion).SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .MinDesiredSize({0, 10})
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBlockBrush::Body())
            .Content(This->GetDisplayName())
        +
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBlockBrush::Body())
            .Align(ETextAlign::Right)
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

    if (GEngine->IsLocalEgoValid() == false)
    {
        LOG_WARNING(LogPreferences, "Local ego is invalid.")
        return;
    }

    check( Self->GetName().IsSet() )

    WParentBase* Container;

    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().GetSize() % 2 == 0 ? 1.8 : 1));

    const LInputAction* Action = GEngine->GetLocalEgo()->GetUserInput()->GetActionByNameChecked(Self->GetName());
    NewNodeCtx(Target, WHRegion).SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBlockBrush::Body())
            .MinDesiredSize({100.0f, 0.0f})
            .Content(Action->GetDisplayName())
        +
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBlockBrush::Body())
            .Content(LexToString(Action->GetCategory()))
        // +
        // NewNodeCtx(Target, WTextBlock)
        //     .Anchor(EAnchor::VCenter)
        //     .Brush(LTextBlockBrush::Body())
        //     .Align(ETextAlign::Right)
        //     .Content(Variable->GetValue())
    ];

    Target->AddChild(Container);

    return;
}
