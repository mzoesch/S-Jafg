// Copyright mzoesch. All rights reserved.

#pragma once

DECLARE_TRAIT(String)
    IMPLEMENT_TRAIT(String, std::string)
    IMPLEMENT_TRAIT(String, std::wstring)
    IMPLEMENT_TRAIT(String, std::u8string)
    IMPLEMENT_TRAIT(String, std::u16string)
    IMPLEMENT_TRAIT(String, std::u32string)
    IMPLEMENT_TRAIT(String, std::filesystem::path)
    IMPLEMENT_TRAIT(String, std::string_view)
    IMPLEMENT_TRAIT(String, Lal::_LString)
    IMPLEMENT_TRAIT(String, Lal::_LWString)
    IMPLEMENT_TRAIT(String, Lal::_Lu8String)
    IMPLEMENT_TRAIT(String, Lal::_Lu16String)
    IMPLEMENT_TRAIT(String, Lal::_Lu32String)
    IMPLEMENT_TRAIT(String, Lal::_LPath)
#if LAL_WITH_LEGACY_LAL_STRING
    IMPLEMENT_TRAIT(String, LString)
    IMPLEMENT_TRAIT(String, Lal::Private::LStringBase)
    IMPLEMENT_TRAIT(String, LStringView)
    IMPLEMENT_TRAIT(String, LMutableStringView)
#endif /* LAL_WITH_LEGACY_LAL_STRING */

static_assert(std::is_same_v<std::string, Lal::_LString> == false);
static_assert(std::is_same_v<std::wstring, Lal::_LWString> == false);
static_assert(std::is_same_v<std::u8string, Lal::_Lu8String> == false);
static_assert(std::is_same_v<std::u16string, Lal::_Lu16String> == false);
static_assert(std::is_same_v<std::u32string, Lal::_Lu32String> == false);
static_assert(std::is_same_v<std::filesystem::path, Lal::_LPath> == false);
