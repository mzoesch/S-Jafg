// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename TPredicate>
inline void FwdCheck(TPredicate&& Predicate, char const* Str, char const* File, int Line);

#define JAFG_FWD_CHECK(...) ::Jafg::FwdCheck(JAFG_COMBINE_ANY_OUTER(__VA_ARGS__), JAFG_MAKE_STRING_OUTER(__VA_ARGS__), __FILE__, __LINE__);

} /* ~Namespace Jafg */
