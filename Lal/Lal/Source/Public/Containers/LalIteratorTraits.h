// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

namespace Private
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
#if LAL_WITH_CLANG && __has_builtin(__is_base_of)
    template <typename T>
    requires (!requires { typename T::iterator_concept; })
          && (!requires { typename T::iterator_category; })
          && std::__detail::__primary_traits_iter<T>
    struct TGimmeThatIterConceptImpl<T>
    {
        typedef std::random_access_iterator_tag type;
    };
#endif /* LAL_WITH_CLANG && __has_builtin(__is_base_of) */

//# If everything else fails, just let the compiler run into a wall here.
template <typename T>
struct TGimmeThatIterConceptImpl
{
    /* You somehow fucked up if this struct is part of your current compiler error. */
};

// ~Get that concept from any it.
///////////////////////////////////////////////////////////////////////////////

} /* ~Namespace Private */

//# Gets the iter concept which is not std c++ for no apparent reason.
template <typename T>
using TGimmeThatIterConcept_t = typename Private::TGimmeThatIterConceptImpl<T>::type;

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

} /* Namespace Lal */
