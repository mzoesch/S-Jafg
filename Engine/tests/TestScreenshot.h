// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ScreenshotSubsystem.h"

TRIVIAL_TEST_CASE("screenshots", ScreenshotFormatName)
{
    using namespace Jafg;

    std::tm Time{};
    Time.tm_year = 2026 - 1900;
    Time.tm_mon = 10;
    Time.tm_mday = 25;
    Time.tm_hour = 18;
    Time.tm_min = 7;
    Time.tm_sec = 5;

    TEST_TRUE(Screenshots::FormatName("%Y-%m-%d-%H-%M", Time) == "2026-11-25-18-07")
    TEST_TRUE(Screenshots::FormatName("Shot %H.%M.%S", Time) == "Shot 18.07.05")
    /* Unknown placeholders and a trailing percent sign stay. */
    TEST_TRUE(Screenshots::FormatName("100%% %q %", Time) == "100% %q %")
    TEST_TRUE(Screenshots::FormatName("a/b:c*?", Time) == "a_b_c__")
    TEST_TRUE(Screenshots::FormatName("  name. ", Time) == "name")
    TEST_TRUE(Screenshots::FormatName("", Time).empty())
    TEST_TRUE(Screenshots::FormatName(" . ", Time).empty())
}

TRIVIAL_TEST_CASE("screenshots", ScreenshotLimitedExtent)
{
    using namespace Jafg;

    auto const Is{[](rhi::extent2 Extent, u32 Width, u32 Height) { return Extent.width == Width && Extent.height == Height; }};

    /* No limit. */
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({3840, 2160}, 0u), 3840u, 2160u))
    /* Taller: Scaled down with the aspect ratio. */
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({3840, 2160}, 1080u), 1920u, 1080u))
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({2560, 1440}, 1080u), 1920u, 1080u))
    /* 1920 * 720 / 900 = 1536 */
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({1920, 900}, 720u), 1536u, 720u))
    /* 1600 * 1080 / 1201 = 1438.80 */
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({1600, 1201}, 1080u), 1439u, 1080u))
    /* As tall or smaller: Never scaled up. */
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({1920, 1080}, 1080u), 1920u, 1080u))
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({1280, 720}, 1080u), 1280u, 720u))
    TEST_TRUE(Is(Screenshots::GetLimitedExtent({0, 0}, 1080u), 0u, 0u))
}

TRIVIAL_TEST_CASE("screenshots", ScreenshotUniqueFile)
{
    using namespace Jafg;

    const LPath Directory{std::filesystem::temp_directory_path()
        / algo::sprintf("JafgScreenshotTest-{}", std::chrono::steady_clock::now().time_since_epoch().count())};
    constexpr u8 Bytes[]{1, 2, 3};

    const auto A{Screenshots::WriteUniqueFile(Directory, "Shot", ".png", Bytes)};
    const auto B{Screenshots::WriteUniqueFile(Directory, "Shot", ".png", Bytes)};
    const auto C{Screenshots::WriteUniqueFile(Directory, "Shot", ".png", Bytes)};
    TEST_TRUE(A.has_value() && A->filename() == "Shot.png")
    TEST_TRUE(B.has_value() && B->filename() == "Shot_1.png")
    TEST_TRUE(C.has_value() && C->filename() == "Shot_2.png")
    if (A)
    {
        std::error_code Error;
        TEST_TRUE(std::filesystem::file_size(*A, Error) == std::size(Bytes))
    }

    std::error_code Error;
    std::filesystem::remove_all(Directory, Error);
}
