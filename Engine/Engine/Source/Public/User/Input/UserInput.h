// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/RawInput.h"
#include "InputContext.h"
#include "User/Input/InputTypes.h"

namespace Jafg
{

class LLocalEgo;

//#
//# Proxy for the raw platform physical input data and the dispatching of input delegates.
//#
class LUserInput final
{
public:

    LUserInput() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInput)
    ~LUserInput() = default;

    //# Whether this key was just downed this frame.
    ENGINE_API bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    ENGINE_API auto GetLocalEgo() const -> LLocalEgo*;

    ENGINE_API void RegisterContext(LUserInputContext&& Context, const bool bMakeActive = false);

    ENGINE_API TArray<LRawInput>  GetTriggeredKeys() const;
    ENGINE_API TArray<LRawInput>& GetOngoingKeys() const;
    ENGINE_API TArray<LRawInput>  GetCompletedKeys() const;

    ENGINE_API auto GetContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API auto GetCheckedContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API auto GetPanickedContextByName(const LSimpleString& InName) -> LUserInputContext*;
    ENGINE_API void GetContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API void GetCheckedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API void GetPanickedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const;
    ENGINE_API auto GetContextByName(const LName InName) -> LUserInputContext*;
    ENGINE_API auto GetCheckedContextByName(const LName InName) -> LUserInputContext*;
    ENGINE_API auto GetPanickedContextByName(const LName InName) -> LUserInputContext*;
    ENGINE_API void GetContextByName(const LName InName, LUserInputContext*& OutContext) const;
    ENGINE_API void GetCheckedContextByName(const LName InName, LUserInputContext*& OutContext) const;
    ENGINE_API void GetPanickedContextByName(const LName InName, LUserInputContext*& OutContext) const;

    //# @return The newly mapped action. This is not the same as the input argument.
    ENGINE_API  auto RegisterAction(LInputAction&& InAction) -> LInputAction*;
    FORCEINLINE auto GetRegisteredActions() const -> const TArray<LInputAction*>& { return this->RegisteredActions; }

    ENGINE_API  void ActivateContext(const LSimpleString& InName);
    ENGINE_API  void ActivateContext(LUserInputContext* InContext);
    ENGINE_API  void DeactivateContext(const LSimpleString& InName);
    ENGINE_API  void DeactivateContext(LUserInputContext* InContext);
    ENGINE_API i32 DeactivateAllContexts();
    FORCEINLINE auto GetActiveContexts() const -> const TArray<LUserInputContext*>& { return this->ActiveContexts; }
    FORCEINLINE auto GetRegisteredContexts() const -> const TArray<LUserInputContext*>& { return this->RegisteredContexts; }

    //#
    //# This frame platform-localized input.
    //#
    ENGINE_API bool HasBufferedPlatformInput() const;
    ENGINE_API auto GetBufferedPlatformInput() const -> const LString&;

private:

    void DispatchInputDelegatesForAction(const LUserInputContext* InContext, const TArray<LRawInput>& InRawInputs, LInputMappedAction* InAction);

    TArray<LInputAction*> RegisteredActions;

    //#
    //# The most important context is stored first.
    //#
    TArray<LUserInputContext*> ActiveContexts;
    TArray<LUserInputContext*> RegisteredContexts;
};

} /* ~Namespace Jafg */
