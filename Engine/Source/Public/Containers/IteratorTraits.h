// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

///////////////////////////////////////////////////////////////////////////////
// Get that concept from any it.

template <typename T>
struct TGimmeThatIterConceptImpl;

//# If valid yippi yuhu.
template <typename T>
    requires requires { typename T::iterator_concept; }
struct TGimmeThatIterConceptImpl<T>
{
    typedef typename T::iterator_concept type;
};

//# If pointer, just default to the std shit.
template <typename T>
struct TGimmeThatIterConceptImpl<T*>
{
    typedef typename std::iterator_traits<T*>::iterator_concept type;
};

//# Unless we have the category, then just copy that into our concept.
template <typename T>
    requires (!requires { typename T::iterator_concept; })
          && ( requires { typename T::iterator_category; })
struct TGimmeThatIterConceptImpl<T>
{
    //# I mean, if someone defines the cat but not the concept, are they mentally ill, or is there
    //# an actual possible reason to do this? Idk and idc. This is my shit, and I do not do that.
    typedef typename T::iterator_category type;
};

//# Ofcourse if primary iter then this is just a random tag - obviously.
#if JAFG_WITH_CLANG && __has_builtin(__is_base_of)
    template <typename T>
    requires (!requires { typename T::iterator_concept; })
          && (!requires { typename T::iterator_category; })
          && std::__detail::__primary_traits_iter<T>
    struct TGimmeThatIterConceptImpl<T>
    {
        typedef std::random_access_iterator_tag type;
    };
#endif /* JAFG_WITH_CLANG && __has_builtin(__is_base_of) */

//# If everything else fails, just let the compiler run into a wall here.
template <typename T>
struct TGimmeThatIterConceptImpl
{
    /* You somehow fucked up if this struct is part of your current compiler error. */
};

// ~Get that concept from any it.
///////////////////////////////////////////////////////////////////////////////

} /* ~Namespace Detail */

//# Gets the iter concept which is not std c++ for no apparent reason.
template <typename T>
using TGimmeThatIterConcept_t = typename Detail::TGimmeThatIterConceptImpl<T>::type;

template <typename TIn>
struct TIteratorTraits : public std::iterator_traits<TIn>
{
    typedef std::iterator_traits<TIn> Super;

    //#
    //# What in the actual fuck did the std committee think here???
    //# "Wheeeoo! Please no violation of backwards compatability :(" or what??!!!1!! me no gusta mucho
    //#
    using iterator_concept = TGimmeThatIterConcept_t<TIn>;
};

template <typename TIn> requires std::is_object_v<TIn>
struct TIteratorTraits<TIn*> : public std::iterator_traits<TIn*>
{
    typedef std::iterator_traits<TIn*> Super;
};

//#
//# The most basic concept that checks if the TIterator can be interpreted as an iterator.
//# This covers all types, also pointers.
//#
template <typename TIterator>
concept TIteratorConcept = std::input_or_output_iterator<TIterator>;

//#
//# Checks if the iterator S can be used as a sentinel for TIterator.
//#
template <typename TIterator, typename S>
concept TIteratorPairConcept = std::sentinel_for<S, TIterator>;

//#
//# Checks if TIterator and UIterator can be used interchangeable.
//#
template <typename TIterator, typename UIterator>
concept TCrossIteratorConcept = TIteratorConcept<TIterator> && TIteratorConcept<UIterator>
    && std::same_as<std::remove_cv_t<typename TIteratorTraits<TIterator>::pointer>, std::remove_cv_t<typename TIteratorTraits<TIterator>::pointer>>;

//# Whether this iterator supports multiple reads and writes.
template <typename TIterator>
struct TIterator_IsReadWrite;
//# Whether this iterator supports multiple traversals from begin to end.
template <typename TIterator>
struct TIterator_CanTraverseMultipleTimes;
//# Whether this iterator supports multiple traversals from end to begin as well.
template <typename TIterator>
struct TIterator_CanTraverseBackward;
//# Whether this iterator behaves like a pointer (can do arithmetic, etc.).
template <typename TIterator>
struct TIterator_IsPointerLike;
//# Whether this iterator is contiguous in memory.
template <typename TIterator>
struct TIterator_IsContiguous;

template <typename TIterator>
struct TIterator_IsReadWrite : std::false_type
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsReadWrite<TIterator>
    : std::bool_constant<std::is_base_of_v<std::forward_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_CanTraverseMultipleTimes : std::false_type
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_CanTraverseMultipleTimes<TIterator>
    : std::bool_constant<std::is_base_of_v<std::forward_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_CanTraverseBackward : std::false_type
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_CanTraverseBackward<TIterator>
    : std::bool_constant<std::is_base_of_v<std::bidirectional_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_IsPointerLike : std::false_type
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsPointerLike<TIterator>
    : std::bool_constant<std::is_base_of_v<std::random_access_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_IsContiguous : std::false_type
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsContiguous<TIterator>
    : std::bool_constant<std::is_base_of_v<std::contiguous_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
inline constexpr bool TIterator_IsReadWrite_v = TIterator_IsReadWrite<TIterator>::value;
template <typename TIterator>
inline constexpr bool TIterator_CanTraverseMultipleTimes_v = TIterator_CanTraverseMultipleTimes<TIterator>::value;
template <typename TIterator>
inline constexpr bool TIterator_CanTraverseBackward_v = TIterator_CanTraverseBackward<TIterator>::value;
template <typename TIterator>
inline constexpr bool TIterator_IsPointerLike_v = TIterator_IsPointerLike<TIterator>::value;
template <typename TIterator>
inline constexpr bool TIterator_IsContiguous_v = TIterator_IsContiguous<TIterator>::value;

static_assert(TIteratorConcept<i64*>);
static_assert(TIterator_IsReadWrite_v<i64*>);
static_assert(TIterator_CanTraverseMultipleTimes_v<i64*>);
static_assert(TIterator_CanTraverseBackward_v<i64*>);
static_assert(TIterator_IsPointerLike_v<i64*>);
static_assert(TIterator_IsContiguous_v<i64*>);

template<typename TRange, typename T>
concept ContainerCompatibleRange =
#if JAFG_WITH_CLANG
    std::__detail::__container_compatible_range<TRange, T>;
#else /* JAFG_WITH_CLANG */
    #error "Missing implementation for compiler."
#endif /* !JAFG_WITH_CLANG */

template<typename TRange>
concept CRange = requires(TRange Range)
{
    Range.begin();
    { Range.end() } -> std::sentinel_for<decltype(Range.begin())>;
};

} /* Namespace Jafg */

#ifndef ITERATOR
    #define ITERATOR ::Jafg::TIteratorConcept auto
#endif /* ITERATOR */

#ifndef ITERATOR_PAIR
    #define ITERATOR_PAIR(A, B) ::Jafg::TIteratorPairConcept<decltype(A), decltype(B)>
#endif /* ITERATOR_PAIR */

#ifndef ITERATOR_CROSS
    #define ITERATOR_CROSS(A, B) ::Jafg::TCrossIteratorConcept<decltype(A), decltype(B)>
#endif /* ITERATOR_CROSS */

#ifndef RANGE
    #define RANGE ::Jafg::CRange auto&&
#endif /* RANGE */
