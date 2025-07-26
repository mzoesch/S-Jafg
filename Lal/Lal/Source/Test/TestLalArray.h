// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

#include "Containers/LalArray.h"

namespace Testing::Lal::Array
{

struct LOne
{
    LOne() = default;
    LOne(LSize InValue) : Value(InValue) { }
    LSize Value { 0 };
};

struct LTwo
{
    LTwo() = default;
    LTwo(LSize InValue) : Value(InValue) { }
    LTwo(const LOne& InOne) : Value(InOne.Value) { }

    LSize Value { 0 };
};

} /* Namespace Testing::Lal::Array */

TEST_CASE(EmptyArrayOperations, "Lal.Containers")
{
    constexpr LSize TestSizeCapacity { 10 };

    TArray<i64> Array;
    TArrayView<i64> ArrayView;
    TMutableArrayView<i64> MutableArrayView;
    TFixedArray<i64, TestSizeCapacity> FixedArray;
    TStackArray<i64, TestSizeCapacity> StackArray;
    TStackOptimizedArray<i64, TestSizeCapacity> StackOptimizedArray;

    static_assert(Array.IsContentConst() == false);
    static_assert(Array.IsContentMutable() == true);
    static_assert(ArrayView.IsContentConst() == true);
    static_assert(ArrayView.IsContentMutable() == false);
    static_assert(MutableArrayView.IsContentConst() == false);
    static_assert(MutableArrayView.IsContentMutable() == true);
    static_assert(FixedArray.IsContentConst() == false);
    static_assert(FixedArray.IsContentMutable() == true);
    static_assert(StackArray.IsContentConst() == false);
    static_assert(StackArray.IsContentMutable() == true);
    static_assert(StackOptimizedArray.IsContentConst() == false);
    static_assert(StackOptimizedArray.IsContentMutable() == true);
    this->IncreaseTestCountBy(12);

    static_assert(Array.IsWeaklyAllocated() == false);
    static_assert(Array.IsStronglyAllocated() == true);
    static_assert(ArrayView.IsWeaklyAllocated() == true);
    static_assert(ArrayView.IsStronglyAllocated() == false);
    static_assert(MutableArrayView.IsWeaklyAllocated() == true);
    static_assert(MutableArrayView.IsStronglyAllocated() == false);
    static_assert(FixedArray.IsWeaklyAllocated() == true);
    static_assert(FixedArray.IsStronglyAllocated() == false);
    static_assert(StackArray.IsWeaklyAllocated() == true);
    static_assert(StackArray.IsStronglyAllocated() == false);
    static_assert(StackOptimizedArray.IsWeaklyAllocated() == false);
    static_assert(StackOptimizedArray.IsStronglyAllocated() == true);
    this->IncreaseTestCountBy(12);

    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(ArrayView.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableArrayView.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)

    QUICK_CHECK_EQUALS(Array.GetAllocatedByteSize(), 0lu)
    QUICK_CHECK_EQUALS(ArrayView.GetAllocatedByteSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableArrayView.GetAllocatedByteSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray.GetAllocatedByteSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray.GetAllocatedByteSize(), TestSizeCapacity * sizeof(i64))
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetAllocatedByteSize(), TestSizeCapacity * sizeof(i64))

    QUICK_CHECK_EQUALS(Array.GetDataPointer(), Array.GetSlackPointer())
    QUICK_CHECK_EQUALS(ArrayView.GetDataPointer(), ArrayView.GetSlackPointer())
    QUICK_CHECK_EQUALS(MutableArrayView.GetDataPointer(), MutableArrayView.GetSlackPointer())
    QUICK_CHECK_EQUALS(FixedArray.GetDataPointer(), FixedArray.GetSlackPointer())
    QUICK_CHECK_EQUALS(StackArray.GetDataPointer(), StackArray.GetSlackPointer())
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArray.GetSlackPointer())

    QUICK_CHECK_EQUALS(Array.GetCapacityPointer(), Array.GetDataPointer() + Array.GetCapacity())
    QUICK_CHECK_EQUALS(ArrayView.GetCapacityPointer(), ArrayView.GetDataPointer() + ArrayView.GetCapacity())
    QUICK_CHECK_EQUALS(MutableArrayView.GetCapacityPointer(), MutableArrayView.GetDataPointer() + MutableArrayView.GetCapacity())
    QUICK_CHECK_EQUALS(FixedArray.GetCapacityPointer(), FixedArray.GetDataPointer() + FixedArray.GetCapacity())
    QUICK_CHECK_EQUALS(StackArray.GetCapacityPointer(), StackArray.GetDataPointer() + StackArray.GetCapacity())
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacityPointer(), StackOptimizedArray.GetDataPointer() + StackOptimizedArray.GetCapacity())

    return;
}

TEST_CASE(CopyArrayOperations, "Lal.Containers")
{
    constexpr LSize TestSize { 512 };
    constexpr LSize TestSizeCapacity { 128 };

    TArray<LSize> Array1 { 1, 2, 3, 4, 5 };
    TArray<LSize> Array2 { Array1 };
    TArrayView<LSize> ArrayView1 { Array1 };
    TArrayView<LSize> ArrayView2 { Array2 };
    TMutableArrayView<LSize> MutableArrayView1 { Array1 };
    TMutableArrayView<LSize> MutableArrayView2 { Array2 };
    TFixedArray<LSize, TestSizeCapacity> FixedArray1 { 1, 2, 3, 4, 5 };
    TFixedArray<LSize, TestSizeCapacity> FixedArray2 { FixedArray1 };
    TStackArray<LSize, TestSizeCapacity> StackArray1 { 1, 2, 3, 4, 5 };
    TStackArray<LSize, TestSizeCapacity> StackArray2 { StackArray1 };
    TStackOptimizedArray<LSize, TestSizeCapacity> StackOptimizedArray1 { 1, 2, 3, 4, 5 };
    TStackOptimizedArray<LSize, TestSizeCapacity> StackOptimizedArray2 { StackOptimizedArray1 };

    QUICK_CHECK_NOT_EQUALS(Array1.GetDataPointer(), Array2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(ArrayView1.GetDataPointer(), ArrayView2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(MutableArrayView1.GetDataPointer(), MutableArrayView2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(FixedArray1.GetDataPointer(), FixedArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackArray1.GetDataPointer(), StackArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArray1.GetDataPointer(), StackOptimizedArray2.GetDataPointer())

    for (LSize Idx { 0 }; Idx < Array1.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array1[Idx], Array2[Idx])
        QUICK_CHECK_EQUALS(ArrayView1[Idx], ArrayView2[Idx])
        QUICK_CHECK_EQUALS(MutableArrayView1[Idx], MutableArrayView2[Idx])
        QUICK_CHECK_EQUALS(FixedArray1[Idx], FixedArray2[Idx])
        QUICK_CHECK_EQUALS(StackArray1[Idx], StackArray2[Idx])
        QUICK_CHECK_EQUALS(StackOptimizedArray1[Idx], StackOptimizedArray2[Idx])

        continue;
    }

    Array1.Empty();
    ArrayView1.Empty();
    MutableArrayView1.Empty();
    FixedArray1.Empty();
    StackArray1.Empty();
    StackOptimizedArray1.Empty();

    QUICK_CHECK_EQUALS(Array1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(ArrayView1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableArrayView1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), 0lu)

    QUICK_CHECK_EQUALS(ArrayView2.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(MutableArrayView2.GetSize(), 5lu)

    ArrayView2.Empty();
    MutableArrayView2.Empty();
    QUICK_CHECK_EQUALS(ArrayView2.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableArrayView2.GetSize(), 0lu)

    for (LSize Idx { 0 }; Idx < TestSize; ++Idx)
    {
        Array1.Add(Idx);
        StackOptimizedArray1.Add(Idx);

        continue;
    }

    for (LSize Idx { 0 }; Idx < TestSizeCapacity; ++Idx)
    {
        FixedArray1.Add(Idx);
        StackArray1.Add(Idx);

        continue;
    }

    QUICK_CHECK_EQUALS(Array1.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), TestSize)
    for (LSize Idx { 0 }; Idx < TestSize; ++Idx)
    {
        QUICK_CHECK_EQUALS(Array1[Idx], Idx)
        QUICK_CHECK_EQUALS(StackOptimizedArray1[Idx], Idx)

        continue;
    }
    for (LSize Idx { 0 }; Idx < TestSizeCapacity; ++Idx)
    {
        QUICK_CHECK_EQUALS(FixedArray1[Idx], Idx)
        QUICK_CHECK_EQUALS(StackArray1[Idx], Idx)

        continue;
    }

    TArray<LSize>::Pointer ArrayPointer { Array1.GetDataPointer() };
    TFixedArray<LSize, TestSizeCapacity>::Pointer FixedArrayPointer { FixedArray1.GetDataPointer() };
    TStackArray<LSize, TestSizeCapacity>::Pointer StackArrayPointer { StackArray1.GetDataPointer() };
    TStackOptimizedArray<LSize, TestSizeCapacity>::Pointer StackOptimizedArrayPointer { StackOptimizedArray1.GetDataPointer() };
    QUICK_CHECK_EQUALS(ArrayPointer, Array1.GetDataPointer())
    QUICK_CHECK_EQUALS(FixedArrayPointer, FixedArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(StackArrayPointer, StackArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray1.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(ArrayPointer, Array2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(FixedArrayPointer, FixedArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackArrayPointer, StackArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray2.GetDataPointer())
    Array2 = std::move(Array1);
    FixedArray2 = std::move(FixedArray1);
    StackArray2 = std::move(StackArray1);
    StackOptimizedArray2 = std::move(StackOptimizedArray1);
    QUICK_CHECK_EQUALS(Array1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array2.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(FixedArray2.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray2.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray2.GetSize(), TestSize)
    QUICK_CHECK_NOT_EQUALS(ArrayPointer, Array1.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(FixedArrayPointer, FixedArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(StackArrayPointer, StackArray1.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(ArrayPointer, Array2.GetDataPointer())
    QUICK_CHECK_EQUALS(FixedArrayPointer, FixedArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackArrayPointer, StackArray2.GetDataPointer())
    QUICK_CHECK_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray2.GetDataPointer())

    Array1 = std::move(Array2);
    FixedArray1 = std::move(FixedArray2);
    StackArray1 = std::move(StackArray2);
    StackOptimizedArray1 = std::move(StackOptimizedArray2);
    QUICK_CHECK_EQUALS(Array1.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(Array2.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray2.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray2.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray2.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(ArrayPointer, Array1.GetDataPointer())
    QUICK_CHECK_EQUALS(FixedArrayPointer, FixedArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(StackArrayPointer, StackArray1.GetDataPointer())
    QUICK_CHECK_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray1.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(ArrayPointer, Array2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(FixedArrayPointer, FixedArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackArrayPointer, StackArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArrayPointer, StackOptimizedArray2.GetDataPointer())
    for (LSize Idx { 0 }; Idx < TestSize; ++Idx)
    {
        QUICK_CHECK_EQUALS(Array1[Idx], Idx)
        QUICK_CHECK_EQUALS(StackOptimizedArray1[Idx], Idx)

        continue;
    }
    for (LSize Idx { 0 }; Idx < TestSizeCapacity; ++Idx)
    {
        QUICK_CHECK_EQUALS(FixedArray1[Idx], Idx)
        QUICK_CHECK_EQUALS(StackArray1[Idx], Idx)

        continue;
    }

    Array2 = Array1;
    FixedArray2 = FixedArray1;
    StackArray2 = StackArray1;
    StackOptimizedArray2 = StackOptimizedArray1;
    QUICK_CHECK_EQUALS(Array1.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(Array2.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(FixedArray2.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray2.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray2.GetSize(), TestSize)
    QUICK_CHECK_NOT_EQUALS(Array1.GetDataPointer(), Array2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(FixedArray1.GetDataPointer(), FixedArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackArray1.GetDataPointer(), StackArray2.GetDataPointer())
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArray1.GetDataPointer(), StackOptimizedArray2.GetDataPointer())
    for (LSize Idx { 0 }; Idx < TestSize; ++Idx)
    {
        QUICK_CHECK_EQUALS(Array1[Idx], Array2[Idx])
        QUICK_CHECK_EQUALS(StackOptimizedArray1[Idx], StackOptimizedArray2[Idx]);

        continue;
    }
    for (LSize Idx { 0 }; Idx < TestSizeCapacity; ++Idx)
    {
        QUICK_CHECK_EQUALS(FixedArray1[Idx], FixedArray2[Idx])
        QUICK_CHECK_EQUALS(StackArray1[Idx], StackArray2[Idx]);

        continue;
    }

    return;
}

TEST_CASE(InitializerListArrayOperations, "Lal.Containers")
{
    typedef Testing::Lal::Array::LOne LOne;
    typedef Testing::Lal::Array::LTwo LTwo;

    constexpr LSize TestSizeCapacity { 10 };

    TArray<i64> Array1 { };
    TArray<i64> Array2 { 1, 2, 3, 4, 5 };
    TArrayView<i64> ArrayView;
    TMutableArrayView<i64> MutableArrayView;
    TFixedArray<i64, TestSizeCapacity> FixedArray1 { };
    TFixedArray<i64, TestSizeCapacity> FixedArray2 { 1, 2, 3, 4, 5 };
    TStackArray<i64, TestSizeCapacity> StackArray1 { };
    TStackArray<i64, TestSizeCapacity> StackArray2 { 1, 2, 3, 4, 5 };
    TStackOptimizedArray<i64, TestSizeCapacity> StackOptimizedArray1 { };
    TStackOptimizedArray<i64, TestSizeCapacity> StackOptimizedArray2 { 1, 2, 3, 4, 5 };
    TStackOptimizedArray<i64, TestSizeCapacity> StackOptimizedArray3 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TStackOptimizedArray<i64, TestSizeCapacity> StackOptimizedArray4 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    QUICK_CHECK_EQUALS(Array1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array2.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(ArrayView.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableArrayView.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray2.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(StackArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray2.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray1.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray2.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray3.GetSize(), 10lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray4.GetSize(), 11lu)

    QUICK_CHECK_FALSE(Array1.IsCurrentDataOnHeap())
    QUICK_CHECK_TRUE(Array2.IsCurrentDataOnHeap())
    // QUICK_CHECK_FALSE(ArrayView.IsCurrentDataOnHeap())
    // QUICK_CHECK_FALSE(MutableArrayView.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(FixedArray1.IsCurrentDataOnHeap())
    QUICK_CHECK_TRUE(FixedArray2.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(StackArray1.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(StackArray2.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(StackOptimizedArray1.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(StackOptimizedArray2.IsCurrentDataOnHeap())
    QUICK_CHECK_FALSE(StackOptimizedArray3.IsCurrentDataOnHeap())
    QUICK_CHECK_TRUE(StackOptimizedArray4.IsCurrentDataOnHeap());

    TArray<LTwo> X { LOne{ 1 }, LOne{ 2 }, LOne{ 3 }, };
    QUICK_CHECK_EQUALS(X.GetSize(), 3lu)
    QUICK_CHECK_EQUALS(X[0].Value, 1lu)
    QUICK_CHECK_EQUALS(X[1].Value, 2lu)
    QUICK_CHECK_EQUALS(X[2].Value, 3lu)

    X = { LOne{ 1 }, LOne{ 2 }, LOne{ 3 }, LOne{ 4 }, LOne{ 5 }, };
    QUICK_CHECK_EQUALS(X.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(X[0].Value, 1lu)
    QUICK_CHECK_EQUALS(X[1].Value, 2lu)
    QUICK_CHECK_EQUALS(X[2].Value, 3lu)
    QUICK_CHECK_EQUALS(X[3].Value, 4lu)
    QUICK_CHECK_EQUALS(X[4].Value, 5lu)
    X = { LOne{ 1 }, LOne{ 2 }, LOne{ 3 }, LOne{ 4 }, LOne{ 5 }, LOne{ 6 }, };
    QUICK_CHECK_EQUALS(X.GetSize(), 6lu)
    QUICK_CHECK_EQUALS(X[0].Value, 1lu)
    QUICK_CHECK_EQUALS(X[1].Value, 2lu)
    QUICK_CHECK_EQUALS(X[2].Value, 3lu)
    QUICK_CHECK_EQUALS(X[3].Value, 4lu)
    QUICK_CHECK_EQUALS(X[4].Value, 5lu)
    QUICK_CHECK_EQUALS(X[5].Value, 6lu)

    return;
}

TEST_CASE(ArrayAddOperations, "Lal.Containers")
{
    constexpr LSize TestSizeCapacity { 10 };

    TArray<LSize> Array;
    TFixedArray<LSize, TestSizeCapacity> FixedArray;
    TStackArray<LSize, TestSizeCapacity> StackArray;
    TStackOptimizedArray<LSize, TestSizeCapacity> StackOptimizedArray;

    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(FixedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)

    for (LSize Index { 0 }; Index < TestSizeCapacity; ++Index)
    {
        Array.Add(Index);
        FixedArray.Add(Index);
        StackArray.Add(Index);
        StackOptimizedArray.Add(Index);

        continue;
    }

    QUICK_CHECK_EQUALS(Array.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(FixedArray.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), TestSizeCapacity)

    for (LSize Index { 0 }; Index < TestSizeCapacity; ++Index)
    {
        QUICK_CHECK_EQUALS(Array[Index], Index)
        QUICK_CHECK_EQUALS(FixedArray[Index], Index)
        QUICK_CHECK_EQUALS(StackArray[Index], Index)
        QUICK_CHECK_EQUALS(StackOptimizedArray[Index], Index)

        continue;
    }

    for (LSize Index { TestSizeCapacity }; Index < TestSizeCapacity * 2; ++Index)
    {
        Array.Add(Index);
        StackOptimizedArray.Add(Index);

        continue;
    }

    QUICK_CHECK_EQUALS(Array.GetSize(), TestSizeCapacity * 2)
    QUICK_CHECK_EQUALS(FixedArray.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackArray.GetSize(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), TestSizeCapacity * 2)

    for (LSize Index { 0 }; Index < TestSizeCapacity * 2; ++Index)
    {
        QUICK_CHECK_EQUALS(Array[Index], Index)
        QUICK_CHECK_EQUALS(StackOptimizedArray[Index], Index)

        continue;
    }

    return;
}

TEST_CASE(ArrayManyAddOperations, "Lal.Containers")
{
    constexpr LSize TestSizeCapacity { 64 };

    TArray<LSize> Array;
    TStackOptimizedArray<LSize, TestSizeCapacity> StackOptimizedArray;

    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)

    constexpr LSize TestSize { 8192 };
    for (LSize Index { 0 }; Index < TestSize; ++Index)
    {
        Array.Add(Index);
        StackOptimizedArray.Add(Index);

        continue;
    }

    QUICK_CHECK_EQUALS(Array.GetSize(), TestSize)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), TestSize)

    for (LSize Index { 0 }; Index < TestSize; ++Index)
    {
        QUICK_CHECK_EQUALS(Array[Index], Index)
        QUICK_CHECK_EQUALS(StackOptimizedArray[Index], Index)

        continue;
    }

    return;
}

TEST_CASE(ArrayShrinkGrowthOperations, "Lal.Containers")
{
    constexpr LSize TestSizeCapacity { 256 };

    TArray<LSize> Array;
    TStackOptimizedArray<LSize, TestSizeCapacity> StackOptimizedArray;

    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    Array.ShrinkToFit();
    StackOptimizedArray.ShrinkToFit();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    Array.ShrinkToFit();
    StackOptimizedArray.ShrinkToFit();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);

    Array.Reserve(TestSizeCapacity);
    StackOptimizedArray.Reserve(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);

    TArray<LSize>::Pointer ArrayPointer { Array.GetDataPointer() };
    TStackOptimizedArray<LSize, TestSizeCapacity>::Pointer StackOptimizedArrayPointer { StackOptimizedArray.GetDataPointer() };
    Array.Reserve(TestSizeCapacity);
    StackOptimizedArray.Reserve(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), ArrayPointer);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.ShrinkToFit();
    StackOptimizedArray.ShrinkToFit();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Reserve(0);
    StackOptimizedArray.Reserve(0);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu);
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Reserve(TestSizeCapacity);
    StackOptimizedArray.Reserve(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    ArrayPointer = Array.GetDataPointer();
    Array.Reserve(TestSizeCapacity);
    StackOptimizedArray.Reserve(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), ArrayPointer);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Reserve(TestSizeCapacity / 2);
    StackOptimizedArray.Reserve(TestSizeCapacity / 2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), ArrayPointer);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Reserve(0);
    StackOptimizedArray.Reserve(0);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), ArrayPointer);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.ShrinkToFit();
    StackOptimizedArray.ShrinkToFit();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Resize(0);
    StackOptimizedArray.Resize(0);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    Array.Resize(TestSizeCapacity);
    StackOptimizedArray.Resize(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);
    Array.Reserve(TestSizeCapacity * 2);
    StackOptimizedArray.Reserve(TestSizeCapacity * 2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity * 2)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity * 2)
    QUICK_CHECK_NOT_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);
    Array.Resize(0);
    StackOptimizedArray.Resize(0);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);
    Array.Resize(TestSizeCapacity);
    StackOptimizedArray.Resize(TestSizeCapacity);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);
    Array.Resize(TestSizeCapacity / 2);
    StackOptimizedArray.Resize(TestSizeCapacity / 2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), TestSizeCapacity / 2)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetCapacity(), TestSizeCapacity)
    QUICK_CHECK_EQUALS(StackOptimizedArray.GetDataPointer(), StackOptimizedArrayPointer);

    return;
}

TEST_CASE(AllocatorPunning, "Lal.Containers")
{
    TArray<LSize> Array { 1, 2, 3, 4, 5 };
    TArrayView<LSize> View;
    TMutableArrayView<LSize> MutableView;

    QUICK_CHECK_EQUALS(Array.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(View.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(MutableView.GetSize(), 0lu)
    View = Array;
    QUICK_CHECK_EQUALS(Array.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(View.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(MutableView.GetSize(), 0lu)
    for (LSize Idx { 0 }; Idx < View.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], View[Idx])
        continue;
    }

    MutableView = Array;
    QUICK_CHECK_EQUALS(Array.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(View.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(MutableView.GetSize(), 5lu)

    for (LSize Idx { 0 }; Idx < MutableView.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], MutableView[Idx])
        continue;
    }

    for (LSize Idx { 0 }; Idx < MutableView.GetSize(); ++Idx)
    {
        MutableView[Idx] *= 2;
        continue;
    }

    for (LSize Idx { 0 }; Idx < MutableView.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], MutableView[Idx])
        continue;
    }

    return;
}

TEST_CASE(ArrayIterators, "Lal.Containers")
{
    TArray<LSize> Array { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TArrayView<LSize> View { Array };
    TMutableArrayView<LSize> MutableView { Array };

    QUICK_CHECK_EQUALS(Array.GetSize(), View.GetSize())
    QUICK_CHECK_EQUALS(Array.GetSize(), MutableView.GetSize())

    LSize Cursor { 0 };
    for (LSize& Element : Array)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, Array.GetSize())

    Cursor = 0;
    for (const LSize& Element : View)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, View.GetSize())

    Cursor = 0;
    for (LSize& Element : MutableView)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, MutableView.GetSize())

    Cursor = 0;
    for (const LSize& Element : Array)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, Array.GetSize())

    Cursor = 0;
    for (const LSize& Element : View)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, View.GetSize())

    Cursor = 0;
    for (const LSize& Element : MutableView)
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, MutableView.GetSize())

    Cursor = 0;
    for (LSize& Element : Array.Iter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, Array.GetSize())

    Cursor = 0;
    for (const LSize& Element : View.Iter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, View.GetSize())

    Cursor = 0;
    for (LSize& Element : MutableView.Iter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, MutableView.GetSize())

    for (const LSize& Element : Array.Iter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = View.GetSize();
    for (const LSize& Element : View.Iter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = MutableView.GetSize();
    for (const LSize& Element : MutableView.Iter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    for (const LSize& Element : Array.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 0;
    for (const LSize& Element : View.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 0;
    for (const LSize& Element : MutableView.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 10;
    for (const LSize& Element : Array.Iter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.Iter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.Iter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : Array.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.Iter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : Array.Iter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.Iter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.Iter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 0;
    for (const LSize& Element : Array.CIter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, Array.GetSize())

    Cursor = 0;
    for (const LSize& Element : View.CIter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, View.GetSize())

    Cursor = 0;
    for (const LSize& Element : MutableView.CIter())
    {
        QUICK_CHECK_EQUALS(Element, ++Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, MutableView.GetSize())

    for (const LSize& Element : Array.CIter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = View.GetSize();
    for (const LSize& Element : View.CIter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = MutableView.GetSize();
    for (const LSize& Element : MutableView.CIter().Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor--)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    for (const LSize& Element : Array.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 0;
    for (const LSize& Element : View.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 0;
    for (const LSize& Element : MutableView.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        Cursor += 2;
        QUICK_CHECK_EQUALS(Element, Cursor)
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 10ul)

    Cursor = 10;
    for (const LSize& Element : Array.CIter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.CIter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.CIter().Reverse().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : Array.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.CIter().Filter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }).Reverse())
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : Array.CIter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : View.CIter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    Cursor = 10;
    for (const LSize& Element : MutableView.CIter().ReversedFilter([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    {
        QUICK_CHECK_EQUALS(Element, Cursor)
        Cursor -= 2;
        continue;
    }
    QUICK_CHECK_EQUALS(Cursor, 0ul)

    return;
}

TEST_CASE(ArraySwaps, "Lal.Containers")
{
    TArray<LSize> Array1 { 1, 2, 3, 4, 5 };
    TArray<LSize> Array2 { 6, 7, 8, 9, 10 };

    QUICK_CHECK_EQUALS(Array1.GetSize(), 5lu)
    QUICK_CHECK_EQUALS(Array2.GetSize(), 5lu)

    Array1.SwapIndices(0, 4);
    QUICK_CHECK_EQUALS(Array1[0], 5ul)
    QUICK_CHECK_EQUALS(Array1[1], 2ul)
    QUICK_CHECK_EQUALS(Array1[2], 3ul)
    QUICK_CHECK_EQUALS(Array1[3], 4ul)
    QUICK_CHECK_EQUALS(Array1[4], 1ul)

    Array1.SwapIndices(Array1.begin(), --Array1.end());
    QUICK_CHECK_EQUALS(Array1[0], 1ul)
    QUICK_CHECK_EQUALS(Array1[1], 2ul)
    QUICK_CHECK_EQUALS(Array1[2], 3ul)
    QUICK_CHECK_EQUALS(Array1[3], 4ul)
    QUICK_CHECK_EQUALS(Array1[4], 5ul)

    TArray<LSize>::Iterator It { Array1.begin() };
    std::advance(It, 2);
    QUICK_CHECK_EQUALS(Array1[It], 3ul)

    TArray<LSize>::ConstIterator CIt { Array1.cbegin() };
    std::advance(CIt, 2);
    QUICK_CHECK_EQUALS(Array1[CIt], 3ul)

    return;
}

TEST_CASE(ArrayComparisons, "Lal.Containers")
{
    const TArray<LSize> Array1 { 1, 2, 3, 4, 5 };
    const TArray<LSize> Array2 { 1, 2, 3, 4, 5 };
    const TArray<LSize> Array3 { 6, 7, 8, 9, 10, 11 };

    QUICK_CHECK_EQUALS(Array1 <=> Array2, std::strong_ordering::equal)
    QUICK_CHECK_EQUALS(Array1 <=> Array3, std::strong_ordering::less)
    QUICK_CHECK_EQUALS(Array2 <=> Array3, std::strong_ordering::less)
    QUICK_CHECK_EQUALS(Array3 <=> Array1, std::strong_ordering::greater)
    QUICK_CHECK_EQUALS(Array3 <=> Array2, std::strong_ordering::greater)

    QUICK_CHECK_FALSE(Array1 < Array2)
    QUICK_CHECK_TRUE(Array1 < Array3)
    QUICK_CHECK_FALSE(Array3 < Array1)
    QUICK_CHECK_TRUE(Array2 < Array3)
    QUICK_CHECK_FALSE(Array3 < Array2)
    QUICK_CHECK_FALSE(Array1 > Array2)
    QUICK_CHECK_FALSE(Array1 > Array3)
    QUICK_CHECK_TRUE(Array3 > Array1)
    QUICK_CHECK_FALSE(Array2 > Array3)
    QUICK_CHECK_TRUE(Array3 > Array2)
    QUICK_CHECK_TRUE(Array1 <= Array2)
    QUICK_CHECK_TRUE(Array1 <= Array3)
    QUICK_CHECK_FALSE(Array3 <= Array1)
    QUICK_CHECK_TRUE(Array2 <= Array3)
    QUICK_CHECK_FALSE(Array3 <= Array2)
    QUICK_CHECK_TRUE(Array1 >= Array2)
    QUICK_CHECK_FALSE(Array1 >= Array3)
    QUICK_CHECK_TRUE(Array3 >= Array1)
    QUICK_CHECK_FALSE(Array2 >= Array3)
    QUICK_CHECK_TRUE(Array3 >= Array2)

    QUICK_CHECK_TRUE(Array1.EqualInSizeTo(Array2))
    QUICK_CHECK_FALSE(Array1.EqualInSizeTo(Array3))
    QUICK_CHECK_TRUE(Array2.EqualInSizeTo(Array1))
    QUICK_CHECK_FALSE(Array2.EqualInSizeTo(Array3))
    QUICK_CHECK_FALSE(Array3.EqualInSizeTo(Array1))
    QUICK_CHECK_FALSE(Array3.EqualInSizeTo(Array2))

    QUICK_CHECK_TRUE(Array1.IsDataEqual(Array2))
    QUICK_CHECK_TRUE(Array2.IsDataEqual(Array1))
    QUICK_CHECK_FALSE(Array1.IsDataEqual(Array3))
    QUICK_CHECK_FALSE(Array2.IsDataEqual(Array3))

    QUICK_CHECK_FALSE(Array1.IsSameArray(Array2))
    QUICK_CHECK_FALSE(Array2.IsSameArray(Array1))
    QUICK_CHECK_FALSE(Array1.IsSameArray(Array3))
    QUICK_CHECK_FALSE(Array2.IsSameArray(Array3))

    return;
}

TEST_CASE(AddArray, "Lal.Containers")
{
    TArray<LSize> Array;

    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)

    Array.Add(1);
    QUICK_CHECK_EQUALS(Array.GetSize(), 1lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)

    Array.Add(2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 2lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)

    Array.Empty();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);

    TArray<LSize>::Iterator It { Array.AddAt(0, 1) };
    QUICK_CHECK_EQUALS(Array.GetSize(), 1lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)
    QUICK_CHECK_EQUALS(It, Array.begin());
    QUICK_CHECK_EQUALS(*It, 1lu)

    It = Array.AddAt(0, 2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 2lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 1lu)
    QUICK_CHECK_EQUALS(It, Array.begin());
    QUICK_CHECK_EQUALS(*It, 2lu)

    It = Array.AddAt(2, 3);
    QUICK_CHECK_EQUALS(Array.GetSize(), 3lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 1lu)
    QUICK_CHECK_EQUALS(Array[2], 3lu)

    It = Array.AddAt(1, 4);
    QUICK_CHECK_EQUALS(Array.GetSize(), 4lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 4lu)
    QUICK_CHECK_EQUALS(Array[2], 1lu)
    QUICK_CHECK_EQUALS(Array[3], 3lu)

    Array.Empty();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)

    Array.Emplace(1);
    QUICK_CHECK_EQUALS(Array.GetSize(), 1lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)

    Array.Emplace(2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 2lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)

    Array.Empty();
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetCapacity(), 0lu)
    QUICK_CHECK_EQUALS(Array.GetDataPointer(), nullptr);

    It = Array.EmplaceAt(0, 1);
    QUICK_CHECK_EQUALS(Array.GetSize(), 1lu)
    QUICK_CHECK_EQUALS(Array[0], 1lu)
    QUICK_CHECK_EQUALS(It, Array.begin());
    QUICK_CHECK_EQUALS(*It, 1lu)

    It = Array.EmplaceAt(0, 2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 2lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 1lu)
    QUICK_CHECK_EQUALS(It, Array.begin());
    QUICK_CHECK_EQUALS(*It, 2lu)

    It = Array.EmplaceAt(2, 3);
    QUICK_CHECK_EQUALS(Array.GetSize(), 3lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 1lu)
    QUICK_CHECK_EQUALS(Array[2], 3lu)

    It = Array.EmplaceAt(1, 4);
    QUICK_CHECK_EQUALS(Array.GetSize(), 4lu)
    QUICK_CHECK_EQUALS(Array[0], 2lu)
    QUICK_CHECK_EQUALS(Array[1], 4lu)
    QUICK_CHECK_EQUALS(Array[2], 1lu)
    QUICK_CHECK_EQUALS(Array[3], 3lu)

    Array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    TArray<LSize> OtherArray { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    QUICK_CHECK_EQUALS(Array.GetSize(), 20lu)
    QUICK_CHECK_EQUALS(OtherArray.GetSize(), 20lu)

    Array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    Array.Append(Array);
    QUICK_CHECK_EQUALS(Array.GetSize(), 40lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, });
    QUICK_CHECK_EQUALS(Array.GetSize(), 50lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append(OtherArray.begin() + 10, OtherArray.end());
    QUICK_CHECK_EQUALS(Array.GetSize(), 60lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append(OtherArray.begin(), OtherArray.end() - 10);
    QUICK_CHECK_EQUALS(Array.GetSize(), 70lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append(OtherArray.begin() + 10, OtherArray.end());
    QUICK_CHECK_EQUALS(Array.GetSize(), 80lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append(OtherArray.begin(), OtherArray.end());
    QUICK_CHECK_EQUALS(Array.GetSize(), 100lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }
    Array.Append(OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 120lu)

    QUICK_CHECK_EQUALS(OtherArray.GetSize(), 20lu)
    Array.Append(std::move(OtherArray));
    QUICK_CHECK_EQUALS(Array.GetSize(), 140lu)
    QUICK_CHECK_EQUALS(OtherArray.GetSize(), 0lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], (Idx % 20) + 1)
        continue;
    }

    OtherArray = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    QUICK_CHECK_EQUALS(OtherArray.GetSize(), 20lu)

    Array.AppendAt(0, OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 160lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], OtherArray[Idx])
        continue;
    }

    Array.AppendAt(20, OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 180lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx + 20], OtherArray[Idx])
        continue;
    }

    Array.AppendAt(Array.begin() + 20, OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 200lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx + 20], OtherArray[Idx])
        continue;
    }

    Array.AppendAt(20, OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 220lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx + 20], OtherArray[Idx])
        continue;
    }

    Array.AppendAt(Array.GetSize(), OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 240lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx + 20], OtherArray[Idx])
        continue;
    }

    Array.AppendAt(Array.end(), OtherArray);
    QUICK_CHECK_EQUALS(Array.GetSize(), 260lu)
    for (LSize Idx { 0 }; Idx < OtherArray.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx + 20], OtherArray[Idx])
        continue;
    }

    return;
}

TEST_CASE(RemoveArray, "Lal.Containers")
{
    TArray<LSize> Array { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    QUICK_CHECK_EQUALS(Array.GetSize(), 30lu)

    QUICK_CHECK_EQUALS(Array.Remove(1ul), 2ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 28lu)
    QUICK_CHECK_EQUALS(Array.Remove(1ul), 0ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 28lu)

    Array.RemoveAt(0);
    QUICK_CHECK_EQUALS(Array.GetSize(), 27lu)
    QUICK_CHECK_EQUALS(Array[0], 3ul)
    Array.RemoveAt(26);
    QUICK_CHECK_EQUALS(Array.GetSize(), 26lu)
    QUICK_CHECK_EQUALS(Array[25], 9ul)

    QUICK_CHECK_TRUE(Array.RemoveOnce(3ul))
    QUICK_CHECK_EQUALS(Array.GetSize(), 25lu)
    QUICK_CHECK_EQUALS(Array[0], 4ul)

    QUICK_CHECK_FALSE(Array.RemoveOnce(1ul))
    QUICK_CHECK_EQUALS(Array.GetSize(), 25lu)

    QUICK_CHECK_EQUALS(Array.RemoveByPredicate([](const LSize& Element)
    {
        return Element % 2 == 0;
    }), 13ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 12lu)
    QUICK_CHECK_EQUALS(Array.RemoveByPredicate([](const LSize& Element)
    {
        return Element % 2 == 0;
    }), 0ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 12lu)
    QUICK_CHECK_EQUALS(Array.RemoveByPredicate([](const LSize& Element)
    {
        return Element % 2 == 1;
    }), 12ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)

    Array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    QUICK_CHECK_EQUALS(Array.GetSize(), 30lu)

    QUICK_CHECK_TRUE(Array.RemoveOnceByPredicate([](const LSize& Element)
    {
        return Element % 2 == 0;
    }))
    QUICK_CHECK_EQUALS(Array.GetSize(), 29lu)

    Array.RemoveAt(Array.begin(), Array.end());
    QUICK_CHECK_EQUALS(Array.GetSize(), 0lu)

    QUICK_CHECK_NULL(Array.Peek())
    Array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    QUICK_CHECK_EQUALS(Array.GetSize(), 30lu)

    Array.RemoveAt(Array.begin(), Array.end() - 10);
    QUICK_CHECK_EQUALS(Array.GetSize(), 10lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], Idx + 1)
        continue;
    }

    Array.RemoveAt(Array.begin(), Array.begin() + 5);
    QUICK_CHECK_EQUALS(Array.GetSize(), 5lu)
    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        QUICK_CHECK_EQUALS(Array[Idx], Idx + 5 + 1)
        continue;
    }

    QUICK_CHECK_EQUALS(*Array.Peek(), 10ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 5lu)
    QUICK_CHECK_TRUE(Array.Pop())
    QUICK_CHECK_EQUALS(*Array.Peek(), 9ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 4lu)
    QUICK_CHECK_TRUE(Array.Pop())
    QUICK_CHECK_EQUALS(*Array.Peek(), 8ul)
    QUICK_CHECK_EQUALS(Array.GetSize(), 3lu)

    Array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TArrayView<LSize> View { Array };
    QUICK_CHECK_EQUALS(View.GetSize(), 30lu)
    View.Pop();
    QUICK_CHECK_EQUALS(View.GetSize(), 29lu)
    QUICK_CHECK_EQUALS(Array.GetSize(), 30lu)

    return;
}

TEST_CASE(RemoveArray, "Lal.Containers")
{
    TArray<LSize> Array { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    QUICK_CHECK_EQUALS(Array.GetSize(), 30lu)

    QUICK_CHECK_EQUALS(Array.Find(1ul), Array.begin())
    QUICK_CHECK_EQUALS(Array.FindIndex(1ul), 0lu)
    QUICK_CHECK_EQUALS(Array.Find(20ul), Array.begin() + 19);
    QUICK_CHECK_EQUALS(Array.FindIndex(20ul), 19lu)
    QUICK_CHECK_TRUE(Array.Contains(5ul))

    QUICK_CHECK_EQUALS(Array.FindLast(1ul), Array.begin() + 20);
    QUICK_CHECK_EQUALS(Array.FindLastIndex(1ul), 20lu)
    QUICK_CHECK_EQUALS(Array.FindLast(20ul), Array.begin() + 19);
    QUICK_CHECK_EQUALS(Array.FindLastIndex(20ul), 19lu)

    return;
}
