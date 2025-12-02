// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template<typename _Tp, typename _Alloc>
class TSimpleArray;

template<typename _CharT, typename _Traits, typename _Alloc>
class TSimpleString;

class LSimplePath;

} /* ~Namespace Lal */

#if LAL_WITH_CLANG
namespace std::filesystem
{
inline namespace __cxx11
{
namespace __detail
{

template<typename TChar, typename TTraits, typename TAlloc>
inline constexpr bool __is_path_src<Lal::TSimpleString<TChar, TTraits, TAlloc>> = __is_encoded_char<TChar>;

// SFINAE constraint for Source parameters as required by [fs.path.req].
template<typename _Tp>
using _LalPath = enable_if_t<__is_path_src<_Tp>, Lal::LSimplePath>;

// SFINAE constraint for InputIterator parameters as required by [fs.req].
template<typename _Iter, typename _Tr = __safe_iterator_traits<_Iter>>
using _LalPath2 = enable_if_t<__is_path_iter_src<_Tr>, Lal::LSimplePath>;

template<typename _CharT, typename _Traits, typename _Alloc>
inline basic_string_view<_CharT> __effective_range(Lal::TSimpleString<_CharT, _Traits, _Alloc> const& __source) noexcept
{
    return __source;
}

} /* ~Namespace __detail */
} /* ~Namespace __cxx11 */
} /* ~Namespace std::filesystem */
#endif /* LAL_WITH_CLANG */

namespace Lal
{

template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
class TSimpleArray : public std::vector<_Tp, _Alloc>
{
public:

    using TSuper = std::vector<_Tp, _Alloc>;

    using TSuper::TSuper;
    using TSuper::operator=;

    NODISCARD FORCEINLINE constexpr auto&& reflexive_emplace_back(this auto&& self, auto&&... args)
    {
        self.emplace_back(std::forward<decltype(args)>(args)...);
        return std::forward<decltype(self)>(self);
    }
};

template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Alloc = std::allocator<_CharT>>
class TSimpleString : public std::basic_string<_CharT, _Traits, _Alloc>
{
public:

    using TSuper = std::basic_string<_CharT, _Traits, _Alloc>;

    using TSuper::TSuper;
    using TSuper::operator=;

    constexpr TSimpleString(TSuper&& Other) noexcept : TSuper(std::move(Other)) { }
    constexpr TSimpleString(TSuper&& Other, _Alloc const& Alloc) noexcept : TSuper(std::move(Other), Alloc) { }
};

//#
//# Q: Why _?
//# A: Because LString and _LString may not always be the same (legacy!!).
//#
using _LString = Lal::TSimpleString<LJafgChar>;
using _LWString = Lal::TSimpleString<wchar_t>;
using _Lu8String = Lal::TSimpleString<char8_t>;
using _Lu16String = Lal::TSimpleString<char16_t>;
using _Lu32String = Lal::TSimpleString<char32_t>;

class LSimplePath : public std::filesystem::path
{
public:

    using TSuper = std::filesystem::path;

    using TSuper::TSuper;
    using TSuper::operator=;

    using TStringType = TSimpleString<typename TSuper::value_type>;
    using TEscalatedStringType = TStringType::TSuper;

    static_assert(std::derived_from<TStringType, TSuper::string_type>);
    static_assert(std::derived_from<TEscalatedStringType, TSuper::string_type>);
    static_assert(std::is_same_v<TSuper::string_type, TEscalatedStringType>);

    LSimplePath(TStringType&& Source, format f = auto_format) : TSuper(static_cast<TEscalatedStringType>(Source), f) { }
    LSimplePath(TStringType const& Source, format f = auto_format) : TSuper(static_cast<TEscalatedStringType>(Source), f) { }

#if !LAL_PLATFORM_USES_UTF8
    LSimplePath(_LString const& Source, format f = auto_format) : TSuper(static_cast<_LString::TSuper>(Source), f) { }
#endif /* !LAL_PLATFORM_USES_UTF8 */

    LSimplePath& operator=(path const& Path) { return static_cast<LSimplePath&>(TSuper::operator=(Path)); }
    LSimplePath& operator=(path&& Path) noexcept { return static_cast<LSimplePath&>(TSuper::operator=(std::move(Path))); }
    LSimplePath& operator=(TStringType&& Source) { return *this = LSimplePath{std::move(Source)}; }
    LSimplePath& assign(TStringType&& Source) { return *this = std::move(Source); }

    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& operator=(_Source const& __source)
    {
        return *this = path(__source);
    }
    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& assign(_Source const& __source)
    {
        return *this = path(__source);
    }
    template<typename _InputIterator>
    std::filesystem::__cxx11::__detail::_LalPath2<_InputIterator>& assign(_InputIterator __first, _InputIterator __last)
    {
        return *this = path(__first, __last);
    }

#if !LAL_PLATFORM_USES_UTF8
    LSimplePath& operator=(_LString const& Source) { return *this = static_cast<_LString::TSuper>(Source); }
    LSimplePath& assign(_LString const& Source) { return *this = static_cast<_LString::TSuper>(Source); }
#endif /* !LAL_PLATFORM_USES_UTF8 */

    LSimplePath& operator/=(path const& Other) { return static_cast<LSimplePath&>(TSuper::operator/=(Other)); }
    LSimplePath& operator/=(LSimplePath const& Other) { return static_cast<LSimplePath&>(TSuper::operator/=(static_cast<const path&>(Other))); }
    LSimplePath& operator/=(TStringType const& Other) { return static_cast<LSimplePath&>(TSuper::operator/=(static_cast<const TEscalatedStringType&>(Other))); }

    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& operator/=(_Source const& __source)
    {
        TSuper::operator/=(__source);
        return *this;
    }

    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& append(_Source const& __source)
    {
        TSuper::append(__source);
        return *this;
    }

    template<typename _InputIterator>
    std::filesystem::__cxx11::__detail::_LalPath2<_InputIterator>& append(_InputIterator __first, _InputIterator __last)
    {
        TSuper::append(__first, __last);
        return *this;
    }

#if !LAL_PLATFORM_USES_UTF8
    LSimplePath& operator/=(_LString const& Other) { TSuper::operator/=(static_cast<const _LString::TSuper&>(Other)); return *this; }
    LSimplePath& append(_LString const& Other) { TSuper::append(static_cast<const _LString::TSuper&>(Other)); return *this; }
#endif /* !LAL_PLATFORM_USES_UTF8 */

    LSimplePath& operator+=(path const& __x) { TSuper::operator+=(__x); return *this; }
    LSimplePath& operator+=(string_type const& __x) { TSuper::operator+=(__x); return *this; }
    LSimplePath& operator+=(TStringType const& __x) { TSuper::operator+=(static_cast<TEscalatedStringType const&>(__x)); return *this; }
    LSimplePath& operator+=(value_type const* __x) { TSuper::operator+=(__x); return *this; }
    LSimplePath& operator+=(value_type __x) { TSuper::operator+=(__x); return *this; }
    LSimplePath& operator+=(std::basic_string_view<value_type> __x) { TSuper::operator+=(__x); return *this; }

    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& operator+=(_Source const& __x) { return TSuper::operator+=(__x), *this; }

    template<typename _CharT>
    std::filesystem::__cxx11::__detail::_LalPath2<_CharT*>& operator+=(_CharT __x) { return TSuper::operator+=(__x), *this; }

    template<typename _Source>
    std::filesystem::__cxx11::__detail::_LalPath<_Source>& concat(_Source const& __x)
    {
        TSuper::concat(__x);
        return *this;
    }

    template<typename _InputIterator>
    std::filesystem::__cxx11::__detail::_LalPath2<_InputIterator>& concat(_InputIterator __first, _InputIterator __last)
    {
        TSuper::concat(__first, __last);
        return *this;
    }

#if !LAL_PLATFORM_USES_UTF8
    LSimplePath& operator+=(_LString const& __x) { return TSuper::operator+=(static_cast<_LString::TSuper const&>(__x)), *this; }
    LSimplePath& concat(_LString const& __x) { TSuper::concat(static_cast<_LString::TSuper const&>(__x)); return *this; }
#endif /* !LAL_PLATFORM_USES_UTF8 */

    LSimplePath& make_preferred() { TSuper::make_preferred(); return *this; }
    LSimplePath& remove_filename() { TSuper::remove_filename(); return *this; }
    LSimplePath& replace_filename(const path& __replacement) { TSuper::replace_filename(__replacement); return *this; }
    LSimplePath& replace_extension(const path& __replacement = path()) { TSuper::replace_extension(__replacement); return *this; }

    TStringType const& native() const noexcept { return static_cast<TStringType const&>(TSuper::native()); }
    operator TStringType() const { return TStringType{TSuper::c_str()}; }

    template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Allocator = std::allocator<_CharT>>
    TSimpleString<_CharT, _Traits, _Allocator> string(_Allocator const& __a = _Allocator()) const
    {
        if constexpr (std::is_same_v<_CharT, value_type>)
        {
            return { TSuper::c_str(), TSuper::native().length(), __a };
        }
        else
        {
            auto Temp { TSuper::template string<_CharT, _Traits, _Allocator>(__a) };
            return TSimpleString<_CharT, _Traits, _Allocator>{std::move(Temp)};
        }
    }

    _LString    string() const { return string<_LString::value_type, _LString::traits_type, _LString::allocator_type>(); }
#if LAL_WITH_CLANG && _GLIBCXX_USE_WCHAR_T
    _LWString   wstring() const { return string<_LWString::value_type, _LWString::traits_type, _LWString::allocator_type>(); }
#endif /* LAL_WITH_CLANG && _GLIBCXX_USE_WCHAR_T */
#if LAL_WITH_CLANG && defined(_GLIBCXX_USE_CHAR8_T)
    __attribute__((__abi_tag__("__u8")))
#endif /* LAL_WITH_CLANG && _GLIBCXX_USE_CHAR8_T */
    _Lu8String  u8string() const { return string<_Lu8String::value_type, _Lu8String::traits_type, _Lu8String::allocator_type>(); }
    _Lu16String u16string() const { return string<_Lu16String::value_type, _Lu16String::traits_type, _Lu16String::allocator_type>(); }
    _Lu32String u32string() const { return string<_Lu32String::value_type, _Lu32String::traits_type, _Lu32String::allocator_type>(); }

    //# TODO: We should be able to just cast this right?
    //# Generic format observers.
    template<typename _CharT, typename _Traits = std::char_traits<_CharT>, typename _Allocator = std::allocator<_CharT>>
    TSimpleString<_CharT, _Traits, _Allocator> generic_string(_Allocator const& __a = _Allocator()) const
    {
        auto Temp { TSuper::generic_string<_CharT, _Traits, _Allocator>(__a) };
        return TSimpleString<_CharT, _Traits, _Allocator>{std::move(Temp)};
    }

    _LString    generic_string() const { return generic_string<_LString::value_type, _LString::traits_type, _LString::allocator_type>(); }
#if LAL_WITH_CLANG && _GLIBCXX_USE_WCHAR_T
    _LWString   generic_wstring() const { return generic_string<_LWString::value_type, _LWString::traits_type, _LWString::allocator_type>(); }
#endif /* LAL_WITH_CLANG && _GLIBCXX_USE_WCHAR_T */
#if LAL_WITH_CLANG && defined(_GLIBCXX_USE_CHAR8_T)
    __attribute__((__abi_tag__("__u8")))
#endif /* LAL_WITH_CLANG && _GLIBCXX_USE_CHAR8_T */
    _Lu8String  generic_u8string() const { return generic_string<_Lu8String::value_type, _Lu8String::traits_type, _Lu8String::allocator_type>(); }
    _Lu16String generic_u16string() const { return generic_string<_Lu16String::value_type, _Lu16String::traits_type, _Lu16String::allocator_type>(); }
    _Lu32String generic_u32string() const { return generic_string<_Lu32String::value_type, _Lu32String::traits_type, _Lu32String::allocator_type>(); }

    //# TODO: We should be able to just cast this right?
    LSimplePath root_name() const { return LSimplePath{TSuper::root_name()}; }
    LSimplePath root_directory() const { return LSimplePath{TSuper::root_directory()}; }
    LSimplePath root_path() const { return LSimplePath{TSuper::root_path()}; }
    LSimplePath relative_path() const { return LSimplePath{TSuper::relative_path()}; }
    LSimplePath parent_path() const { return LSimplePath{TSuper::parent_path()}; }
    LSimplePath filename() const { return LSimplePath{TSuper::filename()}; }
    LSimplePath stem() const { return LSimplePath{TSuper::stem()}; }
    LSimplePath extension() const { return LSimplePath{TSuper::extension()}; }

    LSimplePath lexically_normal() const { return LSimplePath{TSuper::lexically_normal()}; }
    LSimplePath lexically_relative(const path& base) const { return LSimplePath{TSuper::lexically_relative(base)}; }
    LSimplePath lexically_proximate(const path& base) const { return LSimplePath{TSuper::lexically_proximate(base)}; }

    /// Write a path to a stream
    template<typename _CharT, typename _Traits>
    friend std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& __os, const path& __p)
    {
        __os << std::quoted(__p.string<_CharT, _Traits>());
        return __os;
    }

    /// Read a path from a stream
    template<typename _CharT, typename _Traits>
    friend std::basic_istream<_CharT, _Traits>& operator>>(std::basic_istream<_CharT, _Traits>& __is, path& __p)
    {
        std::basic_string<_CharT, _Traits> __tmp;
        if (__is >> std::quoted(__tmp))
        {
            __p = std::move(__tmp);
        }
        return __is;
    }
    template<typename _CharT, typename _Traits>
    friend std::basic_istream<_CharT, _Traits>& operator>>(std::basic_istream<_CharT, _Traits>& __is, LSimplePath& __p)
    {
        std::basic_string<_CharT, _Traits> __tmp;
        if (__is >> std::quoted(__tmp))
        {
            __p = std::move(__tmp);
        }
        return __is;
    }

    template<typename _Source, typename _Require = std::filesystem::__cxx11::__detail::_LalPath<_Source>>
    friend LSimplePath operator/(LSimplePath const& Lhs, _Source const& Rhs)
    {
        LSimplePath Out(Lhs);
        Out.operator/=(Rhs);
        return Out;
    }
    friend LSimplePath operator/(LSimplePath const& Lhs, TStringType const& Rhs)
    {
        LSimplePath Out(Lhs);
        Out.operator/=(static_cast<TEscalatedStringType const&>(Rhs));
        return Out;
    }
    friend LSimplePath operator/(LSimplePath const& Lhs, path const& Rhs)
    {
        LSimplePath Out(Lhs);
        Out.operator/=(Rhs);
        return Out;
    }
};

static_assert(sizeof(LSimplePath) == sizeof(std::filesystem::path));
static_assert(alignof(LSimplePath) == alignof(std::filesystem::path));

namespace Private
{

template<typename T, typename = void>
struct TIsPathSourceValid : std::false_type {};
template<typename T>
struct TIsPathSourceValid<T, std::void_t<decltype(LSimplePath(std::declval<T>()))>> : std::true_type {};
template<typename T>
inline constexpr bool TIsPathSourceValid_v = TIsPathSourceValid<T>::value;

} /* ~Namespace Private */

static_assert(Private::TIsPathSourceValid_v<std::string>);
static_assert(Private::TIsPathSourceValid_v<TSimpleString<char>>);

static_assert(std::filesystem::__detail::__is_path_src<std::string>);
static_assert(std::filesystem::__detail::__is_path_src<std::wstring>);
static_assert(std::filesystem::__detail::__is_path_src<std::u8string>);
static_assert(std::filesystem::__detail::__is_path_src<std::u16string>);
static_assert(std::filesystem::__detail::__is_path_src<std::u32string>);
static_assert(std::filesystem::__detail::__is_path_src<_LString>);
static_assert(std::filesystem::__detail::__is_path_src<_LWString>);
static_assert(std::filesystem::__detail::__is_path_src<_Lu8String>);
static_assert(std::filesystem::__detail::__is_path_src<_Lu16String>);
static_assert(std::filesystem::__detail::__is_path_src<_Lu32String>);

using _LPath = LSimplePath;

} /* ~Namespace Lal */

template<typename T, typename TCompare = std::less<T>, typename TAlloc = std::allocator<T>>
using TSet = std::set<T, TCompare, TAlloc>;

template<typename T, typename Alloc = std::allocator<T>>
using TArray = Lal::TSimpleArray<T, Alloc>;

using LString = Lal::_LString;
using LWString = Lal::_LWString;
using Lu8String = Lal::_Lu8String;
using Lu16String = Lal::_Lu16String;
using Lu32String = Lal::_Lu32String;

using LStringView = std::string_view;
using LWStringView = std::wstring_view;
using Lu8StringView = std::u8string_view;
using Lu16StringView = std::u16string_view;
using Lu32StringView = std::u32string_view;

using LPath = Lal::_LPath;

template<typename T>
using TOptional = std::optional<T>;

namespace Lal
{

FORCEINLINE LWString Utf8ToUtf16(LString const& Utf8) noexcept;
FORCEINLINE LWString Utf8ToUtf16(LString::value_type const* Ptr, LSize Size) noexcept;

FORCEINLINE LString Utf16ToUtf8(LWString const& Utf16) noexcept;
FORCEINLINE LString Utf16ToUtf8(LWString::value_type const* Ptr, LSize Size) noexcept;

FORCEINLINE LWString Utf8ToUtf16(LString const& Utf8) noexcept
{
    return Utf8ToUtf16(Utf8.data(), Utf8.size());
}

FORCEINLINE LWString Utf8ToUtf16(LString::value_type const* Ptr, LSize Size) noexcept
{
    if (Size == 0)
    {
        return {};
    }

    auto RequiredSize = MultiByteToWideChar(
        CP_UTF8,
        0,
        Ptr,
        Size,
        nullptr,
        0
        );

    LWString Utf16(RequiredSize, LITERAL_WIDE('\0'));

    MultiByteToWideChar(
        CP_UTF8,
        0,
        Ptr,
        Size,
        &Utf16[0],
        RequiredSize
        );

    return Utf16;
}

FORCEINLINE LString Utf16ToUtf8(LWString const& Utf16) noexcept
{
    return Utf16ToUtf8(Utf16.data(), Utf16.size());
}

FORCEINLINE LString Utf16ToUtf8(LWString::value_type const* Ptr, LSize Size) noexcept
{
    if (Size == 0)
    {
        return {};
    }

    auto RequiredSize = WideCharToMultiByte(
        CP_UTF8,
        0,
        Ptr,
        Size,
        nullptr,
        0,
        nullptr,
        nullptr
        );

    LString Utf8(RequiredSize, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        Ptr,
        Size,
        &Utf8[0],
        RequiredSize,
        nullptr,
        nullptr
        );

    return Utf8;
}

} /* ~Namespace Lal */

template<>
struct std::formatter<LString> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        LString const& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view{std::to_address(Path.begin()), Path.size()}, InContext);
    }
};

template<>
struct std::formatter<LPath> : std::formatter<std::string>
{
    FORCEINLINE auto format
    (
        const LPath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        const LString PathStr{ Path.string() };
        return std::formatter<std::string>::format(PathStr, InContext);
    }
};
