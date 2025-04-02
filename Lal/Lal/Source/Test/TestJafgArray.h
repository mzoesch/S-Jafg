// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleIntegerArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TArray<i32> Arr1;
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetLast(),        nullptr )

    Arr1.Add(1);
    QUICK_CHECK_EQUALS(Arr1.GetSize(), 1)

    Arr1.Add(10);
    Arr1.Add(100);
    Arr1.Add(1000);
    QUICK_CHECK_EQUALS(Arr1.GetSize(), 4)

    Arr1.Add(50);
    Arr1.Add(51);
    Arr1.Add(52);
    Arr1.Add(53);
    Arr1.Add(54);
    Arr1.Add(55);
    Arr1.Add(56);
    Arr1.Add(57);
    Arr1.Add(58);
    Arr1.Add(59);
    QUICK_CHECK_EQUALS(Arr1.GetSize(), 14)

    Arr1.AddAt(0, -5);
    Arr1.AddAt(1, -4);
    Arr1.AddAt(2, -3);
    Arr1.AddAt(3, -2);
    Arr1.AddAt(4, -1);
    Arr1.AddAt(5,  0);
    QUICK_CHECK_EQUALS(Arr1.GetSize(), 20)
    QUICK_CHECK_EQUALS(Arr1[0],   -5)
    QUICK_CHECK_EQUALS(Arr1[1],   -4)
    QUICK_CHECK_EQUALS(Arr1[2],   -3)
    QUICK_CHECK_EQUALS(Arr1[3],   -2)
    QUICK_CHECK_EQUALS(Arr1[4],   -1)
    QUICK_CHECK_EQUALS(Arr1[5],    0)
    QUICK_CHECK_EQUALS(Arr1[6],    1)
    QUICK_CHECK_EQUALS(Arr1[7],   10)
    QUICK_CHECK_EQUALS(Arr1[8],  100)
    QUICK_CHECK_EQUALS(Arr1[9], 1000)
    QUICK_CHECK_EQUALS(*Arr1.GetLast(),  59)
    QUICK_CHECK_EQUALS(*Arr1.GetFirst(), -5)

    Arr1.Empty();
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetLast(),        nullptr )

    Arr1 = { 100, 101, 102, 103, 104 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1.GetSize(),      5 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1[0],           100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1[1],           101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1[2],           102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1[3],           103 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr1[4],           104 )

    TArray<i32> Arr2 = { 200, 201, 202, 203, 204 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2.GetSize(),   5 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[0],        200 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[1],        201 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[2],        202 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[3],        203 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[4],        204 )

    void* DataPtr = Arr2.GetData();
    Arr2 = Arr1;
    CHECK_EQUALS( "Array copied.", Arr1.GetSize(),         5 )
    CHECK_EQUALS( "Array copied.", Arr1[0],              100 )
    CHECK_EQUALS( "Array copied.", Arr1[1],              101 )
    CHECK_EQUALS( "Array copied.", Arr1[2],              102 )
    CHECK_EQUALS( "Array copied.", Arr1[3],              103 )
    CHECK_EQUALS( "Array copied.", Arr1[4],              104 )
    CHECK_EQUALS( "Array copied.", Arr2.GetSize(),         5 )
    CHECK_EQUALS( "Array copied.", Arr2[0],              100 )
    CHECK_EQUALS( "Array copied.", Arr2[1],              101 )
    CHECK_EQUALS( "Array copied.", Arr2[2],              102 )
    CHECK_EQUALS( "Array copied.", Arr2[3],              103 )
    CHECK_EQUALS( "Array copied.", Arr2[4],              104 )
    CHECK_EQUALS( "Array copied.", Arr2.GetData(),   DataPtr )

    Arr2.Empty();
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetLast(),        nullptr )

    DataPtr = Arr1.GetData();
    Arr2 = std::move(Arr1);
    CHECK_EQUALS( "Array moved.", Arr1.GetSize(),              0 )
    CHECK_EQUALS( "Array moved.", Arr1.GetCapacity(),          0 )
    CHECK_EQUALS( "Array moved.", Arr1.GetData(),        nullptr )
    CHECK_EQUALS( "Array moved.", Arr1.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array moved.", Arr1.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array moved.", Arr1.GetLast(),        nullptr )
    CHECK_EQUALS( "Array moved.", Arr2.GetSize(),              5 )
    CHECK_EQUALS( "Array moved.", Arr2[0],                   100 )
    CHECK_EQUALS( "Array moved.", Arr2[1],                   101 )
    CHECK_EQUALS( "Array moved.", Arr2[2],                   102 )
    CHECK_EQUALS( "Array moved.", Arr2[3],                   103 )
    CHECK_EQUALS( "Array moved.", Arr2[4],                   104 )
    CHECK_EQUALS( "Array moved.", Arr2.GetData(),        DataPtr )

    const i32* OtherData = new i32[5]{ 400, 401, 402, 403, 404 };

    Arr2.AppendAt(3, OtherData, 5);
    CHECK_EQUALS( "Array with raw memory.", Arr2.GetSize(),                10 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[0],                      100 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[1],                      101 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[2],                      102 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[3],                      400 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[4],                      401 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[5],                      402 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[6],                      403 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[7],                      404 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[8],                      103 )
    CHECK_EQUALS( "Array with raw memory.", Arr2[9],                      104 )

    Arr1 = { 900, 901, 902, 903, 904 };
    Arr2.AppendAt(1, Arr1);
    QUICK_CHECK_EQUALS(Arr2.GetSize(),   15 )
    QUICK_CHECK_EQUALS(Arr2[0],         100 )
    QUICK_CHECK_EQUALS(Arr2[1],         900 )
    QUICK_CHECK_EQUALS(Arr2[2],         901 )
    QUICK_CHECK_EQUALS(Arr2[3],         902 )
    QUICK_CHECK_EQUALS(Arr2[4],         903 )
    QUICK_CHECK_EQUALS(Arr2[5],         904 )
    QUICK_CHECK_EQUALS(Arr2[6],         101 )
    QUICK_CHECK_EQUALS(Arr2[7],         102 )
    QUICK_CHECK_EQUALS(Arr2[8],         400 )
    QUICK_CHECK_EQUALS(Arr2[9],         401 )
    QUICK_CHECK_EQUALS(Arr2[10],        402 )
    QUICK_CHECK_EQUALS(Arr2[11],        403 )
    QUICK_CHECK_EQUALS(Arr2[12],        404 )
    QUICK_CHECK_EQUALS(Arr2[13],        103 )
    QUICK_CHECK_EQUALS(Arr2[14],        104 )

    Arr1 = { 2000, 2001, 2002, 2003, 2004 };
    Arr2.AppendAt(0, Arr1);
    QUICK_CHECK_EQUALS( Arr2.GetSize(),     20 )
    QUICK_CHECK_EQUALS( Arr2[0],          2000 )
    QUICK_CHECK_EQUALS( Arr2[1],          2001 )
    QUICK_CHECK_EQUALS( Arr2[2],          2002 )
    QUICK_CHECK_EQUALS( Arr2[3],          2003 )
    QUICK_CHECK_EQUALS( Arr2[4],          2004 )
    QUICK_CHECK_EQUALS( Arr2[5],           100 )
    QUICK_CHECK_EQUALS( Arr2[6],           900 )
    QUICK_CHECK_EQUALS( Arr2[7],           901 )
    QUICK_CHECK_EQUALS( Arr2[8],           902 )
    QUICK_CHECK_EQUALS( Arr2[9],           903 )
    QUICK_CHECK_EQUALS( Arr2[10],          904 )
    QUICK_CHECK_EQUALS( Arr2[11],          101 )
    QUICK_CHECK_EQUALS( Arr2[12],          102 )
    QUICK_CHECK_EQUALS( Arr2[13],          400 )
    QUICK_CHECK_EQUALS( Arr2[14],          401 )
    QUICK_CHECK_EQUALS( Arr2[15],          402 )
    QUICK_CHECK_EQUALS( Arr2[16],          403 )
    QUICK_CHECK_EQUALS( Arr2[17],          404 )
    QUICK_CHECK_EQUALS( Arr2[18],          103 )
    QUICK_CHECK_EQUALS( Arr2[19],          104 )

    Arr2.AppendAt(Arr2.GetSize(), OtherData, 5);
    QUICK_CHECK_EQUALS( Arr2.GetSize(),     25 )
    QUICK_CHECK_EQUALS( Arr2[0],          2000 )
    QUICK_CHECK_EQUALS( Arr2[19],          104 )
    QUICK_CHECK_EQUALS( Arr2[20],          400 )
    QUICK_CHECK_EQUALS( Arr2[21],          401 )
    QUICK_CHECK_EQUALS( Arr2[22],          402 )
    QUICK_CHECK_EQUALS( Arr2[23],          403 )
    QUICK_CHECK_EQUALS( Arr2[24],          404 )

    Arr2.AppendAt(Arr2.GetSize(), std::move(Arr1)),
    QUICK_CHECK_EQUALS( Arr2.GetSize(),     30 )
    QUICK_CHECK_EQUALS( Arr2[0],          2000 )
    QUICK_CHECK_EQUALS( Arr2[19],          104 )
    QUICK_CHECK_EQUALS( Arr2[20],          400 )
    QUICK_CHECK_EQUALS( Arr2[21],          401 )
    QUICK_CHECK_EQUALS( Arr2[22],          402 )
    QUICK_CHECK_EQUALS( Arr2[23],          403 )
    QUICK_CHECK_EQUALS( Arr2[24],          404 )
    QUICK_CHECK_EQUALS( Arr2[25],         2000 )
    QUICK_CHECK_EQUALS( Arr2[26],         2001 )
    QUICK_CHECK_EQUALS( Arr2[27],         2002 )
    QUICK_CHECK_EQUALS( Arr2[28],         2003 )
    QUICK_CHECK_EQUALS( Arr2[29],         2004 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr1.GetLast(),        nullptr )

    Arr2.Empty();
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetLast(),        nullptr )

    Arr1 = { 1, 2, 3, 4, 5, 10, 20, 30, 40, 50 };
    QUICK_CHECK_TRUE(   Arr1.Contains( 1) )
    QUICK_CHECK_TRUE(   Arr1.Contains( 2) )
    QUICK_CHECK_TRUE(   Arr1.Contains( 3) )
    QUICK_CHECK_TRUE(   Arr1.Contains( 4) )
    QUICK_CHECK_TRUE(   Arr1.Contains( 5) )
    QUICK_CHECK_FALSE(  Arr1.Contains( 6) )
    QUICK_CHECK_FALSE(  Arr1.Contains( 7) )
    QUICK_CHECK_TRUE(   Arr1.Contains(50) )
    QUICK_CHECK_TRUE(   Arr1.Contains(40) )
    QUICK_CHECK_TRUE(   Arr1.Contains(30) )
    QUICK_CHECK_FALSE(  Arr1.Contains(51) )

    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  1; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  2; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  3; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  4; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  5; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  6; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  7; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 50; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 40; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 30; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 51; } ) )

    QUICK_CHECK_EQUALS( Arr1.Remove(1),        1 )
    QUICK_CHECK_EQUALS( Arr1.Remove(2),        1 )
    QUICK_CHECK_EQUALS( Arr1.RemoveOnce(3), true )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  1; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  2; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  3; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  4; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  5; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  6; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element ==  7; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 50; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 40; } ) )
    QUICK_CHECK_TRUE(   Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 30; } ) )
    QUICK_CHECK_FALSE(  Arr1.ContainsByPredicate([](const i32& Element) -> bool { return Element == 51; } ) )

    delete[] OtherData;
    return;
}

namespace Jafg::Testing::Array
{

inline i64 Ctor = 0;
inline i64 Dtor = 0;
inline i64 Copy = 0;
inline i64 Move = 0;

} /* Namespace Jafg::Testing::Array */

TEST_CASE(SemanticsArray, "Lal.Containers")
{
    using namespace Jafg;
    using namespace Jafg::Testing::Array;

    struct S final
    {
        FORCEINLINE  S() noexcept { ++Ctor; }
        FORCEINLINE  S(const S&) noexcept { ++Copy; }
        FORCEINLINE  S(S&&) noexcept { ++Move; }
        FORCEINLINE ~S() noexcept { ++Dtor; }
        FORCEINLINE  S& operator=(const S&) noexcept { ++Copy; return *this; }
        FORCEINLINE  S& operator=(S&&) noexcept { ++Move; return *this; }
    };

    TArray<S> Arr1;
    QUICK_CHECK_EQUALS(Ctor,                0)
    QUICK_CHECK_EQUALS(Dtor,                0)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.AddDefault();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      1)
    QUICK_CHECK_EQUALS(Ctor,                1)
    QUICK_CHECK_EQUALS(Dtor,                0)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.AddZeroed();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      2)
    QUICK_CHECK_EQUALS(Ctor,                1)
    QUICK_CHECK_EQUALS(Dtor,                0)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.AddUninitialized();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      3)
    QUICK_CHECK_EQUALS(Ctor,                1)
    QUICK_CHECK_EQUALS(Dtor,                0)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.Empty();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      0)
    QUICK_CHECK_EQUALS(Ctor,                1)
    QUICK_CHECK_EQUALS(Dtor,                3)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.AddDefault(10);
    QUICK_CHECK_EQUALS(Arr1.GetSize(),     10)
    QUICK_CHECK_EQUALS(Ctor,               11)
    QUICK_CHECK_EQUALS(Dtor,                3)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.Reset(0);
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      0)
    QUICK_CHECK_EQUALS(Ctor,               11)
    QUICK_CHECK_EQUALS(Dtor,               13)
    QUICK_CHECK_EQUALS(Copy,                0)
    QUICK_CHECK_EQUALS(Move,                0)

    TArray<S> Arr2 = { S(), S(), S(), S(), S() };
    QUICK_CHECK_EQUALS(Arr2.GetSize(),      5)
    QUICK_CHECK_EQUALS(Ctor,               16)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,                5)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr2.AppendAt(3, Arr1.GetData(), 5);
    QUICK_CHECK_EQUALS(Arr2.GetSize(),     10)
    QUICK_CHECK_EQUALS(Ctor,               16)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               10)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr2.AppendAt(2, std::move(Arr1));
    QUICK_CHECK_EQUALS(Arr2.GetSize(),     10)
    QUICK_CHECK_EQUALS(Ctor,               16)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               10)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.EmplaceAt(0);
    Arr1.Emplace();
    Arr1.EmplaceAt(2);
    Arr1.AddDefault();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      4)
    QUICK_CHECK_EQUALS(Ctor,               20)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               10)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr2.AppendAt(4, std::move(Arr1));
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      0)
    QUICK_CHECK_EQUALS(Arr2.GetSize(),     14)
    QUICK_CHECK_EQUALS(Ctor,               20)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               10)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1.Empty();
    QUICK_CHECK_EQUALS(Arr1.GetSize(),      0)
    QUICK_CHECK_EQUALS(Arr2.GetSize(),     14)
    QUICK_CHECK_EQUALS(Ctor,               20)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               10)
    QUICK_CHECK_EQUALS(Move,                0)

    Arr1 = Arr2;
    QUICK_CHECK_EQUALS(Arr1.GetSize(),     14)
    QUICK_CHECK_EQUALS(Arr2.GetSize(),     14)
    QUICK_CHECK_EQUALS(Ctor,               20)
    QUICK_CHECK_EQUALS(Dtor,               18)
    QUICK_CHECK_EQUALS(Copy,               24)
    QUICK_CHECK_EQUALS(Move,                0)

    return;
}

TEST_CASE(CapacityArray, "Lal.Containers")
{
    using namespace Jafg;

    TArray<i32> Arr;
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetLast(),        nullptr )

    Arr.Reserve(500);
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetSize(),                  0 )
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetCapacity(),            500 )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetData()                     )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetSlack()                    )
    CHECK_NULL(      "Array with reserved size.", Arr.GetFirst()                    )
    CHECK_NULL(      "Array with reserved size.", Arr.GetLast()                     )

    Arr.Shrink();
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetLast(),        nullptr )

    Arr.Reserve(500);
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetSize(),                  0 )
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetCapacity(),            500 )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetData()                     )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetSlack()                    )
    CHECK_NULL(      "Array with reserved size.", Arr.GetFirst()                    )
    CHECK_NULL(      "Array with reserved size.", Arr.GetLast()                     )

    Arr.AddDefault(); Arr.AddDefault(); Arr.AddDefault(); Arr.AddDefault(); Arr.AddDefault();
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetSize(),                  5 )
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetCapacity(),            500 )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetData()                     )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetSlack()                    )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetFirst()                    )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetLast()                     )

    Arr.Shrink();
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetSize(),                  5 )
    CHECK_EQUALS(    "Array with reserved size.", Arr.GetCapacity(),              5 )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetData()                     )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetSlack()                    )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetFirst()                    )
    CHECK_NOT_NULL(  "Array with reserved size.", Arr.GetLast()                     )

    return;
}
