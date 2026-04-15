// Copyright mzoesch. All rights reserved.

#include "Nodes/TextBox.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/NodeRenderInfo.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

namespace
{

f32 TextBoxInSptImpl(Jafg::ETextScale TextScale, Jafg::EApplicationScale Scale) noexcept
{
    auto const& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};

    switch (Scale)
    {
    case Jafg::EApplicationScale::Single:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeSingle; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeSingle; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeSingle; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeSingle; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeSingle; }
        default: break;
        }
    }
    case Jafg::EApplicationScale::Double:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeDouble; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeDouble; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeDouble; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeDouble; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeDouble; }
        default: break;
        }
    }
    case Jafg::EApplicationScale::Triple:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeTriple; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeTriple; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeTriple; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeTriple; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeTriple; }
        default: break;
        }
    }
    default: break;
    }

    unreachable()
}

} /* ~Namespace <Anonymous> */

f32 Jafg::LTextScale::InSptImpl(LViewport const& Viewport, ETextScale TextScale) noexcept
{
    EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    if (EApplicationScale UserMaxScale{*GetSingleton<JUserPreferences>().ApplicationScaleMode}; UserMaxScale != EApplicationScale::Auto)
    {
        Scale = EApplicationScale{maths::min(std::to_underlying(Scale), std::to_underlying(UserMaxScale))};
    }
    return ::TextBoxInSptImpl(TextScale, Scale);
}

void Jafg::WTextBox::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Content.empty() == false && this->TextBrush.bSkipBrushDraw == false)
    {
        if (this->RenderData.bDirty)
        {
            this->UpdateRenderData(Info.FontSubsystem, this->TextBrush.TextScale.InSpt(Info.Viewport));
        }

        LVec2F TopLeft{
              this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)
            + this->GetPadding().GetTopLeftOffsetInSpt(this->GetViewport())
            + this->DrawOffset
            };
        LVec2F PlayRoom{this->GetAnchoredSize_v2() - this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()) - this->RenderData.DesiredSize};
        TopLeft += maths::min(LVec2F{
            this->IsTextLeftAligned() ? 0.0f : (this->IsTextHCenterAligned() ? PlayRoom.x * 0.5f : PlayRoom.x),
            this->IsTextTopAligned()  ? 0.0f : (this->IsTextVCenterAligned() ? PlayRoom.y * 0.5f : PlayRoom.y)
            }, 0.0f);

        for (auto const& GlyphInfo : this->RenderData.Result.GlyphInfos)
        {
            // TODO: Clamp to pixels? Currently sometimes a little bit blurry.
            Info.AddInstance(LVisualInstance{
                .Rect = { {TopLeft.x + GlyphInfo.Rect.x, TopLeft.y + GlyphInfo.Rect.y}, {GlyphInfo.Rect.z, GlyphInfo.Rect.w} },
                .Tint = this->TextBrush.Tint.Bits,
                .BackgroundTint = Colors::Transparent.Bits,
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = this->TextBrush.OutlineTint.Bits,
                .TexCoordRect = GlyphInfo.TexCoordRect,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                .TextureIndex = GlyphInfo.BindlessTextureIndex,
                .SamplerIndex = GlyphInfo.SamplerIndex,
                .MsdfPixelRange = GlyphInfo.MsdfPixelRange,
                });
        }
    }

    return;
}

void Jafg::WTextBox::UpdateDesiredSize() const
{
    if (this->RenderData.bDirty)
    {
        this->UpdateRenderData(*this->GetFrontend().GetSubsystemChecked<JFontSubsystem>(), this->TextBrush.TextScale.InSpt(this->GetViewport()));
    }
    this->SetDesiredSizeInSpt(this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()) + this->RenderData.DesiredSize);
    return;
}

void Jafg::WTextBox::UpdateRenderData(JFontSubsystem const& FontSubsystem, f32 TargetFontSize) const
{
    this->RenderData.bDirty = false;
    this->RenderData.FontSize = TargetFontSize;

    LVec2F Pencil{maths::zero_vector<LVec2F>};
    this->RenderData.Result = FontSubsystem.GetGlyphInfos(this->Content
        , this->RenderData.FontSize
        , &Pencil
        , 0
        );

    //# TODO: This for line gaps. LineHeight + line_gap
    this->RenderData.DesiredSize = LVec2F{Pencil.x, this->RenderData.Result.LineHeight * this->TextBrush.Tightening};

    return;
}
