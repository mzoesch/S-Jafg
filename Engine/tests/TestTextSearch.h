// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/TextSearch.h"

TRIVIAL_TEST_CASE("text", SearchTokenize)
{
    using namespace Jafg;

    TEST_TRUE(TextSearch::ToLowerAscii("MetalPlate_8M") == "metalplate_8m")
    auto const Tokens{TextSearch::Tokenize("  Metal \t Plate ")};
    TEST_EQUAL(Tokens.size(), 2uz)
    if (Tokens.size() == 2uz)
    {
        TEST_TRUE(Tokens[0] == "metal")
        TEST_TRUE(Tokens[1] == "plate")
    }
    TEST_TRUE(TextSearch::Tokenize("   ").empty())
}

TRIVIAL_TEST_CASE("text", SearchFuzzyMatch)
{
    using namespace Jafg;

    /* Characters in order, not necessarily next to each other, case-insensitive. */
    TEST_TRUE(TextSearch::FuzzyScore("MetalPlate", "mt").has_value())
    TEST_TRUE(TextSearch::FuzzyScore("MetalPlate", "mplt").has_value())
    TEST_TRUE(TextSearch::FuzzyScore("MetalPlate", "metalplate").has_value())
    TEST_TRUE(TextSearch::FuzzyScore("JafgGameplayCore/Materials/MetalPlate", "jgc/metal").has_value())
    TEST_FALSE(TextSearch::FuzzyScore("MetalPlate", "tm").has_value())
    TEST_FALSE(TextSearch::FuzzyScore("MetalPlate", "xyz").has_value())
    TEST_FALSE(TextSearch::FuzzyScore("Mt", "mtx").has_value())
    TEST_EQUAL(TextSearch::FuzzyScore("MetalPlate", "").value_or(-1), 0)
}

TRIVIAL_TEST_CASE("text", SearchFuzzyRanking)
{
    using namespace Jafg;

    auto const Score{[](LStringView Text, LStringView Token) { return TextSearch::FuzzyScore(Text, Token).value_or(-1); }};

    /* Word starts and camel case humps beat characters in the middle of words. */
    TEST_GREATER(Score("MetalPlate", "mp"), Score("Simple", "mp"))
    TEST_GREATER(Score("Materials/Pbr", "p"), Score("Ample", "p"))
    /* Consecutive characters beat scattered ones. */
    TEST_GREATER(Score("metal", "met"), Score("mxexxt", "met"))
    /* The best alignment counts, not the first one. "m" of "Materials" would leave a long gap to "pl". */
    TEST_GREATER(Score("Materials/MetalPlate", "mpl"), Score("Materials/Xpl", "mpl"))
}
