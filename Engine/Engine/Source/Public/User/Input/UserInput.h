// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/RawInput.h"
#include "InputContext.h"
#include "User/Input/InputTypes.h"

namespace Jafg
{

class LSurface;
class LLocalEgo;

/**
 * Proxy for the raw platform physical input data and the dispatching of input delegates.
 */
class LUserInput final
{
public:

    LUserInput() = default;

    void BeginNewFrame();

    /** Whether this key was just downed this frame. */
    ENGINE_API bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    ENGINE_API auto GetLocalEgo() const -> LLocalEgo*;
    ENGINE_API auto GetCheckedLocalEgo() const -> LLocalEgo*;
    ENGINE_API auto GetPanickedLocalEgo() const -> LLocalEgo*;

    ENGINE_API void RegisterContext(LUserInputContext&& Context, const bool bMakeActive = false);

    ENGINE_API TdhArray<LRawInput>  GetTriggeredKeys() const;
    ENGINE_API TdhArray<LRawInput>& GetOngoingKeys() const;
    ENGINE_API TdhArray<LRawInput>  GetCompletedKeys() const;

    ENGINE_API  auto GetContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API  auto GetCheckedContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API  void GetContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API  void GetCheckedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    FORCEINLINE auto GetActiveContexts() const -> const TdhArray<LUserInputContext*>& { return this->ActiveContexts; }
    FORCEINLINE auto GetRegisteredContexts() const -> const TdhArray<LUserInputContext*>& { return this->RegisteredContexts; }

private:

    void DispatchInputDelegatesForAction(const TdhArray<LRawInput>& InRawInputs, const LInputMappedAction* InAction);

    /**
     * The most important context is stored first.
     */
    TdhArray<LUserInputContext*> ActiveContexts;
    TdhArray<LUserInputContext*> RegisteredContexts;
};

} /* ~Namespace Jafg */
