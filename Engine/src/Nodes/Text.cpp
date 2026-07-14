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
        this->GetTextRenderData().Render(Info, {
            .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation) + this->GetRelativeTextTopLeft(),
            .extent = this->GetAnchoredSize_v2()
                - this->TextBrush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
                - this->TextDrawOffset
                - this->TextPlayroomReduction,
            },
            {
                .Cutoff = this->TextCutoff,
                .Tint = this->TextBrush.Tint,
                .OutlineTint = this->TextBrush.OutlineTint,
                .OutlineThickness = this->TextBrush.OutlineThickness,
            }
            );
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
