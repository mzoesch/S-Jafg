// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

class LViewport;
class LUserInput;
class APersonaController;

struct LInputCallback final
{
    LViewport& Viewport;
    APersonaController& Controller;
    LUserInput& UserInput;
};

} /* ~Namespace Jafg */
