// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

ENGINE_API extern std::size_t GLogMessagesLimit;
ENGINE_API extern std::list<LLogMessage> GUnprocessedLogMessages;
ENGINE_API extern std::list<LLogMessage> GLogMessages;
ENGINE_API extern algo::tas_lock GLogMessagesLock;

ENGINE_API extern bool GIsQuiet;
ENGINE_API extern bool GAllowHelp;
ENGINE_API extern bool GAllowInformation;

} /* ~Namespace Detail */

template<typename TCategory, ELogVerbosity Verbosity, typename... TArgs>
FORCEINLINE void EmitLog(LString Category, char const* Format, TArgs&&... Args) noexcept
{
    if constexpr (TCategory::template CompilesFor<Verbosity>)
    {
        if (   Detail::GIsQuiet
            && !(Detail::GAllowHelp && std::same_as<std::remove_cvref_t<TCategory>, std::remove_cvref_t<decltype(LogHelp)>>)
            && !(Detail::GAllowInformation && std::same_as<std::remove_cvref_t<TCategory>, std::remove_cvref_t<decltype(LogInformation)>>)
            )
        {
            return;
        }

        Detail::LLogMessage Message{
            .Verbosity = Verbosity,
            .Category = std::move(Category),
            .Message = algo::sprintf(Format, Args...),
            };
        std::scoped_lock Lock{Detail::GLogMessagesLock};
        Detail::GUnprocessedLogMessages.emplace_back(std::move(Message));
    }
}

} /* ~Namespace Jafg */
