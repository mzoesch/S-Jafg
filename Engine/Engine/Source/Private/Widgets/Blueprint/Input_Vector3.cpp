// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/Input_Vector3.h"
#include "Widgets/EditableTextBox.h"
#include "Widgets/TextBox.h"

Jafg::WInput_Vector3::WInput_Vector3(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetPadding(5.0f);
    this->SetOmniOutlineThickness(1.0f);
    this->SetOmniTint(Lal::LColor::Transparent);

    return;
}

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    LEditableTextBoxBrush Brush { LTextBoxBrush::Compact() };
    Brush.Type = ERegionBrush::RoundedOutlineBox;
    Brush.Tint = Lal::LColor::Black;
    Brush.OutlineThickness = 1.0f;
    Brush.OutlineTint = Lal::LColor::Gray;

    WTextBox* Display { ConstructWidgetNode<WTextBox>(this->GetOuter()) };
    Display->SetBrush(Brush);
    Display->SetType(ERegionBrush::None);
    Display->SetContent(this->DisplayName);
    Display->SetMinDesiredSize({128_pt, 0});

    this->AddChild(Display);

    WEditableTextBox* X { ConstructWidgetNode<WEditableTextBox>(this->GetOuter()) };
    X->SetBrush(Brush);
    X->SetContent("1.0");
    X->SetMinDesiredSize({48_pt, 0});
    X->ContentPredicate.BindWeak(WEditableTextBox::IsContentFloatingPoint);
    this->AddChild(X);

    WEditableTextBox* Y { ConstructWidgetNode<WEditableTextBox>(this->GetOuter()) };
    Y->SetBrush(Brush);
    Y->SetContent("1.0");
    Y->SetMinDesiredSize({48_pt, 0});
    Y->ContentPredicate.BindWeak(WEditableTextBox::IsContentFloatingPoint);
    this->AddChild(Y);

    WEditableTextBox* Z { ConstructWidgetNode<WEditableTextBox>(this->GetOuter()) };
    Z->SetBrush(Brush);
    Z->SetContent("1.0");
    Z->SetMinDesiredSize({48_pt, 0});
    Z->ContentPredicate.BindWeak(WEditableTextBox::IsContentFloatingPoint);
    this->AddChild(Z);

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

void Jafg::WInput_Vector3::SetValue(const LVector3& InValue)
{
    this->Value = InValue;

    return;
}
