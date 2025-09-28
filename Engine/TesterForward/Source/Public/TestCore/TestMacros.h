// Copyright mzoesch. All rights reserved.

#pragma once

#if !WITH_TESTS
    #error "Test header file included, but tests are not active."
#endif /* !WITH_TESTS */

//#
//# Create a simple unit test case to test small pieces of code.
//# A very fast test as no setup or teardown is performed.
//# No engine will be created.
//#
//# Tests are automatically discovered and run by the Jafg Build Tool.
//# Run the solution with any test configuration to run all tests.
//#
//# @param PrettyName A human-readable name for the test case.
//# @param Category   A category for the test case. A dot separates subcategories.
//#
#define TEST_CASE(PrettyName, Category)                                 TEST_CASE_IMPL(PrettyName, Category)

//#
//# Check something that is statically evaluated.
//#
#define CHECK_STATIC(Condition)                                         CHECK_STATIC_WITH_LINE(Condition, __LINE__)

//#
//# Checks if A and B are equal.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_EQUALS(What, A, B, ...)                                   CHECK_EQUALS_WITH_LINE(What, A, B, __LINE__, ##__VA_ARGS__)
//# Equality test where the equation (A == B) is the description "What".
#define QUICK_CHECK_EQUALS(A, B, ...)                                   QUICK_CHECK_EQUALS_WITH_LINE(A, B, __LINE__, ##__VA_ARGS__)

//#
//# Checks if A and B are not equal.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_NOT_EQUALS(What, A, B, ...)                               CHECK_NOT_EQUALS_WITH_LINE(What, A, B, __LINE__, ##__VA_ARGS__)
//# Equality test where the equation (A != B) is the description "What".
#define QUICK_CHECK_NOT_EQUALS(A, B, ...)                               QUICK_CHECK_NOT_EQUALS_WITH_LINE(A, B, __LINE__, ##__VA_ARGS__)


//#
//# Checks if A is less than B.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_LESS(What, A, B)                                          CHECK_LESS_WITH_LINE(What, A, B, __LINE__)
//# Equality test where the equation (A < B) is the description "What".
#define QUICK_CHECK_LESS(A, B)                                          QUICK_CHECK_LESS_WITH_LINE(A, B, __LINE__)

//#
//# Checks if A is greater than B.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_GREATER(What, A, B)                                       CHECK_GREATER_WITH_LINE(What, A, B, __LINE__)
//# Equality test where the equation (A > B) is the description "What".
#define QUICK_CHECK_GREATER(A, B)                                       QUICK_CHECK_GREATER_WITH_LINE(A, B, __LINE__)

//#
//# Checks if A is less than or equal to B.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_LESS_OR_EQUAL(What, A, B)                                 CHECK_LESS_OR_EQUAL_WITH_LINE(What, A, B, __LINE__)
//# Equality test where the equation (A <= B) is the description "What".
#define QUICK_CHECK_LESS_OR_EQUAL(A, B)                                 QUICK_CHECK_LESS_OR_EQUAL_WITH_LINE(A, B, __LINE__)

//#
//# Checks if A is greater than or equal to B.
//#
//# @param What Short description of the check.
//# @param A    Left-hand side of the comparison.
//# @param B    Right-hand side of the comparison.
//#
#define CHECK_GREATER_OR_EQUAL(What, A, B)                              CHECK_GREATER_OR_EQUAL_WITH_LINE(What, A, B, __LINE__)
//# Equality test where the equation (A >= B) is the description "What".
#define QUICK_CHECK_GREATER_OR_EQUAL(A, B)                              QUICK_CHECK_GREATER_OR_EQUAL_WITH_LINE(A, B, __LINE__)

//#
//# Checks if A is null.
//#
//# @param What Short description of the check.
//# @param A    The pointer to check.
//#
#define CHECK_NULL(What, A)                                             CHECK_NULL_WITH_LINE(What, A, __LINE__)
//# Equality test where the equation (A == nullptr) is the description "What".
#define QUICK_CHECK_NULL(A)                                             QUICK_CHECK_NULL_WITH_LINE(A, __LINE__)

//#
//# Checks if A is not null.
//#
//# @param What Short description of the check.
//# @param A    The pointer to check.
//#
#define CHECK_NOT_NULL(What, A)                                         CHECK_NOT_NULL_WITH_LINE(What, A, __LINE__)
//# Equality test where the equation (A != nullptr) is the description "What".
#define QUICK_CHECK_NOT_NULL(A)                                         QUICK_CHECK_NOT_NULL_WITH_LINE(A,__LINE__)

//#
//# Checks if A is true.
//#
//# @param What Short description of the check.
//# @param A    The condition to check.
//#
#define CHECK_TRUE(What, A)                                             CHECK_TRUE_WITH_LINE(What, A, __LINE__)
//# Equality test where the equation (A == true) is the description "What".
#define QUICK_CHECK_TRUE(A)                                             QUICK_CHECK_TRUE_WITH_LINE(A, __LINE__)

//#
//# Checks if A is false.
//#
//# @param What Short description of the check.
//# @param A    The condition to check.
//#
#define CHECK_FALSE(What, A)                                            CHECK_FALSE_WITH_LINE(What, A, __LINE__)
//# Equality test where the equation (A == false) is the description "What".
#define QUICK_CHECK_FALSE(A)                                            QUICK_CHECK_FALSE_WITH_LINE(A, __LINE__)


///////////////////////////////////////////////////////////////////////////////
// Private
///////////////////////////////////////////////////////////////////////////////

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

#define PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(ClassName, PrettyName, Category, FileName, LineNumber) \
    struct ClassName : public ::Jafg::Tester::LSimpleTestCase                                     \
    {                                                                                             \
        ClassName() : LSimpleTestCase(PrettyName, Category, FileName) { }                         \
        virtual void Run() override;                                                              \
    };                                                                                            \
    ClassName PRIVATE_JAFG_TEST_JOIN_INNER_TWO(ClassName, Instance);                              \
    void ClassName::Run()                                                                         \

#define TEST_CASE_IMPL(PrettyName, Category)         \
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

#define CHECK_STATIC_WITH_LINE(Condition, Line) \
    static_assert(Condition);                   \
    this->IncreaseTestCount();

#define CHECK_EQUALS_WITH_LINE(What, A, B, Line, ...) \
    this->TestEqual(#What": { "#A" == "#B" }", A, B, Line, ##__VA_ARGS__);
#define QUICK_CHECK_EQUALS_WITH_LINE(A, B, Line, ...) \
    this->TestEqual("Equal test: { "#A" == "#B" }", A, B, Line, ##__VA_ARGS__);

#define CHECK_NOT_EQUALS_WITH_LINE(What, A, B, Line, ...) \
    this->TestNotEqual(#What": { "#A" != "#B" }", A, B, Line, ##__VA_ARGS__);
#define QUICK_CHECK_NOT_EQUALS_WITH_LINE(A, B, Line, ...) \
    this->TestNotEqual("Not equal test: { "#A" != "#B" }", A, B, Line, ##__VA_ARGS__);

#define CHECK_LESS_WITH_LINE(What, A, B, Line) \
    this->TestLess(#What": { "#A" < "#B" }", A, B, Line);
#define QUICK_CHECK_LESS_WITH_LINE(A, B, Line) \
    this->TestLess("Less test: { "#A" < "#B" }", A, B, Line);

#define CHECK_GREATER_WITH_LINE(What, A, B, Line) \
    this->TestGreater(#What": { "#A" > "#B" }", A, B, Line);
#define QUICK_CHECK_GREATER_WITH_LINE(A, B, Line) \
    this->TestGreater("Greater test: { "#A" > "#B" }", A, B, Line);

#define CHECK_LESS_OR_EQUAL_WITH_LINE(What, A, B, Line) \
    this->TestLessOrEqual(#What": { "#A" <= "#B" }", A, B, Line);
#define QUICK_CHECK_LESS_OR_EQUAL_WITH_LINE(A, B, Line) \
    this->TestLessOrEqual("Less or equal test: { "#A" <= "#B" }", A, B, Line);

#define CHECK_GREATER_OR_EQUAL_WITH_LINE(What, A, B, Line) \
    this->TestGreaterOrEqual(#What": { "#A" >= "#B" }", A, B, Line);
#define QUICK_CHECK_GREATER_OR_EQUAL_WITH_LINE(A, B, Line) \
    this->TestGreaterOrEqual("Greater or equal test: { "#A" >= "#B" }", A, B, Line);

#define CHECK_NULL_WITH_LINE(What, A, Line) \
    this->TestNull(#What": { "#A" == nullptr }", A, Line);
#define QUICK_CHECK_NULL_WITH_LINE(A, Line) \
    this->TestNull("Null test: { "#A" == nullptr }", A, Line);

#define CHECK_NOT_NULL_WITH_LINE(What, A, Line) \
    this->TestNotNull(#What": { "#A" != nullptr }", A, Line);
#define QUICK_CHECK_NOT_NULL_WITH_LINE(A, B, Line) \
    this->TestNotNull("Not null test: { "#A" != nullptr }", A, B, Line);

#define CHECK_TRUE_WITH_LINE(What, A, Line) \
    this->TestTrue(#What": { "#A" == true }", A, Line);
#define QUICK_CHECK_TRUE_WITH_LINE(A, Line) \
    this->TestTrue("Boolean test: { "#A" == true }", A, Line);

#define CHECK_FALSE_WITH_LINE(What, A, Line) \
    this->TestFalse(#What": { "#A" == false }", A, Line);
#define QUICK_CHECK_FALSE_WITH_LINE(A, Line) \
    this->TestFalse("Boolean test: { "#A" == false }", A, Line);
