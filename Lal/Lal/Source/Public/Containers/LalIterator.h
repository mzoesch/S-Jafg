// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

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
struct TIterator_IsReadWrite : FalseType
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsReadWrite<TIterator>
    : BoolConstant<std::is_base_of_v<std::forward_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_CanTraverseMultipleTimes : FalseType
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_CanTraverseMultipleTimes<TIterator>
    : BoolConstant<std::is_base_of_v<std::forward_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_CanTraverseBackward : FalseType
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_CanTraverseBackward<TIterator>
    : BoolConstant<std::is_base_of_v<std::bidirectional_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_IsPointerLike : FalseType
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsPointerLike<TIterator>
    : BoolConstant<std::is_base_of_v<std::random_access_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
{
};

template <typename TIterator>
struct TIterator_IsContiguous : FalseType
{
};
template <typename TIterator> requires requires { typename TIteratorTraits<TIterator>::iterator_concept; }
struct TIterator_IsContiguous<TIterator>
    : BoolConstant<std::is_base_of_v<std::contiguous_iterator_tag, typename TIteratorTraits<TIterator>::iterator_concept>>
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

#if LAL_WITH_LEGACY_ITERATORS

namespace Private
{

template <typename T>
concept IsValidIterator = requires
{
    typename T::value_type;
    typename T::difference_type;
    typename T::pointer;
    typename T::reference;
    typename T::value_type;
    typename T::iterator_category;

    //# Although this is not std, we require this as we fuck backwards compatability.
    typename T::iterator_concept;
};

template <typename T>
concept IsValidIteratorWeak = requires
{
    typename TIteratorTraits<T>::value_type;
    typename TIteratorTraits<T>::difference_type;
    typename TIteratorTraits<T>::pointer;
    typename TIteratorTraits<T>::reference;
    typename TIteratorTraits<T>::value_type;
    typename TIteratorTraits<T>::iterator_category;

    //# Has to be used with #TIteratorTraits to make sense here.
    typename TIteratorTraits<T>::iterator_concept;
};

struct LDefaultIterator { };
struct LDefaultReversedIterator { };
struct LDefaultFilteredIterator { };
struct LDefaultReversedFilteredIterator { };

} /* ~Namespace Private */

template <typename TIn, bool bMove = false>
struct TDefaultIterator : public Private::LDefaultIterator
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = TIn;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::contiguous_iterator_tag;
    using iterator_concept  = std::contiguous_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    using _TDefaultIterator                 = TDefaultIterator<value_type, bMove>;
    using _TDefaultReversedIterator         = TDefaultReversedIterator<value_type, bMove>;
    template <std::predicate<value_type> TPredicate>
    using _TDefaultFilteredIterator         = TDefaultFilteredIterator<TPredicate, value_type, bMove>;
    template <std::predicate<value_type> TPredicate>
    using _TDefaultReversedFilteredIterator = TDefaultReversedFilteredIterator<TPredicate, value_type, bMove>;

    typedef TDefaultIteratorFactory<
          _TDefaultIterator
        , _TDefaultReversedIterator
        , _TDefaultFilteredIterator
        , _TDefaultReversedFilteredIterator
        > Factory;

    FORCEINLINE constexpr TDefaultIterator() noexcept : Cursor{nullptr} { }
    FORCEINLINE constexpr TDefaultIterator(LNullptrTy) noexcept : Cursor{nullptr} { }
    FORCEINLINE constexpr TDefaultIterator(const TDefaultIterator& Other) noexcept : Cursor{Other.Cursor} { }
    FORCEINLINE constexpr TDefaultIterator(TDefaultIterator&& Other) noexcept : Cursor{std::move(Other.Cursor)} { }
    FORCEINLINE constexpr TDefaultIterator& operator=(const TDefaultIterator& Other) noexcept { this->Cursor = Other.Cursor; return *this; }
    FORCEINLINE constexpr TDefaultIterator& operator=(TDefaultIterator&& Other) noexcept { this->Cursor = std::move(Other.Cursor); return *this; }

    FORCEINLINE constexpr explicit TDefaultIterator(const pointer InCursor) noexcept : Cursor{InCursor} { }
    FORCEINLINE constexpr explicit TDefaultIterator(std::remove_const_t<value_type>* InCursor) noexcept
        requires(std::is_const_v<value_type>)
        : Cursor{InCursor}
    {
        return;
    }

    FORCEINLINE constexpr TDefaultIterator(const TDefaultIterator<std::remove_const_t<value_type>>& Other) noexcept
        requires(std::is_const_v<value_type>)
        : Cursor{Other.Cursor}
    {
        return;
    }

    FORCEINLINE constexpr decltype(auto) operator*() const noexcept
    {
        if constexpr (bMove)
        {
            return std::move(*this->base());
        }
        else
        {
            return *this->base();
        }
    }

    FORCEINLINE constexpr pointer operator->() const noexcept { return this->base(); }
    FORCEINLINE constexpr TDefaultIterator& operator++() noexcept { ++this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultIterator  operator++(i32) noexcept { TDefaultIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultIterator& operator--() noexcept { --this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultIterator  operator--(i32) noexcept { TDefaultIterator Out { *this }; --*this; return Out; }

    FORCEINLINE constexpr difference_type operator-(const TDefaultIterator& Other) const noexcept;
    FORCEINLINE constexpr difference_type operator-(const TDefaultIterator<std::remove_const_t<value_type>, bMove>& Other) const noexcept requires(std::is_const_v<value_type>);
    FORCEINLINE constexpr difference_type operator-(const TDefaultIterator<const value_type, bMove>& Other) const noexcept requires(!std::is_const_v<value_type>);

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultIterator& operator+=(const SizeType N) noexcept { this->Cursor += N; return *this; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultIterator& operator-=(const SizeType N) noexcept { this->Cursor -= N; return *this; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultIterator operator-(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out -= N; return Out; }

    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr decltype(auto) operator[](const SizeType N) const noexcept
    {
        if constexpr (bMove)
        {
            return std::move(*(this->base() + N));
        }
        else
        {
            return *(this->base() + N);
        }
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultIterator& Lhs, const TDefaultIterator& Rhs) noexcept
    {
        return Lhs.base() == Rhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultIterator& Lhs, const TDefaultIterator& Rhs) noexcept
    {
        return Lhs.base() <=> Rhs.base();
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultIterator& LHs, TDefaultIterator::pointer Rhs) noexcept
    {
        return LHs.base() == Rhs;
    }
    friend
    FORCEINLINE constexpr bool operator==(TDefaultIterator::pointer Lhs, const TDefaultIterator& RHs) noexcept
    {
        return Lhs == RHs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultIterator& Lhs, TDefaultIterator::pointer Rhs) noexcept
    {
        return Lhs.base() <=> Rhs;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(TDefaultIterator::pointer Lhs, const TDefaultIterator& Rhs) noexcept
    {
        return Lhs <=> Rhs.base();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    NODISCARD
    FORCEINLINE constexpr pointer base() const noexcept { return this->Cursor; }
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    pointer Cursor;
};

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr TDefaultIterator<T, bMove> operator+(const TDefaultIterator<T, bMove> It, const typename TDefaultIterator<T, bMove>::difference_type N) noexcept
{
    TDefaultIterator<T, bMove> Out { It };
    Out += N;
    return Out;
}

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr TDefaultIterator<T, bMove> operator+(const typename TDefaultIterator<T, bMove>::difference_type N, const TDefaultIterator<T, bMove> It) noexcept
{
    TDefaultIterator<T, bMove> Out { It };
    Out += N;
    return Out;
}

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr typename TDefaultIterator<T, bMove>::difference_type operator-(const TDefaultIterator<T, bMove> It, const typename TDefaultIterator<T, bMove>::pointer Ptr) noexcept
{
    return It.base() - Ptr;
}

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr typename TDefaultIterator<T, bMove>::difference_type operator-(const typename TDefaultIterator<T, bMove>::pointer Ptr, const TDefaultIterator<T, bMove> It) noexcept
{
    return Ptr - It.base();
}

template <typename TIn, bool bMove = false>
struct TDefaultReversedIterator : public Private::LDefaultReversedIterator
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = TIn;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    /* TODO: Should this be contiguous? */
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept  = std::random_access_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultReversedIterator() noexcept : Cursor{nullptr} { }
    FORCEINLINE constexpr TDefaultReversedIterator(const TDefaultReversedIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator(TDefaultReversedIterator&&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator& operator=(const TDefaultReversedIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator& operator=(TDefaultReversedIterator&&) noexcept = default;

    FORCEINLINE constexpr explicit TDefaultReversedIterator(const pointer InCursor) noexcept : Cursor{InCursor} { }

    FORCEINLINE constexpr TDefaultReversedIterator(const TDefaultReversedIterator<std::remove_const_t<value_type>, bMove>& Other) noexcept
        requires std::is_const_v<value_type>
        : Cursor{Other.Cursor}
    {
        return;
    }

    FORCEINLINE constexpr decltype(auto) operator*() const noexcept
    {
        if constexpr (bMove)
        {
            return std::move(*(this->base() - 1));
        }
        else
        {
            return *(this->base() - 1);
        }
    }

    FORCEINLINE constexpr pointer operator->() const noexcept { return this->base() - 1; }
    FORCEINLINE constexpr TDefaultReversedIterator& operator++() noexcept { --this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultReversedIterator  operator++(i32) noexcept { TDefaultReversedIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultReversedIterator& operator--() noexcept { ++this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultReversedIterator  operator--(i32) noexcept { TDefaultReversedIterator Out { *this }; --*this; return Out; }

    FORCEINLINE constexpr difference_type operator-(const TDefaultReversedIterator& Other) const noexcept;
    FORCEINLINE constexpr difference_type operator-(const TDefaultReversedIterator<std::remove_const_t<value_type>, bMove>& Other) const noexcept requires(std::is_const_v<value_type>);
    FORCEINLINE constexpr difference_type operator-(const TDefaultReversedIterator<const value_type, bMove>& Other) const noexcept requires(!std::is_const_v<value_type>);

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator& operator+=(const SizeType N) noexcept { this->Cursor -= N; return *this; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator& operator-=(const SizeType N) noexcept { this->Cursor += N; return *this; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultReversedIterator operator+(const SizeType N) const noexcept { TDefaultReversedIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultReversedIterator operator-(const SizeType N) const noexcept { TDefaultReversedIterator Out { *this }; Out -= N; return Out; }

    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr decltype(auto) operator[](const SizeType N) const noexcept
    {
        if constexpr (bMove)
        {
            return std::move(*(this->base() - 1 - N));
        }
        else
        {
            return *(this->base() - 1 - N);
        }
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedIterator& Lhs, const TDefaultReversedIterator& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() == Lhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedIterator& Lhs, const TDefaultReversedIterator& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() <=> Lhs.base();
    }

    template <typename UPredicate>
    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedIterator& Lhs, const TDefaultReversedFilteredIterator<UPredicate, value_type, bMove>& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() == Lhs.base();
    }
    template <typename UPredicate>
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedIterator& Lhs, const TDefaultReversedFilteredIterator<UPredicate, value_type, bMove>& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() <=> Lhs.base();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    NODISCARD
    FORCEINLINE constexpr pointer base() const noexcept { return this->Cursor; }
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    pointer Cursor;
};

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr TDefaultReversedIterator<T, bMove> operator+(const TDefaultReversedIterator<T, bMove>& It, const typename TDefaultIterator<T, bMove>::difference_type N) noexcept
{
    TDefaultReversedIterator<T, bMove> Out { It };
    Out += N;
    return Out;
}

template <typename T, bool bMove>
NODISCARD
FORCEINLINE constexpr TDefaultReversedIterator<T, bMove> operator+(const typename TDefaultIterator<T, bMove>::difference_type N, const TDefaultReversedIterator<T, bMove>& It) noexcept
{
    TDefaultReversedIterator<T, bMove> Out { It };
    Out += N;
    return Out;
}

template <typename TPredicate, typename TIn, bool bMove = false>
struct TDefaultFilteredIterator : public Private::LDefaultFilteredIterator
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = TIn;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    // Mmm, for simplicity whe have deleted operator-- and operator--(i32), therefore, we cannot make
    // this a bidirectional iterator. It is a forward iterator. In the future, this might change.
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept  = std::forward_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultFilteredIterator() noexcept
        : Cursor{ nullptr }, Slack{ nullptr }, Filter{ TDefaultFilteredIterator::DefaultFilter() }, bFirstDereference{ true }
    {
        return;
    }

    FORCEINLINE constexpr TDefaultFilteredIterator(const TDefaultFilteredIterator& Other) noexcept
        : Cursor{ Other.Cursor }, Slack{ Other.Slack }, Filter{ Other.Filter }, bFirstDereference{ Other.bFirstDereference }
    {
        return;
    }
    FORCEINLINE constexpr TDefaultFilteredIterator(TDefaultFilteredIterator&& Other) noexcept
        : Cursor{ std::move(Other.Cursor) }, Slack{ std::move(Other.Slack) }, Filter{ std::move(Other.Filter) }, bFirstDereference{ std::move(Other.bFirstDereference) }
    {
        return;
    }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator=(const TDefaultFilteredIterator& Other) noexcept
    {
        this->Cursor = Other.Cursor;
        this->Slack = Other.Slack;
        this->Filter = Other.Filter;
        this->bFirstDereference = Other.bFirstDereference;

        return *this;
    }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator=(TDefaultFilteredIterator&& Other) noexcept
    {
        this->Cursor = std::move(Other.Cursor);
        this->Slack = std::move(Other.Slack);
        this->Filter = std::move(Other.Filter);
        this->bFirstDereference = std::move(Other.bFirstDereference);

        return *this;
    }

    //#
    //# What does bInManualUse do?
    //# Tldr: If you are using the iterator normally, you should set this to true.
    //#
    //# But why?
    //# Because the first dereference of the iterator when using it in a range-based for loop (auto& E : Iterator)
    //# will not call operator++() first (makes sense, duh) but will dereference the iterator first.
    //# The problem is that if the first element is not valid (by the filter) it will still be dereferenced and
    //# returned. Therefore, we have a simple overload in operator*() that prevents this. But this might cause
    //# confusion when using it manually; therefore, we have this flag.
    //#
    FORCEINLINE constexpr explicit TDefaultFilteredIterator(const TPredicate& InFilter, const pointer InCursor, const pointer InSlack, const bool bInManualUse = false) noexcept
        : Cursor(InCursor), Slack(InSlack), Filter(InFilter), bFirstDereference(bInManualUse == false) { }

    FORCEINLINE constexpr TDefaultFilteredIterator(const TDefaultFilteredIterator<TPredicate, std::remove_const_t<pointer>, bMove>& Other) noexcept
        requires std::is_const_v<value_type>
        : Cursor{Other.Cursor}, Slack{Other.Slack}, Filter{Other.Filter}, bFirstDereference{Other.bFirstDereference}
    {
        return;
    }

    FORCEINLINE constexpr decltype(auto) operator*() const noexcept
    {
#if LAL_DO_CHECKS
        TDefaultFilteredIterator::_check([this](void) -> bool { return this->GetThisOrNext() != this->Slack; });
#endif /* ~LAL_DO_CHECKS */

        if constexpr (bMove)
        {
            return std::move(*this->base());
        }
        else
        {
            return *this->base();
        }
    }

    FORCEINLINE constexpr pointer operator->() const noexcept { return this->GetThisOrNext(); }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator++() noexcept;
    FORCEINLINE constexpr TDefaultFilteredIterator  operator++(i32) noexcept { TDefaultFilteredIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator--() noexcept = delete; /* For simplicity removed, see #TDefaultFilteredIterator::iterator_category. */
    FORCEINLINE constexpr TDefaultFilteredIterator  operator--(i32) noexcept = delete;

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator& operator+=(SizeType N) noexcept;
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator& operator-=(const SizeType N) noexcept = delete;
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultFilteredIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultFilteredIterator operator-(const SizeType N) const noexcept = delete;

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultFilteredIterator& Lhs, const TDefaultFilteredIterator& Rhs) noexcept
    {
        return Lhs.base() == Rhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultFilteredIterator& Lhs, const TDefaultFilteredIterator& Rhs) noexcept
    {
        return Lhs.base() <=> Rhs.base();
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultFilteredIterator& Lhs, const TDefaultIterator<value_type, bMove>& Rhs) noexcept
    {
        return Lhs.base() == Rhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultFilteredIterator& Lhs, const TDefaultIterator<value_type, bMove>& Rhs) noexcept
    {
        return Lhs.base() <=> Rhs.base();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    NODISCARD
    FORCEINLINE constexpr pointer base() const noexcept { return this->GetThisOrNext(); }
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    mutable pointer   Cursor;
    pointer           Slack;
    const TPredicate& Filter;
    mutable bool      bFirstDereference;

private:

#if LAL_DO_CHECKS
    template <typename UPredicate>
    FORCEINLINE static void _check(UPredicate&& Predicate);
#endif /* ~LAL_DO_CHECKS */

    inline static constexpr bool DefaultFilter() noexcept { return true; }

    FORCEINLINE constexpr pointer GetThisOrNext() const noexcept;
};

template <typename TPredicate, typename TIn, bool bMove = false>
struct TDefaultReversedFilteredIterator : public Private::LDefaultReversedFilteredIterator
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = TIn;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    /* Please see TDefaultFilteredIterator::iterator_category for explanation. */
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept  = std::forward_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultReversedFilteredIterator() noexcept
        : Cursor{ nullptr }, Begin{ nullptr }, Filter{ TDefaultReversedFilteredIterator::DefaultFilter() }, bFirstDereference{ true }
    {
        return;
    }

    FORCEINLINE constexpr TDefaultReversedFilteredIterator(const TDefaultReversedFilteredIterator& Other) noexcept
        : Cursor{ Other.Cursor }, Begin{ Other.Begin }, Filter{ Other.Filter }, bFirstDereference{ Other.bFirstDereference }
    {
        return;
    }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator(TDefaultReversedFilteredIterator&& Other) noexcept
        : Cursor{ std::move(Other.Cursor) }, Begin{ std::move(Other.Begin) }, Filter{ std::move(Other.Filter) }, bFirstDereference{ std::move(Other.bFirstDereference) }
    {
        return;
    }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator=(const TDefaultReversedFilteredIterator& Other) noexcept
    {
        this->Cursor = Other.Cursor;
        this->Begin = Other.Begin;
        this->Filter = Other.Filter;
        this->bFirstDereference = Other.bFirstDereference;

        return *this;
    }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator=(TDefaultReversedFilteredIterator&& Other) noexcept
    {
        this->Cursor = std::move(Other.Cursor);
        this->Begin = std::move(Other.Begin);
        this->Filter = std::move(Other.Filter);
        this->bFirstDereference = std::move(Other.bFirstDereference);

        return *this;
    }

    //#
    //# What does bInManualUse do?
    //# If you are using the iterator normally, you should set this to true.
    //#
    //# But why?
    //# Because the first dereference of the iterator when using it in a range-based for loop (auto& E : Iterator)
    //# will not call operator++() first (makes sense, duh) but will dereference the iterator first.
    //# The problem is that if the first element is not valid (by the filter) it will still be dereferenced and
    //# returned. Therefore, we have a simple overload in operator*() that prevents this. But this might cause
    //# confusion when using it manually; therefore, we have this flag.
    //#
    FORCEINLINE constexpr explicit TDefaultReversedFilteredIterator(const TPredicate& InFilter, const pointer InCursor, const pointer InBegin, const bool bInManualUse = false) noexcept
        : Cursor(InCursor), Begin(InBegin), Filter(InFilter), bFirstDereference(bInManualUse == false) { }

    FORCEINLINE constexpr TDefaultReversedFilteredIterator(const TDefaultReversedIterator<std::remove_const_t<value_type>, bMove>& Other) noexcept
        requires std::is_const_v<value_type>
        : Cursor{Other.Cursor}, Begin{Other.Cursor}, Filter{Other.Filter}, bFirstDereference{Other.bFirstDereference}
    {
        return;
    }

    FORCEINLINE constexpr decltype(auto) operator*() const noexcept
    {
#if LAL_DO_CHECKS
        TDefaultReversedFilteredIterator::_check([this](void) -> bool { return this->GetThisOrPrevious() != this->Begin - 1; });
#endif /* ~LAL_DO_CHECKS */

        if constexpr (bMove)
        {
            return std::move(*(this->base() - 1));
        }
        else
        {
            return *(this->base() - 1);
        }
    }

    FORCEINLINE constexpr pointer operator->() const noexcept { return this->base() - 1; }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator++() noexcept;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator  operator++(i32) noexcept { TDefaultReversedFilteredIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator--() noexcept = delete; /* For simplicity removed, see #TDefaultReversedFilteredIterator::iterator_category. */
    FORCEINLINE constexpr TDefaultReversedFilteredIterator  operator--(i32) noexcept = delete;

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator+=(SizeType N) noexcept;
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator-=(const SizeType N) noexcept = delete;
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    NODISCARD
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator-(const SizeType N) const noexcept = delete;

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedFilteredIterator& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() == Lhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedFilteredIterator& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() <=> Lhs.base();
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedIterator<value_type, bMove>& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() == Lhs.base();
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedIterator<value_type, bMove>& Rhs) noexcept
    {
        /* As of ISO C++ this must be reversed. */
        return Rhs.base() <=> Lhs.base();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    NODISCARD
    FORCEINLINE constexpr pointer base() const noexcept { return this->GetThisOrPrevious(); }
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    mutable pointer   Cursor;
    pointer           Begin;
    const TPredicate& Filter;
    mutable bool      bFirstDereference;

private:

#if LAL_DO_CHECKS
    template <typename UPredicate>
    FORCEINLINE static void _check(UPredicate&& Predicate);
#endif /* ~LAL_DO_CHECKS */

    inline static constexpr bool DefaultFilter() noexcept { return true; }

    //# Returns as the ISO suggest -- one element past the actual element.
    FORCEINLINE constexpr pointer       GetThisOrPrevious() const noexcept;
};

///////////////////////////////////////////////////////////////////////////////
// C++ ISO
static_assert(std::input_iterator<TDefaultIterator<LSize>>);
static_assert(std::forward_iterator<TDefaultIterator<LSize>>);
static_assert(std::bidirectional_iterator<TDefaultIterator<LSize>>);
static_assert(std::random_access_iterator<TDefaultIterator<LSize>>);
// ~C++ ISO
///////////////////////////////////////////////////////////////////////////////

static_assert(Private::IsValidIterator<TDefaultIterator<LSize>>);
static_assert(Private::IsValidIteratorWeak<TDefaultIterator<LSize>>);
static_assert(TIterator_CanTraverseMultipleTimes_v<TDefaultIterator<LSize>>);
static_assert(TIterator_CanTraverseMultipleTimes_v<TDefaultIterator<LSize>>);
static_assert(TIterator_CanTraverseBackward_v<TDefaultIterator<LSize>>);
static_assert(TIterator_IsPointerLike_v<TDefaultIterator<LSize>>);
static_assert(TIterator_IsContiguous_v<TDefaultIterator<LSize>>);

///////////////////////////////////////////////////////////////////////////////
// C++ ISO
static_assert(std::input_iterator<TDefaultReversedIterator<LSize>>);
static_assert(std::forward_iterator<TDefaultReversedIterator<LSize>>);
static_assert(std::bidirectional_iterator<TDefaultReversedIterator<LSize>>);
static_assert(std::random_access_iterator<TDefaultReversedIterator<LSize>>);
// ~C++ ISO
///////////////////////////////////////////////////////////////////////////////

static_assert(TIterator_IsReadWrite_v<TDefaultReversedIterator<LSize>>);
static_assert(TIterator_CanTraverseMultipleTimes_v<TDefaultReversedIterator<LSize>>);
static_assert(TIterator_CanTraverseBackward_v<TDefaultReversedIterator<LSize>>);
static_assert(TIterator_IsPointerLike_v<TDefaultReversedIterator<LSize>>);
// static_assert(TIterator_IsContiguous_v<TDefaultReversedIterator<LSize>>);

///////////////////////////////////////////////////////////////////////////////
// C++ ISO
static_assert(std::input_iterator<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(std::forward_iterator<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(std::bidirectional_iterator<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(std::random_access_iterator<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
// ~C++ ISO
///////////////////////////////////////////////////////////////////////////////

static_assert(TIterator_IsReadWrite_v<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(TIterator_CanTraverseMultipleTimes_v<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(TIterator_CanTraverseBackward_v<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(TIterator_IsPointerLike_v<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(TIterator_IsContiguous_v<TDefaultFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);

///////////////////////////////////////////////////////////////////////////////
// C++ ISO
static_assert(std::input_iterator<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(std::forward_iterator<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(std::bidirectional_iterator<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(std::random_access_iterator<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
// ~C++ ISO
///////////////////////////////////////////////////////////////////////////////

static_assert(TIterator_IsReadWrite_v<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(TIterator_CanTraverseMultipleTimes_v<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>>);
static_assert(TIterator_CanTraverseBackward_v<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(TIterator_IsPointerLike_v<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);
static_assert(TIterator_IsContiguous_v<TDefaultReversedFilteredIterator<decltype([](const auto& Element) -> bool { return true; }), LSize>> == false);

template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultIteratorFactory
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = typename TIterator::value_type;
    using difference_type   = typename TIterator::difference_type;
    using pointer           = typename TIterator::pointer;
    using reference         = typename TIterator::reference;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    using Iterator                 = TIterator;
    using ReversedIterator         = TReversedIterator;
    template <std::predicate<typename TIterator::value_type> TPredicate>
    using FilteredIterator         = TFilteredIterator<TPredicate>;
    template <std::predicate<typename TIterator::value_type> TPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<TPredicate>;

    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedIterator::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedIterator::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedIterator::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);

    FORCEINLINE constexpr TDefaultIteratorFactory() noexcept : Data{nullptr}, Slack{nullptr} { }
    FORCEINLINE constexpr TDefaultIteratorFactory(const pointer InData, const pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}
    {
        return;
    }

    FORCEINLINE constexpr Iterator begin() noexcept { return Iterator{ this->Data  }; }
    FORCEINLINE constexpr Iterator end()   noexcept { return Iterator{ this->Slack }; }

    //#
    //# Reverses the iteration from front to back, to back to front.
    //#
    FORCEINLINE constexpr TDefaultReversedIteratorFactory<
          Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Reverse() noexcept
    {
        return TDefaultReversedIteratorFactory<
              Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            this->Data, this->Slack
        };
    }

    //#
    //# Applies a filter to the iterator, which will only return elements that match if the predicate is satisfied.
    //#
    template <std::predicate<value_type> TPredicate>
    FORCEINLINE constexpr TDefaultFilteredIteratorFactory<
          TPredicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Filter(const TPredicate& Predicate) noexcept
    {
        return TDefaultFilteredIteratorFactory<
              TPredicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            Predicate, this->Data, this->Slack
        };
    }

    //#
    //# Reverses and applies a filter to the iterator, which will only return elements that match if the predicate is satisfied.
    //#
    template <std::predicate<value_type> TPredicate>
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          TPredicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > ReversedFilter(const TPredicate& Predicate) noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              TPredicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            Predicate, this->Data, this->Slack
        };
    }

    pointer Data;
    pointer Slack;
};

template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedIteratorFactory
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = typename TIterator::value_type;
    using difference_type   = typename TIterator::difference_type;
    using pointer           = typename TIterator::pointer;
    using reference         = typename TIterator::reference;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    using Iterator                  = TIterator;
    using ReversedIterator          = TReversedIterator;
    template <std::predicate<typename TIterator::value_type> TPredicate>
    using FilteredIterator          = TFilteredIterator<TPredicate>;
    template <std::predicate<typename TIterator::value_type> TPredicate>
    using ReversedFilteredIterator  = TReversedFilteredIterator<TPredicate>;

    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedIterator::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedIterator::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedIterator::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);

    FORCEINLINE constexpr TDefaultReversedIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedIteratorFactory(const pointer InData, const pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}
    {
        return;
    }

    FORCEINLINE constexpr ReversedIterator begin() noexcept { return ReversedIterator{ this->Slack }; }
    FORCEINLINE constexpr ReversedIterator end() noexcept { return ReversedIterator{ this->Data }; }

    //#
    //# Applies a filter to the iterator, which will only return elements that match if the predicate is satisfied.
    //#
    template <std::predicate<value_type> TPredicate>
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          TPredicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Filter(const TPredicate& Predicate) noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              TPredicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            Predicate, this->Data, this->Slack
        };
    }

    pointer Data;
    pointer Slack;
};

template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultFilteredIteratorFactory
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = typename TIterator::value_type;
    using difference_type   = typename TIterator::difference_type;
    using pointer           = typename TIterator::pointer;
    using reference         = typename TIterator::reference;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    using Predicate         = TPredicate;

    using Iterator                 = TIterator;
    using ReversedIterator         = TReversedIterator;
    template <std::predicate<typename TIterator::value_type> UPredicate>
    using FilteredIterator         = TFilteredIterator<UPredicate>;
    template <std::predicate<typename TIterator::value_type> UPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<UPredicate>;

    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedIterator::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedIterator::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedIterator::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);

    FORCEINLINE constexpr TDefaultFilteredIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultFilteredIteratorFactory(const Predicate& InFilter, const pointer InData, const pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}, Filter{InFilter}
    {
        return;
    }

    FORCEINLINE constexpr FilteredIterator<Predicate> begin() noexcept { return FilteredIterator<Predicate>{ this->Filter, this->Data, this->Slack }; }
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Slack }; }

    //#
    //# Reverses the iteration from front to back, to back to front.
    //#
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          Predicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Reverse() noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              Predicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            this->Filter, this->Data, this->Slack
        };
    }

    pointer Data;
    pointer Slack;
    const Predicate& Filter;
};

template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedFilteredIteratorFactory
{
    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type        = typename TIterator::value_type;
    using difference_type   = typename TIterator::difference_type;
    using pointer           = typename TIterator::pointer;
    using reference         = typename TIterator::reference;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    using Predicate         = TPredicate;

    using Iterator                 = TIterator;
    using ReversedIterator         = TReversedIterator;
    template <std::predicate<typename TIterator::value_type> UPredicate>
    using FilteredIterator         = TFilteredIterator<UPredicate>;
    template <std::predicate<typename TIterator::value_type> UPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<UPredicate>;

    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedIterator::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::value_type, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::value_type>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedIterator::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::pointer>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedIterator::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);
    static_assert(std::is_same_v<typename TIterator::reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::value_type& Element) -> bool { return true; })>::reference>);

    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory(const Predicate& InFilter, const pointer InData, const pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}, Filter{InFilter}
    {
        return;
    }

    FORCEINLINE constexpr ReversedFilteredIterator<Predicate> begin() noexcept { return ReversedFilteredIterator<Predicate>{ this->Filter, this->Slack, this->Data }; }
    FORCEINLINE constexpr ReversedIterator end() noexcept { return ReversedIterator{ this->Data }; }

    pointer Data;
    pointer Slack;
    const Predicate& Filter;
};

#endif /* LAL_WITH_LEGACY_ITERATORS */

} /* ~Namespace Lal */

#ifndef ITERATOR
    #define ITERATOR ::Lal::TIteratorConcept auto
#endif /* ITERATOR */

#ifndef ITERATOR_PAIR
    #define ITERATOR_PAIR(A, B) ::Lal::TIteratorPairConcept<decltype(A), decltype(B)>
#endif /* ITERATOR_PAIR */

#ifndef ITERATOR_CROSS
    #define ITERATOR_CROSS(A, B) ::Lal::TCrossIteratorConcept<decltype(A), decltype(B)>
#endif /* ITERATOR_CROSS */

#if LAL_WITH_LEGACY_ITERATORS

namespace Lal::Private
{

template <typename TIn, bool bMove>
NODISCARD
FORCEINLINE constexpr auto ToMove_TDefaultIterator(TDefaultIterator<TIn, bMove> It) noexcept -> TDefaultIterator<TIn, true>
{
    return TDefaultIterator<TIn, true>{ It.base() };
}

template <typename TIn, bool bMove>
NODISCARD
FORCEINLINE constexpr auto ToMove_TDefaultReversedIterator(TDefaultReversedIterator<TIn, bMove> It) noexcept -> TDefaultReversedIterator<TIn, true>
{
    return TDefaultReversedIterator<TIn, true>{ It.base() };
}

template <typename TPredicate, typename TIn, bool bMove>
NODISCARD
FORCEINLINE constexpr auto ToMove_TDefaultFilteredIterator(TDefaultFilteredIterator<TPredicate, TIn, bMove> It) noexcept -> TDefaultFilteredIterator<TPredicate, TIn, true>
{
    return TDefaultFilteredIterator<TPredicate, TIn, true>{ It.Filter, It.Cursor, It.Slack, It.bFirstDereference == false };
}

template <typename TPredicate, typename TIn, bool bMove>
NODISCARD
FORCEINLINE constexpr auto ToMove_TDefaultReversedFilteredIterator(TDefaultReversedFilteredIterator<TPredicate, TIn, bMove> It) noexcept -> TDefaultReversedFilteredIterator<TPredicate, TIn, true>
{
    return TDefaultReversedFilteredIterator<TPredicate, TIn, true>{ It.Filter, It.Cursor, It.Begin, It.bFirstDereference == false };
}

} /* ~Namespace Lal::Private */

namespace std
{

template <typename Iterator> requires std::is_base_of_v<Lal::Private::LDefaultIterator, Iterator>
NODISCARD
FORCEINLINE constexpr auto make_move_iterator(Iterator It) noexcept -> decltype(Lal::Private::ToMove_TDefaultIterator(It))
{
    return Lal::Private::ToMove_TDefaultIterator(It);
}

template <typename Iterator> requires std::is_base_of_v<Lal::Private::LDefaultReversedIterator, Iterator>
NODISCARD
FORCEINLINE constexpr auto make_move_iterator(Iterator It) noexcept -> decltype(Lal::Private::ToMove_TDefaultReversedIterator(It))
{
    return Lal::Private::ToMove_TDefaultReversedIterator(It);
}

template <typename Iterator> requires std::is_base_of_v<Lal::Private::LDefaultFilteredIterator, Iterator>
NODISCARD
FORCEINLINE constexpr auto make_move_iterator(Iterator It) noexcept -> decltype(Lal::Private::ToMove_TDefaultFilteredIterator(It))
{
    return Lal::Private::ToMove_TDefaultFilteredIterator(It);
}

template <typename Iterator> requires std::is_base_of_v<Lal::Private::LDefaultReversedFilteredIterator, Iterator>
FORCEINLINE constexpr auto make_move_iterator(Iterator It) noexcept -> decltype(Lal::Private::ToMove_TDefaultReversedFilteredIterator(It))
{
    return Lal::Private::ToMove_TDefaultReversedFilteredIterator(It);
}

} /* ~Namespace std */

#endif /* LAL_WITH_LEGACY_ITERATORS */
