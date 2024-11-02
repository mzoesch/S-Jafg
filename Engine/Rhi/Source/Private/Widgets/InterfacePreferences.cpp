// Copyright mzoesch. All rights reserved.

#include "Widgets/InterfacePreferences.h"

float Jafg::JInterfacePreferences::CalculateDpiScale(const LViewportSize& InViewportSize) const
{
    int32 Eval = Maths::Min(InViewportSize.X, InViewportSize.Y);

    return 1.0f;
}
