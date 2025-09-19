// Copyright mzoesch. All rights reserved.

#pragma once

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
    QUICK_CHECK_EQUALS(*It, 3ul)

    TArray<LSize>::ConstIterator CIt { Array1.cbegin() };
    std::advance(CIt, 2);
    QUICK_CHECK_EQUALS(*CIt, 3ul)

    return;
}

TEST_CASE(ArrayComparisons, "Lal.Containers")
{
    const TArray<LSize> Array1 { 1, 2, 3, 4, 5 };
    const TArray<LSize> Array2 { 1, 2, 3, 4, 5 };
    const TArray<LSize> Array3 { 6, 7, 8, 9, 10, 11 };

    QUICK_CHECK_EQUALS(Array1.GetSize() <=> Array2.GetSize(), std::strong_ordering::equal)
    QUICK_CHECK_EQUALS(Array1.GetSize() <=> Array3.GetSize(), std::strong_ordering::less)
    QUICK_CHECK_EQUALS(Array2.GetSize() <=> Array3.GetSize(), std::strong_ordering::less)
    QUICK_CHECK_EQUALS(Array3.GetSize() <=> Array1.GetSize(), std::strong_ordering::greater)
    QUICK_CHECK_EQUALS(Array3.GetSize() <=> Array2.GetSize(), std::strong_ordering::greater)

    QUICK_CHECK_FALSE(Array1.GetSize() < Array2.GetSize())
    QUICK_CHECK_TRUE(Array1.GetSize() < Array3.GetSize())
    QUICK_CHECK_FALSE(Array3.GetSize() < Array1.GetSize())
    QUICK_CHECK_TRUE(Array2.GetSize() < Array3.GetSize())
    QUICK_CHECK_FALSE(Array3.GetSize() < Array2.GetSize())
    QUICK_CHECK_FALSE(Array1.GetSize() > Array2.GetSize())
    QUICK_CHECK_FALSE(Array1.GetSize() > Array3.GetSize())
    QUICK_CHECK_TRUE(Array3.GetSize() > Array1.GetSize())
    QUICK_CHECK_FALSE(Array2.GetSize() > Array3.GetSize())
    QUICK_CHECK_TRUE(Array3.GetSize() > Array2.GetSize())
    QUICK_CHECK_TRUE(Array1.GetSize() <= Array2.GetSize())
    QUICK_CHECK_TRUE(Array1.GetSize() <= Array3.GetSize())
    QUICK_CHECK_FALSE(Array3.GetSize() <= Array1.GetSize())
    QUICK_CHECK_TRUE(Array2.GetSize() <= Array3.GetSize())
    QUICK_CHECK_FALSE(Array3.GetSize() <= Array2.GetSize())
    QUICK_CHECK_TRUE(Array1.GetSize() >= Array2.GetSize())
    QUICK_CHECK_FALSE(Array1.GetSize() >= Array3.GetSize())
    QUICK_CHECK_TRUE(Array3.GetSize() >= Array1.GetSize())
    QUICK_CHECK_FALSE(Array2.GetSize() >= Array3.GetSize())
    QUICK_CHECK_TRUE(Array3.GetSize() >= Array2.GetSize())

    QUICK_CHECK_TRUE(Array1.CompareSize(Array2) == std::strong_ordering::equal)
    QUICK_CHECK_FALSE(Array1.CompareSize(Array3) == std::strong_ordering::equal)
    QUICK_CHECK_TRUE(Array2.CompareSize(Array1) == std::strong_ordering::equal)
    QUICK_CHECK_FALSE(Array2.CompareSize(Array3) == std::strong_ordering::equal)
    QUICK_CHECK_FALSE(Array3.CompareSize(Array1) == std::strong_ordering::equal)
    QUICK_CHECK_FALSE(Array3.CompareSize(Array2) == std::strong_ordering::equal)

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

namespace Testing::Lal::Array
{

struct X
{
    X() { this->Initialize(); }
    X(X&& x)
    {
        check( X::IsInitialized(&x) )
        this->Initialize();

        return;
    }

    X& operator=(X&& x)
    {
        check( X::IsInitialized(&x) )
        this->Initialize();

        return *this;
    }

    X(const X& x)
    {
        check( X::IsInitialized(&x) )
        this->Initialize();

        return;
    }

    X& operator=(const X& x)
    {
        check( X::IsInitialized(&x) )
        this->Initialize();

        return *this;
    }

    ~X()
    {
        this->Deinitialize();
        return;
    }

private:

    void Initialize()
    {
        this->Data = std::numeric_limits<u64>::max() / 2;
        check( X::IsInitialized(this) )

        return;
    }

    static bool IsInitialized(const X* x)
    {
        return x->Data == std::numeric_limits<u64>::max() / 2;
    }

    void Deinitialize()
    {
        this->Data = std::numeric_limits<u64>::max();
        check( X::IsInitialized(this) == false )

        return;
    }

    u64 Data;
};

} /* ~Namespace Testing::Lal::Array */

TEST_CASE(AppendAtUbArray, "Lal.Containers")
{
    using namespace Testing::Lal::Array;

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(0ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 30)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(1ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 29)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(2ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 28)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(3ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 27)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(4ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 26)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{},
            X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{}, X{} };

        TArray<X>::Iterator It = Array.AppendAt(5ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 25)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(0ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 15)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(1ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 14)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(2ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 13)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(3ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 12)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(4ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 11)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(5ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 10)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(6ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 9)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(7ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 8)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(8ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 7)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(9ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 6)
    }

    {
        TArray<X> Array;
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});
        Array.Push(X{});

        TArray<X> Other { X{}, X{}, X{}, X{}, X{}, };
        TArray<X>::Iterator It = Array.AppendAt(10ul, Other);
        int x { 0 };
        while (It != Array.end())
        {
            ++x;
            ++It;
            continue;
        }

        QUICK_CHECK_EQUALS(x, 5)
    }

    return;
}

TEST_CASE(StackString, "Lal.Containers")
{
    TArray<LString> Array;
    for (int x = 0; x < 50; ++x)
    {
        Array.Add(LString{});
    }
    QUICK_CHECK_EQUALS(Array.GetSize(), 50ul)
    for (LString& Str : Array)
    {
        QUICK_CHECK_TRUE(Str.IsEmpty())
        Str = "Test";
        QUICK_CHECK_EQUALS(Str, "Test")

        continue;
    }

    for (int x = 0; x < 50; ++x)
    {
        Array.Add(LString{});
    }
    QUICK_CHECK_EQUALS(Array.GetSize(), 100ul)
    TArray<LString>::SizeType Idx { 0 };
    for (const LString& Str : Array)
    {
        if (Idx < 50)
        {
            QUICK_CHECK_EQUALS(Str, "Test")
        }
        else
        {
            QUICK_CHECK_TRUE(Str.IsEmpty())
        }

        ++Idx;

        continue;
    }

    while (Idx > 50)
    {
        --Idx;
        Array.RemoveAt(Idx);
        continue;
    }
    QUICK_CHECK_EQUALS(Array.GetSize(), 50ul)
    for (const LString& Str : Array)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    Array.ShrinkToFit();
    QUICK_CHECK_EQUALS(Array.GetSize(), 50ul)
    for (const LString& Str : Array)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    TArray<LString> Array2;
    Array2.Append(Array);
    Array2.AppendAt(25ul, Array);
    QUICK_CHECK_EQUALS(Array2.GetSize(), 100ul)
    for (const LString& Str : Array2)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    Array.AppendAt(30ul, Array2);
    QUICK_CHECK_EQUALS(Array.GetSize(), 150ul)
    for (const LString& Str : Array)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    for (const LString& Str : Array2)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    Array2.AppendAt(50ul, Array.move_begin(), Array.move_end());
    for (const LString& Str : Array2)
    {
        QUICK_CHECK_EQUALS(Str, "Test")
        continue;
    }

    for (const LString& Str : Array)
    {
        QUICK_CHECK_TRUE(Str.IsEmpty())
        continue;
    }

    return;
}

namespace Testing::Lal::Array
{

struct S
{
    static inline LSize Counter { 0 };
    static inline LSize Lifetimes { 0 };

    S() { ++Counter; ++Lifetimes; }
    S(S&&) { ++Counter; ++Lifetimes; }
    S& operator=(S&&) { ++Counter; return *this; }
    S(const S&) { ++Counter; ++Lifetimes; }
    S& operator=(const S&) { ++Counter; return *this; }
    ~S() { ++Counter; --Lifetimes; }
};

static_assert(::Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<S> == false);

} /* ~Namespace Testing::Lal::Array */

TEST_CASE(AddAtComplexArray, "Lal.Containers")
{
    using namespace Testing::Lal::Array;

    {
        TArray<S> Array;
        Array.Reserve(20);

        QUICK_CHECK_EQUALS(S::Counter, 0lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 0lu)
        Array.AddAt(0, S{});
        QUICK_CHECK_EQUALS(S::Counter, 3lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 1lu)

        Array.AddAt(0, S{});
        QUICK_CHECK_EQUALS(S::Counter, 7lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 2lu)

        Array.AddAt(0, S{});
        QUICK_CHECK_EQUALS(S::Counter, 12lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 3lu)

        Array.AddAt(0, S{});
        QUICK_CHECK_EQUALS(S::Counter, 18lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 4lu)

        Array.AddAt(1, S{});
        QUICK_CHECK_EQUALS(S::Counter, 24lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 5lu)

        Array.AddAt(5, S{});
        QUICK_CHECK_EQUALS(S::Counter, 27lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 6lu)

        Array.AddAt(5, S{});
        QUICK_CHECK_EQUALS(S::Counter, 31lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 7lu)
    }

    QUICK_CHECK_EQUALS(S::Counter, 38ul)
    QUICK_CHECK_EQUALS(S::Lifetimes, 0ul)

    S::Counter = 0;
    S::Lifetimes = 0;

    {
        TArray<S> Array;
        Array.Reserve(50);
        Array.Push(S{}).Push(S{}).Push(S{}).Push(S{}).Push(S{});
        QUICK_CHECK_EQUALS(Array.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 15lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 5lu)

        const TArray<S> Other { S{}, S{}, S{}, S{}, S{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 30lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 10lu)

        Array.AppendAt(0ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 10lu)
        QUICK_CHECK_EQUALS(S::Counter, 40lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 15lu)

        Array.AppendAt(1ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 15lu)
        QUICK_CHECK_EQUALS(S::Counter, 54ul)
        QUICK_CHECK_EQUALS(S::Lifetimes, 20lu)

        Array.AppendAt(15ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 20lu)
        QUICK_CHECK_EQUALS(S::Counter, 59ul)
        QUICK_CHECK_EQUALS(S::Lifetimes, 25lu)
    }

    QUICK_CHECK_EQUALS(S::Counter, 84ul)
    QUICK_CHECK_EQUALS(S::Lifetimes, 0ul)

    S::Counter = 0;
    S::Lifetimes = 0;

    {
        TArray<S> Array;
        Array.Reserve(50);
        Array.Push(S{}).Push(S{}).Push(S{}).Push(S{}).Push(S{});
        QUICK_CHECK_EQUALS(Array.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 15lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 5lu)

        TArray<S> Other { S{}, S{}, S{}, S{}, S{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 30lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 10lu)

        Array.AppendAt(0ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 10lu)
        QUICK_CHECK_EQUALS(S::Counter, 45lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 10lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
        Other = { S{}, S{}, S{}, S{}, S{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 60lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 15lu)

        Array.AppendAt(1ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 15lu)
        QUICK_CHECK_EQUALS(S::Counter, 79ul)
        QUICK_CHECK_EQUALS(S::Lifetimes, 15lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
        Other = { S{}, S{}, S{}, S{}, S{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(S::Counter, 94lu)
        QUICK_CHECK_EQUALS(S::Lifetimes, 20lu)

        Array.AppendAt(15ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 20lu)
        QUICK_CHECK_EQUALS(S::Counter, 104ul)
        QUICK_CHECK_EQUALS(S::Lifetimes, 20lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
    }

    QUICK_CHECK_EQUALS(S::Counter, 124ul)
    QUICK_CHECK_EQUALS(S::Lifetimes, 0ul)

    return;
}

namespace Testing::Lal::Array
{

//# Trivial S.
struct T
{
    static inline LSize Counter { 0 };
    static inline LSize Lifetimes { 0 };

    T() { ++Counter; ++Lifetimes; }
    T(T&&) { ++Counter; ++Lifetimes; }
    T& operator=(T&&) { ++Counter; return *this; }
    T(const T&) { ++Counter; ++Lifetimes; }
    T& operator=(const T&) { ++Counter; return *this; }
    ~T() { ++Counter; --Lifetimes; }
};


} /* ~Namespace Testing::Lal::Array */

template <>
struct ::Lal::TArrayBaseAllowTrivialMemoryBufferMove<Testing::Lal::Array::T> : ::Lal::TrueType { };

namespace Testing::Lal::Array
{

static_assert(::Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<T>);

} /* ~Namespace Testing::Lal::Array */


TEST_CASE(AddAtTrivialArray, "Lal.Containers")
{
    using namespace Testing::Lal::Array;

    {
        TArray<T> Array;
        Array.Reserve(20);

        QUICK_CHECK_EQUALS(T::Counter, 0lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 0lu)
        Array.AddAt(0, T{});
        QUICK_CHECK_EQUALS(T::Counter, 3lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 1lu)

        Array.AddAt(0, T{});
        QUICK_CHECK_EQUALS(T::Counter, 6lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 2lu)

        Array.AddAt(0, T{});
        QUICK_CHECK_EQUALS(T::Counter, 9lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 3lu)

        Array.AddAt(0, T{});
        QUICK_CHECK_EQUALS(T::Counter, 12lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 4lu)

        Array.AddAt(1, T{});
        QUICK_CHECK_EQUALS(T::Counter, 15lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 5lu)

        Array.AddAt(5, T{});
        QUICK_CHECK_EQUALS(T::Counter, 18lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 6lu)

        Array.AddAt(5, T{});
        QUICK_CHECK_EQUALS(T::Counter, 21lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 7lu)
    }

    QUICK_CHECK_EQUALS(T::Counter, 28ul)
    QUICK_CHECK_EQUALS(T::Lifetimes, 0ul)

    T::Counter = 0;
    T::Lifetimes = 0;

    {
        TArray<T> Array;
        Array.Reserve(50);
        Array.Push(T{}).Push(T{}).Push(T{}).Push(T{}).Push(T{});
        QUICK_CHECK_EQUALS(Array.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 15lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 5lu)

        const TArray<T> Other { T{}, T{}, T{}, T{}, T{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 30lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 10lu)

        Array.AppendAt(0ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 10lu)
        QUICK_CHECK_EQUALS(T::Counter, 35lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 15lu)

        Array.AppendAt(1ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 15lu)
        QUICK_CHECK_EQUALS(T::Counter, 40ul)
        QUICK_CHECK_EQUALS(T::Lifetimes, 20lu)

        Array.AppendAt(15ul, Other);
        QUICK_CHECK_EQUALS(Array.GetSize(), 20lu)
        QUICK_CHECK_EQUALS(T::Counter, 45ul)
        QUICK_CHECK_EQUALS(T::Lifetimes, 25lu)
    }

    QUICK_CHECK_EQUALS(T::Counter, 70ul)
    QUICK_CHECK_EQUALS(T::Lifetimes, 0ul)

    T::Counter = 0;
    T::Lifetimes = 0;

    {
        TArray<T> Array;
        Array.Reserve(50);
        Array.Push(T{}).Push(T{}).Push(T{}).Push(T{}).Push(T{});
        QUICK_CHECK_EQUALS(Array.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 15lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 5lu)

        TArray<T> Other { T{}, T{}, T{}, T{}, T{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 30lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 10lu)

        Array.AppendAt(0ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 10lu)
        QUICK_CHECK_EQUALS(T::Counter, 30lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 10lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
        Other = { T{}, T{}, T{}, T{}, T{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 45lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 15lu)

        Array.AppendAt(1ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 15lu)
        QUICK_CHECK_EQUALS(T::Counter, 45ul)
        QUICK_CHECK_EQUALS(T::Lifetimes, 15lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
        Other = { T{}, T{}, T{}, T{}, T{} };
        QUICK_CHECK_EQUALS(Other.GetSize(), 5ul)
        QUICK_CHECK_EQUALS(T::Counter, 60lu)
        QUICK_CHECK_EQUALS(T::Lifetimes, 20lu)

        Array.AppendAt(15ul, std::move(Other));
        QUICK_CHECK_EQUALS(Array.GetSize(), 20lu)
        QUICK_CHECK_EQUALS(T::Counter, 60ul)
        QUICK_CHECK_EQUALS(T::Lifetimes, 20lu)

        QUICK_CHECK_EQUALS(Other.GetSize(), 0ul)
    }

    // QUICK_CHECK_EQUALS(T::Counter, 80ul)
    QUICK_CHECK_EQUALS(T::Lifetimes, 0ul)

    return;
}

TEST_CASE(AddAtStringArray, "Lal.Containers")
{
    TArray<LString> Array;

    constexpr LSize Count { 5000 };

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        Array.Push(LString{});
    }

    for (const LString& S : Array)
    {
        QUICK_CHECK_EQUALS(S, "")
    }

    Array.Empty();

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        Array.Push(LString::SprintF("{}", Idx));
    }

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        const LString& S { Array[Idx] };
        QUICK_CHECK_EQUALS(S, LString::SprintF("{}", Idx))

        continue;
    }

    Array.Empty();

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        if (Idx % 2 == 0)
        {
            Array.Push(LString::SprintF("{}", Idx));
        }
        else
        {
            Array.Push(LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx));
        }
    }

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        const LString& S { Array[Idx] };

        if (Idx % 2 == 0)
        {
            QUICK_CHECK_EQUALS(S, LString::SprintF("{}", Idx))
        }
        else
        {
            QUICK_CHECK_EQUALS(S, LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx))
        }

        continue;
    }

    Array.Empty();
    TArray<LString> Other;

    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        if (Idx % 2 == 0)
        {
            Array.Push(LString::SprintF("{}", Idx));
            Other.Push(LString::SprintF("{}", Idx));
        }
        else
        {
            Array.Push(LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx));
            Other.Push(LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx));
        }
    }

    Array.AppendAt(0ul, Other);
    Array.AppendAt(Array.GetSize(), Other);

    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        const LString& S { Array[Idx] };

        if (Idx % 2 == 0)
        {
            QUICK_CHECK_EQUALS(S, LString::SprintF("{}", Idx % Count))
        }
        else
        {
            QUICK_CHECK_EQUALS(S, LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx % Count))
        }

        continue;
    }

    Other.Empty();
    for (LSize Idx { 0 }; Idx < Count; ++Idx)
    {
        if (Idx % 2 == 0)
        {
            Other.Push(LString::SprintF("Other: {}", Idx));
        }
        else
        {
            Other.Push(LString::SprintF("Other: {} This is a very long string, that needs to be heap allocated.", Idx));
        }
    }

    Array.AppendAt(Count / 2, Other);

    for (LSize Idx { 0 }; Idx < Array.GetSize(); ++Idx)
    {
        const LString& S { Array[Idx] };

        if (Idx < Count / 2)
        {
            if (Idx % 2 == 0)
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("{}", Idx % Count))
            }
            else
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx % Count))
            }
        }

        else if (Idx < Count + Count / 2)
        {
            if ((Idx - Count / 2) % 2 == 0)
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("Other: {}", (Idx - Count / 2) % Count))
            }
            else
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("Other: {} This is a very long string, that needs to be heap allocated.", (Idx - Count / 2) % Count))
            }
        }

        else
        {
            if (Idx % 2 == 0)
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("{}", Idx % Count))
            }
            else
            {
                QUICK_CHECK_EQUALS(S, LString::SprintF("{} This is a very long string, that needs to be heap allocated.", Idx % Count))
            }
        }

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
