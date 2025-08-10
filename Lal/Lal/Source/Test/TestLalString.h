// Copyright mzoesch. All rights reserved.

#pragma once

#include "Containers/LalString.h"

TEST_CASE(EmptyStringOperations, "Lal.Containers")
{
    LString String;
    LBigString BigString;
    LSmallString<25> SmallString;
    LStringView StringView;
    LMutableStringView MutableStringView;

    QUICK_CHECK_EQUALS(String.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(BigString.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallString.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(StringView.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetRuneCount(), 0ul)

    QUICK_CHECK_EQUALS(String.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(BigString.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallString.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(StringView.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetCharacterCount(), 0ul)

    QUICK_CHECK_EQUALS(String.GetAllocatedByteSize(), 8ul)
    QUICK_CHECK_EQUALS(BigString.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(SmallString.GetAllocatedByteSize(), 25ul)
    QUICK_CHECK_EQUALS(StringView.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetAllocatedByteSize(), 0ul)

    return;
}

TEST_CASE(StringAssingOperations, "Lal.Containers")
{
    LString String1 { "Hello, World!" };
    LBigString BigString1 { "Hello, World!" };
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(BigString1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String1, "Hello, World!")
    QUICK_CHECK_EQUALS(BigString1, "Hello, World!")
    QUICK_CHECK_NOT_EQUALS(String1, "Hello, World.")
    QUICK_CHECK_NOT_EQUALS(BigString1, "Hello, World.")
    QUICK_CHECK_NOT_EQUALS(String1, "Hello, World! ")
    QUICK_CHECK_NOT_EQUALS(BigString1, "Hello, World! ")

    LString String2;
    LBigString BigString2;
    String2.Assign(String1);
    BigString2.Assign(BigString1);
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(BigString2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String2, "Hello, World!")
    QUICK_CHECK_EQUALS(BigString2, "Hello, World!")
    QUICK_CHECK_EQUALS(String1, String2)
    QUICK_CHECK_EQUALS(BigString1, BigString2)

    String2.Assign("");
    BigString2.Assign("");
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(BigString2.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(static_cast<const void*>(String2.GetAllocator().GetDataPointer()), static_cast<const void*>(String2.GetAllocator().GetAllocator()._Data))
    QUICK_CHECK_EQUALS(static_cast<const void*>(BigString2.GetAllocator().GetDataPointer()), nullptr)

    BigString1.Assign(String1);
    QUICK_CHECK_EQUALS(BigString1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(BigString1, "Hello, World!")

    return;
}

TEST_CASE(StringMemoryReuse, "Lal.Strings")
{
    LString String { "Hello, World! This is a long string." };
    LBigString BigString { "Hello, World! This is a long string." };

    const LString::SizeType StringLength { String.GetRuneCount() };
    const LString::ConstPointer StringPtr { String.ToPtr() };
    const LBigString::SizeType BigStringLength { BigString.GetRuneCount() };
    const LBigString::ConstPointer BigStringPtr { BigString.ToPtr() };

    String = "Hello, World! Is this a long string?";
    BigString = "Hello, World! Is this a long string?";

    QUICK_CHECK_EQUALS(StringLength, String.GetRuneCount())
    QUICK_CHECK_EQUALS(static_cast<const void*>(StringPtr), static_cast<const void*>(String.ToPtr()))

    QUICK_CHECK_EQUALS(BigStringLength, BigString.GetRuneCount())
    QUICK_CHECK_EQUALS(static_cast<const void*>(BigStringPtr), static_cast<const void*>(BigString.ToPtr()))

    return;
}

TEST_CASE(StringAppendOperations, "Lal.Containers")
{
    LString String1 { "Hello" };
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 5ul)
    QUICK_CHECK_EQUALS(String1, "Hello")

    String1.Append(',');
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 6ul)
    QUICK_CHECK_EQUALS(String1, "Hello,")

    String1.Append(" ");
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 7ul)
    QUICK_CHECK_EQUALS(String1, "Hello, ")

    String1.Append("");
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 7ul)
    QUICK_CHECK_EQUALS(String1, "Hello, ")

    LStringView View { "World" };
    String1.Append(View);
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 12ul)
    QUICK_CHECK_EQUALS(String1, "Hello, World")

    LString::Iterator It { String1.AppendAt(String1.end(), '!') };
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String1, "Hello, World!")
    QUICK_CHECK_EQUALS(*It, '!')
    QUICK_CHECK_EQUALS(It, --String1.end())
    QUICK_CHECK_EQUALS(It.Cursor, String1.end_ptr() - 1)

    It = String1.AppendAt(String1.end(), "");
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String1, "Hello, World!")
    QUICK_CHECK_EQUALS(It, String1.end())
    QUICK_CHECK_EQUALS(It.Cursor, String1.end_ptr())

    LString String2 { String1 + " How are you?" };
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(String2, "Hello, World! How are you?")

    String2 = { LString{"Hello"} + ',' + " " + "World" + '!' };
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String2, "Hello, World!")

    return;
}

TEST_CASE(StringPathOperations, "Lal.Containers")
{
    LString String { "/home/user" };
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 10ul)
    QUICK_CHECK_EQUALS(String, "/home/user")

    String /= "Development";
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 22ul)
    QUICK_CHECK_EQUALS(String, "/home/user/Development")

    String.Append('/');
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 23ul)
    QUICK_CHECK_EQUALS(String, "/home/user/Development/")

    String = String / "/Rust/" / "/Sucks.sln";
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 37ul)
    QUICK_CHECK_EQUALS(String, "/home/user/Development/Rust/Sucks.sln")

    return;
}

TEST_CASE(StringRemovalOperations, "Lal.Containers")
{
    LString String { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 36ul)
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String.RemoveAt(0ul);
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 35ul)
    QUICK_CHECK_EQUALS(String, "ello, World! This is a long string.")
    String.RemoveAt(String.begin());
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 34ul)
    QUICK_CHECK_EQUALS(String, "llo, World! This is a long string.")
    String.RemoveAt(String.begin_ptr());
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 33ul)
    QUICK_CHECK_EQUALS(String, "lo, World! This is a long string.")
    String.RemoveAt(++String.begin());
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 32ul)
    QUICK_CHECK_EQUALS(String, "l, World! This is a long string.")
    String.RemoveAt(--String.end());
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 31ul)
    QUICK_CHECK_EQUALS(String, "l, World! This is a long string")
    String.RemoveAt(String.end_ptr() - 1);
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 30ul)
    QUICK_CHECK_EQUALS(String, "l, World! This is a long strin")

    QUICK_CHECK_TRUE(String.RemoveOnce('o'))
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 29ul)
    QUICK_CHECK_EQUALS(String, "l, Wrld! This is a long strin")

    QUICK_CHECK_EQUALS(String.Remove('s'), 3ul)
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(String, "l, Wrld! Thi i a long trin")

    QUICK_CHECK_EQUALS(String.Remove(" "), 6ul)
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 20ul)
    QUICK_CHECK_EQUALS(String, "l,Wrld!Thiialongtrin")

    LStringView View = { "aabbabababaabbbaabbbaaabba" };
    String.Assign(View);
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(String, "aabbabababaabbbaabbbaaabba")

    QUICK_CHECK_EQUALS(String.Remove("aa"), 4ul)
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 18ul)
    QUICK_CHECK_EQUALS(String, "bbabababbbbbbbabba")

    return;
}

TEST_CASE(StringFindOperations, "Lal.Containers")
{
    LString String { "bbabababbbbbbbabba" };
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 18ul)
    QUICK_CHECK_EQUALS(String, "bbabababbbbbbbabba")

    QUICK_CHECK_EQUALS(String.FindFirst('b'), String.begin())
    QUICK_CHECK_EQUALS(String.FindFirst('a'), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindFirst('c'), String.end())

    QUICK_CHECK_EQUALS(String.FindNth('b', 1), String.begin())
    QUICK_CHECK_EQUALS(String.FindNth('a', 1), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindNth('c', 1), String.end())

    QUICK_CHECK_EQUALS(String.FindSecond('b'), String.begin() + 1)
    QUICK_CHECK_EQUALS(String.FindSecond('a'), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindSecond('c'), String.end())

    QUICK_CHECK_EQUALS(String.FindNth('b', 2), String.begin() + 1)
    QUICK_CHECK_EQUALS(String.FindNth('a', 2), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindNth('c', 2), String.end())

    QUICK_CHECK_EQUALS(String.FindNth('b', 3), String.begin() + 3)
    QUICK_CHECK_EQUALS(String.FindNth('a', 3), String.begin() + 6)
    QUICK_CHECK_EQUALS(String.FindNth('c', 3), String.end())

    QUICK_CHECK_EQUALS(String.FindNth('b', 12), String.begin() + 15)
    QUICK_CHECK_EQUALS(String.FindNth('a', 4), String.begin() + 14)
    QUICK_CHECK_EQUALS(String.FindNth('b', 13), String.begin() + 16)
    QUICK_CHECK_EQUALS(String.FindNth('a', 5), String.begin() + 17)
    QUICK_CHECK_EQUALS(String.FindNth('b', 14), String.end())
    QUICK_CHECK_EQUALS(String.FindNth('a', 6), String.end())

    QUICK_CHECK_EQUALS(String.FindNth('b', 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth('a', 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth('c', 3), String.end())

    QUICK_CHECK_EQUALS(String.FindFirst("b"), String.begin())
    QUICK_CHECK_EQUALS(String.FindFirst("a"), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindFirst("c"), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("b", 1), String.begin())
    QUICK_CHECK_EQUALS(String.FindNth("a", 1), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindNth("c", 1), String.end())

    QUICK_CHECK_EQUALS(String.FindSecond("b"), String.begin() + 1)
    QUICK_CHECK_EQUALS(String.FindSecond("a"), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindSecond("c"), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("b", 2), String.begin() + 1)
    QUICK_CHECK_EQUALS(String.FindNth("a", 2), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindNth("c", 2), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("b", 3), String.begin() + 3)
    QUICK_CHECK_EQUALS(String.FindNth("a", 3), String.begin() + 6)
    QUICK_CHECK_EQUALS(String.FindNth("c", 3), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("b", 12), String.begin() + 15)
    QUICK_CHECK_EQUALS(String.FindNth("a", 4), String.begin() + 14)
    QUICK_CHECK_EQUALS(String.FindNth("b", 13), String.begin() + 16)
    QUICK_CHECK_EQUALS(String.FindNth("a", 5), String.begin() + 17)
    QUICK_CHECK_EQUALS(String.FindNth("b", 14), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("a", 6), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("b", 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("a", 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("c", 3), String.end())

    QUICK_CHECK_EQUALS(String.FindFirst("bb"), String.begin())
    QUICK_CHECK_EQUALS(String.FindFirst("ab"), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindFirst("cd"), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("bb", 1), String.begin())
    QUICK_CHECK_EQUALS(String.FindNth("ab", 1), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindNth("cd", 1), String.end())

    QUICK_CHECK_EQUALS(String.FindSecond("ba"), String.begin() + 3)
    QUICK_CHECK_EQUALS(String.FindSecond("ab"), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindSecond("cd"), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("ba", 2), String.begin() + 3)
    QUICK_CHECK_EQUALS(String.FindNth("ab", 2), String.begin() + 4)
    QUICK_CHECK_EQUALS(String.FindNth("cd", 2), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("ba", 3), String.begin() + 5)
    QUICK_CHECK_EQUALS(String.FindNth("ab", 3), String.begin() + 6)
    QUICK_CHECK_EQUALS(String.FindNth("cd", 3), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("ba", 4), String.begin() + 13)
    QUICK_CHECK_EQUALS(String.FindNth("ab", 3), String.begin() + 6)
    QUICK_CHECK_EQUALS(String.FindNth("ba", 5), String.begin() + 16)
    QUICK_CHECK_EQUALS(String.FindNth("ab", 4), String.begin() + 14)
    QUICK_CHECK_EQUALS(String.FindNth("ba", 6), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("ab", 5), String.end())

    QUICK_CHECK_EQUALS(String.FindNth("ba", 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("ab", 20), String.end())
    QUICK_CHECK_EQUALS(String.FindNth("cd", 3), String.end())

    QUICK_CHECK_TRUE(String.StartsWith('b'))
    QUICK_CHECK_FALSE(String.StartsWith('a'))
    QUICK_CHECK_FALSE(String.StartsWith('c'))
    QUICK_CHECK_TRUE(String.StartsWith("b"))
    QUICK_CHECK_FALSE(String.StartsWith("a"))
    QUICK_CHECK_FALSE(String.StartsWith("c"))
    QUICK_CHECK_TRUE(String.StartsWith("bb"))
    QUICK_CHECK_FALSE(String.StartsWith("ab"))
    QUICK_CHECK_FALSE(String.StartsWith("cd"))
    QUICK_CHECK_TRUE(String.StartsWith("bbabababbbbbb"))
    QUICK_CHECK_TRUE(String.StartsWith("bbabababbbbbbbabb"))
    QUICK_CHECK_TRUE(String.StartsWith("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(String.StartsWith("bbabababbbbbbbabbaa"))

    QUICK_CHECK_TRUE(String.EndsWith('a'))
    QUICK_CHECK_FALSE(String.EndsWith('b'))
    QUICK_CHECK_FALSE(String.EndsWith('c'))
    QUICK_CHECK_TRUE(String.EndsWith("a"))
    QUICK_CHECK_FALSE(String.EndsWith("b"))
    QUICK_CHECK_FALSE(String.EndsWith("c"))
    QUICK_CHECK_TRUE(String.EndsWith("ba"))
    QUICK_CHECK_FALSE(String.EndsWith("ab"))
    QUICK_CHECK_FALSE(String.EndsWith("cd"))
    QUICK_CHECK_TRUE(String.EndsWith("bbabba"))
    QUICK_CHECK_TRUE(String.EndsWith("babababbbbbbbabba"))
    QUICK_CHECK_TRUE(String.EndsWith("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(String.EndsWith("abbabababbbbbbbabba"))

    QUICK_CHECK_EQUALS(String.FindLast('a'), String.end() - 1)
    QUICK_CHECK_EQUALS(String.FindLast('b'), String.end() - 2)
    QUICK_CHECK_EQUALS(String.FindLast("ba"), String.end() - 2)
    QUICK_CHECK_EQUALS(String.FindLast("bba"), String.end() - 3)
    QUICK_CHECK_EQUALS(String.FindLast('c'), String.end())
    QUICK_CHECK_EQUALS(String.FindLast("c"), String.end())
    QUICK_CHECK_EQUALS(String.FindLast("bbabababbbbbbbabba"), String.begin())
    QUICK_CHECK_EQUALS(String.FindLast("bbabababbbbbbbabbaa"), String.end())
    QUICK_CHECK_EQUALS(String.FindLast("abbabababbbbbbbabba"), String.end())
    QUICK_CHECK_EQUALS(String.FindNthLast("ab", 4), String.begin() + 2)
    QUICK_CHECK_EQUALS(String.FindNthLast("ab", 3), String.begin() + 4)

    QUICK_CHECK_EQUALS(String.Count('a'), 5ul)
    QUICK_CHECK_EQUALS(String.Count('b'), 13ul)
    QUICK_CHECK_EQUALS(String.Count("a"), 5ul)
    QUICK_CHECK_EQUALS(String.Count("b"), 13ul)
    QUICK_CHECK_EQUALS(String.Count("ba"), 5ul)
    QUICK_CHECK_EQUALS(String.Count("ab"), 4ul)
    QUICK_CHECK_EQUALS(String.Count("baa"), 0ul)
    QUICK_CHECK_EQUALS(String.Count("bab"), 3ul)

    QUICK_CHECK_TRUE(String.Contains('a'))
    QUICK_CHECK_TRUE(String.Contains('b'))
    QUICK_CHECK_FALSE(String.Contains('c'))
    QUICK_CHECK_TRUE(String.Contains("a"))
    QUICK_CHECK_TRUE(String.Contains("b"))
    QUICK_CHECK_FALSE(String.Contains("c"))
    QUICK_CHECK_TRUE(String.Contains("ba"))
    QUICK_CHECK_TRUE(String.Contains("ab"))
    QUICK_CHECK_FALSE(String.Contains("baa"))
    QUICK_CHECK_TRUE(String.Contains("bbabababbbbbbbabb"))
    QUICK_CHECK_TRUE(String.Contains("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(String.Contains("bbabababbbbbbbabbaa"))

    return;
}

TEST_CASE(StringChopsOperatios, "Lal.Containers")
{
    LString String { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(String.GetRuneCount(), 36ul)
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String.InlineCut(0ul, 0ul);
    QUICK_CHECK_EQUALS(String, "")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin_ptr(), String.begin_ptr());
    QUICK_CHECK_EQUALS(String, "")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin(), String.begin());
    QUICK_CHECK_EQUALS(String, "")

    String = "Hello, World! This is a long string.";
    String.InlineCut(0ul, 1ul);
    QUICK_CHECK_EQUALS(String, "H")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin_ptr(), String.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(String, "H")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin(), String.begin() + 1);
    QUICK_CHECK_EQUALS(String, "H")

    String.InlineCut(String.begin(), String.begin());
    QUICK_CHECK_EQUALS(String, "")
    String.InlineCut(String.begin(), String.begin());
    QUICK_CHECK_EQUALS(String, "")
    String.InlineCut(String.begin(), String.begin());
    QUICK_CHECK_EQUALS(String, "")

    String = "Hello, World! This is a long string.";
    String.InlineCut(0ul, String.GetRuneCount());
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin_ptr(), String.end_ptr());
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin(), String.end());
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String = "Hello, World! This is a long string.";
    String.InlineCut(0ul, String.GetRuneCount() - 1);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string")

    String = "Hello, World! This is a long string.";
    String.InlineCut(0ul, String.end_idx() - 1);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin_ptr(), String.end_ptr() - 1);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin(), String.end() - 1);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin(), String.end() - 7);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long ")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin() + 1, String.end());
    QUICK_CHECK_EQUALS(String, "ello, World! This is a long string.")

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.end(), String.end());
    QUICK_CHECK_EQUALS(String, "");

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.end() - 1, String.end());
    QUICK_CHECK_EQUALS(String, ".");

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.end() - 7, String.end());
    QUICK_CHECK_EQUALS(String, "string.");

    String = "Hello, World! This is a long string.";
    String.InlineCut(String.begin() + 14, String.end() - 8);
    QUICK_CHECK_EQUALS(String, "This is a long");

    String = "Hello, World! This is a long string.";
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    LString OtherString;
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.Cut(0ul, 0ul);
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.Cut(String.begin_ptr(), String.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.Cut(String.begin(), String.begin());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.Cut(0ul, 1ul);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = String.Cut(String.begin_ptr(), String.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = String.Cut(String.begin(), String.begin() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = String.Cut(String.begin() + 7, String.end());
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long string.")
    OtherString = String.Cut(String.begin() + 7, String.end() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")
    OtherString = String.Cut(String.begin_ptr() + 7, String.end_ptr() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")
    OtherString = String.Cut(7ul, String.end_idx() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")

    OtherString = String.LeftCut(0ul);
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.LeftCut(String.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = String.LeftCut(String.begin());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = String.LeftCut(1ul);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = String.LeftCut(String.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = String.LeftCut(String.begin() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")

    OtherString = String.LeftCut(String.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftCut(String.end_ptr() - String.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftCut(String.end() - String.begin());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftCut(String.GetRuneCount() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = String.LeftCut(String.end_idx() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = String.LeftCut(String.end_ptr() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = String.LeftCut(String.end());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftCut(String.end_idx());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftCut(String.end_ptr());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")

    OtherString = String.RightCut(0ul);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")

    OtherString = String.LeftChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.LeftChop(1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = String.LeftChop(2);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long strin")
    OtherString = String.LeftChop(String.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = String;
    OtherString.InlineLeftChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString.InlineLeftChop(1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString.InlineLeftChop(2);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long stri")
    OtherString.InlineLeftChop(String.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = String.RightChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = String.RightChop(1);
    QUICK_CHECK_EQUALS(OtherString, "ello, World! This is a long string.")
    OtherString = String.RightChop(2);
    QUICK_CHECK_EQUALS(OtherString, "llo, World! This is a long string.")
    OtherString = String.RightChop(String.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = String;
    OtherString.InlineRightChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString.InlineRightChop(1);
    QUICK_CHECK_EQUALS(OtherString, "ello, World! This is a long string.")
    OtherString.InlineRightChop(2);
    QUICK_CHECK_EQUALS(OtherString, "lo, World! This is a long string.")
    OtherString.InlineRightChop(OtherString.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    return;
}

TEST_CASE(StringLowerUpperOperatios, "Lal.Containers")
{
    LString String { " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };

    String.ToUpper();
    QUICK_CHECK_EQUALS(String, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")
    String.ToLower();
    QUICK_CHECK_EQUALS(String, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")

    String = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    String.ToLower();
    QUICK_CHECK_EQUALS(String, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
    String.ToUpper();
    QUICK_CHECK_EQUALS(String, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")

    String = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    LString Other;
    Other = String.GetLower();
    QUICK_CHECK_EQUALS(Other, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
    Other = String.GetUpper();
    QUICK_CHECK_EQUALS(Other, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")

    return;
}

TEST_CASE(StringPops, "Lal.Containers")
{
    LString String { "Hello, World! This is a long string." };

    QUICK_CHECK_TRUE(String.Pop())
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string")
    QUICK_CHECK_TRUE(String.Pop())
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long strin")
    String.Pop(0);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long strin")
    String.Pop(1);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long stri")
    String.Pop(2);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long st")
    String.Pop(3);
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long")

    LStringView View { String };
    QUICK_CHECK_EQUALS(View, String)
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a long")
    View.Pop();
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a lon")
    View.Pop();
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a lo")
    View.Pop(0);
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a lo")
    View.Pop(1);
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a l")
    View.Pop(2);
    QUICK_CHECK_EQUALS(View, "Hello, World! This is a")
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long")

    String = "Hello, World! This is a long string.";
    View.Assign(String);

    QUICK_CHECK_TRUE(View.Drop())
    QUICK_CHECK_EQUALS(View, "ello, World! This is a long string.")
    QUICK_CHECK_TRUE(View.Drop())
    QUICK_CHECK_EQUALS(View, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(View.Drop(0), 0ul)
    QUICK_CHECK_EQUALS(View, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(View.Drop(1), 1ul)
    QUICK_CHECK_EQUALS(View, "lo, World! This is a long string.")
    QUICK_CHECK_EQUALS(View.Drop(2), 2ul)
    QUICK_CHECK_EQUALS(View, ", World! This is a long string.")
    QUICK_CHECK_EQUALS(View.Drop(99), 31ul)
    QUICK_CHECK_EQUALS(View, "")

    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")
    QUICK_CHECK_TRUE(String.Drop())
    QUICK_CHECK_EQUALS(String, "ello, World! This is a long string.")
    QUICK_CHECK_TRUE(String.Drop())
    QUICK_CHECK_EQUALS(String, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(String.Drop(0), 0ul)
    QUICK_CHECK_EQUALS(String, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(String.Drop(1), 1ul)
    QUICK_CHECK_EQUALS(String, "lo, World! This is a long string.")
    QUICK_CHECK_EQUALS(String.Drop(2), 2ul)
    QUICK_CHECK_EQUALS(String, ", World! This is a long string.")
    QUICK_CHECK_EQUALS(String.Drop(99), 31ul)
    QUICK_CHECK_EQUALS(String, "")

    return;
}

TEST_CASE(StringFormat, "Lal.Containers")
{
    LString String { LString::SprintF("{} {}", "Five", 5) };
    QUICK_CHECK_EQUALS(String, "Five 5")

    return;
}

TEST_CASE(StringSubstitute, "Lal.Containers")
{
    LString String { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    String.Substitute(String.begin(), String.begin() + 5, "Hi");
    QUICK_CHECK_EQUALS(String, "Hi, World! This is a long string.")
    String.Substitute(String.begin(), String.begin() + 2, "Servus");
    QUICK_CHECK_EQUALS(String, "Servus, World! This is a long string.")
    String.Substitute(String.begin() + 8, String.begin() + 8 + 5, "City");
    QUICK_CHECK_EQUALS(String, "Servus, City! This is a long string.")
    String.Substitute(String.begin() + 8, String.begin() + 8, "my ");
    QUICK_CHECK_EQUALS(String, "Servus, my City! This is a long string.")
    String.Substitute(String.begin(), String.begin() + 6, "");
    QUICK_CHECK_EQUALS(String, ", my City! This is a long string.")
    String.Substitute(String.begin(), String.begin() + 5, '!');
    QUICK_CHECK_EQUALS(String, "!City! This is a long string.")
    String.Substitute(String.begin() + 1, String.begin() + 1, '*');
    QUICK_CHECK_EQUALS(String, "!*City! This is a long string.")
    String.Substitute(String.begin() + 6, String.begin() + 6, '*');
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long string.")
    String.Substitute(String.begin(), String.begin(), "");
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long string.")
    String.Substitute(String.begin() + 1, String.begin() + 1, "");
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long string.")
    String.Substitute(--String.end(), --String.end(), "");
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long string.")
    String.Substitute(--String.end(), String.end(), "");
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long string")
    String.Substitute(String.end() - 6, String.end(), "sentence");
    QUICK_CHECK_EQUALS(String, "!*City*! This is a long sentence")

    return;
}

TEST_CASE(StringRepalce, "Lal.Containers")
{
    LString String { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(String, "Hello, World! This is a long string.")

    QUICK_CHECK_EQUALS(String.Replace('e', 'W'), 1ul)
    QUICK_CHECK_EQUALS(String, "HWllo, World! This is a long string.")
    QUICK_CHECK_EQUALS(String.Replace('W', 'o'), 2ul)
    QUICK_CHECK_EQUALS(String, "Hollo, oorld! This is a long string.")
    QUICK_CHECK_EQUALS(String.Replace('o', 'i'), 5ul)
    QUICK_CHECK_EQUALS(String, "Hilli, iirld! This is a ling string.")
    QUICK_CHECK_EQUALS(String.Count('i'), 8ul)

    QUICK_CHECK_EQUALS(String.Replace("This", "That"), 1ul)
    QUICK_CHECK_EQUALS(String, "Hilli, iirld! That is a ling string.")

    String = "AAaBbAAAAa";
    QUICK_CHECK_EQUALS(String.Replace("AAa", "Bb"), 2ul)
    QUICK_CHECK_EQUALS(String, "BbBbAABb")
    QUICK_CHECK_EQUALS(String.Replace("Bb", "AA"), 3ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("A", "B"), 8ul)
    QUICK_CHECK_EQUALS(String, "BBBBBBBB")
    QUICK_CHECK_EQUALS(String.Replace("BB", "AA"), 4ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("AA", "AA"), 4ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("A", "AA"), 8ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("A", "AAA"), 16ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("AAAA", "A"), 12ul)
    QUICK_CHECK_EQUALS(String, "AAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(String.Replace("A", ""), 12ul)
    QUICK_CHECK_EQUALS(String, "")

    return;
}
