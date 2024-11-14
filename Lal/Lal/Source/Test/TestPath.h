// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"
#include "System/Path.h"

TEST_CASE(SimplePathStringOperations, "Lal.Path")
{
    using namespace Jafg;

    LPath MyPath;
    CHECK_NOT_NULL( "Implicitly constructed path.", MyPath.GetPath().ToC()              )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath,                          "" )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath,                        "\0" )
    CHECK_LESS(     "Implicitly constructed path.", MyPath,                         "A" )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath.GetPath().GetSize(),        0 )
    CHECK_EQUALS(   "Implicitly constructed path.", MyPath.GetPath().GetRuneCount(),   0 )

    MyPath = "Abc";
    CHECK_NOT_NULL( "Assigned path.", MyPath.GetPath().ToC()                   )
    CHECK_EQUALS(   "Assigned path.", MyPath,                            "Abc" )
    CHECK_EQUALS(   "Assigned path.", MyPath,                          "Abc\0" )
    CHECK_LESS(     "Assigned path.", MyPath,                            "Abd" )
    CHECK_GREATER(  "Assigned path.", MyPath,                            "Abb" )
    CHECK_EQUALS(   "Assigned path.", MyPath.GetPath().GetSize(),            4 )
    CHECK_EQUALS(   "Assigned path.", MyPath.GetPath().GetRuneCount(),       3 )

    MyPath /= "d";
    CHECK_EQUALS(   "Appended path.", MyPath,                          "Abc/d" )
    MyPath /= "e";
    CHECK_EQUALS(   "Appended path.", MyPath,                        "Abc/d/e" )
    MyPath /= "";
    CHECK_EQUALS(   "Appended path.", MyPath,                       "Abc/d/e" )
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

    return;
}
