// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TPredicate>
inline void FwdCheck(TPredicate&& Predicate, char const* Str, char const* File, int Line)
{
    static_assert(std::is_invocable_r_v<bool, TPredicate>);

    if (LAL_UNLIKELY(!Predicate()))
    {
        LAL_UNSAFE_FLUSH_OUT_STREAMS()
        PRIVATE_LAL_TRY_BREAK_NO_FACADE()
        LOnPlatformBreak::OnProgramPanic(Str, File, Line);
    }

    return;
}

} /* ~Namespace Lal */
