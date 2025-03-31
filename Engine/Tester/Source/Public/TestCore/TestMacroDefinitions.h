// Copyright mzoesch. All rights reserved.

#pragma once

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
    #undef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
#endif /* PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST */
#define PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(ClassName, PrettyName, Category, FileName, LineNumber) \
    struct ClassName : public ::Jafg::Tester::LSimpleTestCase                                     \
    {                                                                                             \
        ClassName() : LSimpleTestCase(PrettyName, Category, FileName) { }                         \
        virtual void Run() override;                                                              \
    };                                                                                            \
    ClassName PRIVATE_JAFG_TEST_JOIN_INNER_TWO(ClassName, Instance);                              \
    void ClassName::Run()                                                                         \

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_EQUALS
    #undef CHECK_EQUALS
#endif /* CHECK_EQUALS */
#define CHECK_EQUALS(What, A, B)                 \
    CHECK_EQUALS_WITH_LINE(What, A, B, __LINE__)
#define CHECK_EQUALS_WITH_LINE(What, A, B, Line)                              \
    this->TestEqual("Equality test ["#What"]: { "#A" == "#B" }", A, B, Line);
#ifdef QUICK_CHECK_EQUALS
    #undef QUICK_CHECK_EQUALS
#endif /* QUICK_CHECK_EQUALS */
#define QUICK_CHECK_EQUALS(A, B)                 \
    QUICK_CHECK_EQUALS_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_EQUALS_WITH_LINE(A, B, Line)                    \
    this->TestEqual("Equality test: { "#A" == "#B" }", A, B, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_NOT_EQUALS
    #undef CHECK_NOT_EQUALS
#endif /* CHECK_NOT_EQUALS */
#define CHECK_NOT_EQUALS(What, A, B)                 \
    CHECK_NOT_EQUALS_WITH_LINE(What, A, B, __LINE__)
#define CHECK_NOT_EQUALS_WITH_LINE(What, A, B, Line)                                 \
    this->TestNotEqual("Not equality test ["#What"]: { "#A" != "#B" }", A, B, Line);
#ifdef QUICK_CHECK_NOT_EQUALS
    #undef QUICK_CHECK_NOT_EQUALS
#endif /* QUICK_CHECK_NOT_EQUALS */
#define QUICK_CHECK_NOT_EQUALS(A, B)                 \
    QUICK_CHECK_NOT_EQUALS_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_NOT_EQUALS_WITH_LINE(A, B, Line)                    \
    this->TestNotEqual("Not equality test: { "#A" != "#B" }", A, B, Line);

//#* @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_LESS
    #undef CHECK_LESS
#endif /* CHECK_LESS */
#define CHECK_LESS(What, A, B)                 \
    CHECK_LESS_WITH_LINE(What, A, B, __LINE__)
#define CHECK_LESS_WITH_LINE(What, A, B, Line)                          \
    this->TestLess("Less test ["#What"]: { "#A" < "#B" }", A, B, Line);
#ifdef QUICK_CHECK_LESS
    #undef QUICK_CHECK_LESS
#endif /* QUICK_CHECK_LESS */
#define QUICK_CHECK_LESS(A, B)                 \
    QUICK_CHECK_LESS_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_LESS_WITH_LINE(A, B, Line)                \
    this->TestLess("Less test: { "#A" < "#B" }", A, B, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_GREATER
    #undef CHECK_GREATER
#endif /* CHECK_GREATER */
#define CHECK_GREATER(What, A, B)                 \
    CHECK_GREATER_WITH_LINE(What, A, B, __LINE__)
#define CHECK_GREATER_WITH_LINE(What, A, B, Line)                             \
    this->TestGreater("Greater test ["#What"]: { "#A" > "#B" }", A, B, Line);
#ifdef QUICK_CHECK_GREATER
    #undef QUICK_CHECK_GREATER
#endif /* QUICK_CHECK_GREATER */
#define QUICK_CHECK_GREATER(A, B)                 \
    QUICK_CHECK_GREATER_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_GREATER_WITH_LINE(A, B, Line)                   \
    this->TestGreater("Greater test: { "#A" > "#B" }", A, B, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_LESS_OR_EQUAL
    #undef CHECK_LESS_OR_EQUAL
#endif /* CHECK_LESS_OR_EQUAL */
#define CHECK_LESS_OR_EQUAL(What, A, B)                 \
    CHECK_LESS_OR_EQUAL_WITH_LINE(What, A, B, __LINE__)
#define CHECK_LESS_OR_EQUAL_WITH_LINE(What, A, B, Line)                                  \
    this->TestLessOrEqual("Less or equal test ["#What"]: { "#A" <= "#B" }", A, B, Line);
#ifdef QUICK_CHECK_LESS_OR_EQUAL
    #undef QUICK_CHECK_LESS_OR_EQUAL
#endif /* QUICK_CHECK_LESS_OR_EQUAL */
#define QUICK_CHECK_LESS_OR_EQUAL(A, B)                 \
    QUICK_CHECK_LESS_OR_EQUAL_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_LESS_OR_EQUAL_WITH_LINE(A, B, Line)                        \
    this->TestLessOrEqual("Less or equal test: { "#A" <= "#B" }", A, B, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_GREATER_OR_EQUAL
    #undef CHECK_GREATER_OR_EQUAL
#endif /* CHECK_GREATER_OR_EQUAL */
#define CHECK_GREATER_OR_EQUAL(What, A, B)                 \
    CHECK_GREATER_OR_EQUAL_WITH_LINE(What, A, B, __LINE__)
#define CHECK_GREATER_OR_EQUAL_WITH_LINE(What, A, B, Line)                                     \
    this->TestGreaterOrEqual("Greater or equal test ["#What"]: { "#A" >= "#B" }", A, B, Line);
#ifdef QUICK_CHECK_GREATER_OR_EQUAL
    #undef QUICK_CHECK_GREATER_OR_EQUAL
#endif /* CHECK_GREATER_OR_EQUAL */
#define QUICK_CHECK_GREATER_OR_EQUAL(A, B)                 \
    QUICK_CHECK_GREATER_OR_EQUAL_WITH_LINE(A, B, __LINE__)
#define QUICK_CHECK_GREATER_OR_EQUAL_WITH_LINE(A, B, Line)                           \
    this->TestGreaterOrEqual("Greater or equal test: { "#A" >= "#B" }", A, B, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_NULL
    #undef CHECK_NULL
#endif /* CHECK_NULL */
#define CHECK_NULL(What, A)                 \
    CHECK_NULL_WITH_LINE(What, A, __LINE__)
#define CHECK_NULL_WITH_LINE(What, A, Line)                              \
    this->TestNull("Null test ["#What"]: { "#A" == nullptr }", A, Line);
#ifdef QUICK_CHECK_NULL
    #undef QUICK_CHECK_NULL
#endif /* QUICK_CHECK_NULL */
#define QUICK_CHECK_NULL(A)                 \
    QUICK_CHECK_NULL_WITH_LINE(A, __LINE__)
#define QUICK_CHECK_NULL_WITH_LINE(A, Line)                    \
    this->TestNull("Null test: { "#A" == nullptr }", A, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_NOT_NULL
    #undef CHECK_NOT_NULL
#endif /* CHECK_NOT_NULL */
#define CHECK_NOT_NULL(What, A)                 \
    CHECK_NOT_NULL_WITH_LINE(What, A, __LINE__)
#define CHECK_NOT_NULL_WITH_LINE(What, A, Line)                                 \
    this->TestNotNull("Not null test ["#What"]: { "#A" != nullptr }", A, Line);
#ifdef QUICK_CHECK_NOT_NULL
    #undef QUICK_CHECK_NOT_NULL
#endif /* QUICK_CHECK_NOT_NULL */
#define QUICK_CHECK_NOT_NULL(A)                 \
    QUICK_CHECK_NOT_NULL_WITH_LINE(A, __LINE__)
#define QUICK_CHECK_NOT_NULL_WITH_LINE(A, Line)                       \
    this->TestNotNull("Not null test: { "#A" != nullptr }", A, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_TRUE
    #undef CHECK_TRUE
#endif /* CHECK_TRUE */
#define CHECK_TRUE(What, A)                 \
    CHECK_TRUE_WITH_LINE(What, A, __LINE__)
#define CHECK_TRUE_WITH_LINE(What, A, Line)                           \
    this->TestTrue("True test ["#What"]: { "#A" == true }", A, Line);
#ifdef QUICK_CHECK_TRUE
    #undef QUICK_CHECK_TRUE
#endif /* QUICK_CHECK_TRUE */
#define QUICK_CHECK_TRUE(A)                 \
    QUICK_CHECK_TRUE_WITH_LINE(A, __LINE__)
#define QUICK_CHECK_TRUE_WITH_LINE(A, Line)                 \
    this->TestTrue("True test: { "#A" == true }", A, Line);

//# @see Engine/TestForward/TestCore/TestMacroForward.h
#ifdef CHECK_FALSE
    #undef CHECK_FALSE
#endif /* CHECK_FALSE */
#define CHECK_FALSE(What, A)                 \
    CHECK_FALSE_WITH_LINE(What, A, __LINE__)
#define CHECK_FALSE_WITH_LINE(What, A, Line)                             \
    this->TestFalse("False test ["#What"]: { "#A" == false }", A, Line);
#ifdef QUICK_CHECK_FALSE
    #undef QUICK_CHECK_FALSE
#endif /* QUICK_CHECK_FALSE */
#define QUICK_CHECK_FALSE(A)                 \
    QUICK_CHECK_FALSE_WITH_LINE(A, __LINE__)
#define QUICK_CHECK_FALSE_WITH_LINE(A, Line)                   \
    this->TestFalse("False test: { "#A" == false }", A, Line);
