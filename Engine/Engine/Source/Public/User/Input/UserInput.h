// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/SurfaceForward.h"
#include "User/Input/RawInput.h"
#include "User/Input/InputContext.h"

namespace Jafg
{

class LLocalEgo;

//#
//# Proxy for the raw platform physical input data and the dispatching of input delegates.
//#
class LUserInput final
{
public:

    typedef TArray<TArray<LName>> LContextStack;

    constexpr LUserInput() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInput)
    constexpr ~LUserInput() noexcept = default;

    void DispatchInputDelegates(LSurface& Surface);

    //#
    //# Activate a context.
    //# @param Where Position where to insert the context. INDEX_NONE to add at the end.
    //# @return True if the context was activated.
    //#
    ENGINE_API  bool ActivateContext(LName Name, LSize Where = INDEX_NONE) noexcept;
    FORCEINLINE bool ActivateContext(LUserInputContext const& Context, LSize Where = INDEX_NONE) noexcept { return this->ActivateContext(Context.GetName(), Where); }
    FORCEINLINE bool ActivateContext(LStringView Name, LSize Where = INDEX_NONE) noexcept { return this->ActivateContext(GET_NAME(Name), Where); }
    ENGINE_API  bool ActivateContexts(TArray<LUserInputContext const*> const& Contexts, LSize Where = INDEX_NONE) noexcept;
    ENGINE_API  bool ActivateContexts(TArray<LName> const& Names, LSize Where = INDEX_NONE) noexcept;
    ENGINE_API  bool ActivateContexts(TArray<LStringView> const& Names, LSize Where = INDEX_NONE) noexcept;

    //#
    //# Deactivate a context. Nullptr is ok to pass.
    //# @return True if the context was activated.
    //#
    FORCEINLINE bool DeactivateContext(LUserInputContext const& Context) noexcept { return this->DeactivateContext(Context.GetName()); }
    ENGINE_API  bool DeactivateContext(LName Name) noexcept;
    FORCEINLINE bool DeactivateContext(LStringView Name) noexcept { return this->DeactivateContext(GET_NAME(Name)); }
    ENGINE_API  bool DeactivateContexts(TArray<LUserInputContext const*> const& Contexts) noexcept;
    ENGINE_API  bool DeactivateContexts(TArray<LName> const& Names) noexcept;
    ENGINE_API  bool DeactivateContexts(TArray<LStringView> const& Names) noexcept;

    //#
    //# Push all current active contexts as a snapshot to a stack.
    //# @param bEmpty If true, the current active contexts array will be emptied. This will result no active contexts
    //#               after this call.
    //#
    ENGINE_API void PushContexts(bool bEmpty = true) noexcept;
    //#
    //# Removes a snapshot, pushed with #PushContexts, from the stack and applies it to the current active contexts.
    //# The active contexts will be removed.
    //# @return True if a previous applied context snapshot was applied.
    //#
    ENGINE_API bool PopContexts() noexcept;

    FORCEINLINE auto const& GetActiveContexts() const noexcept { return this->ActiveContexts; }
    FORCEINLINE auto& GetMutableActiveContexts() noexcept { return this->ActiveContexts; }

    FORCEINLINE auto const& GetContextStack() const noexcept { return this->ContextStack; }
    FORCEINLINE auto& GetMutableContextStack() noexcept { return this->ContextStack; }

private:

    void DispatchInputDelegatesForKeyCategory(LSurface& Surface, TArray<LRawInput>* Inputs, EInputActionTrigger::Type TriggerType);

    //# Early contexts will be processed first.
    TArray<LName> ActiveContexts;
    LContextStack ContextStack;
};

} /* ~Namespace Jafg */
