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
                            (void)this->OnValueChanged.InvokeIfBound(Self.AsStatic<WCheckmarkButton>().IsChecked());
                        }
                    }
                    else
                    {
                        Self.AsStatic<WCheckmarkButton>().SetChecked(!Self.AsStatic<WCheckmarkButton>().IsChecked());
                        (void)this->OnValueChanged.InvokeIfBound(Self.AsStatic<WCheckmarkButton>().IsChecked());
                    }

                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
    ];
}

bool Jafg::WInput_Boolean::GetValue() const noexcept
{
    check(this->CheckmarkButton)
    return this->CheckmarkButton->IsChecked();
}
