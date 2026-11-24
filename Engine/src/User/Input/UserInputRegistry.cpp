// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInputRegistry.h"
#include "Core/TaskUtility.h"
#include "Engine/Engine.h"

Jafg::LMasterThreadTagRegistry<Jafg::LUserInputTag>& Jafg::LUserInputTag::GetMutableRegistry() noexcept
{
    if (Detail::GMutableEngine)
    {
        return Detail::GMutableEngine->GetLocalEgo()
            .GetUserInputRegistry()
            .GetMutableTagRegistry();
    }
    LOG_FATAL(LogUserInput, "Engine is invalid.")
}

Jafg::LInputContext* Jafg::LUserInputRegistry::RegisterContext(ContextCreateInfo Info)
{
    check(Tasks::IsOnMasterThread())

    LUserInputTag Tag{LUserInputTag::ToTag(Info.Identifier)};
    if (algo::contains(this->Contexts, Tag, &LInputContext::GetTag))
    {
        LOG_WARNING(LogUserInput, "[{}]: Already registered.", Tag.ToString())
        return nullptr;
    }

    LOG_VERBOSE(LogUserInput, "[{}]: Registered.", Tag.ToString())
    if (Info.DisplayName.empty())
    {
        this->Contexts.emplace_back(Tag, algo::add_spaces_to_camel_case(Tag.ToString()));
    }
    else
    {
        this->Contexts.emplace_back(Tag, LString{Info.DisplayName});
    }
    return &this->Contexts.back();
}
