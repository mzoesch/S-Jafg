// Copyright mzoesch. All rights reserved.

#pragma once

#if TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS

TEST_CASE(TraitsCore, "Lal.Traits")
{
    CHECK_STATIC(Trait::CString<std::string>)
    CHECK_STATIC(Trait::CString<std::string&>)
    CHECK_STATIC(Trait::CString<std::string&&>)
    CHECK_STATIC(Trait::CString<std::wstring>)
    CHECK_STATIC(Trait::CString<std::wstring&>)
    CHECK_STATIC(Trait::CString<std::wstring&&>)
    CHECK_STATIC(Trait::CString<std::u8string>)
    CHECK_STATIC(Trait::CString<std::u8string&>)
    CHECK_STATIC(Trait::CString<std::u8string&&>)
    CHECK_STATIC(Trait::CString<std::u16string>)
    CHECK_STATIC(Trait::CString<std::u16string&>)
    CHECK_STATIC(Trait::CString<std::u16string&&>)
    CHECK_STATIC(Trait::CString<std::u32string>)
    CHECK_STATIC(Trait::CString<std::u32string&>)
    CHECK_STATIC(Trait::CString<std::u32string&&>)
    CHECK_STATIC(Trait::CString<std::filesystem::path>)
    CHECK_STATIC(Trait::CString<std::filesystem::path&>)
    CHECK_STATIC(Trait::CString<std::filesystem::path&&>)
    CHECK_STATIC(Trait::CString<LString>)
    CHECK_STATIC(Trait::CString<LString&>)
    CHECK_STATIC(Trait::CString<LString&&>)
    CHECK_STATIC(Trait::CString<LPath>)
    CHECK_STATIC(Trait::CString<LPath&>)
    CHECK_STATIC(Trait::CString<LPath&&>)

    return;
}

#endif /* TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS */
