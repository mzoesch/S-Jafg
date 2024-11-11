// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/RawInput.h"
#include "UserInputContext.h"
#include "User/InputTypes.h"

namespace Jafg
{

class LSurface;
class LLocalEgo;

class LUserInput final
{
public:

    LUserInput() = default;

    void BeginNewFrame();

    /** Whether this key was just downed this frame. */
    bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    auto GetLocalEgo() const -> LLocalEgo*;
    auto GetCheckedLocalEgo() const -> LLocalEgo*;
    auto GetPanickedLocalEgo() const -> LLocalEgo*;

    void RegisterContext(LUserInputContext&& Context, const bool bMakeActive = false);

    TdhArray<LRawInput>  GetTriggeredKeys() const;
    TdhArray<LRawInput>& GetOngoingKeys() const;
    TdhArray<LRawInput>  GetCompletedKeys() const;

private:

    /**
     * The most important context is stored first.
     */
    TdhArray<LUserInputContext*> ActiveContexts;
    TdhArray<LUserInputContext*> RegisteredContexts;
};

} /* ~Namespace Jafg */
