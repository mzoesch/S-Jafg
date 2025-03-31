// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

#include "Containers/JafgArray.h"

TEST_CASE(SimpleIntegerArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TArrayBase<i32> Arr;
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetLast(),        nullptr )

    Arr.Add(1);
    QUICK_CHECK_EQUALS(Arr.GetSize(), 1)

    Arr.Add(10);
    Arr.Add(100);
    Arr.Add(1000);
    QUICK_CHECK_EQUALS(Arr.GetSize(), 4)

    Arr.Add(50);
    Arr.Add(51);
    Arr.Add(52);
    Arr.Add(53);
    Arr.Add(54);
    Arr.Add(55);
    Arr.Add(56);
    Arr.Add(57);
    Arr.Add(58);
    Arr.Add(59);
    QUICK_CHECK_EQUALS(Arr.GetSize(), 14)

    Arr.AddAt(0, -5);
    Arr.AddAt(1, -4);
    Arr.AddAt(2, -3);
    Arr.AddAt(3, -2);
    Arr.AddAt(4, -1);
    Arr.AddAt(5,  0);
    QUICK_CHECK_EQUALS(Arr.GetSize(), 20)
    QUICK_CHECK_EQUALS(Arr[0],   -5)
    QUICK_CHECK_EQUALS(Arr[1],   -4)
    QUICK_CHECK_EQUALS(Arr[2],   -3)
    QUICK_CHECK_EQUALS(Arr[3],   -2)
    QUICK_CHECK_EQUALS(Arr[4],   -1)
    QUICK_CHECK_EQUALS(Arr[5],    0)
    QUICK_CHECK_EQUALS(Arr[6],    1)
    QUICK_CHECK_EQUALS(Arr[7],   10)
    QUICK_CHECK_EQUALS(Arr[8],  100)
    QUICK_CHECK_EQUALS(Arr[9], 1000)
    QUICK_CHECK_EQUALS(*Arr.GetLast(),  59)
    QUICK_CHECK_EQUALS(*Arr.GetFirst(), -5)

    Arr.Empty();
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetLast(),        nullptr )

    Arr = { 100, 101, 102, 103, 104 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr.GetSize(),      5 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[0],           100 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[1],           101 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[2],           102 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[3],           103 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr[4],           104 )

    TArrayBase<i32> Arr2 = { 200, 201, 202, 203, 204 };
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2.GetSize(),   5 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[0],        200 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[1],        201 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[2],        202 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[3],        203 )
    CHECK_EQUALS( "Array with std::initializer_list.", Arr2[4],        204 )

    void* DataPtr = Arr2.GetData();
    Arr2 = Arr;
    CHECK_EQUALS( "Array copied.", Arr.GetSize(),          5 )
    CHECK_EQUALS( "Array copied.", Arr[0],               100 )
    CHECK_EQUALS( "Array copied.", Arr[1],               101 )
    CHECK_EQUALS( "Array copied.", Arr[2],               102 )
    CHECK_EQUALS( "Array copied.", Arr[3],               103 )
    CHECK_EQUALS( "Array copied.", Arr[4],               104 )
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

    DataPtr = Arr.GetData();
    Arr2 = std::move(Arr);
    CHECK_EQUALS( "Array moved.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array moved.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array moved.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array moved.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array moved.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array moved.", Arr.GetLast(),        nullptr )
    CHECK_EQUALS( "Array moved.", Arr2.GetSize(),             5 )
    CHECK_EQUALS( "Array moved.", Arr2[0],                  100 )
    CHECK_EQUALS( "Array moved.", Arr2[1],                  101 )
    CHECK_EQUALS( "Array moved.", Arr2[2],                  102 )
    CHECK_EQUALS( "Array moved.", Arr2[3],                  103 )
    CHECK_EQUALS( "Array moved.", Arr2[4],                  104 )
    CHECK_EQUALS( "Array moved.", Arr2.GetData(),       DataPtr )

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

    Arr = { 900, 901, 902, 903, 904 };
    Arr2.AppendAt(1, Arr);
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

    Arr = { 2000, 2001, 2002, 2003, 2004 };
    Arr2.AppendAt(0, Arr);
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

    Arr2.AppendAt(Arr2.GetSize(), std::move(Arr)),
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
    CHECK_EQUALS( "Array with zero size.", Arr.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr.GetLast(),        nullptr )

    Arr2.Empty();
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSize(),              0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetCapacity(),          0 )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetData(),        nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetSlack(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetFirst(),       nullptr )
    CHECK_EQUALS( "Array with zero size.", Arr2.GetLast(),        nullptr )

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

    TArrayBase<S> Arr1;
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

    TArrayBase<S> Arr2 = { S(), S(), S(), S(), S() };
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
