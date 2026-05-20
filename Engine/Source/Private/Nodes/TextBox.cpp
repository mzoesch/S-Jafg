// Copyright mzoesch. All rights reserved.

#include "Nodes/TextBox.h"
#include "Framework/Frontend.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WTextBox::Draw(LNodeRenderInfo const& Info) const
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

void Jafg::WTextBox::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    if (this->IsTextRenderDataDirty<&WTextBox::TextBrush>())
    {
        this->GetTextRenderData().Update(this->GetViewport()
            , *this->GetFrontend().GetSubsystemChecked<JFontSubsystem>()
            , this->TextBrush
            , this->GetContent()
            );
    }

    this->SetDesiredSizeInSpt(
        this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
        + this->GetTextRenderData().DesiredSize
        );

    return;
}

void Jafg::WTextBoxIconizedLeft::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WTextBoxIconizedLeft::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextDrawOffset = Size;
    return;
}


void Jafg::WTextBoxIconizedRight::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    if (this->Icon.get())
    {
        Detail::DrawIcon(Info, this->GetIconTopLeft(Info.Translation), this->Icon, this->IconBrush);
    }
    return;
}

void Jafg::WTextBoxIconizedRight::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    auto Size{this->GetIconSize(this->GetViewport())};
    this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    this->TextPlayroomReduction = Size;
    return;
}
