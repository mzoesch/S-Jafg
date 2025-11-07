// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInputRegistry.h"

Jafg::LInputAction const* Jafg::LUserInputRegistry::RegisterAction(LInputAction&& InAction)
{
    if (algo::contains(this->Actions, InAction.GetName(), &LInputAction::GetName))
    {
        LOG_WARNING(LogUserInput, "Action [{}] was already registered.", InAction.GetName())
        return nullptr;
    }

    this->Actions.emplace_back(std::move(InAction));
    return &this->Actions.back();
}

Jafg::LUserInputContext* Jafg::LUserInputRegistry::RegisterContext(LUserInputContext&& Context)
{
    if (algo::contains(this->Contexts, Context.GetName(), &LUserInputContext::GetName))
    {
        LOG_WARNING(LogUserInput, "Context [{}] was already registered.", Context.GetName())
        return nullptr;
    }

    this->Contexts.emplace_back(std::move(Context));
    return &this->Contexts.back();
}
