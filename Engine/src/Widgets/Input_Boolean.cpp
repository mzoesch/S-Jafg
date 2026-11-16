// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Boolean.h"
#include "Nodes/CheckmarkButton.h"

void Jafg::WInput_Boolean::Construct()
{
    Super::Construct();

    check(this->bInitialValue)

    BeginStyling(*this).StaticRoot<WOverlay>()
        .Anchor(EAnchor::Fill)
        .Padding({ENodeSize::StaticPoints, 2 + this->Brush.OutlineThickness, 2 + this->Brush.OutlineThickness, 2, 2 + this->Brush.OutlineThickness})
    [
        NewStaticNode(WCheckmarkButton).SaveTo(&this->CheckmarkButton)
            .Anchor(EAnchor::Fill)
            .Tint(Colors::Black)
            .Checked(*this->bInitialValue)
            .Enabled(this->bInitialEnabledState.has_value() ? *this->bInitialEnabledState : true)
            .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton)
                    && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (this->OnToggle)
                    {
                        if (bool NewValue{std::invoke(this->OnToggle, Self.AsStatic<WCheckmarkButton>().IsChecked())};
                            NewValue != Self.AsStatic<WCheckmarkButton>().IsChecked())
                        {
                            Self.AsStatic<WCheckmarkButton>().SetChecked(NewValue);
                            (void)this->OnValueChanged.InvokeIfBound(*this);
                        }
                    }
                    else
                    {
                        Self.AsStatic<WCheckmarkButton>().SetChecked(!Self.AsStatic<WCheckmarkButton>().IsChecked());
                        (void)this->OnValueChanged.InvokeIfBound(*this);
                    }

                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
    ];

    this->bInitialEnabledState.reset();
}

bool Jafg::WInput_Boolean::GetValue() const noexcept
{
    check(this->CheckmarkButton)
    return this->CheckmarkButton->IsChecked();
}

void Jafg::WInput_Boolean::SetValue(bool bValue, bool bCallEvent /* = true */) noexcept
{
    check(this->CheckmarkButton)
    this->CheckmarkButton->SetChecked(bValue);
    if (bCallEvent)
    {
        (void)this->OnValueChanged.InvokeIfBound(*this);
    }
}
