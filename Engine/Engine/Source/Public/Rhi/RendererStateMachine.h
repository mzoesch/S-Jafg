// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class LViewport;

namespace RendererStateMachine
{

void PrepareForPerspectivePainting();
void PrepareForOrthographicPainting();

void ClipOrthographic(const LViewport& InViewport, LVector2D&& InLocation, const LVector2D& InSize);
void DisableClipOrthographic();

} /* ~Namespace RendererStateMachine */

} /* ~Namespace Jafg */
