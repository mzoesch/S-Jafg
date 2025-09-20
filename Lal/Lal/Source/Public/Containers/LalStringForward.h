// Copyright mzoesch. All rights reserved.

#pragma once

///////////////////////////////////////////////////////////////////////////////
// String.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

namespace Private
{

class LStringBase { };

} /* ~Namespace Private */

/*----------------------------------------------------------------------------
    Concepts.
----------------------------------------------------------------------------*/

template <typename T>
concept TStringBaseConcept = std::is_base_of_v<Private::LStringBase, T> && requires
{
    typename T::Encoding;
    typename T::Allocator;
    typename T::T;
    typename T::SizeType;
    typename T::Iterator;
    typename T::ConstIterator;
    typename T::Pointer;
    typename T::ConstPointer;
    typename T::Reference;
    typename T::ConstReference;
};

template <typename T, typename U>
concept TValidOtherStringConcept = TStringBaseConcept<T> && TStringBaseConcept<U> && requires
{
    std::is_same_v<typename T::T, typename U::T>;
};

template <typename T>
concept TStringBaseAllocatorConcept = requires(T Allocator)
{
    typename T::T;
    typename T::SizeType;
    typename T::Iterator;
    typename T::ConstIterator;
    typename T::Pointer;
    typename T::ConstPointer;
    typename T::Reference;
    typename T::ConstReference;
};


/*----------------------------------------------------------------------------
    Forwards.
----------------------------------------------------------------------------*/

template <typename TIn, std::integral TSizeType>
struct TStringBaseDefaultUtf8Traits;

//#
//# Default Lal string implementation.
//#
//# Supports ASCII and UTF-8 encoded strings by default. You may add your own encodings.
//#
//# This string allows for heap, stack allocation, or a mixture, depending on the size of the string.
//#
template <template <typename, typename> typename TEncoding, TStringBaseAllocatorConcept TAllocator>
    requires(TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TStringBase;

template <template <typename, typename> typename TEncoding, typename T>
using THeapString = TStringBase<TEncoding, TArray<T>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TStringCapacity>
using TSmallString = TStringBase<TEncoding, TStackArray<T, TStringCapacity>>;
template <template <typename, typename> typename TEncoding, typename T, LSize TStringCapacity>
using TOptimizedString = TStringBase<TEncoding, TStackOptimizedArray<T, TStringCapacity>>;

template <template <typename, typename> typename TEncoding, typename T>
using TStringView = TStringBase<TEncoding, TArrayView<T>>;
template <template <typename, typename> typename TEncoding, typename T>
using TMutableStringView = TStringBase<TEncoding, TMutableArrayView<T>>;

template <typename T>
using THeapStringUtf8 = THeapString<TStringBaseDefaultUtf8Traits, T>;
template <typename T, LSize TStringCapacity>
using TSmallStringUtf8 = TSmallString<TStringBaseDefaultUtf8Traits, T, TStringCapacity>;
template <typename T, LSize TStringCapacity>
using TOptimizedStringUtf8 = TOptimizedString<TStringBaseDefaultUtf8Traits, T, TStringCapacity>;

template <typename T>
using TStringViewUtf8 = TStringView<TStringBaseDefaultUtf8Traits, T>;
template <typename T>
using TMutableStringViewUtf8 = TMutableStringView<TStringBaseDefaultUtf8Traits, T>;

} /* ~Namespace Lal */

//#
//# The default string for Lal with UTF-8 encoding.
//# This string is a good all-rounder. It is optimized for small strings but uses different allocation strategies
//# when the string gets larger.
//#
typedef Lal::TOptimizedStringUtf8<LJafgChar, 8> LString;

//#
//# This string is optimized for large strings.
//#
typedef Lal::THeapStringUtf8<LJafgChar> LBigString;

//#
//# This string is optimized for small strings. This string uses stack allocation and is meant if the string's max
//# sizes are well-defined and more or less small.
//#
//# @tparam The capacity to reserve on the stack. This is the maximum size of the string.
//#
template <LSize TCapacity>
using LSmallString = Lal::TSmallStringUtf8<LJafgChar, TCapacity>;

//#
//# The default string view for Lal with UTF-8 encoding.
//#
typedef Lal::TStringViewUtf8<LJafgChar> LStringView;
typedef Lal::TMutableStringViewUtf8<LJafgChar> LMutableStringView;
