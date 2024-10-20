// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

#ifdef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
    #undef PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST
#endif /* PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST */
#define PRIVATE_JAFG_TEST_MAKE_SIMPLE_TEST(ClassName, PrettyName, Category, FileName, LineNumber) \
    struct ClassName : public ::Jafg::Tester::LSimpleTestCase \
    { \
        ClassName() : LSimpleTestCase(PrettyName, Category) { } \
        void Run() override; \
    }; \
    ClassName PRIVATE_JAFG_TEST_JOIN_INNER_TWO(ClassName, ##Instance); \
    void ClassName::Run() \

#ifdef CHECK_EQUALS
    #undef CHECK_EQUALS
#endif /* CHECK_EQUALS */
#define CHECK_EQUALS(What, X, Y) \
    this->TestEqual(What, X, Y, __LINE__);
