// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

#if TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS

#include "System/Path.h"

TEST_CASE(SimplePathStringOperations, "Lal.Path")
{
    using namespace Jafg;

    LPath MyPath;
    CHECK_NOT_NULL( "Implicitly constructed path.", MyPath.ToPtr()                       )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath,                           "" )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath,                         "\0" )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath.GetSize(),                  0 )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath.GetRuneCount(),             0 )

    MyPath = "Abc";
    CHECK_NOT_NULL( "Assigned path.", MyPath.ToPtr()                           )
    CHECK_EQUALS(   "Assigned path.", MyPath,                            "Abc" )
    CHECK_EQUALS(   "Assigned path.", MyPath,                          "Abc\0" )
    CHECK_EQUALS(   "Assigned path.", MyPath.GetSize(),                      4 )
    CHECK_EQUALS(   "Assigned path.", MyPath.GetRuneCount(),                 3 )

    MyPath /= "d";
    CHECK_EQUALS(   "Appended path.", MyPath,                          "Abc/d" )
    MyPath /= "e";
    CHECK_EQUALS(   "Appended path.", MyPath,                        "Abc/d/e" )
    MyPath /= "";
    CHECK_EQUALS(   "Appended path.", MyPath,                      "Abc/d/e/" )
    MyPath /= "/";
    CHECK_EQUALS(   "Appended path.", MyPath,                      "Abc/d/e/" )
    MyPath /= "/f";
    CHECK_EQUALS(   "Appended path.", MyPath,                     "Abc/d/e/f" )

    LPath MyOtherPath = MyPath / "g" / "h" / "i" / "j";
    CHECK_EQUALS(   "Implicitly constructed path.", MyOtherPath, "Abc/d/e/f/g/h/i/j" )
    MyPath.Empty();
    CHECK_EQUALS(   "Empty path.", MyPath,                                  "" )
    MyPath = MyPath / "A" / "B" / "C" / "D";
    CHECK_EQUALS(   "Assigned path.", MyPath,                       "/A/B/C/D" )
    MyOtherPath /= MyPath;
    CHECK_EQUALS(   "Appended path.", MyOtherPath, "Abc/d/e/f/g/h/i/j/A/B/C/D" )
    MyOtherPath = MyPath;
    CHECK_EQUALS(   "Assigned path.", MyOtherPath,                  "/A/B/C/D" )

    MyPath = "A\\B\\C\\D";
    QUICK_CHECK_EQUALS( MyPath,                  "A\\B\\C\\D" )
    QUICK_CHECK_EQUALS( MyPath.Normalize(),                 3 )
    QUICK_CHECK_EQUALS( MyPath,                     "A/B/C/D" )
    QUICK_CHECK_EQUALS( MyPath.Normalize(),                 0 )
    QUICK_CHECK_EQUALS( MyPath,                     "A/B/C/D" )

    QUICK_CHECK_TRUE(   MyPath.PopSubPath()                 )
    QUICK_CHECK_EQUALS( MyPath,                     "A/B/C" )
    QUICK_CHECK_EQUALS( MyPath.PopSubPaths(2),            2 )
    QUICK_CHECK_EQUALS( MyPath,                         'A' )
    QUICK_CHECK_EQUALS( MyPath,                         "A" )
    QUICK_CHECK_TRUE(   MyPath.PopSubPath()                 )
    QUICK_CHECK_EQUALS( MyPath,                          "" )
    QUICK_CHECK_FALSE(  MyPath.PopSubPath()                 )
    QUICK_CHECK_EQUALS( MyPath,                          "" )
    QUICK_CHECK_EQUALS( MyPath,                          "" )
    QUICK_CHECK_EQUALS( MyPath.PopSubPaths(20),           0 )
    MyPath = "A\\B\\C\\D";
    QUICK_CHECK_EQUALS( MyPath.PopSubPaths(20, true),     4 )
    QUICK_CHECK_EQUALS( MyPath,                          "" )
    MyPath = "A\\B\\C\\D"; MyPath.Normalize();
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                "D" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                'D' )
    MyPath = "A\\B\\C\\"; MyPath.Normalize();
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                "" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),              '\0' )
    MyPath = "A\\"; MyPath.Normalize();
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                 "" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),               '\0' )
    MyPath = "\\"; MyPath.Normalize();
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                 "" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),               '\0' )
    MyPath.Empty();
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                 "" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),               '\0' )
    MyPath = "";
    QUICK_CHECK_EQUALS( MyPath.GetBase(),                 "" )
    QUICK_CHECK_EQUALS( MyPath.GetBase(),               '\0' )

    MyPath = "jafg.jafgworkspace";
    QUICK_CHECK_TRUE(  MyPath.DoesExist() )
    QUICK_CHECK_FALSE( MyPath.IsDir()     )
    QUICK_CHECK_TRUE(  MyPath.IsFile()    )

    return;
}

#endif /* TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS */
