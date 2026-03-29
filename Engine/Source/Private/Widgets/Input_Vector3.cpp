// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Vector3.h"
#include "Nodes/EditableTextBox.h"
#include "Nodes/TextBox.h"
#include "Serialization/StringStatements.h"

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    LEditableTextBrush Brush;
    Brush.Tint = Colors::Black;
    Brush.OutlineThickness = 1.0f;
    Brush.OutlineTint = Colors::Gray;
    Brush.TextScale = ETextScale::Body;

    BeginStyling(*this).Root<WTextBox>()
        .TextBrush(Brush)
        .Content(this->DisplayName)
        .MinDesiredSize({128_pt, 0});

    BeginStyling(*this).Root<WEditableTextBox>()
        .TextBrush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(Serde::IsNumeric)
        ;

    BeginStyling(*this).Root<WEditableTextBox>()
        .TextBrush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(Serde::IsNumeric)
        ;

    BeginStyling(*this).Root<WEditableTextBox>()
        .TextBrush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(Serde::IsNumeric)
        ;

    return;
}

void Jafg::WInput_Vector3::SetDisplayName(const LString& InDisplayName)
{
    this->DisplayName = InDisplayName;

    return;
}

void Jafg::WInput_Vector3::SetDisplayName(LString&& InDisplayName)
{
    this->DisplayName = std::move(InDisplayName);

    return;
}

void Jafg::WInput_Vector3::SetValue(const LVec3F& InValue)
{
    this->Value = InValue;

    return;
}
