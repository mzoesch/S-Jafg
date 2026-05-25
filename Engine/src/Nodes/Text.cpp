// Copyright mzoesch. All rights reserved.

#include "Nodes/Text.h"
#include "Framework/Frontend.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WText::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (!(this->GetContent().empty() || this->TextBrush.bSkipBrushDraw))
    {
        auto TopLeft{this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation) + this->GetRelativeTextTopLeft()};
        for (auto const& Glyph : this->GetTextRenderData().Collection)
        {
            // TODO: Clamp to pixels? Currently sometimes a little bit blurry.
            Info.AddInstance({
                .Rect = {{TopLeft.x + Glyph.Rect.x, TopLeft.y + Glyph.Rect.y}, {Glyph.Rect.z, Glyph.Rect.w}},
                .TexCoordRect = Glyph.TexCoordRect,
                .Tint = this->TextBrush.Tint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                .TextureIndex = Glyph.BindlessTextureIndex,
                .SamplerIndex = Glyph.SamplerIndex,
                .MsdfPixelRange = Glyph.MsdfPixelRange,
                });
        }
    }

    return;
}

void Jafg::WText::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    if (this->IsTextRenderDataDirty<&WText::TextBrush>())
    {
        this->GetTextRenderData().Update(this->GetViewport()
            , *this->GetFrontend().GetSubsystemChecked<JFontSubsystem>()
            , this->TextBrush
            , this->GetContent()
            );
    }

    this->SetDesiredSizeInSpt(
        this->TextBrush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
        + this->GetTextRenderData().DesiredSize
        );

    return;
}
