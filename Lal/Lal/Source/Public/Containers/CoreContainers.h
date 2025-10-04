// Copyright mzoesch. All rights reserved.

#pragma once

template <typename T>
using TArray = std::vector<T>;

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

template <typename T>
using TOptional = std::optional<T>;

template <>
struct std::formatter<LPath> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const LPath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view{std::to_address(Path.native().begin()), Path.native().size()}, InContext);
    }
};
