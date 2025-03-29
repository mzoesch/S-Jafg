// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LRendererComponent;
class LEye;
class LViewport;

class ENGINE_API LRendererComponent
{
public:

    LRendererComponent() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LRendererComponent)
    virtual ~LRendererComponent() = default;

    // virtual bool Cull(const LViewport& Context, const LEye& Eye) const = 0;
    virtual void Draw(const LViewport& Context, const LEye& Eye) = 0;
};

} /* ~Namespace Jafg. */
