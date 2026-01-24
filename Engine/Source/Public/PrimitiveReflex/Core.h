// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename T>
FORCEINLINE constexpr std::string_view GetTypeName() noexcept
{
    constexpr std::string_view Signature{ JAFG_FUNCTION_SIG };

#if JAFG_WITH_MSVC
    constexpr std::string_view Prefix{ "std::string_view __cdecl type_name<" };
    constexpr std::string_view Suffix{ ">(void)" };
#elif JAFG_WITH_CLANG || JAFG_WITH_GCC
    constexpr std::string_view Prefix{ "std::string_view Jafg::GetTypeName() [T = " };
    constexpr std::string_view Suffix{ "]" };
#else /* JAFG_WITH_CLANG || JAFG_WITH_GCC */
    #error "Missing compiler implementation."
#endif /* !(JAFG_WITH_CLANG || JAFG_WITH_GCC) */

    return Signature.substr(Prefix.size(), Signature.size() - Prefix.size() - Suffix.size());
}

} /* ~Namespace Jafg */
