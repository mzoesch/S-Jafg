// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTracePlane.h"
#include "Debug/DebugTraceLine.h"

void Jafg::LDebugTracePlane::Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const
{
    LDebugTraceLine::DrawLine(InViewport, InEye, this->P1, this->P2, this->VisualParams.Color, this->VisualParams.Thickness);
    LDebugTraceLine::DrawLineCachedValues(this->P2, this->P3, this->VisualParams.Color, this->VisualParams.Thickness);
    LDebugTraceLine::DrawLineCachedValues(this->P3, this->P4, this->VisualParams.Color, this->VisualParams.Thickness);
    LDebugTraceLine::DrawLineCachedValues(this->P4, this->P1, this->VisualParams.Color, this->VisualParams.Thickness);

    return;
}
