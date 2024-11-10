// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "User/InputTypes.h"

namespace Jafg
{

class LSurface;
class LLocalEgo;

class LUserInput final
{
public:

    LUserInput() = default;

    /** Called when a new frame is started. */
    void BeginNewFrame();

    /** Whether this key was just downed this frame. */
    bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    auto GetLocalEgo() const -> LLocalEgo*;
    auto GetCheckedLocalEgo() const -> LLocalEgo*;
    auto GetPanickedLocalEgo() const -> LLocalEgo*;

    auto GetPrimaryContext() const -> LSurface*;
    auto GetCheckedPrimaryContext() const -> LSurface*;
    auto GetPanickedPrimaryContext() const -> LSurface*;
};

} /* ~Namespace Jafg */
