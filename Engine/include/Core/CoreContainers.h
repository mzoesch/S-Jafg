// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename _Tp, typename _Alloc>
class TSimpleArray;

} /* ~Namespace Jafg */

namespace Jafg
{

template<typename T, typename TAlloc = std::allocator<T>>
class TSimpleArray : public std::vector<T, TAlloc>
{
public:

    using TSuper = std::vector<T, TAlloc>;

    using TSuper::TSuper;
    using TSuper::operator=;

    NODISCARD FORCEINLINE constexpr auto&& reflexive_push_back(this auto&& self, T const& What)
        requires std::is_copy_constructible_v<T>
    {
        self.push_back(What);
        return std::forward<decltype(self)>(self);
    }

    NODISCARD FORCEINLINE constexpr auto&& reflexive_push_back(this auto&& self, T&& What)
        requires std::is_move_constructible_v<T>
    {
        self.push_back(std::move(What));
        return std::forward<decltype(self)>(self);
    }

    NODISCARD FORCEINLINE constexpr auto&& reflexive_emplace_back(this auto&& self, auto&&... args)
        requires std::is_constructible_v<T, decltype(args)&&...>
    {
        self.emplace_back(std::forward<decltype(args)>(args)...);
        return std::forward<decltype(self)>(self);
    }
};

} /* ~Namespace Jafg */

template<typename T, typename TCompare = std::less<T>, typename TAlloc = std::allocator<T>>
using TSet = std::set<T, TCompare, TAlloc>;

template<typename T, typename Alloc = std::allocator<T>>
using TArray = Jafg::TSimpleArray<T, Alloc>;

using LString = std::string;
using LWString = std::wstring;
using Lu8String = std::u8string;
using Lu16String = std::u16string;
using Lu32String = std::u32string;

using LStringView = std::string_view;
using LWStringView = std::wstring_view;
using Lu8StringView = std::u8string_view;
using Lu16StringView = std::u16string_view;
using Lu32StringView = std::u32string_view;

#if JAFG_PLATFORM_USES_UTF8
    using LNativeString = LString;
#elif JAFG_PLATFORM_USES_UTF16
    using LNativeString = LWString;
#else /* JAFG_PLATFORM_USES_UTF16 */
    #error "Missing encoing for native strings."
#endif /* !JAFG_PLATFORM_USES_UTF16 */

using LPath = std::filesystem::path;

template<typename T, typename Deleter = std::default_delete<T>>
using TUnique = std::unique_ptr<T, Deleter>;

template<>
struct std::formatter<LPath>: std::formatter<std::string>
{
    FORCEINLINE std::format_context::iterator format(LPath const& Path, std::format_context& InContext) const
    {
#if JAFG_PLATFORM_USES_UTF8
        return std::formatter<std::string>::format(Path.native(), InContext);
#else /* JAFG_PLATFORM_USES_UTF8 */
        return std::formatter<std::string>::format(Path.string(), InContext);
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
};
