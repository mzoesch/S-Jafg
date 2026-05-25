// Copyright mzoesch. All rights reserved.

#include "User/Input/InputContext.h"
#include "User/Input/InputAction.h"
#include "User/Input/UserInput.h"
#if JAFG_DO_CHECKS
    #include "Engine/Engine.h"
#endif /* WITH_CHECKS */

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(LUserInputRegistry* Registry, LInputAction&& InAction) noexceptcheck
{
    check(Tasks::IsOnMasterThread())
    check(Registry)
    return this->MapAction(Registry->RegisterAction(std::move(InAction))->GetTag());
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(LUserInputTag ActionTag) noexceptcheck
{
    check(Tasks::IsOnMasterThread())
    check(ActionTag.IsSet())
    check(GEngine)
    check(GEngine->GetLocalEgo().GetUserInputRegistry().GetActionByName(ActionTag))
    this->MappedActions.emplace_back(ActionTag);
    return &this->MappedActions.back();
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(
    LUserInputRegistry* Registry,
    LInputAction&& TransientAction,
    LPhysicalKey DefaultKey,
    EInputActionTriggerFlags TriggerFlags,
    TArray<TUnique<LInputActionMappedTriggerModifier>>&& Modifiers,
    LOnUserInputAction&& Callback
    ) noexcept
{
    check(Tasks::IsOnMasterThread())
    check(Registry)
    return this->MapAction(
        Registry->RegisterAction(std::move(TransientAction))->GetTag(),
        DefaultKey,
        TriggerFlags,
        std::move(Modifiers),
        std::move(Callback)
        );
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(
    LUserInputRegistry* Registry,
    LInputAction&& TransientAction,
    TArray<LInputTrigger>&& Triggers,
    LOnUserInputAction&& Callback
    ) noexcept
{
    check(Tasks::IsOnMasterThread())
    return this->MapAction(
        Registry->RegisterAction(std::move(TransientAction))->GetTag(),
        std::move(Triggers),
        std::move(Callback)
        );
}
