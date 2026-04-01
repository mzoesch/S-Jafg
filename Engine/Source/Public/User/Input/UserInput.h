// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/SurfaceForward.h"
#include "User/Input/RawInput.h"
#include "User/Input/InputContext.h"
#include "User/Input/InputMode.h"

namespace Jafg
{

class LLocalEgo;
class APersonaController;

//#
//# Proxy for the raw platform physical input data and the dispatching of input delegates.
//#
class LUserInput final
{
public:

    typedef TArray<std::pair<EInputMode, TArray<LUserInputTag>>> LContextStack;

    constexpr LUserInput() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInput)
    constexpr ~LUserInput() noexcept = default;
    //# Jafg internal method. Do not use!
    FORCEINLINE constexpr void _SetSurface(LSurface& InSurface) noexcept { this->Surface = &InSurface; }

    void DispatchInputDelegates(APersonaController& ActingController);

    //#
    //# Activate a context.
    //# @param Where Position where to insert the context. INDEX_NONE to add at the end.
    //# @return True if the context was activated.
    //#
    ENGINE_API  bool ActivateContext(LUserInputTag Tag, LSize Where = INDEX_NONE) noexcept;
    FORCEINLINE bool ActivateContext(LUserInputContext const& Context, LSize Where = INDEX_NONE) noexcept { return this->ActivateContext(Context.GetTag(), Where); }
    FORCEINLINE bool ActivateContext(LStringView Name, LSize Where = INDEX_NONE) noexcept { return this->ActivateContext(LUserInputTag::ToTag(Name), Where); }
    ENGINE_API  bool ActivateContexts(TArray<LUserInputContext const*> const& Contexts, LSize Where = INDEX_NONE) noexcept;
    ENGINE_API  bool ActivateContexts(TArray<LUserInputTag> const& Tags, LSize Where = INDEX_NONE) noexcept;
    ENGINE_API  bool ActivateContexts(TArray<LStringView> const& Names, LSize Where = INDEX_NONE) noexcept;

    //#
    //# Deactivate a context. Nullptr is ok to pass.
    //# @return True if the context was activated.
    //#
    FORCEINLINE bool DeactivateContext(LUserInputContext const& Context) noexcept { return this->DeactivateContext(Context.GetTag()); }
    ENGINE_API  bool DeactivateContext(LUserInputTag Tag) noexcept;
    FORCEINLINE bool DeactivateContext(LStringView Name) noexcept { return this->DeactivateContext(LUserInputTag::ToTag(Name)); }
    ENGINE_API  bool DeactivateContexts(TArray<LUserInputContext const*> const& Contexts) noexcept;
    ENGINE_API  bool DeactivateContexts(TArray<LUserInputTag> const& Names) noexcept;
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
    //# @return Has value if a previous applied context snapshot was applied.
    //#
    ENGINE_API TOptional<EInputMode> PopContexts() noexcept;

    FORCEINLINE auto const& GetActiveContexts() const noexcept { return this->ActiveContexts; }
    FORCEINLINE auto& GetMutableActiveContexts() noexcept { return this->ActiveContexts; }

    FORCEINLINE auto const& GetContextStack() const noexcept { return this->ContextStack; }
    FORCEINLINE auto& GetMutableContextStack() noexcept { return this->ContextStack; }

private:

    void DispatchInputDelegatesForKeyCategory(APersonaController& ActingController, TArray<LRawInput>* Inputs, EInputActionTriggerBits Trigger);

    //# Early contexts will be processed first.
    TArray<LUserInputTag> ActiveContexts;
    LContextStack ContextStack;
    LSurface* Surface{};
};

} /* ~Namespace Jafg */
