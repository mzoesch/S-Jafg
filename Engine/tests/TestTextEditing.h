// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/EditableTextButton.h"
#include "Rhi/Glyph.h"

namespace TestTextEditing
{

//# A fake shaped collection. Each entry is (cluster, pencil x).
NODISCARD inline Jafg::LGlyphCollection MakeCollection(std::initializer_list<std::pair<u32, f32>> Glyphs, f32 PencilEnd)
{
    Jafg::LGlyphCollection Result;
    for (auto [Cluster, X]: Glyphs)
    {
        Result.GlyphInfos.push_back({.Cluster = Cluster, .Pencil = LVec2F{X, 0.0f}});
    }
    Result.PencilEnd = LVec2F{PencilEnd, 0.0f};
    return Result;
}

} /* ~Namespace TestTextEditing */

TRIVIAL_TEST_CASE("text", CodePointNavigation)
{
    using namespace Jafg;

    /* 'a' (1 byte), U+00E9 (2 bytes), U+20AC (3 bytes), U+1F600 (4 bytes). */
    constexpr LStringView Text{"a\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80"};
    static_assert(Text.size() == 10);

    TEST_EQUAL(TextEditing::NextCodePoint(Text, 0), 1uz)
    TEST_EQUAL(TextEditing::NextCodePoint(Text, 1), 3uz)
    TEST_EQUAL(TextEditing::NextCodePoint(Text, 3), 6uz)
    TEST_EQUAL(TextEditing::NextCodePoint(Text, 6), 10uz)
    TEST_EQUAL(TextEditing::NextCodePoint(Text, 10), 10uz)

    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 10), 6uz)
    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 6), 3uz)
    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 3), 1uz)
    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 1), 0uz)
    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 0), 0uz)

    /* Indices inside of a code point snap back to its beginning. Out of bounds clamps. */
    TEST_EQUAL(TextEditing::SnapToCodePoint(Text, 2), 1uz)
    TEST_EQUAL(TextEditing::SnapToCodePoint(Text, 8), 6uz)
    TEST_EQUAL(TextEditing::SnapToCodePoint(Text, 3), 3uz)
    TEST_EQUAL(TextEditing::SnapToCodePoint(Text, 99), 10uz)
    TEST_EQUAL(TextEditing::PrevCodePoint(Text, 8), 6uz)
    TEST_EQUAL(TextEditing::NextCodePoint(Text, 8), 10uz)
}

TRIVIAL_TEST_CASE("text", WordNavigation)
{
    using namespace Jafg;

    constexpr LStringView Text{"hello  world.foo -12.5"};
    /*                          0123456789012345678901 */

    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, Text.size()), 21uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 21), 20uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 20), 18uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 18), 17uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 17), 13uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 13), 12uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 12), 7uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 7), 0uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 3), 0uz)
    TEST_EQUAL(TextEditing::PrevWordBoundary(Text, 0), 0uz)

#if JAFG_PLATFORM_WINDOWS
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 0), 7uz)
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 5), 7uz)
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 7), 12uz)
#else /* JAFG_PLATFORM_WINDOWS */
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 0), 5uz)
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 5), 12uz)
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 7), 12uz)
#endif /* !JAFG_PLATFORM_WINDOWS */
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, 12), 13uz)
    TEST_EQUAL(TextEditing::NextWordBoundary(Text, Text.size()), Text.size())

    /* Non ascii characters belong to words. */
    constexpr LStringView Umlauts{"f\xC3\xBC\xC3\x9F" " x"};
    TEST_EQUAL(TextEditing::PrevWordBoundary(Umlauts, 5), 0uz)
    TEST_EQUAL(TextEditing::WordAt(Umlauts, 1), (std::pair{0uz, 5uz}))

    TEST_EQUAL(TextEditing::WordAt(Text, 2), (std::pair{0uz, 5uz}))
    TEST_EQUAL(TextEditing::WordAt(Text, 5), (std::pair{5uz, 7uz}))
    TEST_EQUAL(TextEditing::WordAt(Text, Text.size()), (std::pair{21uz, 22uz}))
    TEST_EQUAL(TextEditing::WordAt(LStringView{}, 0), (std::pair{0uz, 0uz}))
}

TRIVIAL_TEST_CASE("text", SanitizeSingleLine)
{
    using namespace Jafg;

    TEST_TRUE(TextEditing::SanitizeSingleLine("12.5\n") == LStringView{"12.5"})
    TEST_TRUE(TextEditing::SanitizeSingleLine("12.5\r\n") == LStringView{"12.5"})
    TEST_TRUE(TextEditing::SanitizeSingleLine("a\r\nb\tc") == LStringView{"a b c"})
    TEST_TRUE(TextEditing::SanitizeSingleLine("a\x01" "b\x7F") == LStringView{"ab"})
    TEST_TRUE(TextEditing::SanitizeSingleLine("\xC3\xA9") == LStringView{"\xC3\xA9"})
}

TRIVIAL_TEST_CASE("text", ReplaceInvalidUtf8)
{
    using namespace Jafg;

    /* One U+FFFD per ill-formed byte. This matches how HarfBuzz renders it. */
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("a\x80" "b") == LStringView{"a" "\xEF\xBF\xBD" "b"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("a\xC3" "b") == LStringView{"a" "\xEF\xBF\xBD" "b"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("a\xE2\x82" "b") == LStringView{"a" "\xEF\xBF\xBD" "\xEF\xBF\xBD" "b"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("a\xC0\xAF\xFF" "b") == LStringView{"a" "\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD" "b"})
    /* Surrogates, overlong encodings, beyond U+10FFFF, and sequences truncated by the end of the text. */
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("\xED\xA0\x80") == LStringView{"\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("\xE0\x80\x80") == LStringView{"\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("\xF4\x90\x80\x80") == LStringView{"\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD"})
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8("\xF0\x9F\x98") == LStringView{"\xEF\xBF\xBD" "\xEF\xBF\xBD" "\xEF\xBF\xBD"})

    /* Well-formed text is untouched. */
    constexpr LStringView Valid{"a\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80\xED\x9F\xBF\xF4\x8F\xBF\xBF"};
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8(Valid) == Valid)
    TEST_TRUE(TextEditing::ReplaceInvalidUtf8(LStringView{}).empty())

    /* Pasting replaces first, so stripping a control character never joins stray bytes into a new character. */
    TEST_TRUE(TextEditing::SanitizeSingleLine("\xC3\x01\xA9") == LStringView{"\xEF\xBF\xBD" "\xEF\xBF\xBD"})
    TEST_TRUE(TextEditing::SanitizeSingleLine("1\xFF" "2\n") == LStringView{"1" "\xEF\xBF\xBD" "2"})
}

TRIVIAL_TEST_CASE("text", CaretStops)
{
    using namespace Jafg;
    using TestTextEditing::MakeCollection;
    constexpr f32 Epsilon{1.0e-4f};

    /* "afib" where "fi" is shaped as one ligature glyph. Clusters: a=0, fi=1, b=3. */
    {
        constexpr LStringView Text{"afib"};
        LGlyphCollection const Collection{MakeCollection({{0u, 0.0f}, {1u, 10.0f}, {3u, 30.0f}}, 40.0f)};

        TEST_LESS(maths::abs(Collection.caret_location(Text, 0) - 0.0f), Epsilon)
        TEST_LESS(maths::abs(Collection.caret_location(Text, 1) - 10.0f), Epsilon)
        /* Inside of the ligature. This panicked before. */
        TEST_LESS(maths::abs(Collection.caret_location(Text, 2) - 20.0f), Epsilon)
        TEST_LESS(maths::abs(Collection.caret_location(Text, 3) - 30.0f), Epsilon)
        TEST_LESS(maths::abs(Collection.caret_location(Text, 4) - 40.0f), Epsilon)

        TEST_EQUAL(Collection.caret_index(Text, -50.0f), 0uz)
        TEST_EQUAL(Collection.caret_index(Text, 4.0f), 0uz)
        TEST_EQUAL(Collection.caret_index(Text, 6.0f), 1uz)
        TEST_EQUAL(Collection.caret_index(Text, 19.0f), 2uz)
        TEST_EQUAL(Collection.caret_index(Text, 33.0f), 3uz)
        TEST_EQUAL(Collection.caret_index(Text, 500.0f), 4uz)
    }

    /* "aéb" where the second code point is two bytes long. */
    {
        constexpr LStringView Text{"a\xC3\xA9" "b"};
        LGlyphCollection const Collection{MakeCollection({{0u, 0.0f}, {1u, 10.0f}, {3u, 20.0f}}, 30.0f)};

        TEST_LESS(maths::abs(Collection.caret_location(Text, 1) - 10.0f), Epsilon)
        TEST_LESS(maths::abs(Collection.caret_location(Text, 3) - 20.0f), Epsilon)
        TEST_LESS(maths::abs(Collection.caret_location(Text, 4) - 30.0f), Epsilon)
        /* Never in the middle of a code point. */
        TEST_EQUAL(Collection.caret_index(Text, 14.0f), 1uz)
        TEST_EQUAL(Collection.caret_index(Text, 16.0f), 3uz)
    }

    /* A base glyph and a combining mark share a cluster. The caret must not stop in between. */
    {
        constexpr LStringView Text{"e\xCC\x81x"};
        LGlyphCollection const Collection{MakeCollection({{0u, 0.0f}, {0u, 10.0f}, {3u, 10.0f}}, 20.0f)};

        TEST_LESS(maths::abs(Collection.caret_location(Text, 3) - 10.0f), Epsilon)
        TEST_EQUAL(Collection.caret_index(Text, 9.0f), 3uz)
    }

    /* Empty. */
    {
        LGlyphCollection const Collection{MakeCollection({}, 0.0f)};
        TEST_LESS(maths::abs(Collection.caret_location(LStringView{}, 0) - 0.0f), Epsilon)
        TEST_EQUAL(Collection.caret_index(LStringView{}, 10.0f), 0uz)
    }
}
