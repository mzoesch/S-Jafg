// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTracePlane.h"
#include "Debug/DebugTraceLine.h"

void Jafg::LDebugTracePlane::Draw(const LWorld& InContext) const
{
    LDebugTraceLine::DrawLine(InContext, this->P1, this->P2, this->VisualParams.Color, this->VisualParams.Thickness, false);
    LDebugTraceLine::DrawLine(InContext, this->P2, this->P3, this->VisualParams.Color, this->VisualParams.Thickness, true);
    LDebugTraceLine::DrawLine(InContext, this->P3, this->P4, this->VisualParams.Color, this->VisualParams.Thickness, true);
    LDebugTraceLine::DrawLine(InContext, this->P4, this->P1, this->VisualParams.Color, this->VisualParams.Thickness, true);

    return;
}
