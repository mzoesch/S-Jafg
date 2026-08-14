// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

//# Use sparingly.
ENGINE_API void EmitAndFlushLogs() noexcept;

} /* ~Namespace Detail */

template<typename TPredicate>
inline void FwdCheck(TPredicate&& Predicate, char const* Str, char const* File, int Line)
{
    static_assert(std::is_invocable_r_v<bool, TPredicate>);

    if (JAFG_UNLIKELY(!Predicate()))
    {
        Detail::EmitAndFlushLogs();
        PRIVATE_JAFG_TRY_BREAK_NO_FACADE()
        App::Detail::TrapMe(Str, File, Line);
    }

    return;
}

} /* ~Namespace Jafg */
