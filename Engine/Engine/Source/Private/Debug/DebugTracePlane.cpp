// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Debug/DebugTracePlane.h"
#include "Debug/DebugTraceLine.h"

void Jafg::LDebugTracePlane::Draw(const LWorld& InContext) const
{
    LDebugTraceLine::DrawLine(InContext, this->P1, this->P2, this->VisualParams.Color, false);
    LDebugTraceLine::DrawLine(InContext, this->P2, this->P3, this->VisualParams.Color, true);
    LDebugTraceLine::DrawLine(InContext, this->P3, this->P4, this->VisualParams.Color, true);
    LDebugTraceLine::DrawLine(InContext, this->P4, this->P1, this->VisualParams.Color, true);

    return;
}
