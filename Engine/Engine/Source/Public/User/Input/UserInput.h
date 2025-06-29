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

    //#
    //# Deactivate a context. Nullptr is ok to pass.
    //# @return True if the context was activated.
    //#
    ENGINE_API  bool DeactivateContext(LUserInputContext* InContext);
    FORCEINLINE bool DeactivateContext(const LName& InName) { return this->DeactivateContext(this->GetContextByName(InName)); }
    FORCEINLINE bool DeactivateContext(const LString& InName) { return this->DeactivateContext(GET_NAME(InName)); }

    ENGINE_API  i32  DeactivateAllContexts(TArray<LUserInputContext*>* OutActiveContexts = nullptr);

    ENGINE_API TArray<LRawInput>        GetTriggeredKeys() const;
    ENGINE_API const TArray<LRawInput>& GetOngoingKeys() const;
    ENGINE_API TArray<LRawInput>        GetCompletedKeys() const;

    FORCEINLINE       LUserInputContext* GetContextByName(const LName& InName) { Smart::TUnique<LUserInputContext>* Out = this->RegisteredContexts.FindRef(InName); return Out ? Out->GetPointer() : nullptr; }
    FORCEINLINE const LUserInputContext* GetContextByName(const LName& InName) const { const Smart::TUnique<LUserInputContext>* Out = this->RegisteredContexts.FindRef(InName); return Out ? Out->GetPointer() : nullptr; }
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

    FORCEINLINE       LInputAction* GetActionByName(const LName& InName) { Smart::TUnique<LInputAction>* Out = this->RegisteredActions.FindRef(InName); return Out ? Out->GetPointer() : nullptr; }
    FORCEINLINE const LInputAction* GetActionByName(const LName& InName) const { const Smart::TUnique<LInputAction>* Out = this->RegisteredActions.FindRef(InName); return Out ? Out->GetPointer() : nullptr; }
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

    FORCEINLINE const TArray<LUserInputContext*>& GetActiveContexts() const { return this->ActiveContexts; }

    FORCEINLINE const TArray<Smart::TUnique<LInputAction>>& GetRegisteredActions() const { return this->RegisteredActions; }
    FORCEINLINE const TArray<Smart::TUnique<LUserInputContext>>& GetRegisteredContexts() const { return this->RegisteredContexts; }

private:

    void DispatchInputDelegatesForKeyCategory(TArray<LRawInput>* InRawInputs, const EInputActionTrigger::Type InActionTriggerType);

    TArray<Smart::TUnique<LInputAction>> RegisteredActions;
    TArray<Smart::TUnique<LUserInputContext>> RegisteredContexts;

    //#
    //# The most important context is stored first.
    //#
    TArray<LUserInputContext*> ActiveContexts;
};

} /* ~Namespace Jafg */
