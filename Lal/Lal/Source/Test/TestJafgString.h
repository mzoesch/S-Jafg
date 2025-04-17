// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleNewSimpleStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr;
    CHECK_EQUALS(   "Implicitly constructed string.",*MyStr.ToPtr(),              '\0' )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                      "" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                    "\0" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetSize(),             0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetByteSize(),         0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetRuneCount(),        0 )
    CHECK_EQUALS(   "Implicitly constructed string.", *MyStr.Peek(),            '\0' )

    MyStr = "Abc";
    CHECK_NOT_NULL( "Assigned string.", MyStr.ToPtr()                       )
    CHECK_EQUALS(   "Assigned string.", MyStr,                      "Abc" )
    CHECK_EQUALS(   "Assigned string.", MyStr,                    "Abc\0" )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetSize(),                4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetByteSize(),            4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetRuneCount(),           3 )

    MyStr += "d";
    CHECK_NOT_NULL( "Appended string.", MyStr.ToPtr()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                    "Abcd" )
    CHECK_EQUALS(   "Appended string.", MyStr,                  "Abcd\0" )
    CHECK_TRUE(     "Appended string.", MyStr == "Abcd"                  )
    CHECK_FALSE(    "Appended string.", MyStr != "Abcd"                  )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          4 )

    LString MyOtherStr = "efgh";
    CHECK_NOT_NULL( "Implicitly constructed string.", MyOtherStr.ToPtr()                  )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,                "efgh" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,              "efgh\0" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetSize(),           5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetByteSize(),       5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetRuneCount(),      4 )

    MyStr += MyOtherStr;
    CHECK_NOT_NULL( "Appended string.", MyStr.ToPtr()                  )
    CHECK_EQUALS(   "Appended string.", MyStr,            "Abcdefgh" )
    CHECK_EQUALS(   "Appended string.", MyStr,          "Abcdefgh\0" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),           9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),       9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),      8 )

    return;
}

TEST_CASE(RawLiteraLStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr  = "Abc";
    LString MyStr2 = "Abc";

    constexpr char       RawChars[]  = { 'A', 'b', 'c', '\0' };
    const char*          RawCharsPtr = RawChars;
    const std::string    StdString   = "Abc";

    CHECK_EQUALS( "Raw literal string.", MyStr,        MyStr2.ToPtr() )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(MyStr2.ToPtr()) )

    CHECK_EQUALS( "Raw literal string.", MyStr,        RawCharsPtr )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(RawCharsPtr) )

    CHECK_TRUE(    "Std string.", MyStr.Equals(StdString.c_str()) )
    CHECK_EQUALS(  "Std string.", MyStr,        StdString.c_str() )

    MyStr.Append(StdString.c_str());
    CHECK_EQUALS(  "Std string.", MyStr,            "AbcAbc" )

    return;
}

TEST_CASE(SimpleNewSimpleStringManipulations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr;
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("A");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("B");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("C");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'C' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      2 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'B' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      1 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'A' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )

    MyStr.Append("ABC");

    MyStr.Reset(0);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("ABC");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Empty();
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Reserve(5);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    return;
}

TEST_CASE(NewSimpleStringCopyingAndMoving, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr1;
    LString MyStr2 = MyStr1;

    CHECK_EQUALS(   "Copy constructed string.", MyStr1,                 MyStr2 )
    CHECK_EQUALS(   "Copy constructed string.", MyStr1.ToPtr(),     MyStr2.ToPtr() )

    MyStr2.Append("Abcdefgh");
    CHECK_EQUALS(       "Copy constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Copy constructed string.", MyStr2,           "Abcdefgh" )

    MyStr1 = MyStr2;
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,                 MyStr2 )
    CHECK_NOT_EQUALS(   "Copy assigned string.", MyStr1.ToPtr(),     MyStr2.ToPtr() )

    MyStr1.Append("Ijklmnop");
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,   "AbcdefghIjklmnop" )
    MyStr1 = std::move(MyStr2);
    CHECK_EQUALS(       "Move assigned string.", MyStr1,           "Abcdefgh" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                 "\0" )
    CHECK_NOT_EQUALS(   "Move assigned string.", MyStr1.ToPtr(),   MyStr2.ToPtr() )

    LString MyStr3 = std::move(MyStr1);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                 "\0" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr3.ToPtr(),   MyStr1.ToPtr() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )

    MyStr2 = std::move(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr3 = std::move(MyStr2);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )
    const char* OldDataOfMyStr2 = MyStr2.ToPtr();
    const char* OldDataOfMyStr3 = MyStr3.ToPtr();

    MyStr2.SwapStrings(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.ToPtr(), OldDataOfMyStr3 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.ToPtr(), OldDataOfMyStr2 )

    OldDataOfMyStr2 = nullptr;
    OldDataOfMyStr3 = nullptr;
    MyStr3 = MyStr2;
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr2.Empty();
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                 "\0" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr2 = MyStr3;
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetRuneCount(),      8 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetRuneCount(),      8 )

    return;
}

TEST_CASE(NewSimpleStringStartsWith, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "";
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith("")      )
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith('\0')    )
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith('\0')    )
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith("\0")    )
    CHECK_FALSE( "Empty string.", MyStr.StartsWith("A")     )

    MyStr += "";
    CHECK_EQUALS( "Empty string.", MyStr, ""                 )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith('\0')    )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith("")      )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith('\0')    )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith("\0")    )
    CHECK_FALSE(  "Empty string.", MyStr.StartsWith("A")     )

    MyStr += "A";
    CHECK_EQUALS( "A string.", MyStr, "A"                )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("")      )
    CHECK_FALSE(  "A string.", MyStr.StartsWith('\0')    )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("\0")    )
    CHECK_TRUE(   "A string.", MyStr.StartsWith('A')     )
    CHECK_TRUE(   "A string.", MyStr.StartsWith("A")     )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("AB")    )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("B")     )

    MyStr += "B";
    CHECK_EQUALS( "AB string.", MyStr, "AB"              )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("")     )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("\0")   )
    CHECK_TRUE(   "AB string.", MyStr.StartsWith("A")    )
    CHECK_TRUE(   "AB string.", MyStr.StartsWith("AB")   )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("ABC")  )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("B")    )

    MyStr += "C";
    CHECK_EQUALS( "ABC string.", MyStr, "ABC"            )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("")    )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("\0")  )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith('A')   )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("A")   )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("AB")  )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("ABC") )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("ABCD"))
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("B")   )

    MyStr += "D";
    CHECK_EQUALS( "ABCD string.", MyStr, "ABCD"             )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("")      )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("\0")    )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith('A')     )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("A")     )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("AB")    )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("ABC")   )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("ABCD")  )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("ABCDE") )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("B")     )

    return;
}

TEST_CASE(NewSimpleStringCount, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count('A'),    7 )
    CHECK_EQUALS( "String count.", MyStr.Count('a'),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count('b'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('c'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('d'),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    CHECK_EQUALS( "String count.", MyStr.Count("a"),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count("b"),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count("c"),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count("d"),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count("Aa"),   1 )
    CHECK_EQUALS( "String count.", MyStr.Count("AA"),   3 )
    CHECK_EQUALS( "String count.", MyStr.Count("cAA"),  2 )
    CHECK_EQUALS( "String count.", MyStr.Count('\0'),   1 )
    CHECK_EQUALS( "String count.", MyStr.Count(""),     0 )
    CHECK_EQUALS( "String count.", MyStr.Count("\0"),   0 )

    CHECK_EQUALS( "String count.", MyStr.Count("AAq"),    0 )
    CHECK_EQUALS( "String count.", MyStr.Count("AAq", 2), 3 )

    CHECK_EQUALS( "String count.", MyStr.Count('A'),    7 )
    CHECK_EQUALS( "String count.", MyStr.Count('a'),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count('b'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('c'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('d'),    1 )

    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr, "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr, "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    LString MyOtherStr;
    CHECK_EQUALS( "String count.", MyOtherStr, "" )
    CHECK_EQUALS( "String count.", MyOtherStr.Count("A"), 0 )

    return;
}

TEST_CASE(NewSimpleStringReplace, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    MyStr.Replace('A', 'X');
    CHECK_EQUALS( "String count.", MyStr, "XaXXbcXXdbcXX" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"),    0 )
    CHECK_EQUALS( "String count.", MyStr.Count("X"),    7 )
    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )
    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    MyStr = "AaAAbcAAdbcAA";
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"),    7 )
    MyStr.Replace(*"A", *"X");
    CHECK_EQUALS( "String count.", MyStr, "XaXXbcXXdbcXX" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"),    0 )
    CHECK_EQUALS( "String count.", MyStr.Count(*"X"),    7 )
    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"), 0 )
    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"), 0 )

    return;
}

TEST_CASE(NewSimpleStringFindFirst, "Lal.Strings")
{
    using namespace Jafg;

    const LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find first.", MyStr.FindFirst("A"),     0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("a"),     1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("b"),     4 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("c"),     5 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("d"),     8 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("e"),    -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('A'),     0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('a'),     1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('b'),     4 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('c'),     5 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('d'),     8 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('e'),    -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst('\0'),   -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(""),      0 )

    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AA"),                2 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AaAA"),              0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AaAAbcAAdbcAA"),     0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AaAAbcAAdbcA"),      0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AaAAbcAAdbc"),       0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("aAAbcAAdbcAA"),      1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AAbcAAdbcAA"),       2 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("aAAbc"),             1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("aAAbcq"),           -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("bcAA"),              4 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("bcAAx"),            -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("cA"),                5 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("dbcAA"),             8 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("e"),                -1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("AAdaqwe", 2),        2 )

    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"A"),  0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"a"),  1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"b"),  4 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"c"),  5 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"d"),  8 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst(*"e"), -1 )

    return;
}

TEST_CASE(NewSimpleStringFindSecond, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String find second.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find second.", MyStr.FindSecond("A"),      2 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("a"),     -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("b"),      9 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("c"),     10 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("d"),     -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("e"),     -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("AA"),     6 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("dbcAA"), -1 )

    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"A"),  2 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"a"), -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"b"),  9 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"c"), 10 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"d"), -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond(*"e"), -1 )

    return;
}

TEST_CASE(NewSimpleStringFindLast, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String find last.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find last.", MyStr.FindLast("A"),     12 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("AA"),    11 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("cAA"),   10 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("dbc"),    8 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("a"),      1 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("b"),      9 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("c"),     10 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("d"),      8 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("e"),     -1 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast('a'),      1 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast('b'),      9 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast('c'),     10 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast('d'),      8 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast('e'),     -1 )

    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"A"), 12 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"a"),  1 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"b"),  9 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"c"), 10 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"d"),  8 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast(*"e"), -1 )

    return;
}

TEST_CASE(NewSimpleStringCuts, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghijklmn" )

    MyStr.InlineCut(10);
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghij" )

    LString MyOtherString = MyStr.Cut(5);
    CHECK_EQUALS( "String cut.", MyStr,         "abcdefghij" )
    CHECK_EQUALS( "String cut.", MyOtherString, "abcde"      )

    MyOtherString = MyOtherString.Cut(0);
    CHECK_EQUALS( "String cut.", MyOtherString, "" )

    return;
}

TEST_CASE(NewSimpleStringSubs, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineSub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineSubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    LString MyOtherString = MyStr.Sub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr,         "abcdefghijklmn" )
    CHECK_EQUALS( "String sub.", MyOtherString,             "fg" )

    MyOtherString = MyOtherString.Sub(0, 0);
    CHECK_EQUALS( "String sub.", MyOtherString, "" )
    MyOtherString = MyStr;
    CHECK_EQUALS( "String sub.", MyOtherString, "abcdefghijklmn" )
    MyOtherString = MyOtherString.SubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyOtherString, "fg" )

    return;
}

TEST_CASE(NewSimpleStringSubs, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineRightChop(0);
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineRightChop(1);
    CHECK_EQUALS( "String sub.", MyStr, "bcdefghijklmn" )
    MyStr.InlineRightChop(1);
    CHECK_EQUALS( "String sub.", MyStr, "cdefghijklmn" )
    MyStr.InlineRightChop(5);
    CHECK_EQUALS( "String sub.", MyStr, "hijklmn" )
    MyStr.InlineRightChop(5);
    CHECK_EQUALS( "String sub.", MyStr, "mn" )
    MyStr.InlineRightChop(0);
    CHECK_EQUALS( "String sub.", MyStr, "mn" )
    MyStr.InlineRightChop(2);
    CHECK_EQUALS( "String sub.", MyStr, "" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineLeftChop(14);
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineLeftChop(5);
    CHECK_EQUALS( "String sub.", MyStr, "abcde" )
    MyStr.InlineLeftChop(5);
    CHECK_EQUALS( "String sub.", MyStr, "abcde" )
    MyStr.InlineLeftChop(1);
    CHECK_EQUALS( "String sub.", MyStr, "a" )
    MyStr.InlineLeftChop(1);
    CHECK_EQUALS( "String sub.", MyStr, "a" )
    MyStr.InlineLeftChop(0);
    CHECK_EQUALS( "String sub.", MyStr, "" )

    return;
}

TEST_CASE(NewSimpleStringLowerCase, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AbCdEfGhIjKlMn";
    CHECK_EQUALS( "String lower case.", MyStr,         "AbCdEfGhIjKlMn" )
    LString MyOtherStr = MyStr.GetLowerCase();
    CHECK_EQUALS( "String lower case.", MyStr,         "AbCdEfGhIjKlMn" )
    CHECK_EQUALS( "String lower case.", MyOtherStr,    "abcdefghijklmn" )

    MyStr = "a0,123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    CHECK_EQUALS( "String lower case.", MyStr,         "a0,123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" )
    MyOtherStr = MyStr.GetLowerCase();
    CHECK_EQUALS( "String lower case.", MyStr,         "a0,123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" )
    CHECK_EQUALS( "String lower case.", MyOtherStr,    "a0,123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" )

    return;
}

TEST_CASE(SimpleNewEightStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr;
    CHECK_NOT_NULL( "Implicitly constructed string.", MyStr.ToPtr()                    )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                      "" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                    "\0" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetByteSize(),         0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetCharacterCount(),       0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetRuneCount(),        0 )

    MyStr = "Abc";
    CHECK_NOT_NULL( "Assigned string.", MyStr.ToPtr()                       )
    CHECK_EQUALS(   "Assigned string.", MyStr,                      "Abc" )
    CHECK_EQUALS(   "Assigned string.", MyStr,                    "Abc\0" )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetSize(),                4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetByteSize(),            4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetCharacterCount(),      3 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetRuneCount(),           3 )

    MyStr += "d";
    CHECK_NOT_NULL( "Appended string.", MyStr.ToPtr()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                    "Abcd" )
    CHECK_EQUALS(   "Appended string.", MyStr,                  "Abcd\0" )
    CHECK_TRUE(     "Appended string.", MyStr == "Abcd"                  )
    CHECK_FALSE(    "Appended string.", MyStr != "Abcd"                  )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(),         4 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          4 )

    LString MyOtherStr = "efgh";
    CHECK_NOT_NULL( "Implicitly constructed string.", MyOtherStr.ToPtr()                  )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,                "efgh" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,              "efgh\0" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetSize(),           5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetByteSize(),       5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetCharacterCount(),     4 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetRuneCount(),      4 )

    MyStr += MyOtherStr;
    CHECK_NOT_NULL( "Appended string.", MyStr.ToPtr()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdefgh" )
    CHECK_EQUALS(   "Appended string.", MyStr,              "Abcdefgh\0" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(),         8 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          8 )


    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdefgh" )
    QUICK_CHECK_TRUE( MyStr.Pop() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdefg" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdef" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcde" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcd" )
    QUICK_CHECK_TRUE( MyStr.Pop() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abc" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Ab" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "A" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )
    MyStr += "A";
    CHECK_EQUALS(   "Appended string.", MyStr,                "A" )
    QUICK_CHECK_TRUE( MyStr.Pop() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )
    MyStr += 'A';
    CHECK_EQUALS(   "Appended string.", MyStr,                "A" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "A\0" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.Pop() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    CHECK_EQUALS(   "Appended string.", MyStr,                "" )
    CHECK_EQUALS(   "Appended string.", MyStr,                "\0" )

    return;
}

TEST_CASE(RawLiteralNewEightStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr  = "Abc";
    LString MyStr2 = "Abc";

    constexpr char       RawChars[]  = { 'A', 'b', 'c', '\0' };
    const char*          RawCharsPtr = RawChars;
    const std::string    StdString   = "Abc";

    CHECK_EQUALS( "Raw literal string.", MyStr,        MyStr2.ToPtr() )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(MyStr2.ToPtr()) )

    CHECK_EQUALS( "Raw literal string.", MyStr,        RawCharsPtr )
    CHECK_TRUE  ( "Raw literal string.", MyStr.Equals(RawCharsPtr) )

    CHECK_TRUE(    "Std string.", MyStr.Equals(StdString.c_str()) )
    CHECK_EQUALS(  "Std string.", MyStr,        StdString.c_str() )

    MyStr.Append(StdString.c_str());
    CHECK_EQUALS(  "Std string.", MyStr,            "AbcAbc" )

    return;
}

TEST_CASE(SimpleNewEightStringManipulations, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr;
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("A");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("B");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("C");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'C' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(),     2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      2 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'B' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(),     1 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      1 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'A' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )

    MyStr.Append("ABC");

    MyStr.Reset(0);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("ABC");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Empty();
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Reserve(5);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(),     0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    return;
}

TEST_CASE(NewEightStringCopyingAndMoving, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr1;
    LString MyStr2 = MyStr1;

    CHECK_EQUALS(   "Copy constructed string.", MyStr1,                 MyStr2 )
    CHECK_EQUALS(   "Copy constructed string.", MyStr1.ToPtr(),     MyStr2.ToPtr() )

    MyStr2.Append("Abcdefgh");
    CHECK_EQUALS(       "Copy constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Copy constructed string.", MyStr2,           "Abcdefgh" )

    MyStr1 = MyStr2;
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,                 MyStr2 )
    CHECK_NOT_EQUALS(   "Copy assigned string.", MyStr1.ToPtr(),     MyStr2.ToPtr() )

    MyStr1.Append("Ijklmnop");
    CHECK_EQUALS(       "Copy assigned string.", MyStr1,   "AbcdefghIjklmnop" )
    MyStr1 = std::move(MyStr2);
    CHECK_EQUALS(       "Move assigned string.", MyStr1,           "Abcdefgh" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move assigned string.", MyStr2,                 "\0" )
    CHECK_NOT_EQUALS(   "Move assigned string.", MyStr1.ToPtr(),   MyStr2.ToPtr() )

    LString MyStr3 = std::move(MyStr1);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                 "\0" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr3.ToPtr(),   MyStr1.ToPtr() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )

    MyStr2 = std::move(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr3 = std::move(MyStr2);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )
    const char* OldDataOfMyStr2 = MyStr2.ToPtr();
    const char* OldDataOfMyStr3 = MyStr3.ToPtr();

    MyStr2.SwapStrings(MyStr3);
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,                   "" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.ToPtr(), OldDataOfMyStr3 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.ToPtr(), OldDataOfMyStr2 )

    OldDataOfMyStr2 = nullptr;
    OldDataOfMyStr3 = nullptr;
    MyStr3 = MyStr2;
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr2.Empty();
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                 "\0" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    MyStr2 = MyStr3;
    CHECK_EQUALS(       "Move constructed string.", MyStr2,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr2.ToPtr(),   MyStr3.ToPtr() )

    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetRuneCount(),      8 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetRuneCount(),      8 )

    return;
}

TEST_CASE(NewEightStringStartsWith, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "";
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith("")      )
    CHECK_TRUE(  "Empty string.", MyStr.StartsWith("\0")    )
    CHECK_FALSE( "Empty string.", MyStr.StartsWith("A")     )

    MyStr += "";
    CHECK_EQUALS( "Empty string.", MyStr, ""                 )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith("")      )
    CHECK_TRUE(   "Empty string.", MyStr.StartsWith("\0")    )
    CHECK_FALSE(  "Empty string.", MyStr.StartsWith("A")     )

    MyStr += "A";
    CHECK_EQUALS( "A string.", MyStr, "A"                )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("")      )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("\0")    )
    CHECK_TRUE(   "A string.", MyStr.StartsWith("A")     )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("AB")    )
    CHECK_FALSE(  "A string.", MyStr.StartsWith("B")     )

    MyStr += "B";
    CHECK_EQUALS( "AB string.", MyStr, "AB"              )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("")     )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("\0")   )
    CHECK_TRUE(   "AB string.", MyStr.StartsWith("A")    )
    CHECK_TRUE(   "AB string.", MyStr.StartsWith("AB")   )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("ABC")  )
    CHECK_FALSE(  "AB string.", MyStr.StartsWith("B")    )

    MyStr += "C";
    CHECK_EQUALS( "ABC string.", MyStr, "ABC"            )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("")    )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("\0")  )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("A")   )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("AB")  )
    CHECK_TRUE(   "ABC string.", MyStr.StartsWith("ABC") )
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("ABCD"))
    CHECK_FALSE(  "ABC string.", MyStr.StartsWith("B")   )

    MyStr += "D";
    CHECK_EQUALS( "ABCD string.", MyStr, "ABCD"             )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("")      )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("\0")    )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("A")     )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("AB")    )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("ABC")   )
    CHECK_TRUE(   "ABCD string.", MyStr.StartsWith("ABCD")  )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("ABCDE") )
    CHECK_FALSE(  "ABCD string.", MyStr.StartsWith("B")     )

    return;
}

TEST_CASE(NewEightStringCount, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    CHECK_EQUALS( "String count.", MyStr.Count("a"),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count("b"),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count("c"),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count("d"),    1 )

    CHECK_EQUALS( "String count.", MyStr.Count('A'),    7 )
    CHECK_EQUALS( "String count.", MyStr.Count('a'),    1 )
    CHECK_EQUALS( "String count.", MyStr.Count('b'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('c'),    2 )
    CHECK_EQUALS( "String count.", MyStr.Count('d'),    1 )

    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr,           "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    LString MyOtherStr;
    CHECK_EQUALS( "String count.", MyOtherStr,           "" )
    CHECK_EQUALS( "String count.", MyOtherStr.Count("A"), 0 )

    return;
}

TEST_CASE(NewEightStringReplace, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,     "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    MyStr.Replace('A', 'X');
    CHECK_EQUALS( "String count.", MyStr, "XaXXbcXXdbcXX" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"),    0 )
    CHECK_EQUALS( "String count.", MyStr.Count("X"),    7 )
    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr,              "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"),    0 )
    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr,              "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"),    0 )

    MyStr = "AaAAbcAAdbcAA";
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"),   7 )
    MyStr.Replace(*"A", *"X");
    CHECK_EQUALS( "String count.", MyStr, "XaXXbcXXdbcXX" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"),   0 )
    CHECK_EQUALS( "String count.", MyStr.Count(*"X"),   7 )
    MyStr.Reset(0);
    CHECK_EQUALS( "String count.", MyStr,            "" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"), 0 )
    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr,            "" )
    CHECK_EQUALS( "String count.", MyStr.Count(*"A"), 0 )

    return;
}

TEST_CASE(NewEightStringFindFirst, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.FindFirst("A"),   0 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst("a"),   1 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst("b"),   4 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst("c"),   5 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst("d"),   8 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst("e"),  -1 )

    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"A"),  0 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"a"),  1 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"b"),  4 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"c"),  5 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"d"),  8 )
    CHECK_EQUALS( "String count.", MyStr.FindFirst(*"e"), -1 )

    return;
}

TEST_CASE(NewEightStringFindSecond, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,     "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.FindSecond("A"),   2 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond("a"),  -1 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond("b"),   9 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond("c"),  10 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond("d"),  -1 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond("e"),  -1 )

    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"A"),  2 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"a"), -1 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"b"),  9 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"c"), 10 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"d"), -1 )
    CHECK_EQUALS( "String count.", MyStr.FindSecond(*"e"), -1 )

    return;
}

TEST_CASE(NewEightStringFindLast, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.FindLast("A"),  12 )
    CHECK_EQUALS( "String count.", MyStr.FindLast("a"),   1 )
    CHECK_EQUALS( "String count.", MyStr.FindLast("b"),   9 )
    CHECK_EQUALS( "String count.", MyStr.FindLast("c"),  10 )
    CHECK_EQUALS( "String count.", MyStr.FindLast("d"),   8 )
    CHECK_EQUALS( "String count.", MyStr.FindLast("e"),  -1 )

    CHECK_EQUALS( "String count.", MyStr.FindLast(*"A"), 12 )
    CHECK_EQUALS( "String count.", MyStr.FindLast(*"a"),  1 )
    CHECK_EQUALS( "String count.", MyStr.FindLast(*"b"),  9 )
    CHECK_EQUALS( "String count.", MyStr.FindLast(*"c"), 10 )
    CHECK_EQUALS( "String count.", MyStr.FindLast(*"d"),  8 )
    CHECK_EQUALS( "String count.", MyStr.FindLast(*"e"), -1 )

    return;
}

TEST_CASE(NewEightStringCuts, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghijklmn" )

    MyStr.InlineCut(10);
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghij" )

    LString MyOtherString = MyStr.Cut(5);
    CHECK_EQUALS( "String cut.", MyStr,         "abcdefghij" )
    CHECK_EQUALS( "String cut.", MyOtherString, "abcde"      )

    MyOtherString = MyOtherString.Cut(0);
    CHECK_EQUALS( "String cut.", MyOtherString, "" )

    return;
}

TEST_CASE(NewEightStringSubs, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineSub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineSubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr,         "abcdefghijklmn" )
    LString MyOtherString = MyStr.Sub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr,         "abcdefghijklmn" )
    CHECK_EQUALS( "String sub.", MyOtherString,             "fg" )

    MyOtherString = MyOtherString.Sub(0, 0);
    CHECK_EQUALS( "String sub.", MyOtherString, "" )
    MyOtherString = MyStr;
    CHECK_EQUALS( "String sub.", MyOtherString, "abcdefghijklmn" )
    MyOtherString = MyOtherString.SubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyOtherString, "fg" )

    return;
}

TEST_CASE(NewEightStringAdvancedCharacters, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "これわテストです。";
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテストです。" )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),           28 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetByteSize(),       28 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(),      9 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),      27 )

    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("")          )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("\0")        )
    CHECK_TRUE(  "Advanced characters.", MyStr.StartsWith("こ")        )
    CHECK_TRUE(  "Advanced characters.", MyStr.StartsWith("これ")       )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("これテ")     )

    MyStr = "で";
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("")          )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("\0")        )
    CHECK_EQUALS("Advanced characters.", MyStr, "で"                   )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("で"),          1 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("て"),          0 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("こ"),          0 )
    CHECK_TRUE(  "Advanced characters.", MyStr.StartsWith("で")        )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("でこ")      )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("て")        )

    MyStr = "スススこれスわステスストです。ススス";
    CHECK_EQUALS( "Advanced characters.", MyStr, "スススこれスわステスストです。ススス" )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),            55 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),       54 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(),      18 )
    QUICK_CHECK_EQUALS( MyStr.Replace("ス", "こ"),                     10 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),            55 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),       54 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(),      18 )
    CHECK_EQUALS( "Advanced characters.", MyStr, "ここここれこわこテここトです。こここ" )

    MyStr = "で";
    QUICK_CHECK_EQUALS( MyStr.Replace("で", "こ"),                    1 )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("")          )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("\0")        )
    CHECK_EQUALS("Advanced characters.", MyStr, "こ"                   )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("で"),          0 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("こ"),          1 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("て"),          0 )

    MyStr = "これわテストです。";
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("こ"),  0 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("れ"),  3 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("わ"),  6 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("テ"),  9 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("ス"), 12 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("ト"), 15 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("で"), 18 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("す"), 21 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("。"), 24 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("あ"), -1 )
    CHECK_EQUALS( "Advanced characters.", MyStr[0],           *"こ" ) // This ofcourse only compares the first byte. Not the whole character.
    CHECK_EQUALS( "Advanced characters.", MyStr[3],           *"れ" )
    CHECK_EQUALS( "Advanced characters.", MyStr[6],           *"わ" )
    CHECK_EQUALS( "Advanced characters.", MyStr[9],           *"テ" )
    CHECK_EQUALS( "Advanced characters.", MyStr[12],          *"ス" )
    CHECK_EQUALS( "Advanced characters.", MyStr[15],          *"ト" )
    CHECK_EQUALS( "Advanced characters.", MyStr[18],          *"で" )
    CHECK_EQUALS( "Advanced characters.", MyStr[21],          *"す" )
    CHECK_EQUALS( "Advanced characters.", MyStr[24],          *"。" )

    MyStr = "こふふこああこあふここ"; // こ:5; ふ:3; あ:3
    CHECK_EQUALS( "Advanced characters.", MyStr,  "こふふこああこあふここ" )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),           34 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetByteSize(),       34 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(),     11 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),      33 )

    CHECK_EQUALS( "Advanced characters.", MyStr.Count("こ"),         5 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("こ"),    9 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("ふ"),    6 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("あ"),   15 )

    CHECK_EQUALS( "Advanced characters.", MyStr, "こふふこああこあふここ" )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("こ"),     30 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("ふ"),     24 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("あ"),     21 )

    MyStr = "こふふこああこあふここ"; // こ:5; ふ:3; あ:3
    CHECK_EQUALS("Advanced characters.", MyStr, "こふふこああこあふここ" )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("こ"),         5 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("ふ"),         3 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("あ"),         3 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("ほ"),         0 )

    MyStr = "äää";
    CHECK_EQUALS("Advanced characters.", MyStr, "äää" )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("ä"),         3 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("a"),         0 )

    MyStr = "これわテストです。";
    CHECK_EQUALS("Advanced characters.", MyStr, "これわテストです。" )
    MyStr.InlineCut(MyStr.FindFirst("テ"));
    CHECK_EQUALS("Advanced characters.", MyStr,           "これわ" )
    MyStr = "これわテストです。";
    CHECK_EQUALS("Advanced characters.", MyStr, "これわテストです。" )
    LString MyOtherString = MyStr.Cut(MyStr.FindFirst("ト"));
    CHECK_EQUALS("Advanced characters.", MyStr,         "これわテストです。" )
    CHECK_EQUALS("Advanced characters.", MyOtherString,       "これわテス"  )
    MyOtherString = MyStr.Cut(0);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "" )
    MyOtherString = MyStr.Cut(3);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "こ" )
    MyOtherString = MyStr;
    CHECK_EQUALS("Advanced characters.", MyOtherString, "これわテストです。" )
    MyOtherString.InlineSub(15, 6);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "トで" )
    MyOtherString = MyStr;
    CHECK_EQUALS("Advanced characters.", MyOtherString, "これわテストです。" )
    MyOtherString.InlineSubIdx(15, 24);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "トです" )
    MyOtherString = MyStr.Sub(6, 15);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテストで" )
    MyOtherString = MyStr.Sub(6, 21);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテストです。" )
    MyOtherString = MyStr.SubIdx(6, 15);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテス" )
    MyStr.InlineSub(6, 15);
    CHECK_EQUALS("Advanced characters.", MyStr, "わテストで" )

    MyStr = "これ";
    MyStr = LString::SprintF("{}わ{}です{}", MyStr.ToPtr(), "テスト", "。");
    CHECK_EQUALS("Advanced characters.", MyStr, "これわテストです。" )

    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテストです" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテストで" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテスト" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテス" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテ" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわ" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "これ" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "こ" )
    QUICK_CHECK_TRUE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "" )
    CHECK_NULL(   "Advanced characters.", MyStr.GetUnderlyingDataStructure().GetData() )
    CHECK_EQUALS( "Advanced characters.", *MyStr.Peek(), '\0' )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    QUICK_CHECK_FALSE( MyStr.PopCharacter() )
    CHECK_EQUALS( "Advanced characters.", MyStr, "" )
    CHECK_NULL(   "Advanced characters.", MyStr.GetUnderlyingDataStructure().GetData() )
    CHECK_EQUALS( "Advanced characters.", *MyStr.Peek(), '\0' )

    MyStr         = "これわです。";
    MyOtherString = "テスト";
    CHECK_EQUALS( "Advanced characters.", MyStr,    "これわです。" )
    CHECK_EQUALS( "Advanced characters.", MyOtherString, "テスト" )
    MyStr.AppendAt(9, MyOtherString.ToPtr());
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテストです。" )

    MyStr.RemoveCharacterAt(9);
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわストです。" )
    MyStr.RemoveCharacterAt(9);
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわトです。" )
    MyStr.RemoveCharacterAt(9);
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわです。" )

    return;
}

TEST_CASE(NewEightStringLowerCase, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "AbCdEfGhIjKlMn";
    CHECK_EQUALS( "String lower case.", MyStr,         "AbCdEfGhIjKlMn" )
    LString MyOtherStr = MyStr.GetLowerCase();
    CHECK_EQUALS( "String lower case.", MyStr,         "AbCdEfGhIjKlMn" )
    CHECK_EQUALS( "String lower case.", MyOtherStr,    "abcdefghijklmn" )

    MyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    CHECK_EQUALS( "String lower case.", MyStr,         "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" )
    MyOtherStr = MyStr.GetLowerCase();
    CHECK_EQUALS( "String lower case.", MyStr,         "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" )
    CHECK_EQUALS( "String lower case.", MyOtherStr,    "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" )

    MyStr = "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z";
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    MyOtherStr = MyStr.GetLowerCase();
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    CHECK_EQUALS( "String lower case.", MyOtherStr,    "a123bcdefghこれわテストです。ijklmnopqr456789stuvwxyzabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )

    return;
}

TEST_CASE(NewEightStringChops, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z";
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    LString MyOtherString = MyStr.RightChop(38);
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    CHECK_EQUALS( "String lower case.", MyOtherString, "IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    MyOtherString.InlineRightChop(1);
    CHECK_EQUALS( "String lower case.", MyOtherString,  "JKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    MyOtherString.InlineRightChop(0);
    CHECK_EQUALS( "String lower case.", MyOtherString,  "JKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )

    MyStr = "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z";
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    MyOtherString = MyStr.LeftChop(38);
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    CHECK_EQUALS( "String lower case.", MyOtherString, "A123BCDEFGHこれわテストです。" )
    MyOtherString.InlineLeftChop(1);
    CHECK_EQUALS( "String lower case.", MyOtherString,  "A" )
    CHECK_EQUALS( "String lower case.", MyStr,         "A123BCDEFGHこれわテストです。IJKLMNOPQR456789STUVWXYZabcdeこれわテストです。fghijklmnopqrstuvwxy0z" )
    MyOtherString.InlineLeftChop(0);
    CHECK_EQUALS( "String lower case.", MyOtherString,  "" )

    return;
}

TEST_CASE(StringUtlity, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "これわテストです。";
    CHECK_EQUALS( "String utility.", MyStr, "これわテストです。" )
    CHECK_TRUE(   "String utility.", Str::IsValidUtf8(MyStr.ToPtr())  )
    CHECK_FALSE(  "String utility.", Str::IsValidAscii(MyStr.ToPtr()) )
    MyStr = "Abc";
    CHECK_EQUALS( "String utility.", MyStr, "Abc" )
    CHECK_TRUE(   "String utility.", Str::IsValidAscii(MyStr.ToPtr()) )
    CHECK_TRUE(   "String utility.", Str::IsValidUtf8(MyStr.ToPtr())  )

    return;
}

TEST_CASE(StringPaths, "Lal.Strings")
{
    using namespace Jafg;

    LString MyStr = "";
    CHECK_TRUE(   "String paths.", MyStr.IsEmpty() )

    MyStr /= "abc";
    CHECK_EQUALS( "String paths.", MyStr, "abc" )
    MyStr /= "def";
    CHECK_EQUALS( "String paths.", MyStr, "abc/def" )
    MyStr /= "ghi";
    CHECK_EQUALS( "String paths.", MyStr, "abc/def/ghi" )
    MyStr /= "jkl";
    CHECK_EQUALS( "String paths.", MyStr, "abc/def/ghi/jkl" )
    MyStr.Add('/');
    CHECK_EQUALS( "String paths.", MyStr, "abc/def/ghi/jkl/" )
    MyStr /= "mno";
    CHECK_EQUALS( "String paths.", MyStr, "abc/def/ghi/jkl/mno" )

    return;
}

TEST_CASE(StringLoops, "Lal.Strings")
{
    using namespace Jafg;

    i32 i = 0;

    LString MyStr;
    for (const LString::T x: MyStr)
    {
        ++i;
    }
    CHECK_EQUALS( "String loops.", i, 0 )

    MyStr = "";
    for (const LString::T x: MyStr)
    {
        ++i;
    }
    CHECK_EQUALS( "String loops.", i, 0 )

    MyStr = "A";
    for (const LString::T x: MyStr)
    {
        ++i;
    }
    CHECK_EQUALS( "String loops.", i, 1 )
    i = 0;

    MyStr = "Abcdefgh";
    for (const LString::T x: MyStr)
    {
        ++i;
    }
    CHECK_EQUALS( "String loops.", i, 8 )
    i = 0;

    MyStr.Empty();
    CHECK_EQUALS( "String loops.", MyStr, "" )

    for (const LString::T x: MyStr)
    {
        ++i;
    }
    CHECK_EQUALS( "String loops.", i, 0 )

    return;
}

TEST_CASE(StringView, "Lal.Strings")
{
    using namespace Jafg;

    LStringView MyStrView("Hello World!");
    QUICK_CHECK_EQUALS(MyStrView,           "Hello World!")
    QUICK_CHECK_EQUALS(MyStrView.GetSize(),             12)
    QUICK_CHECK_EQUALS(MyStrView.GetRuneCount(),        12)
    QUICK_CHECK_EQUALS(MyStrView.GetCharacterCount(),   12)
    LStringView::SizeType i = 0;
    for (const LStringView::T x : MyStrView)
    {
        CHECK_EQUALS( "String view.", x, MyStrView[i] )
        ++i;
    }
    CHECK_EQUALS( "String view.", i, 12 )

    MyStrView = "Abcdefgh";
    CHECK_EQUALS( "String view.", MyStrView,           "Abcdefgh" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           8 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      8 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 8 )
    CHECK_EQUALS( "String view.", MyStrView[0],           *"A" )
    CHECK_EQUALS( "String view.", MyStrView[1],           *"b" )
    CHECK_EQUALS( "String view.", MyStrView[2],           *"c" )
    CHECK_EQUALS( "String view.", MyStrView[3],           *"d" )
    CHECK_EQUALS( "String view.", MyStrView[4],           *"e" )
    CHECK_EQUALS( "String view.", MyStrView[5],           *"f" )
    CHECK_EQUALS( "String view.", MyStrView[6],           *"g" )
    CHECK_EQUALS( "String view.", MyStrView[7],           *"h" )
    CHECK_EQUALS( "String view.", MyStrView[0],            'A' )
    CHECK_EQUALS( "String view.", MyStrView[1],            'b' )
    CHECK_EQUALS( "String view.", MyStrView[2],            'c' )
    CHECK_EQUALS( "String view.", MyStrView[3],            'd' )
    CHECK_EQUALS( "String view.", MyStrView[4],            'e' )
    CHECK_EQUALS( "String view.", MyStrView[5],            'f' )
    CHECK_EQUALS( "String view.", MyStrView[6],            'g' )
    CHECK_EQUALS( "String view.", MyStrView[7],            'h' )
    i = 0;
    for (const LStringView::T x : MyStrView)
    {
        CHECK_EQUALS( "String view.", x, MyStrView[i] )
        ++i;
    }
    CHECK_EQUALS( "String view.", i, 8 )

    QUICK_CHECK_TRUE( MyStrView.MoveDataPointerUp()                )
    CHECK_EQUALS( "String view.", MyStrView,             "bcdefgh" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           7 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      7 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 7 )
    QUICK_CHECK_TRUE( MyStrView.MoveDataPointerUp(3)               )
    CHECK_EQUALS( "String view.", MyStrView,                "efgh" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           4 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      4 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 4 )
    QUICK_CHECK_TRUE( MyStrView.MoveDataPointerUp(MyStrView.GetEnd() - 1) )
    CHECK_EQUALS( "String view.", MyStrView,                   "h" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           1 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      1 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 1 )

    MyStrView = "";
    CHECK_EQUALS( "String view.", MyStrView, "" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           0 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      0 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 0 )
    i = 0;
    for (const LStringView::T x : MyStrView)
    {
        CHECK_EQUALS( "String view.", x, MyStrView[i] )
        ++i;
    }
    CHECK_EQUALS( "String view.", i, 0 )

    MyStrView.Invalidate();
    CHECK_EQUALS( "String view.", MyStrView, "" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           0 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      0 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 0 )
    i = 0;
    for (const LStringView::T x : MyStrView)
    {
        CHECK_EQUALS( "String view.", x, MyStrView[i] )
        ++i;
    }
    CHECK_EQUALS( "String view.", i, 0 )

    MyStrView = "A";
    CHECK_EQUALS( "String view.", MyStrView, "A" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           1 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      1 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 1 )
    CHECK_EQUALS( "String view.", MyStrView[0],           *"A" )
    CHECK_EQUALS( "String view.", MyStrView[0],            'A' )
    i = 0;
    for (const LStringView::T x : MyStrView)
    {
        CHECK_EQUALS( "String view.", x, MyStrView[i] )
        ++i;
    }
    CHECK_EQUALS( "String view.", i, 1 )
    QUICK_CHECK_TRUE( MyStrView.MoveDataPointerUp(500) )
    CHECK_EQUALS( "String view.", MyStrView, "" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),           0 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),      0 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(), 0 )

    LString MyStr = "Hello World!";
    QUICK_CHECK_EQUALS(MyStr, "Hello World!" )
    MyStrView = MyStr;
    CHECK_EQUALS( "String view.", MyStrView,           "Hello World!" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),             13 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),        12 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(),   12 )
    CHECK_EQUALS( "String view.", MyStrView[0],           *"H" )
    CHECK_EQUALS( "String view.", MyStrView[1],           *"e" )
    CHECK_EQUALS( "String view.", MyStrView[2],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[3],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[4],           *"o" )
    CHECK_EQUALS( "String view.", MyStrView[5],           *" " )
    CHECK_EQUALS( "String view.", MyStrView[6],           *"W" )
    CHECK_EQUALS( "String view.", MyStrView[7],           *"o" )
    CHECK_EQUALS( "String view.", MyStrView[8],           *"r" )
    CHECK_EQUALS( "String view.", MyStrView[9],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[10],          *"d" )
    CHECK_EQUALS( "String view.", MyStrView[11],          *"!" )
    CHECK_EQUALS( "String view.", MyStrView[0],            'H' )
    CHECK_EQUALS( "String view.", MyStrView[1],            'e' )
    CHECK_EQUALS( "String view.", MyStrView[2],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[3],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[4],            'o' )
    CHECK_EQUALS( "String view.", MyStrView[5],            ' ' )
    CHECK_EQUALS( "String view.", MyStrView[6],            'W' )
    CHECK_EQUALS( "String view.", MyStrView[7],            'o' )
    CHECK_EQUALS( "String view.", MyStrView[8],            'r' )
    CHECK_EQUALS( "String view.", MyStrView[9],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[10],           'd' )
    CHECK_EQUALS( "String view.", MyStrView[11],           '!' )
    MyStr.Append("Abcdefgh");
    CHECK_EQUALS( "String view.", MyStrView,          "Hello World!A" )
    CHECK_EQUALS( "String view.", MyStrView.GetSize(),             13 )
    CHECK_EQUALS( "String view.", MyStrView.GetRuneCount(),        13 )
    CHECK_EQUALS( "String view.", MyStrView.GetCharacterCount(),   13 )
    CHECK_EQUALS( "String view.", MyStrView[0],           *"H" )
    CHECK_EQUALS( "String view.", MyStrView[1],           *"e" )
    CHECK_EQUALS( "String view.", MyStrView[2],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[3],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[4],           *"o" )
    CHECK_EQUALS( "String view.", MyStrView[5],           *" " )
    CHECK_EQUALS( "String view.", MyStrView[6],           *"W" )
    CHECK_EQUALS( "String view.", MyStrView[7],           *"o" )
    CHECK_EQUALS( "String view.", MyStrView[8],           *"r" )
    CHECK_EQUALS( "String view.", MyStrView[9],           *"l" )
    CHECK_EQUALS( "String view.", MyStrView[10],          *"d" )
    CHECK_EQUALS( "String view.", MyStrView[11],          *"!" )
    CHECK_EQUALS( "String view.", MyStrView[12],          *"A" )
    CHECK_EQUALS( "String view.", MyStrView[0],            'H' )
    CHECK_EQUALS( "String view.", MyStrView[1],            'e' )
    CHECK_EQUALS( "String view.", MyStrView[2],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[3],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[4],            'o' )
    CHECK_EQUALS( "String view.", MyStrView[5],            ' ' )
    CHECK_EQUALS( "String view.", MyStrView[6],            'W' )
    CHECK_EQUALS( "String view.", MyStrView[7],            'o' )
    CHECK_EQUALS( "String view.", MyStrView[8],            'r' )
    CHECK_EQUALS( "String view.", MyStrView[9],            'l' )
    CHECK_EQUALS( "String view.", MyStrView[10],           'd' )
    CHECK_EQUALS( "String view.", MyStrView[11],           '!' )
    CHECK_EQUALS( "String view.", MyStrView[12],           'A' )

    return;
}
