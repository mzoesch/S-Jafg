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
    ENGINE_API  auto GetPanickedContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API  void GetContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API  void GetCheckedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API  void GetPanickedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;

    /** @return The newly mapped action. This is not the same as the input argument. */
    ENGINE_API  auto RegisterAction(LInputAction&& InAction) -> LInputAction*;
    FORCEINLINE auto GetRegisteredActions() const -> const TdhArray<LInputAction*>& { return this->RegisteredActions; }

    ENGINE_API  void ActivateContext(const LSimpleString& InName);
    ENGINE_API  void ActivateContext(LUserInputContext* InContext);
    ENGINE_API  void DeactivateContext(const LSimpleString& InName);
    ENGINE_API  void DeactivateContext(LUserInputContext* InContext);
    ENGINE_API int32 DeactivateAllContexts();
    FORCEINLINE auto GetActiveContexts() const -> const TdhArray<LUserInputContext*>& { return this->ActiveContexts; }
    FORCEINLINE auto GetRegisteredContexts() const -> const TdhArray<LUserInputContext*>& { return this->RegisteredContexts; }

private:

    void DispatchInputDelegatesForAction(const LUserInputContext* InContext, const TdhArray<LRawInput>& InRawInputs, LInputMappedAction* InAction);

    TdhArray<LInputAction*> RegisteredActions;

    /**
     * The most important context is stored first.
     */
    TdhArray<LUserInputContext*> ActiveContexts;
    TdhArray<LUserInputContext*> RegisteredContexts;
};

} /* ~Namespace Jafg */
