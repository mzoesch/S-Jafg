// Copyright mzoesch. All rights reserved.

#include "Widgets/Blueprint/Input_Vector3.h"

#include "Widgets/EditableTextBlock.h"
#include "Widgets/TextBlock.h"

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    LTextBlockBrush Brush { LTextBlockBrush::Body() };

    WTextBlock* Display { ConstructWidgetNode<WTextBlock>(this->GetOuter()) };

    Display->SetBrush(Brush);
    Display->SetContent(this->DisplayName);

    this->AddChild(Display);

    WEditableTextBlock* X { ConstructWidgetNode<WEditableTextBlock>(this->GetOuter()) };
    X->SetTextColor(Brush.Color);
    X->SetTextScale(Brush.Scale);
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
