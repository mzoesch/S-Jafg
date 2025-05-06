// Copyright mzoesch. All rights reserved.

#include "User/Preferences/CommonPreferenceValues.h"
#include "Widgets/HBox.h"
#include "Widgets/WidgetParentBase.h"
#include "Widgets/TextBlock.h"
#include "Widgets/WidgetRegion.h"

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

void Jafg::LPreferenceValue_Scalar::BuildDefault(const LPreference* Self, WWidgetParentBase* Target)
{
    const LPreferenceValue_Scalar* This = static_cast<const LPreferenceValue_Scalar*>(Self);

    WWidgetParentBase* Container;
    WTextBlock*        Text;

    const u8 ColorSpace = static_cast<u8>(20 * ((Target->GetChildren().GetSize() / 2) % 2 == 0 ? 1 : 1.8));

    NewNode(WHBox).SaveTo(Container)
    .Anchor(EAnchor::HFill)
    .Padding({15.0f, 10.0f})
    .MinDesiredSize({0, 10})
    .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNode(WTextBlock)
            .Anchor(EAnchor::CenterLeft)
            .Brush(LTextBlockBrush::Body())
            .Content(This->GetDisplayName())
        + NewNode(WTextBlock).SaveTo(Text)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBlockBrush::Body())
            .Align(ETextAlign::Right)
            .Content(This->GetFormattedText())
    ];

    Target->AddChild(Container);

    return;
}
