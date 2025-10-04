// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TPredicate>
inline void FwdCheck(TPredicate&& Predicate, char const* Str, char const* File, int Line);

#define LAL_FWD_CHECK(...) ::Lal::FwdCheck(LAL_COMBINE_ANY_OUTER(__VA_ARGS__), LAL_MAKE_STRING_OUTER(__VA_ARGS__), __FILE__, __LINE__);

} /* ~Namespace Lal */
