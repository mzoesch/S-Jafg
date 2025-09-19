// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

#if TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS

#include "Containers/Array.h"

TEST_CASE(SimpleIntegerArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<i32> Arr;
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.Contains(0),        false )
    CHECK_EQUALS( "Array with zero size.", Arr.Contains(1),        false )
    CHECK_EQUALS( "Array with zero size.", Arr.FindRef(0),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.FindRef(1),       nullptr )

    Arr.Add(100);
    CHECK_EQUALS( "Array with one element.", Arr.GetSize(),              1 )
    CHECK_EQUALS( "Array with one element.", Arr.GetCapacity(),          5 )

    CHECK_EQUALS( "Array with one element.", Arr.Contains(0),        false )
    CHECK_EQUALS( "Array with one element.", Arr.Contains(1),        false )
    CHECK_EQUALS( "Array with one element.", Arr.Contains(100),       true )
    CHECK_EQUALS( "Array with one element.", Arr.FindRef(0),       nullptr )
    CHECK_EQUALS( "Array with one element.", Arr.FindRef(1),       nullptr )
    CHECK_EQUALS( "Array with one element.", Arr.IsCapped(),         false )

    Arr.Add(101); Arr.Add(102); Arr.Add(103); Arr.Add(104);
    CHECK_EQUALS( "Array with five elements.", Arr.GetSize(),              5 )
    CHECK_EQUALS( "Array with five elements.", Arr.GetCapacity(),          5 )
    CHECK_EQUALS( "Array with five elements.", Arr.IsCapped(),          true )

    Arr.Add(105);
    CHECK_EQUALS( "Array with six elements.", Arr.GetSize(),              6 )
    CHECK_EQUALS( "Array with six elements.", Arr.GetCapacity(),         20 )
    CHECK_EQUALS( "Array with six elements.", Arr.IsCapped(),         false )

    return;
}

TEST_CASE(SimpleStringArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<LStringLegacy> Arr;
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),                0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),            0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),          nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.Contains("0"),        false )
    CHECK_EQUALS( "Array with zero size.", Arr.Contains("1"),        false )
    CHECK_EQUALS( "Array with zero size.", Arr.FindRef("0"),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.FindRef("1"),       nullptr )

    Arr.Add("My Literal String.");
    CHECK_EQUALS( "Array with one element.", Arr.GetSize(),                1 )
    CHECK_EQUALS( "Array with one element.", Arr.GetCapacity(),            5 )

    CHECK_EQUALS( "Array with one element.", Arr.Contains("0"),                 false )
    CHECK_EQUALS( "Array with one element.", Arr.Contains("1"),                 false )
    CHECK_EQUALS( "Array with one element.", Arr.Contains("My Literal String."), true )
    CHECK_EQUALS( "Array with one element.", Arr[0],             "My Literal String." )

    Arr.Add("My Other Literal String.");
    CHECK_EQUALS( "Array with two elements.", Arr.GetSize(),                2 )
    CHECK_EQUALS( "Array with two elements.", Arr.GetCapacity(),            5 )
    CHECK_EQUALS( "Array with two elements.", Arr[0],    "My Literal String." )
    CHECK_EQUALS( "Array with two elements.", Arr[1], "My Other Literal String." )

    Arr.Add("My Third Literal String."); Arr.Add("My Fourth Literal String."); Arr.Add("My Fifth Literal String.");
    Arr.Add("My Sixth Literal String."); Arr.Add("My Seventh Literal String."); Arr.Add("My Eighth Literal String.");
    CHECK_EQUALS( "Array with eight elements.", Arr.GetSize(),                8 )
    CHECK_EQUALS( "Array with eight elements.", Arr.GetCapacity(),            20 )

    Arr.Add("My Ninth Literal String."); Arr.Add("My Tenth Literal String."); Arr.Add("My Eleventh Literal String.");
    Arr.Add("My Twelfth Literal String."); Arr.Add("My Thirteenth Literal String."); Arr.Add("My Fourteenth Literal String.");
    CHECK_EQUALS( "Array with fourteen elements.", Arr.GetSize(),                14 )
    CHECK_EQUALS( "Array with fourteen elements.", Arr.GetCapacity(),            20 )
    CHECK_EQUALS( "Array with fourteen elements.", Arr.IsCapped(),             false )
    CHECK_EQUALS( "Array with fourteen elements.", Arr[8],   "My Ninth Literal String." )
    CHECK_EQUALS( "Array with fourteen elements.", Arr[13], "My Fourteenth Literal String." )

    return;
}

TEST_CASE(MidArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<i32> Arr = { 100, 101, 102, 103 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetSize(),                4 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetCapacity(),            4 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[0],                     100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[1],                     101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[2],                     102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[3],                     103 )

    Arr.RemoveAt(0);
    CHECK_EQUALS( "Array with three elements.", Arr.GetSize(),                3 )
    CHECK_EQUALS( "Array with three elements.", Arr.GetCapacity(),            4 )
    CHECK_EQUALS( "Array with three elements.", Arr[0],                     101 )
    CHECK_EQUALS( "Array with three elements.", Arr[1],                     102 )
    CHECK_EQUALS( "Array with three elements.", Arr[2],                     103 )

    Arr.Append( { 104, 105, 106, 107 } );
    CHECK_EQUALS( "Array with seven elements.", Arr.GetSize(),                7 )
    CHECK_EQUALS( "Array with seven elements.", Arr.GetCapacity(),            7 )
    CHECK_EQUALS( "Array with seven elements.", Arr[3],                     104 )
    CHECK_EQUALS( "Array with seven elements.", Arr[4],                     105 )
    CHECK_EQUALS( "Array with seven elements.", Arr[5],                     106 )
    CHECK_EQUALS( "Array with seven elements.", Arr[6],                     107 )

    Arr.RemoveAt(4);
    CHECK_EQUALS( "Array with six elements.", Arr.GetSize(),                6 )
    CHECK_EQUALS( "Array with six elements.", Arr.GetCapacity(),            7 )
    CHECK_EQUALS( "Array with six elements.", Arr[0],                     101 )
    CHECK_EQUALS( "Array with six elements.", Arr[1],                     102 )
    CHECK_EQUALS( "Array with six elements.", Arr[2],                     103 )
    CHECK_EQUALS( "Array with six elements.", Arr[3],                     104 )
    CHECK_EQUALS( "Array with six elements.", Arr[4],                     106 )
    CHECK_EQUALS( "Array with six elements.", Arr[5],                     107 )

    Arr.Add(108);
    CHECK_EQUALS( "Array with eight elements.", Arr.GetSize(),                7 )
    CHECK_EQUALS( "Array with eight elements.", Arr.GetCapacity(),            7 )
    CHECK_EQUALS( "Array with eight elements.", Arr[6],                     108 )

    Arr.Add(109);
    CHECK_EQUALS( "Array with nine elements.", Arr.GetSize(),                8 )
    CHECK_EQUALS( "Array with nine elements.", Arr.GetCapacity(),           20 )
    CHECK_EQUALS( "Array with nine elements.", Arr[7],                     109 )

    return;
}

TEST_CASE(NonTrivialTypeOperations, "Lal.Containers")
{
    using namespace Jafg;

    i32 ConstructorCounter = 0;
    i32 DestructorCounter  = 0;

    struct MyStruct final
    {
        FORCEINLINE explicit MyStruct(i32* InCCounter, i32* InDCounter) : CCounter(InCCounter), DCounter(InDCounter) { ++*CCounter; }
        FORCEINLINE ~MyStruct() { ++*DCounter; }
        MyStruct(MyStruct& O) noexcept : CCounter(O.CCounter), DCounter(O.DCounter) { }
        MyStruct(MyStruct&& O) noexcept : CCounter(O.CCounter), DCounter(O.DCounter) { }

        MyStruct& operator=(const MyStruct& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        MyStruct& operator=(MyStruct& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        MyStruct& operator=(MyStruct&& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        i32* CCounter;
        i32* DCounter;
    };

    TdhArray<MyStruct> Arr;
    CHECK_EQUALS( "Non trivial operation initialization.", ConstructorCounter, 0 )
    CHECK_EQUALS( "Non trivial operation initialization.", DestructorCounter,  0 )

    Arr.Add(MyStruct(&ConstructorCounter, &DestructorCounter));
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            1 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             1 )

    Arr.Empty();
    CHECK_EQUALS( "Non trivial operation empty.", ConstructorCounter,          1 )
    CHECK_EQUALS( "Non trivial operation empty.", DestructorCounter,           2 )

    Arr.Emplace(&ConstructorCounter, &DestructorCounter);
    CHECK_EQUALS( "Non trivial operation emplace.", ConstructorCounter,        2 )
    CHECK_EQUALS( "Non trivial operation emplace.", DestructorCounter,         2 )

    Arr.Empty();
    CHECK_EQUALS( "Non trivial operation emplace.", ConstructorCounter,        2 )
    CHECK_EQUALS( "Non trivial operation emplace.", DestructorCounter,         3 )

    Arr.Append( { MyStruct(&ConstructorCounter, &DestructorCounter), MyStruct(&ConstructorCounter, &DestructorCounter) } );
    CHECK_EQUALS( "Non trivial operation append.", ConstructorCounter,        4 )
    CHECK_EQUALS( "Non trivial operation append.", DestructorCounter,         5 )

    Arr.Add(MyStruct(&ConstructorCounter, &DestructorCounter));
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            5 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             6 )

    Arr.Reset(0);
    CHECK_EQUALS( "Non trivial operation reset.", ConstructorCounter,          5 )
    CHECK_EQUALS( "Non trivial operation reset.", DestructorCounter,           9 )

    return;
}

TEST_CASE(NonTrivialPointerTypeOperations, "Lal.Containers")
{
    using namespace Jafg;

    i32 ConstructorCounter = 0;
    i32 DestructorCounter  = 0;

    struct MyStruct final
    {
        FORCEINLINE explicit MyStruct(i32* InCCounter, i32* InDCounter) : CCounter(InCCounter), DCounter(InDCounter) { ++*CCounter; }
        FORCEINLINE ~MyStruct() { ++*DCounter; }
        MyStruct(MyStruct& O) noexcept : CCounter(O.CCounter), DCounter(O.DCounter) { }
        MyStruct(MyStruct&& O) noexcept : CCounter(O.CCounter), DCounter(O.DCounter) { }

        MyStruct& operator=(const MyStruct& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        MyStruct& operator=(MyStruct& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        MyStruct& operator=(MyStruct&& O) noexcept
        {
            CCounter = O.CCounter;
            DCounter = O.DCounter;
            return *this;
        }

        i32* CCounter;
        i32* DCounter;
    };

    TdhArray<const MyStruct*> Arr;
    CHECK_EQUALS( "Non trivial operation initialization.", ConstructorCounter, 0 )
    CHECK_EQUALS( "Non trivial operation initialization.", DestructorCounter,  0 )

    Arr.Add(new MyStruct(&ConstructorCounter, &DestructorCounter));
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            1 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             0 )

    const MyStruct* One = Arr[0];

    Arr.Empty();
    CHECK_EQUALS( "Non trivial operation empty.", ConstructorCounter,          1 )
    CHECK_EQUALS( "Non trivial operation empty.", DestructorCounter,           0 )

    Arr.Add(One);
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            1 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             0 )

    Arr[0] = nullptr;
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            1 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             0 )

    Arr.Add(new MyStruct(&ConstructorCounter, &DestructorCounter));
    CHECK_EQUALS( "Non trivial operation add.", ConstructorCounter,            2 )
    CHECK_EQUALS( "Non trivial operation add.", DestructorCounter,             0 )

    const MyStruct* Two = Arr[1];

    Arr.Empty();
    CHECK_EQUALS( "Non trivial operation empty.", ConstructorCounter,          2 )
    CHECK_EQUALS( "Non trivial operation empty.", DestructorCounter,           0 )

    Arr.Append( { One, Two } );
    CHECK_EQUALS( "Non trivial operation append.", ConstructorCounter,        2 )
    CHECK_EQUALS( "Non trivial operation append.", DestructorCounter,         0 )

    Arr.Reset(0);
    CHECK_EQUALS( "Non trivial operation reset.", ConstructorCounter,          2 )
    CHECK_EQUALS( "Non trivial operation reset.", DestructorCounter,           0 )

    Arr.AddZeroed();
    Arr.AddZeroed();
    Arr.AddZeroed();
    Arr.AddZeroed();
    CHECK_EQUALS( "Non trivial operation add zeroed.", ConstructorCounter,     2 )
    CHECK_EQUALS( "Non trivial operation add zeroed.", DestructorCounter,      0 )
    CHECK_EQUALS( "Non trivial operation add zeroed.", Arr.GetSize(),          4 )
    CHECK_EQUALS( "Non trivial operation add zeroed.", Arr.GetCapacity(),     20 )

    Arr.RemoveAt(1);
    CHECK_EQUALS( "Non trivial operation remove at.", ConstructorCounter,      2 )
    CHECK_EQUALS( "Non trivial operation remove at.", DestructorCounter,       0 )
    CHECK_EQUALS( "Non trivial operation remove at.", Arr.GetSize(),           3 )
    CHECK_EQUALS( "Non trivial operation remove at.", Arr.GetCapacity(),      20 )

    Arr.Empty();
    CHECK_EQUALS( "Non trivial operation empty.", ConstructorCounter,          2 )
    CHECK_EQUALS( "Non trivial operation empty.", DestructorCounter,           0 )
    CHECK_EQUALS( "Non trivial operation empty.", Arr.GetSize(),               0 )
    CHECK_EQUALS( "Non trivial operation empty.", Arr.GetCapacity(),           0 )

    delete One;
    delete Two;

    CHECK_EQUALS( "Non trivial operation empty.", ConstructorCounter,          2 )
    CHECK_EQUALS( "Non trivial operation empty.", DestructorCounter,           2 )

    return;
}

TEST_CASE(AppendAtArray, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<i32> Arr = { 100, 101, 102, 103 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetSize(),                4 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetCapacity(),            4 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[0],                     100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[1],                     101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[2],                     102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[3],                     103 )

    i32* Data = new i32[4] { 1000, 1001, 1002, 1003 };
    Arr.AppendAt(2, Data, 4);
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetSize(),                8 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetCapacity(),            8 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[0],                     100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[1],                     101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[2],                    1000 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[3],                    1001 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[4],                    1002 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[5],                    1003 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[6],                     102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[7],                     103 )

    delete[] Data;

    return;
}

TEST_CASE(MultiRemoveAtIndexArray, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<i32> Arr = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
    Arr.RemoveAt(5, 3);
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetSize(),                7 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetCapacity(),           10 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[0],                     100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[1],                     101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[2],                     102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[3],                     103 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[4],                     104 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[5],                     108 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[6],                     109 )

    return;
}

#endif /* TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS */
