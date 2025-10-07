// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/RawInput.h"
#include "User/Input/InputContext.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputAction.h"

namespace Jafg
{

class LLocalEgo;

//#
//# Proxy for the raw platform physical input data and the dispatching of input delegates.
//#
class LUserInput final
{
public:

    typedef TArray<TArray<LUserInputContext*>> LContextStack;

    LUserInput() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LUserInput)
    ~LUserInput() = default;

    //# Whether this key was just downed this frame.
    ENGINE_API bool IsNewDown(const LKey Key) const;

    void DispatchInputDelegates();

    ENGINE_API LLocalEgo* GetLocalEgo() const;

    //# @return The newly mapped action. This is not the same as the input argument. Nullptr if something went wrong.
    ENGINE_API const LInputAction* RegisterAction(LInputAction&& InAction);

    //# @return The context that was added. Nullptr if something went wrong.
    ENGINE_API LUserInputContext* RegisterContext(LUserInputContext&& Context, const bool bMakeActive = false);

    //#
    //# Activate a context. Nullptr is ok to pass.
    //# @return True if the context was activated.
    //#
    ENGINE_API  bool ActivateContext(LUserInputContext* InContext);
    FORCEINLINE bool ActivateContext(const LName& InName) { return this->ActivateContext(this->GetContextByName(InName)); }
    FORCEINLINE bool ActivateContext(const LString& InName) { return this->ActivateContext(GET_NAME(InName)); }
    FORCEINLINE bool ActivateContexts(const TArray<LUserInputContext*>& InContexts);
    FORCEINLINE bool ActivateContexts(const TArray<LName>& InNames);
    FORCEINLINE bool ActivateContexts(const TArray<LString>& InNames);

    //#
    //# Deactivate a context. Nullptr is ok to pass.
    //# @return True if the context was activated.
    //#
    ENGINE_API  bool DeactivateContext(LUserInputContext* InContext);
    FORCEINLINE bool DeactivateContext(const LName& InName) { return this->DeactivateContext(this->GetContextByName(InName)); }
    FORCEINLINE bool DeactivateContext(const LString& InName) { return this->DeactivateContext(GET_NAME(InName)); }
    FORCEINLINE bool DeactivateContexts(const TArray<LUserInputContext*>& InContexts);
    FORCEINLINE bool DeactivateContexts(const TArray<LName>& InNames);
    FORCEINLINE bool DeactivateContexts(const TArray<LString>& InNames);

    ENGINE_API  i32  DeactivateAllContexts(TArray<LUserInputContext*>* OutActiveContexts = nullptr);

    //#
    //# Push all current active contexts as a snapshot to a stack.
    //# @param bReset If true, the current active contexts array will be emptied. This will result no active contexts
    //#               after this call.
    //#
    ENGINE_API void PushContexts(const bool bEmpty = true);
    //#
    //# Removes a snapshot, pushed with #PushContexts, from the stack and applies it to the current active contexts.
    //# The active contexts will be removed.
    //# @return True if a previous applied context snapshot was applied.
    //#
    ENGINE_API bool PopContexts();

    ENGINE_API TArray<LRawInput>        GetTriggeredKeys() const;
    ENGINE_API const TArray<LRawInput>& GetOngoingKeys() const;
    ENGINE_API TArray<LRawInput>        GetCompletedKeys() const;

    FORCEINLINE       LUserInputContext* GetContextByName(const LName& InName) { auto* Out { algo::wfind_pointer(this->RegisteredContexts, InName, algo::unique_raw{}) }; if (Out) { return Out->get(); } return nullptr; }
    FORCEINLINE const LUserInputContext* GetContextByName(const LName& InName) const { auto const* Out { algo::wfind_pointer(this->RegisteredContexts, InName, algo::unique_raw{}) }; if (Out) { return Out->get(); } return nullptr; }
    FORCEINLINE       LUserInputContext* GetContextByNameChecked(const LName& InName) { LUserInputContext* Out = this->GetContextByName(InName); check( Out ) return Out; }
    FORCEINLINE const LUserInputContext* GetContextByNameChecked(const LName& InName) const { const LUserInputContext* Out = this->GetContextByName(InName); check( Out ) return Out; }
    FORCEINLINE       LUserInputContext* GetContextByNameAsserted(const LName& InName) { LUserInputContext* Out = this->GetContextByName(InName); jassert( Out ) return Out; }
    FORCEINLINE const LUserInputContext* GetContextByNameAsserted(const LName& InName) const { const LUserInputContext* Out = this->GetContextByName(InName); jassert( Out ) return Out; }

    FORCEINLINE       LUserInputContext* GetContextByName(const LString& InName) { return this->GetContextByName(GET_NAME_CHECKED(InName)); }
    FORCEINLINE const LUserInputContext* GetContextByName(const LString& InName) const { return this->GetContextByName(GET_NAME_CHECKED(InName)); }
    FORCEINLINE       LUserInputContext* GetContextByNameChecked(const LString& InName) { LUserInputContext* Out = this->GetContextByName(InName); check( Out ) return Out; }
    FORCEINLINE const LUserInputContext* GetContextByNameChecked(const LString& InName) const { const LUserInputContext* Out = this->GetContextByName(InName); check( Out ) return Out; }
    FORCEINLINE       LUserInputContext* GetContextByNameAsserted(const LString& InName) { LUserInputContext* Out = this->GetContextByName(InName); jassert( Out ) return Out; }
    FORCEINLINE const LUserInputContext* GetContextByNameAsserted(const LString& InName) const { const LUserInputContext* Out = this->GetContextByName(InName); jassert( Out ) return Out; }

    FORCEINLINE       LInputAction* GetActionByName(const LName& InName) { TUnique<LInputAction>* Out { algo::wfind_pointer(this->RegisteredActions, InName, algo::unique_raw{}) }; if (Out) { return Out->get(); } return nullptr; }
    FORCEINLINE const LInputAction* GetActionByName(const LName& InName) const { const TUnique<LInputAction>* Out { algo::wfind_pointer(this->RegisteredActions, InName, algo::unique_raw{}) }; if (Out) { return Out->get(); } return nullptr; }
    FORCEINLINE       LInputAction* GetActionByNameChecked(const LName& InName) { LInputAction* Out = this->GetActionByName(InName); check( Out ) return Out; }
    FORCEINLINE const LInputAction* GetActionByNameChecked(const LName& InName) const { const LInputAction* Out = this->GetActionByName(InName); check( Out ) return Out; }
    FORCEINLINE       LInputAction* GetActionByNameAsserted(const LName& InName) { LInputAction* Out = this->GetActionByName(InName); jassert( Out ) return Out; }
    FORCEINLINE const LInputAction* GetActionByNameAsserted(const LName& InName) const { const LInputAction* Out = this->GetActionByName(InName); jassert( Out ) return Out; }

    FORCEINLINE       LInputAction* GetActionByName(const LString& InName) { return this->GetActionByName(GET_NAME_CHECKED(InName)); }
    FORCEINLINE const LInputAction* GetActionByName(const LString& InName) const { return this->GetActionByName(GET_NAME_CHECKED(InName)); }
    FORCEINLINE       LInputAction* GetActionByNameChecked(const LString& InName) { LInputAction* Out = this->GetActionByName(InName); check( Out ) return Out; }
    FORCEINLINE const LInputAction* GetActionByNameChecked(const LString& InName) const { const LInputAction* Out = this->GetActionByName(InName); check( Out ) return Out; }
    FORCEINLINE       LInputAction* GetActionByNameAsserted(const LString& InName) { LInputAction* Out = this->GetActionByName(InName); jassert( Out ) return Out; }
    FORCEINLINE const LInputAction* GetActionByNameAsserted(const LString& InName) const { const LInputAction* Out = this->GetActionByName(InName); jassert( Out ) return Out; }

    //#
    //# This frame platform-localized input for focused surface.
    //#
    ENGINE_API bool HasBufferedPlatformInput() const;
    ENGINE_API const TArray<LString>& GetBufferedPlatformInput() const;
    ENGINE_API LString GetBufferedPlatformInputAsStr() const;

    FORCEINLINE const TArray<LUserInputContext*>& GetActiveContexts() const noexcept { return this->ActiveContexts; }
    FORCEINLINE const LContextStack& GetContextStack() const noexcept { return this->ContextStack; }
    FORCEINLINE void SetReferenceContexts(const TArray<LUserInputContext*>& InContexts) { this->ReferenceContexts = InContexts; }
    FORCEINLINE const TArray<LUserInputContext*>& GetReferenceContexts() const noexcept { return this->ReferenceContexts; }

    FORCEINLINE const TArray<TUnique<LInputAction>>& GetRegisteredActions() const noexcept { return this->RegisteredActions; }
    FORCEINLINE const TArray<TUnique<LUserInputContext>>& GetRegisteredContexts() const noexcept { return this->RegisteredContexts; }

private:

    void DispatchInputDelegatesForKeyCategory(TArray<LRawInput>* InRawInputs, const EInputActionTrigger::Type InActionTriggerType);

    TArray<TUnique<LInputAction>> RegisteredActions;
    TArray<TUnique<LUserInputContext>> RegisteredContexts;

    //#
    //# The most important context is stored first.
    //#
    TArray<LUserInputContext*> ActiveContexts;
    LContextStack ContextStack;
    TArray<LUserInputContext*> ReferenceContexts;
};

FORCEINLINE bool LUserInput::ActivateContexts(const TArray<LUserInputContext*>& InContexts)
{
    bool bOut { false };

    algo::for_each(InContexts, [this, &bOut](LUserInputContext* InContext) -> void
    {
        if (this->ActivateContext(InContext))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

FORCEINLINE bool LUserInput::ActivateContexts(const TArray<LName>& InNames)
{
    bool bOut { false };

    algo::for_each(InNames, [this, &bOut](const LName& InName) -> void
    {
        if (this->ActivateContext(InName))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}
FORCEINLINE bool LUserInput::ActivateContexts(const TArray<LString>& InNames)
{
    bool bOut { false };

    algo::for_each(InNames, [this, &bOut](const LString& InName) -> void
    {
        if (this->ActivateContext(InName))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

FORCEINLINE bool LUserInput::DeactivateContexts(const TArray<LUserInputContext*>& InContexts)
{
    bool bOut { false };

    algo::for_each(InContexts, [this, &bOut](LUserInputContext* InContext) -> void
    {
        if (this->DeactivateContext(InContext))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

FORCEINLINE bool LUserInput:: DeactivateContexts(const TArray<LName>& InNames)
{
    bool bOut { false };

    algo::for_each(InNames, [this, &bOut](const LName& InName) -> void
    {
        if (this->DeactivateContext(InName))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

FORCEINLINE bool LUserInput:: DeactivateContexts(const TArray<LString>& InNames)
{
    bool bOut { false };

    algo::for_each(InNames, [this, &bOut](const LString& InName) -> void
    {
        if (this->DeactivateContext(InName))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}
} /* ~Namespace Jafg */
