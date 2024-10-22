// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Player/InputTypes.h"

namespace Jafg
{

class LSurface;
class LLocalPlayer;

class LPlayerInput final
{
public:

    LPlayerInput() = default;

    /** Called when a new frame is started. */
    void BeginNewFrame();

    /** Whether this key was just downed this frame. */
    bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    auto GetLocalPlayer() const -> LLocalPlayer*;
    auto GetCheckedLocalPlayer() const -> LLocalPlayer*;
    auto GetPanickedLocalPlayer() const -> LLocalPlayer*;

    auto GetPrimaryContext() const -> LSurface*;
    auto GetCheckedPrimaryContext() const -> LSurface*;
    auto GetPanickedPrimaryContext() const -> LSurface*;
};

} /* ~Namespace Jafg. */
