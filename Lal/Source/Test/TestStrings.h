// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleAsciiStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LAsciiString MyStr;
    CHECK_NOT_NULL( "Implicitly constructed string.", MyStr.ToC()                    )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                      "" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                    "\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyStr,                     "A" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetSize(),             1 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetRuneCount(),        0 )

    MyStr = "Abc";
    CHECK_NOT_NULL( "Assigned string.", MyStr.ToC()                       )
    CHECK_EQUALS(   "Assigned string.", MyStr,                      "Abc" )
    CHECK_EQUALS(   "Assigned string.", MyStr,                    "Abc\0" )
    CHECK_LESS(     "Assigned string.", MyStr,                      "Abd" )
    CHECK_GREATER(  "Assigned string.", MyStr,                      "Abb" )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetSize(),                4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetRuneCount(),           3 )

    MyStr += "d";
    CHECK_NOT_NULL( "Appended string.", MyStr.ToC()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                    "Abcd" )
    CHECK_EQUALS(   "Appended string.", MyStr,                  "Abcd\0" )
    CHECK_FALSE(    "Appended string.", MyStr <  "Abcd"                  )
    CHECK_FALSE(    "Appended string.", MyStr >  "Abcd"                  )
    CHECK_TRUE(     "Appended string.", MyStr >= "Abcd"                  )
    CHECK_TRUE(     "Appended string.", MyStr <= "Abcd"                  )
    CHECK_TRUE(     "Appended string.", MyStr == "Abcd"                  )
    CHECK_FALSE(    "Appended string.", MyStr != "Abcd"                  )
    CHECK_LESS(     "Appended string.", MyStr,                    "Abce" )
    CHECK_GREATER(  "Appended string.", MyStr,                    "Abcc" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          4 )

    LAsciiString MyOtherStr = "efgh";
    CHECK_NOT_NULL( "Implicitly constructed string.", MyOtherStr.ToC()             )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,           "efgh" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,         "efgh\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyOtherStr,           "efgi" )
    CHECK_GREATER(  "Implicitly constructed string.", MyOtherStr,           "efgg" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetSize(),      5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetRuneCount(), 4 )

    MyStr += MyOtherStr;
    CHECK_NOT_NULL( "Appended string.", MyStr.ToC()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdefgh" )
    CHECK_EQUALS(   "Appended string.", MyStr,              "Abcdefgh\0" )
    CHECK_LESS(     "Appended string.", MyStr,                 "Abcdefi" )
    CHECK_GREATER(  "Appended string.", MyStr,                 "Abcdefg" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          8 )

    return;
}

TEST_CASE(RawLiteralAsciiStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LAsciiString MyStr  = "Abc";
    LAsciiString MyStr2 = "Abc";

    constexpr char       RawChars[]  = { 'A', 'b', 'c', '\0' };
    const char*          RawCharsPtr = RawChars;
    const std::string    StdString   = "Abc";

    CHECK_EQUALS( "Raw literal string.", MyStr,        MyStr2.ToC() )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(MyStr2.ToC()) )

    CHECK_EQUALS( "Raw literal string.", MyStr,        RawCharsPtr )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(RawCharsPtr) )

    CHECK_TRUE(    "Std string.", MyStr.Equals(StdString.c_str()) )
    CHECK_EQUALS(  "Std string.", MyStr,        StdString.c_str() )

    MyStr.Append(StdString.c_str());
    CHECK_EQUALS(  "Std string.", MyStr,            "AbcAbc" )
    CHECK_GREATER( "Std string.", MyStr,   StdString.c_str() )

    return;
}

TEST_CASE(SimpleAsciiStringManipulations, "Lal.Strings")
{
    using namespace Jafg;

    LAsciiString MyStr;
    CHECK_EQUALS( "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,            "\0" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    MyStr.Append("A");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  1 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  1 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  1 )

    MyStr.Append("B");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 2 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 2 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,              "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,            "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 2 )

    MyStr.Append("C");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize()  ,     4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  3 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize()  ,     4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  3 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize()  ,     4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  3 )

    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'C' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize()  ,         3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      2 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'B' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize()  ,         2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      1 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'A' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize()  ,         1 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize()  ,         1 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )

    MyStr.Append("ABC");

    MyStr.Reset(0);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    MyStr.Append("ABC");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,           "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),  3 )

    MyStr.Empty();
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    MyStr.Reserve(5);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,             "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),      1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(), 0 )

    return;
}

TEST_CASE(AsciiArrayCopyingAndMoving, "Lal.Strings")
{
    using namespace Jafg;

    LAsciiString MyStr1;
    LAsciiString MyStr2 = MyStr1;

    CHECK_EQUALS(       "Copy constructed string.", MyStr1,                 MyStr2 )
    CHECK_NOT_EQUALS(   "Copy constructed string.", MyStr1.ToC(),     MyStr2.ToC() )

    MyStr2.Append("Abcdefgh");
    CHECK_EQUALS(       "Copy constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Copy constructed string.", MyStr2,           "Abcdefgh" )

    MyStr1 = MyStr2;
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,                 MyStr2 )
    CHECK_NOT_EQUALS(   "Copy assigned string.", MyStr1.ToC(),     MyStr2.ToC() )

    MyStr1.Append("Ijklmnop");
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,   "AbcdefghIjklmnop" )
    MyStr1 = std::move(MyStr2);
    CHECK_EQUALS(       "Move assigned string.", MyStr1,           "Abcdefgh" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                 "\0" )
    CHECK_NOT_EQUALS(   "Move assigned string.", MyStr1.ToC(),   MyStr2.ToC() )

    LAsciiString MyStr3 = std::move(MyStr1);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                 "\0" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr3.ToC(),   MyStr1.ToC() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )

    MyStr2.MoveFrom(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )

    MyStr2.MoveInto(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )
    const char* OldDataOfMyStr2 = MyStr2.ToC();
    const char* OldDataOfMyStr3 = MyStr3.ToC();

    MyStr2.SwapStrings(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.ToC(), OldDataOfMyStr3 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.ToC(), OldDataOfMyStr2 )

    OldDataOfMyStr2 = nullptr;
    OldDataOfMyStr3 = nullptr;
    MyStr3.CopyFrom(MyStr2);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )

    MyStr2.Empty();
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                 "\0" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )

    MyStr3.CopyInto(MyStr2);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToC(),   MyStr3.ToC() )

    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetSize(),          9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetRuneCount(),     8 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetSize(),          9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetRuneCount(),     8 )

    return;
}
