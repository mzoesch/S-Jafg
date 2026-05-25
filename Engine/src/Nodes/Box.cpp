// Copyright mzoesch. All rights reserved.

#include "Nodes/Box.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WBox::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    this->Brush.Draw(Info, {
        .Offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
        .Extent = this->GetAnchoredSize_v2(),
        });
    return;
}
