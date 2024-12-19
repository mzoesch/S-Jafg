// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

#include "Containers/StringUtility.h"
#include "Containers/StringConversions.h"

#pragma region Simple String

TEST_CASE(SimpleNewSimpleStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr;
    CHECK_NOT_NULL( "Implicitly constructed string.", MyStr.ToC()                    )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                      "" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                    "\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyStr,                     "A" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetSize(),             0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetByteSize(),         0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetCharacterCount(),   0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetRuneCount(),        0 )
    CHECK_EQUALS(   "Implicitly constructed string.", *MyStr.Peek(),            '\0' )

    MyStr = "Abc";
    CHECK_NOT_NULL( "Assigned string.", MyStr.ToC()                       )
    CHECK_EQUALS(   "Assigned string.", MyStr,                      "Abc" )
    CHECK_EQUALS(   "Assigned string.", MyStr,                    "Abc\0" )
    CHECK_LESS(     "Assigned string.", MyStr,                      "Abd" )
    CHECK_GREATER(  "Assigned string.", MyStr,                      "Abb" )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetSize(),                4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetByteSize(),            4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetCharacterCount(),      3 )
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
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(),     4 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          4 )

    LSimpleString MyOtherStr = "efgh";
    CHECK_NOT_NULL( "Implicitly constructed string.", MyOtherStr.ToC()                  )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,                "efgh" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,              "efgh\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyOtherStr,                "efgi" )
    CHECK_GREATER(  "Implicitly constructed string.", MyOtherStr,                "efgg" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetSize(),           5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetByteSize(),       5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetCharacterCount(), 4 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetRuneCount(),      4 )

    MyStr += MyOtherStr;
    CHECK_NOT_NULL( "Appended string.", MyStr.ToC()                  )
    CHECK_EQUALS(   "Appended string.", MyStr,            "Abcdefgh" )
    CHECK_EQUALS(   "Appended string.", MyStr,          "Abcdefgh\0" )
    CHECK_LESS(     "Appended string.", MyStr,             "Abcdefi" )
    CHECK_GREATER(  "Appended string.", MyStr,             "Abcdefg" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),           9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),       9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(), 8 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),      8 )

    return;
}

TEST_CASE(RawLiteraLSimpleStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr  = "Abc";
    LSimpleString MyStr2 = "Abc";

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


TEST_CASE(SimpleNewSimpleStringManipulations, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr;
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("A");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("B");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("C");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'C' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      2 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'B' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      1 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'A' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )

    MyStr.Append("ABC");

    MyStr.Reset(0);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("ABC");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Empty();
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Reserve(5);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    return;
}

TEST_CASE(NewSimpleStringCopyingAndMoving, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr1;
    LSimpleString MyStr2 = MyStr1;

    CHECK_EQUALS(   "Copy constructed string.", MyStr1,                 MyStr2 )
    CHECK_EQUALS(   "Copy constructed string.", MyStr1.ToC(),     MyStr2.ToC() )

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

    LSimpleString MyStr3 = std::move(MyStr1);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                 "\0" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr3.ToC(),   MyStr1.ToC() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )

    MyStr2.MoveFrom(std::move(MyStr3));
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

    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetRuneCount(),      8 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetRuneCount(),      8 )

    return;
}

TEST_CASE(NewSimpleStringStartsWith, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr = "";
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

TEST_CASE(NewSimpleStringCount, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
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
    CHECK_EQUALS( "String count.", MyStr, "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    MyStr.Empty();
    CHECK_EQUALS( "String count.", MyStr, "" )
    CHECK_EQUALS( "String count.", MyStr.Count("A"), 0 )

    LSimpleString MyOtherStr;
    CHECK_EQUALS( "String count.", MyOtherStr, "" )
    CHECK_EQUALS( "String count.", MyOtherStr.Count("A"), 0 )

    return;
}

TEST_CASE(NewSimpleStringReplace, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr, "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    MyStr.Replace("A", "X");
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

    LSimpleString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find first.", MyStr.FindFirst("A"),  0 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("a"),  1 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("b"),  4 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("c"),  5 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("d"),  8 )
    CHECK_EQUALS( "String find first.", MyStr.FindFirst("e"), -1 )

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

    LSimpleString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String find second.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find second.", MyStr.FindSecond("A"),  2 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("a"), -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("b"),  9 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("c"), 10 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("d"), -1 )
    CHECK_EQUALS( "String find second.", MyStr.FindSecond("e"), -1 )

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

    LSimpleString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String find last.", MyStr,   "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String find last.", MyStr.FindLast("A"), 12 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("a"),  1 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("b"),  9 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("c"), 10 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("d"),  8 )
    CHECK_EQUALS( "String find last.", MyStr.FindLast("e"), -1 )

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

    LSimpleString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghijklmn" )

    MyStr.InlineCut(10);
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghij" )

    LSimpleString MyOtherString = MyStr.Cut(5);
    CHECK_EQUALS( "String cut.", MyStr,         "abcdefghij" )
    CHECK_EQUALS( "String cut.", MyOtherString, "abcde"      )

    MyOtherString = MyOtherString.Cut(0);
    CHECK_EQUALS( "String cut.", MyOtherString, "" )

    return;
}

TEST_CASE(NewSimpleStringSubs, "Lal.Strings")
{
    using namespace Jafg;

    LSimpleString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineSub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineSubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    LSimpleString MyOtherString = MyStr.Sub(5, 2);
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

#pragma endregion Simple String

#pragma region Uni-Eight String

TEST_CASE(SimpleNewEightStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr;
    CHECK_NOT_NULL( "Implicitly constructed string.", MyStr.ToC()                    )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                      "" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr,                    "\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyStr,                     "A" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetByteSize(),         0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetCharacterCount(),   0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetCharacterCount(),   0 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyStr.GetRuneCount(),        0 )

    MyStr = "Abc";
    CHECK_NOT_NULL( "Assigned string.", MyStr.ToC()                       )
    CHECK_EQUALS(   "Assigned string.", MyStr,                      "Abc" )
    CHECK_EQUALS(   "Assigned string.", MyStr,                    "Abc\0" )
    CHECK_LESS(     "Assigned string.", MyStr,                      "Abd" )
    CHECK_GREATER(  "Assigned string.", MyStr,                      "Abb" )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetSize(),                4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetByteSize(),            4 )
    CHECK_EQUALS(   "Assigned string.", MyStr.GetCharacterCount(),      3 )
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
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           5 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(),     4 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          4 )

    LEightString MyOtherStr = "efgh";
    CHECK_NOT_NULL( "Implicitly constructed string.", MyOtherStr.ToC()                  )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,                "efgh" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr,              "efgh\0" )
    CHECK_LESS(     "Implicitly constructed string.", MyOtherStr,                "efgi" )
    CHECK_GREATER(  "Implicitly constructed string.", MyOtherStr,                "efgg" )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetSize(),           5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetByteSize(),       5 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetCharacterCount(), 4 )
    CHECK_EQUALS(   "Implicitly constructed string.", MyOtherStr.GetRuneCount(),      4 )

    MyStr += MyOtherStr;
    CHECK_NOT_NULL( "Appended string.", MyStr.ToC()                      )
    CHECK_EQUALS(   "Appended string.", MyStr,                "Abcdefgh" )
    CHECK_EQUALS(   "Appended string.", MyStr,              "Abcdefgh\0" )
    CHECK_LESS(     "Appended string.", MyStr,                 "Abcdefi" )
    CHECK_GREATER(  "Appended string.", MyStr,                 "Abcdefg" )
    CHECK_EQUALS(   "Appended string.", MyStr.GetSize(),               9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetByteSize(),           9 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetCharacterCount(),     8 )
    CHECK_EQUALS(   "Appended string.", MyStr.GetRuneCount(),          8 )

    return;
}

TEST_CASE(RawLiteralNewEightStringOperations, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr  = "Abc";
    LEightString MyStr2 = "Abc";

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

TEST_CASE(SimpleNewEightStringManipulations, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr;
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS( "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("A");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      1 )

    MyStr.Append("B");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      2 )

    MyStr.Append("C");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Append("\0");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'C' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "AB" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                "AB\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       3 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      2 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'B' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                   "A" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                 "A\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       2 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 1 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      1 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),           'A' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )
    MyStr.Pop();
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Not so simple manipulations.", MyStr.GetRuneCount(),      0 )
    CHECK_EQUALS(  "Not so simple manipulations.", *MyStr.Peek(),          '\0' )

    MyStr.Append("ABC");

    MyStr.Reset(0);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       1 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Append("ABC");
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                 "ABC" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,               "ABC\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       4 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 3 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      3 )

    MyStr.Empty();
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    MyStr.Reserve(5);
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                    "" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr,                  "\0" )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetSize(),           0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetByteSize(),       0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetCharacterCount(), 0 )
    CHECK_EQUALS(  "Simple Manipulations.", MyStr.GetRuneCount(),      0 )

    return;
}

TEST_CASE(NewEightStringCopyingAndMoving, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr1;
    LEightString MyStr2 = MyStr1;

    CHECK_EQUALS(   "Copy constructed string.", MyStr1,                 MyStr2 )
    CHECK_EQUALS(   "Copy constructed string.", MyStr1.ToC(),     MyStr2.ToC() )

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

    LEightString MyStr3 = std::move(MyStr1);
    CHECK_EQUALS(       "Move constructed string.", MyStr3,           "Abcdefgh" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                   "" )
    CHECK_EQUALS(       "Move constructed string.", MyStr1,                 "\0" )
    CHECK_NOT_EQUALS(   "Move constructed string.", MyStr3.ToC(),   MyStr1.ToC() )
    CHECK_EQUALS(       "Move constructed string.", MyStr2,                   "" )

    MyStr2.MoveFrom(std::move(MyStr3));
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

    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr2.GetRuneCount(),      8 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetSize(),           9 )
    CHECK_EQUALS(       "Move constructed string.", MyStr3.GetRuneCount(),      8 )

    return;
}

TEST_CASE(NewEightStringStartsWith, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr = "";
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

    LEightString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
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

    LEightString MyOtherStr;
    CHECK_EQUALS( "String count.", MyOtherStr,           "" )
    CHECK_EQUALS( "String count.", MyOtherStr.Count("A"), 0 )

    return;
}

TEST_CASE(NewEightStringReplace, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
    CHECK_EQUALS( "String count.", MyStr,     "AaAAbcAAdbcAA" )

    CHECK_EQUALS( "String count.", MyStr.Count("A"),    7 )
    MyStr.Replace("A", "X");
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

    LEightString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
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

    LEightString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
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

    LEightString MyStr = "AaAAbcAAdbcAA"; // A:7; a:1; b:2; c:2; d:1
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

    LEightString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghijklmn" )

    MyStr.InlineCut(10);
    CHECK_EQUALS( "String cut.", MyStr, "abcdefghij" )

    LEightString MyOtherString = MyStr.Cut(5);
    CHECK_EQUALS( "String cut.", MyStr,         "abcdefghij" )
    CHECK_EQUALS( "String cut.", MyOtherString, "abcde"      )

    MyOtherString = MyOtherString.Cut(0);
    CHECK_EQUALS( "String cut.", MyOtherString, "" )

    return;
}

TEST_CASE(NewEightStringSubs, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )

    MyStr.InlineSub(5, 2);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr, "abcdefghijklmn" )
    MyStr.InlineSubIdx(5, 7);
    CHECK_EQUALS( "String sub.", MyStr, "fg" )

    MyStr = "abcdefghijklmn";
    CHECK_EQUALS( "String sub.", MyStr,         "abcdefghijklmn" )
    LEightString MyOtherString = MyStr.Sub(5, 2);
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

    LEightString MyStr = "これわテストです。";
    CHECK_EQUALS( "Advanced characters.", MyStr, "これわテストです。" )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),           28 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetByteSize(),       28 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(), 27 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),       9 )

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

    MyStr.Replace("で", "こ");
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("")          )
    CHECK_FALSE( "Advanced characters.", MyStr.StartsWith("\0")        )
    CHECK_EQUALS("Advanced characters.", MyStr, "こ"                   )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("で"),          0 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("こ"),          1 )
    CHECK_EQUALS("Advanced characters.", MyStr.Count("て"),          0 )

    MyStr = "これわテストです。";
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("こ"),  0 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("れ"),  1 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("わ"),  2 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("テ"),  3 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("ス"),  4 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("ト"),  5 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("で"),  6 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("す"),  7 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("。"),  8 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindFirst("あ"), -1 )

    MyStr = "こふふこああこあふここ"; // こ:5; ふ:3; あ:3
    CHECK_EQUALS( "Advanced characters.", MyStr,  "こふふこああこあふここ" )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetSize(),           34 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetByteSize(),       34 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetCharacterCount(), 33 )
    CHECK_EQUALS( "Advanced characters.", MyStr.GetRuneCount(),      11 )

    CHECK_EQUALS( "Advanced characters.", MyStr.Count("こ"),         5 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("こ"),    3 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("ふ"),    2 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindSecond("あ"),    5 )

    CHECK_EQUALS( "Advanced characters.", MyStr, "こふふこああこあふここ" )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("こ"),     10 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("ふ"),      8 )
    CHECK_EQUALS( "Advanced characters.", MyStr.FindLast("あ"),      7 )

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
    MyStr.InlineCut(3);
    CHECK_EQUALS("Advanced characters.", MyStr,           "これわ" )
    MyStr = "これわテストです。";
    CHECK_EQUALS("Advanced characters.", MyStr, "これわテストです。" )
    LEightString MyOtherString = MyStr.Cut(5);
    CHECK_EQUALS("Advanced characters.", MyStr,         "これわテストです。" )
    CHECK_EQUALS("Advanced characters.", MyOtherString,       "これわテス"  )
    MyOtherString = MyStr.Cut(0);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "" )
    MyOtherString = MyStr.Cut(1);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "こ" )
    MyOtherString = MyStr;
    CHECK_EQUALS("Advanced characters.", MyOtherString, "これわテストです。" )
    MyOtherString.InlineSub(5, 2);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "トで" )
    MyOtherString = MyStr;
    CHECK_EQUALS("Advanced characters.", MyOtherString, "これわテストです。" )
    MyOtherString.InlineSubIdx(5, 8);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "トです" )
    MyOtherString = MyStr.Sub(2, 5);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテストで" )
    MyOtherString = MyStr.Sub(2, 7);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテストです。" )
    MyOtherString = MyStr.SubIdx(2, 5);
    CHECK_EQUALS("Advanced characters.", MyOtherString, "わテス" )
    MyStr.InlineSub(2, 5);
    CHECK_EQUALS("Advanced characters.", MyStr, "わテストで" )

    MyStr = "これ";
    MyStr = LEightString::SprintF("{}わ{}です{}", MyStr.ToPtr(), "テスト", "。");
    CHECK_EQUALS("Advanced characters.", MyStr, "これわテストです。" )

    return;
}

TEST_CASE(StringUtlity, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr = "これわテストです。";
    CHECK_EQUALS( "String utility.", MyStr, "これわテストです。" )
    CHECK_TRUE(   "String utility.", Str::IsValidUtf8(MyStr.ToC())  )
    CHECK_FALSE(  "String utility.", Str::IsValidAscii(MyStr.ToC()) )
    MyStr = "Abc";
    CHECK_EQUALS( "String utility.", MyStr, "Abc" )
    CHECK_TRUE(   "String utility.", Str::IsValidAscii(MyStr.ToC()) )
    CHECK_TRUE(   "String utility.", Str::IsValidUtf8(MyStr.ToC())  )

    return;
}

TEST_CASE(StringConversion, "Lal.Strings")
{
    using namespace Jafg;

    LEightString MyStr = "1234567890Abcdefg";
    CHECK_EQUALS( "String conversion.", MyStr, "1234567890Abcdefg" )
    LSimpleString MySimpleStr = Str::ToSimpleString(MyStr);
    CHECK_EQUALS( "String conversion.", MySimpleStr, "1234567890Abcdefg" )

    MyStr = "123Abcテスト";
    CHECK_EQUALS( "String conversion.", MyStr, "123Abcテスト" )
    MySimpleStr = Str::ToSimpleString(MyStr);
    CHECK_EQUALS( "String conversion.", MySimpleStr, "123Abc" )

    return;
}

#pragma endregion Uni-Eight String
