// Copyright mzoesch. All rights reserved.

#include "Nodes/SliderRegion.h"
#include "Framework/Surface.h"
#include "Rhi/NodeRenderInfo.h"
#include "Rhi/VisualInstance.h"

void Jafg::LSliderBrush::Draw(LNodeRenderInfo const& Info, LRect2F const& Rect, f64 Value) const noexcept
{
    if (this->bSkipBrushDraw || Rect.extent.x <= 0.0f || Rect.extent.y <= 0.0f)
    {
        return;
    }

    Info.AddInstance({
        .Rect = {.offset=Rect.offset,.extent={Rect.extent.x * static_cast<f32>(Value), Rect.extent.y}},
        .Radii = maths::zero_vector<LVec4F>,
        .Tint = this->Tint,
        .BorderTint = Colors::Black,
        .OutlineTint = Colors::Black,
        .OutlineThickness = 0,
        .SamplerIndex = std::to_underlying(UBO::Bindless::Sampler::LinearClampToEdgeSamplerIdx),
        });
}

void Jafg::WSliderRegion::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    check(this->Value >= 0.0 && this->Value <= 1.0)
    this->SliderBrush.Draw(Info, {
        .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
        .extent = this->GetAnchoredSize_v2(),
        }, this->Value);
}

Jafg::LNodeReply Jafg::WSliderRegion::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (auto S{Super::OnKeyEventFocused(Info, Event)}; S.IsHandled())
    {
        return S;
    }

    if (Event.Is<ERawInputStateBits::Press>(ELogicalKey::LeftMouseButton) && Info.CursorLocation)
    {
        Info.Viewport.EmplaceUntil<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton),
        [this,LastLocation=Info.CursorLocation->x,LastValue=std::optional<f64>{}](auto&&...) mutable
        {
            auto& Surface{this->GetViewport().GetSurface()};
            if (!Surface.HasMouseLocation())
            {
                return true;
            }

            auto Delta{(Surface.GetMouseLocation()->x - LastLocation) * 0.01};
            LastLocation = Surface.GetMouseLocation()->x;
            if (!LastValue || Delta != 0.0f)
            {
                LastValue = this->Value;

                if (auto TranslatedMouse{
                    Surface.GetMouseLocationValue()
                    - this->GetAnchoredAndTranslatedTopLeftFromMostOuter(this->GetTranslationFromMostOuter())
                    };
                    TranslatedMouse.x < 0.0f)
                {
                    this->Value = 0.0f;
                }
                else if (TranslatedMouse.x > this->GetAnchoredSize_v2().x)
                {
                    this->Value = 1.0f;
                }
                else
                {
                    this->Value = TranslatedMouse.x / this->GetAnchoredSize_v2().x;
                }

                if (this->OnValueChanged.IsValid())
                {
                    if (!this->OnValueChanged(this->Value))
                    {
                        this->Value = *LastValue;
                    }
                }
            }

            return false;
        });
    }

    return LNodeReply::Unhandled();
}
