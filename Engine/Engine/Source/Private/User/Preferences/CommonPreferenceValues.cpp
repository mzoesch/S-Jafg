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

    return LString::SprintF("{}%", static_cast<int32>(Maths::Round(JAFG_TO_PERCENT * Value)));
}

void Jafg::LPreferenceValue_Scalar::BuildDefault(const LPreference* Self, WWidgetParentBase* Target)
{
    const LPreferenceValue_Scalar* This = static_cast<const LPreferenceValue_Scalar*>(Self);

    WWidgetRegion* Container;
    WTextBlock* Text;

    NewNode(WWidgetRegion).SaveTo(Container)
    .Tint(LColor(0, 0, 0, 64))
    .Anchor(EAnchor::HFill)
    .Padding({15.0f, 10.0f})
    [
        NewNode(WHBox).Anchor(EAnchor::Fill)
        [
            NewNode(WTextBlock)
                .Anchor(EAnchor::TopLeft)
                .Brush(LTextBlockBrush::Body())
                .Content(This->GetDisplayName())
            + NewNode(WTextBlock).SaveTo(Text)
                .Anchor(EAnchor::TopLeft)
                .Tint(LColor::Emerald)
                .Brush(LTextBlockBrush::Body())
                .Content(This->GetFormattedText())
        ]
    ];
    Target->AddChild(Container);

    return;
}
