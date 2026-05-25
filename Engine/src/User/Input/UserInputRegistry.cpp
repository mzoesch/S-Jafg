// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInputRegistry.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"

Jafg::LUserInputTag Jafg::LUserInputTag::ToTag(LStringView S) noexcept
{
    if (Detail::GMutableEngine)
    {
        return Detail::GMutableEngine->GetLocalEgo().GetUserInputRegistry().GetMutableTagRegistry().RegisterOrGet(S);
    }

    LOG_FATAL(LogUserInput, "Attempted to convert string [{}] to user input tag but engine is not valid.", S)
}

Jafg::LUserInputTag Jafg::LUserInputTag::AsTag(LStringView S)
{
    if (Detail::GMutableEngine)
    {
        return Detail::GMutableEngine->GetLocalEgo().GetUserInputRegistry().GetMutableTagRegistry().GetTag(S);
    }

    LOG_FATAL(LogUserInput, "Attempted to convert string [{}] to user input tag but engine is not valid.", S)
}

LString Jafg::LUserInputTag::ToString() const noexcept
{
    if (GEngine)
    {
        return GEngine->GetLocalEgo().GetUserInputRegistry().GetTagRegistry().GetReprSafe(*this);
    }

    LOG_FATAL(LogUserInput, "Attempted to convert user input tag [{}] to string but engine is not valid.", *this)
}

Jafg::LInputAction const* Jafg::LUserInputRegistry::RegisterAction(LInputAction&& InAction)
{
    check( Tasks::IsOnMasterThread() )

    if (algo::contains(this->Actions, InAction.GetTag(), &LInputAction::GetTag))
    {
        LOG_WARNING(LogUserInput, "Action [{}] was already registered.", InAction.GetTag())
        return nullptr;
    }

    LOG_VERBOSE(LogUserInput, "Registered user input action [{}].", InAction.GetTag())
    this->Actions.emplace_back(std::move(InAction));
    return &this->Actions.back();
}

Jafg::LUserInputContext* Jafg::LUserInputRegistry::RegisterContext(LUserInputContext&& Context)
{
    check( Tasks::IsOnMasterThread() )

    if (algo::contains(this->Contexts, Context.GetTag(), &LUserInputContext::GetTag))
    {
        LOG_WARNING(LogUserInput, "Context [{}] was already registered.", Context.GetTag())
        return nullptr;
    }

    LOG_VERBOSE(LogUserInput, "Registered user input context [{}].", Context.GetTag())
    this->Contexts.emplace_back(std::move(Context));
    return &this->Contexts.back();
}
