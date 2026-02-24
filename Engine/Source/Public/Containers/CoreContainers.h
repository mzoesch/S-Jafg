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

    NODISCARD FORCEINLINE constexpr auto&& reflexive_emplace_back(this auto&& self, auto&&... args)
    {
        self.emplace_back(std::forward<decltype(args)>(args)...);
        return std::forward<decltype(self)>(self);
    }
};

/* TODO: C++26 introduces a better option for this. So remove this class then. Just temp. */
template<typename _Tp, std::size_t _Nm> requires(std::is_default_constructible_v<_Tp>)
class TMySimpleFixedArray
{
public:

    inline void push_back(_Tp const& Value) noexcept
    {
        JAFG_FWD_CHECK([_Size = this->Size]{ return _Size < _Nm; })
        this->Data[this->Size++] = Value;
    }

    inline void push_back(_Tp&& Value) noexcept
    {
        JAFG_FWD_CHECK([_Size = this->Size]{ return _Size < _Nm; })
        this->Data[this->Size++] = std::move(Value);
    }

    inline bool empty() noexcept
    {
        return this->Size == 0;
    }

    inline void clear() noexcept
    {
        for (LSize Idx{ 0 }; Idx < this->Size; ++Idx)
        {
            this->Data[Idx] = _Tp{};
            continue;
        }

        this->Size = 0;

        return;
    }

    [[nodiscard]] inline _Tp* data() noexcept
    {
        return this->Data.data();
    }

    [[nodiscard]] inline _Tp const* data() const noexcept
    {
        return this->Data.data();
    }

    [[nodiscard]] inline _Tp* begin() noexcept
    {
        return this->Data.data();
    }

    [[nodiscard]] inline _Tp const* begin() const noexcept
    {
        return this->Data.data();
    }

    [[nodiscard]] inline _Tp* end() noexcept
    {
        return this->Data.data() + this->Size;
    }

    [[nodiscard]] inline _Tp const* end() const noexcept
    {
        return this->Data.data() + this->Size;
    }

    [[nodiscard]] inline _Tp& operator[](LSize Index) noexcept
    {
        JAFG_FWD_CHECK([_Size = this->Size, Index]{ return Index < _Size; })
        return this->Data[Index];
    }

    [[nodiscard]] inline _Tp const& operator[](LSize Index) const noexcept
    {
        JAFG_FWD_CHECK([_Size = this->Size, Index]{ return Index < _Size; })
        return this->Data[Index];
    }

    [[nodiscard]] inline LSize size() const noexcept
    {
        return this->Size;
    }

    std::array<_Tp, _Nm> Data;
    LSize Size{ 0 };
};

} /* ~Namespace Jafg */

template<typename T, typename TCompare = std::less<T>, typename TAlloc = std::allocator<T>>
using TSet = std::set<T, TCompare, TAlloc>;

template<typename T, typename Alloc = std::allocator<T>>
using TArray = Jafg::TSimpleArray<T, Alloc>;

template<typename _Tp, std::size_t _Nm>
using TStackArray = Jafg::TMySimpleFixedArray<_Tp, _Nm>;

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

using LPath = std::filesystem::path;

template<typename T>
using TOptional = std::optional<T>;

template<>
struct std::formatter<LPath> : std::formatter<std::string>
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
