// Copyright mzoesch. All rights reserved.

#pragma once

/**
 * Create a simple unit test case to test small pieces of code.
 * A very fast test as no setup or teardown is performed.
 * No engine will be created.
 *
 * Tests are automatically discovered and run by the Jafg Build Tool.
 * Run the solution with any test configuration to run all tests.
 *
 * @param PrettyName A human-readable name for the test case.
 * @param Category   A category for the test case. A dot separates subcategories.
 */
#define TEST_CASE(PrettyName, Category)              \
    PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(              \
        PRIVATE_JAFG_TEST_MAKE_UNIQUE_NAME_FOR_FILE( \
            PrettyName,                              \
            __LINE__,                                \
            __COUNTER__                              \
        ),                                           \
        #PrettyName,                                 \
        Category,                                    \
        __FILE__,                                    \
        __LINE__                                     \
    )

/**
 * Checks if A and B are equal.
 *
 * @param What Short description of the check.
 * @param A    Left-hand side of the comparison.
 * @param B    Right-hand side of the comparison.
 */
#define CHECK_EQUALS(What, A, B)                                          \
    /* Just doing some simple local variable initialization to get the */ \
    /* syntax highlighting and intellisense working inside an IDEA.    */ \
    {                                                                     \
        auto What_ = ( What );                                            \
        auto A_ = ( A );                                                  \
        auto B_ = ( B );                                                  \
    }

#define PRIVATE_JAFG_TEST_JOIN_INNER_TWO(   A, B                        ) A##B
#define PRIVATE_JAFG_TEST_JOIN_INNER_THREE( A, B, C                     ) A##B##C
#define PRIVATE_JAFG_TEST_JOIN_INNER_FOUR(  A, B, C, D                  ) A##B##C##D
#define PRIVATE_JAFG_TEST_JOIN_INNER_FIVE(  A, B, C, D, E               ) A##B##C##D##E
#define PRIVATE_JAFG_TEST_JOIN_INNER_SIX(   A, B, C, D, E, F            ) A##B##C##D##E##F
#define PRIVATE_JAFG_TEST_JOIN_INNER_SEVEN( A, B, C, D, E, F, G         ) A##B##C##D##E##F##G
#define PRIVATE_JAFG_TEST_JOIN_INNER_EIGHT( A, B, C, D, E, F, G, H      ) A##B##C##D##E##F##G##H
#define PRIVATE_JAFG_TEST_JOIN_INNER_NINE(  A, B, C, D, E, F, G, H, I   ) A##B##C##D##E##F##G##H##I

#define PRIVATE_JAFG_TEST_MAKE_UNIQUE_NAME_FOR_FILE(Prefix, Line, Counter) \
    PRIVATE_JAFG_TEST_JOIN_INNER_FIVE(Prefix, _, Line, _, Counter)

/** Forward simple test macro to expand to an unused static (in translation private scope) function. */
#define PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(ClassName, PrettyName, Category, FileName, LineNumber ) \
    static void PRIVATE_JAFG_TEST_JOIN_INNER_TWO(DiscardMe, ClassName)( void )
