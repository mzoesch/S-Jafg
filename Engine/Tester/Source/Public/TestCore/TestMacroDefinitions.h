// Copyright mzoesch. All rights reserved.

#pragma once

/** @see Engine/TestForward/TestCore/TestMacroForward.h */
#ifdef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
    #undef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
#endif /* PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST */
#define PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(ClassName, PrettyName, Category, FileName, LineNumber) \
    struct ClassName : public ::Jafg::Tester::LSimpleTestCase                                     \
    {                                                                                             \
        ClassName() : LSimpleTestCase(PrettyName, Category, FileName) { }                         \
        virtual void Run() override;                                                              \
    };                                                                                            \
    ClassName PRIVATE_JAFG_TEST_JOIN_INNER_TWO(ClassName, ##Instance);                            \
    void ClassName::Run()                                                                         \


/** @see Engine/TestForward/TestCore/TestMacroForward.h */
#ifdef CHECK_EQUALS
    #undef CHECK_EQUALS
#endif /* CHECK_EQUALS */
#define CHECK_EQUALS(What, A, B)                 \
    CHECK_EQUALS_WITH_LINE(What, A, B, __LINE__)
#define CHECK_EQUALS_WITH_LINE(What, A, B, Line)                        \
    this->TestEqual("Equality test ["#What"]: { "#A" == "#B" }", A, B, Line);
