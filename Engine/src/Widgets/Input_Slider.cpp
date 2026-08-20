// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Slider.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/SliderRegion.h"
#include "Serialization/StringStatements.h"

void Jafg::WInput_Slider::Construct()
{
    Super::Construct();

    checkCode
    (
        check(this->Minimum <= this->Maximum)
        check(this->Step >= 0.0)
    )

    BeginStyling(*this).StaticRoot<WOverlay>()
        .Anchor(EAnchor::Fill)
        .Padding({ENodeSize::StaticPoints, 2 + this->Brush.OutlineThickness, 2 + this->Brush.OutlineThickness, 2, 2 + this->Brush.OutlineThickness})
    [
        NewStaticNode(WSliderRegion).SaveTo(&this->SliderRegion)
            .Anchor(EAnchor::Fill)
            .Tint(Colors::Black)
            .SliderTint(Colors::White)
            .Value((this->Value - this->Minimum) / (this->Maximum - this->Minimum))
            .OnValueChanged([this](f64 Value)
            {
                check(this->SliderRegion && this->EditableTextButton)
                Value = this->Minimum + Value * (this->Maximum - this->Minimum);
                check(Value >= this->Minimum && Value <= this->Maximum)
                this->Value = maths::round(Value / this->Step) * this->Step;
                this->EditableTextButton->SetContent(this->GetFormattedValueString());

                (void)this->OnValueChanged.InvokeIfBound(this->Value);

                return true;
            })
    ];
    BeginStyling(*this).StaticRoot<WEditableTextButton>().SaveTo(&this->EditableTextButton)
        .Anchor(EAnchor::TopLeft)
        .MinDesiredSize({10_pt, 0})
        .InAllTextBrushesChained<&LTextBoxBrush::Tint,&LTextBoxBrush::TextHAlign>(Colors::Black, ETextHAlign::Center)
        .CaretTint(Colors::Black)
        .ContentPredicate(serde::IsNumeric)
        .Content(this->GetFormattedValueString())
        .OnContentCommitted([this](WEditableTextButton& Btn, LString const& NewValue, ETextCommit Commit)
        {
            if (Commit == ETextCommit::OnCleared)
            {
                check(this->EditableTextButton == &Btn)
                this->EditableTextButton->SetContent(this->GetFormattedValueString());
            }
            else
            {
                f64 Input{serde::from_string<f64>(NewValue)};
                Input = maths::clamp(Input, this->Minimum, this->Maximum);
                this->Value = maths::round(Input / this->Step) * this->Step;
                this->EditableTextButton->SetContent(this->GetFormattedValueString());
                this->SliderRegion->Value = (this->Value - this->Minimum) / (this->Maximum - this->Minimum);

                (void)this->OnValueChanged.InvokeIfBound(this->Value);
            }
        })
        ;
}

LString Jafg::WInput_Slider::GetFormattedValueString() const noexcept
{
    if (this->Step > 0.0)
    {
        f64 SteppedValue{std::round(this->Value / this->Step) * this->Step};
        return serde::to_string(SteppedValue);
    }
    return serde::to_string(this->Value);
}
