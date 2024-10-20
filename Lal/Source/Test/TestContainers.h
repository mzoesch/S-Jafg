// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleIntegerArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<int32> Arr;
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

    TdhArray<LString> Arr;
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

    std::string* ZeroPtr = &Arr[0];
    std::string* OnePtr = &Arr[1];
    std::string* TwoPtr = &Arr[2];
    std::string* ThreePtr = &Arr[3];
    std::string* FourPtr = &Arr[4];
    std::string* FivePtr = &Arr[5];
    std::string* SixPtr = &Arr[6];
    std::string* SevenPtr = &Arr[7];

    return;
}
