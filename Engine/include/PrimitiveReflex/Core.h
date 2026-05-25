// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename T>
FORCEINLINE constexpr std::string_view GetTypeName() noexcept
{
    constexpr std::string_view Signature{ JAFG_FUNCTION_SIG };

#if JAFG_WITH_MSVC
    constexpr std::string_view StructPrefix{ "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Jafg::GetTypeName<struct " };
    constexpr std::string_view ClassPrefix{ "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Jafg::GetTypeName<class " };
    constexpr std::string_view Suffix{ ">(void) noexcept" };

    if (Signature.starts_with(StructPrefix))
    {
        return Signature.substr(StructPrefix.size(), Signature.size() - StructPrefix.size() - Suffix.size());
    }
    return Signature.substr(ClassPrefix.size(), Signature.size() - ClassPrefix.size() - Suffix.size());
#elif JAFG_WITH_CLANG || JAFG_WITH_GCC
    constexpr std::string_view Prefix{ "std::string_view Jafg::GetTypeName() [T = " };
    constexpr std::string_view Suffix{ "]" };
    return Signature.substr(Prefix.size(), Signature.size() - Prefix.size() - Suffix.size());
#else /* JAFG_WITH_CLANG || JAFG_WITH_GCC */
    #error "Missing compiler implementation."
#endif /* !(JAFG_WITH_CLANG || JAFG_WITH_GCC) */
}

} /* ~Namespace Jafg */
