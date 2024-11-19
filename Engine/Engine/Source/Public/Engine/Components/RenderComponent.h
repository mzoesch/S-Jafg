// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LViewport;

class ENGINE_API LRendererComponent
{
public:

    LRendererComponent() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LRendererComponent)
    virtual ~LRendererComponent() = default;

    virtual void Draw(const LViewport& Context) = 0;
};

} /* ~Namespace Jafg. */
