// Copyright mzoesch. All rights reserved.

#pragma once

DECLARE_TRAIT(String)
    IMPLEMENT_TRAIT(String, std::string)
    IMPLEMENT_TRAIT(String, std::wstring)
    IMPLEMENT_TRAIT(String, std::u8string)
    IMPLEMENT_TRAIT(String, std::u16string)
    IMPLEMENT_TRAIT(String, std::u32string)
    IMPLEMENT_TRAIT(String, std::filesystem::path)
    IMPLEMENT_TRAIT(String, Lal::Private::LStringBase)
    IMPLEMENT_TRAIT(String, LString)
    IMPLEMENT_TRAIT(String, LStringView)
    IMPLEMENT_TRAIT(String, LMutableStringView)
