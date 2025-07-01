// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/Input_Vector3.h"

#include "Widgets/EditableTextBox.h"
#include "Widgets/TextBox.h"

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    LTextBoxBrush Brush { LTextBoxBrush::Body() };

    WTextBox* Display { ConstructWidgetNode<WTextBox>(this->GetOuter()) };

    Display->SetBrush(Brush);
    Display->SetContent(this->DisplayName);

    this->AddChild(Display);

    WEditableTextBox* X { ConstructWidgetNode<WEditableTextBox>(this->GetOuter()) };
    X->SetTextColor(Brush.TextColor);
    X->SetTextScale(Brush.TextScale);
    X->SetMinDesiredSize(10);
    this->AddChild(X);

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
