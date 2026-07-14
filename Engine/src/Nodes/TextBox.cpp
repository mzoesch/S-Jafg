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
        this->GetTextRenderData().Render(Info, {
            .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation) + this->GetRelativeTextTopLeft(),
            .extent = this->GetAnchoredSize_v2()
                - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
                - this->TextDrawOffset
                - this->TextPlayroomReduction,
            },
            {
                .Cutoff = this->TextCutoff,
                .Tint = this->TextBrush.Tint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                .LeftThrust = this->TextThrust.x,
                .bFadeLeftOverdraw = true,
            }
            );
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

    if (this->bTextAffectsDesiredWidth)
    {
        this->SetDesiredSizeInSpt(
            this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
            + this->GetTextRenderData().DesiredSize
            );
    }
    else
    {
        this->SetDesiredSizeInSpt(
            this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
            + LVec2F{0.0f, this->GetTextRenderData().DesiredSize.y}
            );
    }

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
