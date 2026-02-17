// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/Input_Vector3.h"
#include "Widgets/EditableTextBox.h"
#include "Widgets/TextBox.h"

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    LEditableTextBoxBrush Brush{LTextBoxBrush::Compact()};
    Brush.Type = ERegionBrush::RoundedOutlineBox;
    Brush.Tint = Colors::Black;
    Brush.OutlineThickness = 1.0f;
    Brush.OutlineTint = Colors::Gray;

    BeginStyling(*this).Root<WTextBox>()
        .Type(ERegionBrush::None)
        .Brush(Brush)
        .Content(this->DisplayName)
        .MinDesiredSize({128_pt, 0});

    BeginStyling(*this).Root<WEditableTextBox>()
        .Brush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(WEditableTextBox::IsContentFloatingPoint)
        ;

    BeginStyling(*this).Root<WEditableTextBox>()
        .Brush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(WEditableTextBox::IsContentFloatingPoint)
        ;

    BeginStyling(*this).Root<WEditableTextBox>()
        .Brush(Brush)
        .Content("1.0")
        .MinDesiredSize({48_pt, 0})
        .ContentPredicate(WEditableTextBox::IsContentFloatingPoint)
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
