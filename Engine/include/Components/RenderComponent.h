// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

class LEye;
class LRendererComponent;
class LViewport;

class LRendererComponent
{
public:

    LRendererComponent() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LRendererComponent)
    virtual ~LRendererComponent() = default;

    //#
    //# In order:
    //#   Near Bottom Left
    //#   Near Bottom Right
    //#   Near Top Left
    //#   Near Top Right
    //#   Far Bottom Left
    //#   Far Bottom Right
    //#   Far Top Left
    //#   Far Top Right
    //#
    NODISCARD
    virtual bool Cull(const std::span<LWorldVec3>& Corners) const = 0;
    virtual void Draw(const LViewport& Context, const LEye& Eye) = 0;
};

} /* ~Namespace Jafg */
