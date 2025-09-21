// Copyright mzoesch. All rights reserved.

#pragma once

#include "Containers/LalString.h"

TEST_CASE(EmptyStringOperations, "Lal.Containers")
{
    LString String;
    LOptimizedString OptimizedString;
    LSmallString<25> SmallString;
    LStringView StringView;
    LMutableStringView MutableStringView;

    QUICK_CHECK_EQUALS(String.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallString.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(StringView.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetRuneCount(), 0ul)

    QUICK_CHECK_EQUALS(String.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(SmallString.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(StringView.GetCharacterCount(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetCharacterCount(), 0ul)

    QUICK_CHECK_EQUALS(String.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetAllocatedByteSize(), 8ul)
    QUICK_CHECK_EQUALS(SmallString.GetAllocatedByteSize(), 25ul)
    QUICK_CHECK_EQUALS(StringView.GetAllocatedByteSize(), 0ul)
    QUICK_CHECK_EQUALS(MutableStringView.GetAllocatedByteSize(), 0ul)

    return;
}

TEST_CASE(OptimizedStringAssingOperations, "Lal.Containers")
{
    LOptimizedString OptimizedString1 { "Hello, World!" };
    LString String1 { "Hello, World!" };
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, World!")
    QUICK_CHECK_EQUALS(String1, "Hello, World!")
    QUICK_CHECK_NOT_EQUALS(OptimizedString1, "Hello, World.")
    QUICK_CHECK_NOT_EQUALS(String1, "Hello, World.")
    QUICK_CHECK_NOT_EQUALS(OptimizedString1, "Hello, World! ")
    QUICK_CHECK_NOT_EQUALS(String1, "Hello, World! ")

    LOptimizedString OptimizedString2;
    LString String2;
    OptimizedString2.Assign(OptimizedString1);
    String2.Assign(String1);
    QUICK_CHECK_EQUALS(OptimizedString2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString2, "Hello, World!")
    QUICK_CHECK_EQUALS(String2, "Hello, World!")
    QUICK_CHECK_EQUALS(OptimizedString1, OptimizedString2)
    QUICK_CHECK_EQUALS(String1, String2)

    OptimizedString2.Assign("");
    String2.Assign("");
    QUICK_CHECK_EQUALS(OptimizedString2.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(String2.GetRuneCount(), 0ul)
    QUICK_CHECK_EQUALS(static_cast<const void*>(OptimizedString2.GetAllocator().GetDataPointer()), static_cast<const void*>(OptimizedString2.GetAllocator().GetAllocator()._Data))
    QUICK_CHECK_EQUALS(static_cast<const void*>(String2.GetAllocator().GetDataPointer()), nullptr)

    String1.Assign(OptimizedString1);
    QUICK_CHECK_EQUALS(String1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(String1, "Hello, World!")

    return;
}

TEST_CASE(OptimizedStringMemoryReuse, "Lal.Strings")
{
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };
    LString String { "Hello, World! This is a long string." };

    const LOptimizedString::SizeType OptimizedStringLength { OptimizedString.GetRuneCount() };
    const LOptimizedString::ConstPointer OptimizedStringPtr { OptimizedString.ToPtr() };
    const LString::SizeType StringLength { String.GetRuneCount() };
    const LString::ConstPointer StringPtr { String.ToPtr() };

    OptimizedString = "Hello, World! Is this a long string?";
    String = "Hello, World! Is this a long string?";

    QUICK_CHECK_EQUALS(OptimizedStringLength, OptimizedString.GetRuneCount())
    QUICK_CHECK_EQUALS(static_cast<const void*>(OptimizedStringPtr), static_cast<const void*>(OptimizedString.ToPtr()))

    QUICK_CHECK_EQUALS(StringLength, String.GetRuneCount())
    QUICK_CHECK_EQUALS(static_cast<const void*>(StringPtr), static_cast<const void*>(String.ToPtr()))

    return;
}

TEST_CASE(OptimizedStringAppendOperations, "Lal.Containers")
{
    LOptimizedString OptimizedString1 { "Hello" };
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 5ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello")

    OptimizedString1.Append(',');
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 6ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello,")

    OptimizedString1.Append(" ");
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 7ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, ")

    OptimizedString1.Append("");
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 7ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, ")

    LStringView View { "World" };
    OptimizedString1.Append(View);
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 12ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, World")

    LOptimizedString::Iterator It { OptimizedString1.AppendAt(OptimizedString1.end(), '!') };
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, World!")
    QUICK_CHECK_EQUALS(*It, '!')
    QUICK_CHECK_EQUALS(It, --OptimizedString1.end())
    QUICK_CHECK_EQUALS(It.Cursor, OptimizedString1.end_ptr() - 1)

    It = OptimizedString1.AppendAt(OptimizedString1.end(), "");
    QUICK_CHECK_EQUALS(OptimizedString1.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString1, "Hello, World!")
    QUICK_CHECK_EQUALS(It, OptimizedString1.end())
    QUICK_CHECK_EQUALS(It.Cursor, OptimizedString1.end_ptr())

    LOptimizedString OptimizedString2 { OptimizedString1 + " How are you?" };
    QUICK_CHECK_EQUALS(OptimizedString2.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(OptimizedString2, "Hello, World! How are you?")

    OptimizedString2 = { LOptimizedString{"Hello"} + ',' + " " + "World" + '!' };
    QUICK_CHECK_EQUALS(OptimizedString2.GetRuneCount(), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString2, "Hello, World!")

    return;
}

TEST_CASE(OptimizedStringPathOperations, "Lal.Containers")
{
    LOptimizedString OptimizedString { "/home/user" };
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 10ul)
    QUICK_CHECK_EQUALS(OptimizedString, "/home/user")

    OptimizedString /= "Development";
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 22ul)
    QUICK_CHECK_EQUALS(OptimizedString, "/home/user/Development")

    OptimizedString.Append('/');
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 23ul)
    QUICK_CHECK_EQUALS(OptimizedString, "/home/user/Development/")

    OptimizedString = OptimizedString / "/Rust/" / "/Sucks.sln";
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 37ul)
    QUICK_CHECK_EQUALS(OptimizedString, "/home/user/Development/Rust/Sucks.sln")

    return;
}

TEST_CASE(OptimizedStringRemovalOperations, "Lal.Containers")
{
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 36ul)
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString.RemoveAt(0ul);
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 35ul)
    QUICK_CHECK_EQUALS(OptimizedString, "ello, World! This is a long string.")
    OptimizedString.RemoveAt(OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 34ul)
    QUICK_CHECK_EQUALS(OptimizedString, "llo, World! This is a long string.")
    OptimizedString.RemoveAt(OptimizedString.begin_ptr());
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 33ul)
    QUICK_CHECK_EQUALS(OptimizedString, "lo, World! This is a long string.")
    OptimizedString.RemoveAt(++OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 32ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l, World! This is a long string.")
    OptimizedString.RemoveAt(--OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 31ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l, World! This is a long string")
    OptimizedString.RemoveAt(OptimizedString.end_ptr() - 1);
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 30ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l, World! This is a long strin")

    QUICK_CHECK_TRUE(OptimizedString.RemoveOnce('o'))
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 29ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l, Wrld! This is a long strin")

    QUICK_CHECK_EQUALS(OptimizedString.Remove('s'), 3ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l, Wrld! Thi i a long trin")

    QUICK_CHECK_EQUALS(OptimizedString.Remove(" "), 6ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 20ul)
    QUICK_CHECK_EQUALS(OptimizedString, "l,Wrld!Thiialongtrin")

    LStringView View = { "aabbabababaabbbaabbbaaabba" };
    OptimizedString.Assign(View);
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 26ul)
    QUICK_CHECK_EQUALS(OptimizedString, "aabbabababaabbbaabbbaaabba")

    QUICK_CHECK_EQUALS(OptimizedString.Remove("aa"), 4ul)
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 18ul)
    QUICK_CHECK_EQUALS(OptimizedString, "bbabababbbbbbbabba")

    return;
}

TEST_CASE(OptimizedStringFindOperations, "Lal.Containers")
{
    LOptimizedString OptimizedString { "bbabababbbbbbbabba" };
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 18ul)
    QUICK_CHECK_EQUALS(OptimizedString, "bbabababbbbbbbabba")

    QUICK_CHECK_EQUALS(OptimizedString.FindFirst('b'), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst('a'), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst('c'), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 1), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 1), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('c', 1), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindSecond('b'), OptimizedString.begin() + 1)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond('a'), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond('c'), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 2), OptimizedString.begin() + 1)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 2), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('c', 2), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 3), OptimizedString.begin() + 3)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 3), OptimizedString.begin() + 6)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('c', 3), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 12), OptimizedString.begin() + 15)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 4), OptimizedString.begin() + 14)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 13), OptimizedString.begin() + 16)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 5), OptimizedString.begin() + 17)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 14), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 6), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth('b', 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('a', 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth('c', 3), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("b"), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("a"), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("c"), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 1), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 1), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("c", 1), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("b"), OptimizedString.begin() + 1)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("a"), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("c"), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 2), OptimizedString.begin() + 1)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 2), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("c", 2), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 3), OptimizedString.begin() + 3)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 3), OptimizedString.begin() + 6)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("c", 3), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 12), OptimizedString.begin() + 15)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 4), OptimizedString.begin() + 14)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 13), OptimizedString.begin() + 16)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 5), OptimizedString.begin() + 17)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 14), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 6), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("b", 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("a", 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("c", 3), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("bb"), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("ab"), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindFirst("cd"), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("bb", 1), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 1), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("cd", 1), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("ba"), OptimizedString.begin() + 3)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("ab"), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindSecond("cd"), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 2), OptimizedString.begin() + 3)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 2), OptimizedString.begin() + 4)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("cd", 2), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 3), OptimizedString.begin() + 5)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 3), OptimizedString.begin() + 6)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("cd", 3), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 4), OptimizedString.begin() + 13)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 3), OptimizedString.begin() + 6)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 5), OptimizedString.begin() + 16)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 4), OptimizedString.begin() + 14)
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 6), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 5), OptimizedString.end())

    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ba", 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("ab", 20), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNth("cd", 3), OptimizedString.end())

    QUICK_CHECK_TRUE(OptimizedString.StartsWith('b'))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith('a'))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith('c'))
    QUICK_CHECK_TRUE(OptimizedString.StartsWith("b"))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith("a"))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith("c"))
    QUICK_CHECK_TRUE(OptimizedString.StartsWith("bb"))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith("ab"))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith("cd"))
    QUICK_CHECK_TRUE(OptimizedString.StartsWith("bbabababbbbbb"))
    QUICK_CHECK_TRUE(OptimizedString.StartsWith("bbabababbbbbbbabb"))
    QUICK_CHECK_TRUE(OptimizedString.StartsWith("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(OptimizedString.StartsWith("bbabababbbbbbbabbaa"))

    QUICK_CHECK_TRUE(OptimizedString.EndsWith('a'))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith('b'))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith('c'))
    QUICK_CHECK_TRUE(OptimizedString.EndsWith("a"))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith("b"))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith("c"))
    QUICK_CHECK_TRUE(OptimizedString.EndsWith("ba"))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith("ab"))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith("cd"))
    QUICK_CHECK_TRUE(OptimizedString.EndsWith("bbabba"))
    QUICK_CHECK_TRUE(OptimizedString.EndsWith("babababbbbbbbabba"))
    QUICK_CHECK_TRUE(OptimizedString.EndsWith("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(OptimizedString.EndsWith("abbabababbbbbbbabba"))

    QUICK_CHECK_EQUALS(OptimizedString.FindLast('a'), OptimizedString.end() - 1)
    QUICK_CHECK_EQUALS(OptimizedString.FindLast('b'), OptimizedString.end() - 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("ba"), OptimizedString.end() - 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("bba"), OptimizedString.end() - 3)
    QUICK_CHECK_EQUALS(OptimizedString.FindLast('c'), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("c"), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("bbabababbbbbbbabba"), OptimizedString.begin())
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("bbabababbbbbbbabbaa"), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindLast("abbabababbbbbbbabba"), OptimizedString.end())
    QUICK_CHECK_EQUALS(OptimizedString.FindNthLast("ab", 4), OptimizedString.begin() + 2)
    QUICK_CHECK_EQUALS(OptimizedString.FindNthLast("ab", 3), OptimizedString.begin() + 4)

    QUICK_CHECK_EQUALS(OptimizedString.Count('a'), 5ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count('b'), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("a"), 5ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("b"), 13ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("ba"), 5ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("ab"), 4ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("baa"), 0ul)
    QUICK_CHECK_EQUALS(OptimizedString.Count("bab"), 3ul)

    QUICK_CHECK_TRUE(OptimizedString.Contains('a'))
    QUICK_CHECK_TRUE(OptimizedString.Contains('b'))
    QUICK_CHECK_FALSE(OptimizedString.Contains('c'))
    QUICK_CHECK_TRUE(OptimizedString.Contains("a"))
    QUICK_CHECK_TRUE(OptimizedString.Contains("b"))
    QUICK_CHECK_FALSE(OptimizedString.Contains("c"))
    QUICK_CHECK_TRUE(OptimizedString.Contains("ba"))
    QUICK_CHECK_TRUE(OptimizedString.Contains("ab"))
    QUICK_CHECK_FALSE(OptimizedString.Contains("baa"))
    QUICK_CHECK_TRUE(OptimizedString.Contains("bbabababbbbbbbabb"))
    QUICK_CHECK_TRUE(OptimizedString.Contains("bbabababbbbbbbabba"))
    QUICK_CHECK_FALSE(OptimizedString.Contains("bbabababbbbbbbabbaa"))

    return;
}

TEST_CASE(OptimizedStringChopsOperatios, "Lal.Containers")
{
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(OptimizedString.GetRuneCount(), 36ul)
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString.InlineCut(0ul, 0ul);
    QUICK_CHECK_EQUALS(OptimizedString, "")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin_ptr(), OptimizedString.begin_ptr());
    QUICK_CHECK_EQUALS(OptimizedString, "")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString, "")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(0ul, 1ul);
    QUICK_CHECK_EQUALS(OptimizedString, "H")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin_ptr(), OptimizedString.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(OptimizedString, "H")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.begin() + 1);
    QUICK_CHECK_EQUALS(OptimizedString, "H")

    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString, "")
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString, "")
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.begin());
    QUICK_CHECK_EQUALS(OptimizedString, "")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(0ul, OptimizedString.GetRuneCount());
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin_ptr(), OptimizedString.end_ptr());
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(0ul, OptimizedString.GetRuneCount() - 1);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(0ul, OptimizedString.end_idx() - 1);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin_ptr(), OptimizedString.end_ptr() - 1);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.end() - 1);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin(), OptimizedString.end() - 7);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long ")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin() + 1, OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString, "ello, World! This is a long string.")

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.end(), OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString, "");

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.end() - 1, OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString, ".");

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.end() - 7, OptimizedString.end());
    QUICK_CHECK_EQUALS(OptimizedString, "string.");

    OptimizedString = "Hello, World! This is a long string.";
    OptimizedString.InlineCut(OptimizedString.begin() + 14, OptimizedString.end() - 8);
    QUICK_CHECK_EQUALS(OptimizedString, "This is a long");

    OptimizedString = "Hello, World! This is a long string.";
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    LOptimizedString OtherString;
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.Cut(0ul, 0ul);
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.Cut(OptimizedString.begin_ptr(), OptimizedString.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.Cut(OptimizedString.begin(), OptimizedString.begin());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.Cut(0ul, 1ul);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = OptimizedString.Cut(OptimizedString.begin_ptr(), OptimizedString.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = OptimizedString.Cut(OptimizedString.begin(), OptimizedString.begin() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = OptimizedString.Cut(OptimizedString.begin() + 7, OptimizedString.end());
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long string.")
    OtherString = OptimizedString.Cut(OptimizedString.begin() + 7, OptimizedString.end() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")
    OtherString = OptimizedString.Cut(OptimizedString.begin_ptr() + 7, OptimizedString.end_ptr() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")
    OtherString = OptimizedString.Cut(7ul, OptimizedString.end_idx() - 8);
    QUICK_CHECK_EQUALS(OtherString, "World! This is a long")

    OtherString = OptimizedString.LeftCut(0ul);
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.LeftCut(OptimizedString.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "")
    OtherString = OptimizedString.LeftCut(OptimizedString.begin());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = OptimizedString.LeftCut(1ul);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = OptimizedString.LeftCut(OptimizedString.begin_ptr() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")
    OtherString = OptimizedString.LeftCut(OptimizedString.begin() + 1);
    QUICK_CHECK_EQUALS(OtherString, "H")

    OtherString = OptimizedString.LeftCut(OptimizedString.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftCut(OptimizedString.end_ptr() - OptimizedString.begin_ptr());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftCut(OptimizedString.end() - OptimizedString.begin());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftCut(OptimizedString.GetRuneCount() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = OptimizedString.LeftCut(OptimizedString.end_idx() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = OptimizedString.LeftCut(OptimizedString.end_ptr() - 1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = OptimizedString.LeftCut(OptimizedString.end());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftCut(OptimizedString.end_idx());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftCut(OptimizedString.end_ptr());
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")

    OtherString = OptimizedString.RightCut(0ul);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")

    OtherString = OptimizedString.LeftChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.LeftChop(1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString = OptimizedString.LeftChop(2);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long strin")
    OtherString = OptimizedString.LeftChop(OptimizedString.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = OptimizedString;
    OtherString.InlineLeftChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString.InlineLeftChop(1);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string")
    OtherString.InlineLeftChop(2);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long stri")
    OtherString.InlineLeftChop(OptimizedString.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = OptimizedString.RightChop(0);
    QUICK_CHECK_EQUALS(OtherString, "Hello, World! This is a long string.")
    OtherString = OptimizedString.RightChop(1);
    QUICK_CHECK_EQUALS(OtherString, "ello, World! This is a long string.")
    OtherString = OptimizedString.RightChop(2);
    QUICK_CHECK_EQUALS(OtherString, "llo, World! This is a long string.")
    OtherString = OptimizedString.RightChop(OptimizedString.GetRuneCount());
    QUICK_CHECK_EQUALS(OtherString, "")

    OtherString = OptimizedString;
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

TEST_CASE(OptimizedStringLowerUpperOperatios, "Lal.Containers")
{
    LOptimizedString OptimizedString { " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };

    OptimizedString.ToUpper();
    QUICK_CHECK_EQUALS(OptimizedString, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")
    OptimizedString.ToLower();
    QUICK_CHECK_EQUALS(OptimizedString, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")

    OptimizedString = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    OptimizedString.ToLower();
    QUICK_CHECK_EQUALS(OptimizedString, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
    OptimizedString.ToUpper();
    QUICK_CHECK_EQUALS(OptimizedString, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")

    OptimizedString = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    LOptimizedString Other;
    Other = OptimizedString.GetLower();
    QUICK_CHECK_EQUALS(Other, " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
    Other = OptimizedString.GetUpper();
    QUICK_CHECK_EQUALS(Other, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}~")

    return;
}

TEST_CASE(OptimizedStringPops, "Lal.Containers")
{
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };

    QUICK_CHECK_TRUE(OptimizedString.Pop())
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string")
    QUICK_CHECK_TRUE(OptimizedString.Pop())
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long strin")
    OptimizedString.Pop(0);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long strin")
    OptimizedString.Pop(1);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long stri")
    OptimizedString.Pop(2);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long st")
    OptimizedString.Pop(3);
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long")

    LStringView View { OptimizedString };
    QUICK_CHECK_EQUALS(View, OptimizedString)
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
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long")

    OptimizedString = "Hello, World! This is a long string.";
    View.Assign(OptimizedString);

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

    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")
    QUICK_CHECK_TRUE(OptimizedString.Drop())
    QUICK_CHECK_EQUALS(OptimizedString, "ello, World! This is a long string.")
    QUICK_CHECK_TRUE(OptimizedString.Drop())
    QUICK_CHECK_EQUALS(OptimizedString, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Drop(0), 0ul)
    QUICK_CHECK_EQUALS(OptimizedString, "llo, World! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Drop(1), 1ul)
    QUICK_CHECK_EQUALS(OptimizedString, "lo, World! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Drop(2), 2ul)
    QUICK_CHECK_EQUALS(OptimizedString, ", World! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Drop(99), 31ul)
    QUICK_CHECK_EQUALS(OptimizedString, "")

    return;
}

TEST_CASE(OptimizedStringFormat, "Lal.Containers")
{
    LOptimizedString OptimizedString { LOptimizedString::SprintF("{} {}", "Five", 5) };
    QUICK_CHECK_EQUALS(OptimizedString, "Five 5")

    return;
}

TEST_CASE(OptimizedStringSubstitute, "Lal.Containers")
{
    std::string StdString { "Hello, World! This is a long string." };
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    OptimizedString.Substitute(OptimizedString.begin(), OptimizedString.begin() + 5, "Hi");
    QUICK_CHECK_EQUALS(OptimizedString, "Hi, World! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin(), OptimizedString.begin() + 2, "Servus");
    QUICK_CHECK_EQUALS(OptimizedString, "Servus, World! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin() + 8, OptimizedString.begin() + 8 + 5, "City");
    QUICK_CHECK_EQUALS(OptimizedString, "Servus, City! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin() + 8, OptimizedString.begin() + 8, "my ");
    QUICK_CHECK_EQUALS(OptimizedString, "Servus, my City! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin(), OptimizedString.begin() + 6, "");
    QUICK_CHECK_EQUALS(OptimizedString, ", my City! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin(), OptimizedString.begin() + 5, '!');
    QUICK_CHECK_EQUALS(OptimizedString, "!City! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin() + 1, OptimizedString.begin() + 1, '*');
    QUICK_CHECK_EQUALS(OptimizedString, "!*City! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin() + 6, OptimizedString.begin() + 6, '*');
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin(), OptimizedString.begin(), "");
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long string.")
    OptimizedString.Substitute(OptimizedString.begin() + 1, OptimizedString.begin() + 1, "");
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long string.")
    OptimizedString.Substitute(--OptimizedString.end(), --OptimizedString.end(), "");
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long string.")
    OptimizedString.Substitute(--OptimizedString.end(), OptimizedString.end(), "");
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long string")
    OptimizedString.Substitute(OptimizedString.end() - 6, OptimizedString.end(), "sentence");
    QUICK_CHECK_EQUALS(OptimizedString, "!*City*! This is a long sentence")

    return;
}

TEST_CASE(OptimizedStringRepalce, "Lal.Containers")
{
    LOptimizedString OptimizedString { "Hello, World! This is a long string." };
    QUICK_CHECK_EQUALS(OptimizedString, "Hello, World! This is a long string.")

    QUICK_CHECK_EQUALS(OptimizedString.Replace('e', 'W'), 1ul)
    QUICK_CHECK_EQUALS(OptimizedString, "HWllo, World! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Replace('W', 'o'), 2ul)
    QUICK_CHECK_EQUALS(OptimizedString, "Hollo, oorld! This is a long string.")
    QUICK_CHECK_EQUALS(OptimizedString.Replace('o', 'i'), 5ul)
    QUICK_CHECK_EQUALS(OptimizedString, "Hilli, iirld! This is a ling string.")
    QUICK_CHECK_EQUALS(OptimizedString.Count('i'), 8ul)

    QUICK_CHECK_EQUALS(OptimizedString.Replace("This", "That"), 1ul)
    QUICK_CHECK_EQUALS(OptimizedString, "Hilli, iirld! That is a ling string.")

    OptimizedString = "AAaBbAAAAa";
    QUICK_CHECK_EQUALS(OptimizedString.Replace("AAa", "Bb"), 2ul)
    QUICK_CHECK_EQUALS(OptimizedString, "BbBbAABb")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("Bb", "AA"), 3ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("A", "B"), 8ul)
    QUICK_CHECK_EQUALS(OptimizedString, "BBBBBBBB")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("BB", "AA"), 4ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("AA", "AA"), 4ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("A", "AA"), 8ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("A", "AAA"), 16ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("AAAA", "A"), 12ul)
    QUICK_CHECK_EQUALS(OptimizedString, "AAAAAAAAAAAA")
    QUICK_CHECK_EQUALS(OptimizedString.Replace("A", ""), 12ul)
    QUICK_CHECK_EQUALS(OptimizedString, "")

    return;
}
