// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template<typename T>
FORCEINLINE constexpr std::string_view GetTypeName() noexcept
{
    constexpr std::string_view Signature{ LAL_FUNCTION_SIG };

#if LAL_WITH_MSVC
    constexpr std::string_view Prefix{ "std::string_view __cdecl type_name<" };
    constexpr std::string_view Suffix{ ">(void)" };
#elif LAL_WITH_CLANG || LAL_WITH_GCC
    constexpr std::string_view Prefix{ "std::string_view Lal::GetTypeName() [T = " };
    constexpr std::string_view Suffix{ "]" };
#else /* LAL_WITH_CLANG || LAL_WITH_GCC */
    #error "Missing compiler implementation."
#endif /* !(LAL_WITH_CLANG || LAL_WITH_GCC) */

    return Signature.substr(Prefix.size(), Signature.size() - Prefix.size() - Suffix.size());
}

} /* ~Namespace Lal */
