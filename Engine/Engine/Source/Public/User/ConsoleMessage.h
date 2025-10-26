// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

struct LConsoleMessage final
{
    LString Origin;
    LString Content;
    Lal::LColor Color{ Lal::LColor::White };
};

} /* ~Namespace Jafg */
