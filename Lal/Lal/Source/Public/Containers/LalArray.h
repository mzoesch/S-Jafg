// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_CHECK_ARRAY
    #if LAL_CHECK_CONTAINER_BOUNDS
        #define LAL_CHECK_ARRAY(Expr)                                   jassert( Expr )
    #else /* LAL_CHECK_CONTAINER_BOUNDS */
        #define LAL_CHECK_ARRAY(Expr)
    #endif /* !LAL_CHECK_CONTAINER_BOUNDS */
#endif /* !LAL_CHECK_ARRAY */

namespace Lal
{

//#
//# Use these traits to qualify your iterators with the Lal iterator factories.
//#
template <typename T>
concept TIteratorConcept = requires(T t)
{
    typename T::T;
    typename T::Pointer;
    typename T::Reference;

    { t.Cursor };

    //# The other requirements just follow the ISO of C++.
};

//#
//# The factories used by Lal containers to create iterators.
//#
template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultIteratorFactory;
template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedIteratorFactory;
template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultFilteredIteratorFactory;
template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedFilteredIteratorFactory;

//#
//# The default iterators used by Lal containers.
//# You may define your own iterators if you want to. You may also define your own factories.
//#
template <typename TIn>
struct TDefaultIterator;
template <typename TIn>
struct TDefaultReversedIterator;
template <typename TPredicate, typename TIn>
struct TDefaultFilteredIterator;
template <typename TPredicate, typename TIn>
struct TDefaultReversedFilteredIterator;

template <typename TIn>
struct TDefaultIterator
{
    typedef TIn            T;
    typedef std::ptrdiff_t Difference;
    typedef T*             Pointer;
    typedef T&             Reference;

    typedef TDefaultIterator<T> _TDefaultIterator;
    typedef TDefaultReversedIterator<T> _TDefaultReversedIterator;
    template <std::predicate<T> TPredicate>
    using _TDefaultFilteredIterator = TDefaultFilteredIterator<TPredicate, TIn>;
    template <std::predicate<T> TPredicate>
    using _TDefaultReversedFilteredIterator = TDefaultReversedFilteredIterator<TPredicate, TIn>;

    typedef TDefaultIteratorFactory<
          _TDefaultIterator
        , _TDefaultReversedIterator
        , _TDefaultFilteredIterator
        , _TDefaultReversedFilteredIterator
        > Factory;

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type = T;
    using difference_type = Difference;
    using pointer = Pointer;
    using reference = Reference;
    using iterator_category = std::random_access_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultIterator() noexcept : Cursor{nullptr} { }
    FORCEINLINE constexpr TDefaultIterator(const TDefaultIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultIterator(TDefaultIterator&&) noexcept = default;
    FORCEINLINE constexpr TDefaultIterator& operator=(const TDefaultIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultIterator& operator=(TDefaultIterator&&) noexcept = default;

    FORCEINLINE constexpr explicit TDefaultIterator(const Pointer InCursor) noexcept : Cursor{InCursor} { }

    FORCEINLINE constexpr TDefaultIterator(const TDefaultIterator<std::remove_const_t<T>>& Other) noexcept
        requires std::is_const_v<T>
        : Cursor{Other.Cursor}
    {
        return;
    }

    FORCEINLINE constexpr Reference operator*() const noexcept { return *this->Cursor; }
    FORCEINLINE constexpr Pointer operator->() const noexcept { return this->Cursor; }
    FORCEINLINE constexpr TDefaultIterator& operator++() noexcept { ++this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultIterator operator++(i32) noexcept { TDefaultIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultIterator& operator--() noexcept { --this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultIterator operator--(i32) noexcept { TDefaultIterator Out { *this }; --*this; return Out; }

    FORCEINLINE constexpr Difference operator-(const TDefaultIterator& Other) const noexcept
    {
        checkSlow( this->Cursor != nullptr && "Underflow" )
        return this->Cursor - Other.Cursor;
    }

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

    FORCEINLINE constexpr TDefaultIterator& operator+=(const Difference N) noexcept { this->Cursor += N; return *this; }
    FORCEINLINE constexpr TDefaultIterator& operator-=(const Difference N) noexcept { this->Cursor -= N; return *this; }
    NODISCARD
    FORCEINLINE constexpr TDefaultIterator operator+(const Difference N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    NODISCARD
    FORCEINLINE constexpr TDefaultIterator operator-(const Difference N) const noexcept { TDefaultIterator Out { *this }; Out -= N; return Out; }

    NODISCARD
    FORCEINLINE constexpr Reference operator[](const Difference N) const noexcept
    {
        return *(this->Cursor + N);
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultIterator& Lhs, const TDefaultIterator& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultIterator& Lhs, const TDefaultIterator& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    Pointer Cursor;
};

template <typename T>
NODISCARD
FORCEINLINE constexpr TDefaultIterator<T> operator+(const TDefaultIterator<T>& It, const typename TDefaultIterator<T>::Difference N) noexcept
{
    TDefaultIterator<T> Out { It };
    Out += N;
    return Out;
}

template <typename T>
NODISCARD
FORCEINLINE constexpr TDefaultIterator<T> operator+(const typename TDefaultIterator<T>::Difference N, const TDefaultIterator<T>& It) noexcept
{
    TDefaultIterator<T> Out { It };
    Out += N;
    return Out;
}

template <typename TIn>
struct TDefaultReversedIterator
{
    typedef TIn            T;
    typedef std::ptrdiff_t Difference;
    typedef T*             Pointer;
    typedef T&             Reference;

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type = T;
    using difference_type = Difference;
    using pointer = Pointer;
    using reference = Reference;
    using iterator_category = std::random_access_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultReversedIterator() noexcept : Cursor{nullptr} { }
    FORCEINLINE constexpr TDefaultReversedIterator(const TDefaultReversedIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator(TDefaultReversedIterator&&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator& operator=(const TDefaultReversedIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedIterator& operator=(TDefaultReversedIterator&&) noexcept = default;

    FORCEINLINE constexpr explicit TDefaultReversedIterator(const Pointer InCursor) noexcept : Cursor{InCursor} { }

    FORCEINLINE constexpr TDefaultReversedIterator(const TDefaultReversedIterator<std::remove_const_t<T>>& Other) noexcept
        requires std::is_const_v<T>
        : Cursor{Other.Cursor}
    {
        return;
    }

    FORCEINLINE constexpr Reference operator*() const noexcept { Pointer Out { this->Cursor }; return *--Out; }
    FORCEINLINE constexpr Pointer operator->() const noexcept { Pointer Out { this->Cursor }; return --Out; }
    FORCEINLINE constexpr TDefaultReversedIterator& operator++() noexcept { --this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultReversedIterator operator++(i32) noexcept { TDefaultReversedIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultReversedIterator& operator--() noexcept { ++this->Cursor; return *this; }
    FORCEINLINE constexpr TDefaultReversedIterator operator--(i32) noexcept { TDefaultReversedIterator Out { *this }; --*this; return Out; }

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator& operator+=(const SizeType N) noexcept { this->Cursor -= N; return *this; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator& operator-=(const SizeType N) noexcept { this->Cursor += N; return *this; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedIterator operator-(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out -= N; return Out; }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedIterator& Lhs, const TDefaultReversedIterator& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedIterator& Lhs, const TDefaultReversedIterator& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    Pointer Cursor;
};

template <typename TPredicate, typename TIn>
struct TDefaultFilteredIterator
{
    typedef TPredicate     Predicate;
    typedef TIn            T;
    typedef std::ptrdiff_t Difference;
    typedef T*             Pointer;
    typedef T&             Reference;

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type = T;
    using difference_type = Difference;
    using pointer = Pointer;
    using reference = Reference;
    using iterator_category = std::random_access_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultFilteredIterator() noexcept = delete;
    FORCEINLINE constexpr TDefaultFilteredIterator(const TDefaultFilteredIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultFilteredIterator(TDefaultFilteredIterator&&) noexcept = default;
    FORCEINLINE constexpr TDefaultFilteredIterator& operator=(const TDefaultFilteredIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultFilteredIterator& operator=(TDefaultFilteredIterator&&) noexcept = default;

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
    FORCEINLINE constexpr explicit TDefaultFilteredIterator(const Predicate& InFilter, const Pointer InCursor, const Pointer InSlack, const bool bInManualUse = false) noexcept
        : Cursor(InCursor), Slack(InSlack), Filter(InFilter), bFirstDereference(bInManualUse == false) { }

    FORCEINLINE constexpr TDefaultFilteredIterator(const TDefaultFilteredIterator<TPredicate, std::remove_const_t<T>>& Other) noexcept
        requires std::is_const_v<T>
        : Cursor{Other.Cursor}, Slack{Other.Slack}, Filter{Other.Filter}, bFirstDereference{Other.bFirstDereference}
    {
        return;
    }

    FORCEINLINE constexpr Reference operator*() const noexcept
    {
        if (this->bFirstDereference)
        {
            this->bFirstDereference = false;

            if (this->Filter(*this->Cursor) == false)
            {
                ++this->Cursor;
                while (this->Cursor != this->Slack)
                {
                    if (this->Filter(*this->Cursor))
                    {
                        break;
                    }

                    ++this->Cursor;

                    continue;
                }
            }
        }

        return *this->Cursor;
    }
    FORCEINLINE constexpr Pointer operator->() const noexcept { return &this->operator*(); }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator++() noexcept
    {
        checkSlow( this->bFirstDereference == false )

        ++this->Cursor;
        while (this->Cursor != this->Slack)
        {
            if (this->Filter(*this->Cursor))
            {
                break;
            }

            ++this->Cursor;

            continue;
        }

        return *this;
    }
    FORCEINLINE constexpr TDefaultFilteredIterator operator++(i32) noexcept { TDefaultFilteredIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultFilteredIterator& operator--() noexcept = delete;
    FORCEINLINE constexpr TDefaultFilteredIterator operator--(i32) noexcept = delete;

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator& operator+=(SizeType N) noexcept
    {
        checkSlow( this->bFirstDereference == false )

        ++this->Cursor;
        while (this->Cursor != this->Slack)
        {
            if (this->Filter(*this->Cursor))
            {
                if (--N == 0)
                {
                    break;
                }
            }

            ++this->Cursor;

            continue;
        }

        return *this;
    }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator& operator-=(const SizeType N) noexcept = delete;
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultFilteredIterator operator-(const SizeType N) const noexcept = delete;

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultFilteredIterator& Lhs, const TDefaultFilteredIterator& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultFilteredIterator& Lhs, const TDefaultFilteredIterator& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultFilteredIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultFilteredIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    mutable Pointer Cursor;
    Pointer Slack;
    const Predicate& Filter;
    mutable bool bFirstDereference;
};

template <typename TPredicate, typename TIn>
struct TDefaultReversedFilteredIterator
{
    typedef TPredicate     Predicate;
    typedef TIn            T;
    typedef std::ptrdiff_t Difference;
    typedef T*             Pointer;
    typedef T&             Reference;

    ///////////////////////////////////////////////////////////////////////////////
    // C++ ISO
    using value_type = T;
    using difference_type = Difference;
    using pointer = Pointer;
    using reference = Reference;
    using iterator_category = std::random_access_iterator_tag;
    // ~C++ ISO
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE constexpr TDefaultReversedFilteredIterator() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator(const TDefaultReversedFilteredIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator(TDefaultReversedFilteredIterator&&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator=(const TDefaultReversedFilteredIterator&) noexcept = default;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator=(TDefaultReversedFilteredIterator&&) noexcept = default;

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
    FORCEINLINE constexpr explicit TDefaultReversedFilteredIterator(const Predicate& InFilter, const Pointer InCursor, const Pointer InBegin, const bool bInManualUse = false) noexcept
        : Cursor(InCursor), Begin(InBegin), Filter(InFilter), bFirstDereference(bInManualUse == false) { }

    FORCEINLINE constexpr TDefaultReversedFilteredIterator(const TDefaultReversedIterator<std::remove_const_t<T>>& Other) noexcept
        requires std::is_const_v<T>
        : Cursor{Other.Cursor}, Begin{Other.Cursor}, Filter{Other.Filter}, bFirstDereference{Other.bFirstDereference}
    {
        return;
    }

    FORCEINLINE constexpr Reference operator*() const noexcept
    {
        if (this->bFirstDereference)
        {
            this->bFirstDereference = false;

            if (Pointer OuterElement { this->Cursor }; this->Filter(*--OuterElement) == false)
            {
                --this->Cursor;
                while (this->Cursor != this->Begin)
                {
                    if (Pointer InnerElement { this->Cursor }; this->Filter(*--InnerElement))
                    {
                        break;
                    }

                    --this->Cursor;

                    continue;
                }
            }
        }

        Pointer Out { this->Cursor };
        return *--Out;
    }
    FORCEINLINE constexpr Pointer operator->() const noexcept { return &this->operator*(); }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator++() noexcept
    {
        checkSlow( this->bFirstDereference == false )

        --this->Cursor;
        while (this->Cursor != this->Begin)
        {
            if (Pointer Element { this->Cursor }; this->Filter(*--Element))
            {
                break;
            }

            --this->Cursor;

            continue;
        }

        return *this;
    }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator++(i32) noexcept { TDefaultReversedFilteredIterator Out { *this }; ++*this; return Out; }
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator--() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator--(i32) noexcept = delete;

    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator+=(SizeType N) noexcept
    {
        checkSlow( this->bFirstDereference == false )

        --this->Cursor;
        while (this->Cursor != this->Begin)
        {
            if (Pointer Element { this->Cursor }; this->Filter(*--Element))
            {
                if (--N == 0)
                {
                    break;
                }
            }

            --this->Cursor;

            continue;
        }

        return *this;
    }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator& operator-=(const SizeType N) noexcept = delete;
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator+(const SizeType N) const noexcept { TDefaultIterator Out { *this }; Out += N; return Out; }
    template <std::integral SizeType>
    FORCEINLINE constexpr TDefaultReversedFilteredIterator operator-(const SizeType N) const noexcept = delete;

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedFilteredIterator& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedFilteredIterator& Lhs, const TDefaultReversedFilteredIterator& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    friend
    FORCEINLINE constexpr bool operator==(const TDefaultReversedFilteredIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor == Rhs.Cursor;
    }
    friend
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TDefaultReversedFilteredIterator& Lhs, const TDefaultIterator<T>& Rhs) noexcept
    {
        return Lhs.Cursor <=> Rhs.Cursor;
    }

    mutable Pointer Cursor;
    Pointer Begin;
    const Predicate& Filter;
    mutable bool bFirstDereference;
};

static_assert(TIteratorConcept<TDefaultIterator<LSize>>);
static_assert(TIteratorConcept<TDefaultReversedIterator<LSize>>);
static_assert(TIteratorConcept<TDefaultFilteredIterator<decltype([](const LSize& Element) -> bool { return true; }), LSize>>);
static_assert(TIteratorConcept<TDefaultReversedFilteredIterator<decltype([](const LSize& Element) -> bool { return true; }), LSize>>);

///////////////////////////////////////////////////////////////////////////////
// C++ ISO
static_assert(std::input_iterator<TDefaultIterator<LSize>>);
static_assert(std::forward_iterator<TDefaultIterator<LSize>>);
static_assert(std::bidirectional_iterator<TDefaultIterator<LSize>>);
static_assert(std::random_access_iterator<TDefaultIterator<LSize>>);
// ~C++ ISO
///////////////////////////////////////////////////////////////////////////////

template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultIteratorFactory
{
    static_assert(TIteratorConcept<TIterator>);
    static_assert(TIteratorConcept<TReversedIterator>);
    static_assert(TIteratorConcept<TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);
    static_assert(TIteratorConcept<TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);

    typedef TIterator Iterator;
    typedef TReversedIterator ReversedIterator;
    template <std::predicate<typename TIterator::T> TPredicate>
    using FilteredIterator = TFilteredIterator<TPredicate>;
    template <std::predicate<typename TIterator::T> TPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<TPredicate>;

    static_assert(std::is_same_v<typename TIterator::T, typename TReversedIterator::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedIterator::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedIterator::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);

    typedef typename TIterator::T         T;
    typedef typename TIterator::Pointer   Pointer;
    typedef typename TIterator::Reference Reference;

    FORCEINLINE constexpr TDefaultIteratorFactory() noexcept : Data{nullptr}, Slack{nullptr} { }
    FORCEINLINE constexpr TDefaultIteratorFactory(const Pointer InData, const Pointer InEnd) noexcept : Data{InData}, Slack{InEnd} { }

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
    template <std::predicate<T> T>
    FORCEINLINE constexpr TDefaultFilteredIteratorFactory<
          T
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Filter(const T& Predicate) noexcept
    {
        return TDefaultFilteredIteratorFactory<
              T
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
    template <std::predicate<T> T>
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          T
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > ReversedFilter(const T& Predicate) noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              T
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            Predicate, this->Data, this->Slack
        };
    }

    Pointer Data;
    Pointer Slack;
};

template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedIteratorFactory
{
    static_assert(TIteratorConcept<TIterator>);
    static_assert(TIteratorConcept<TReversedIterator>);
    static_assert(TIteratorConcept<TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);
    static_assert(TIteratorConcept<TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);

    typedef TIterator Iterator;
    typedef TReversedIterator ReversedIterator;
    template <std::predicate<typename TIterator::T> TPredicate>
    using FilteredIterator = TFilteredIterator<TPredicate>;
    template <std::predicate<typename TIterator::T> TPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<TPredicate>;

    static_assert(std::is_same_v<typename TIterator::T, typename TReversedIterator::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedIterator::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedIterator::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);

    typedef typename TIterator::T         T;
    typedef typename TIterator::Pointer   Pointer;
    typedef typename TIterator::Reference Reference;

    FORCEINLINE constexpr TDefaultReversedIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedIteratorFactory(const Pointer InData, const Pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}
    {
        return;
    }

    FORCEINLINE constexpr ReversedIterator begin() noexcept { return ReversedIterator{ this->Slack }; }
    // We could just return ReversedIterator but because filtered iterators cannot do this, we
    // (to follow the same pattern) return Iterator. But it does not make any difference in the binary.
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Data }; }

    //#
    //# Applies a filter to the iterator, which will only return elements that match if the predicate is satisfied.
    //#
    template <std::predicate<T> TPredicate>
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

    Pointer Data;
    Pointer Slack;
};

template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultFilteredIteratorFactory
{
    static_assert(TIteratorConcept<TIterator>);
    static_assert(TIteratorConcept<TReversedIterator>);
    static_assert(TIteratorConcept<TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);
    static_assert(TIteratorConcept<TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);

    typedef TIterator Iterator;
    typedef TReversedIterator ReversedIterator;
    template <typename UPredicate>
    using FilteredIterator = TFilteredIterator<UPredicate>;
    template <typename UPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<UPredicate>;

    static_assert(std::is_same_v<typename TIterator::T, typename TReversedIterator::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedIterator::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedIterator::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);

    typedef TPredicate                    Predicate;
    typedef typename TIterator::T         T;
    typedef typename TIterator::Pointer   Pointer;
    typedef typename TIterator::Reference Reference;

    FORCEINLINE constexpr TDefaultFilteredIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultFilteredIteratorFactory(const Predicate& InFilter, const Pointer InData, const Pointer InEnd) noexcept
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

    Pointer Data;
    Pointer Slack;
    const Predicate& Filter;
};

template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedFilteredIteratorFactory
{
    static_assert(TIteratorConcept<TIterator>);
    static_assert(TIteratorConcept<TReversedIterator>);
    static_assert(TIteratorConcept<TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);
    static_assert(TIteratorConcept<TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>>);

    typedef TIterator Iterator;
    typedef TReversedIterator ReversedIterator;
    template <typename UPredicate>
    using FilteredIterator = TFilteredIterator<UPredicate>;
    template <typename UPredicate>
    using ReversedFilteredIterator = TReversedFilteredIterator<UPredicate>;

    static_assert(std::is_same_v<typename TIterator::T, typename TReversedIterator::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::T, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::T>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedIterator::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Pointer, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Pointer>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedIterator::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);
    static_assert(std::is_same_v<typename TIterator::Reference, typename TReversedFilteredIterator<decltype([](const typename TIterator::T& Element) -> bool { return true; })>::Reference>);

    typedef TPredicate                    Predicate;
    typedef typename TIterator::T         T;
    typedef typename TIterator::Pointer   Pointer;
    typedef typename TIterator::Reference Reference;

    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory() noexcept = delete;
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory(const Predicate& InFilter, const Pointer InData, const Pointer InEnd) noexcept
        : Data{InData}, Slack{InEnd}, Filter{InFilter}
    {
        return;
    }

    FORCEINLINE constexpr ReversedFilteredIterator<Predicate> begin() noexcept { return ReversedFilteredIterator<Predicate>{ this->Filter, this->Slack, this->Data }; }
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Data }; }

    Pointer Data;
    Pointer Slack;
    const Predicate& Filter;
};

///////////////////////////////////////////////////////////////////////////////
// Concepts.
///////////////////////////////////////////////////////////////////////////////

//#
//# The traits a trait of a weak allocator must implement to qualify as a weak array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptBase = requires
{
    //#
    //# The size type that the allocator is going to use for its internal memory management.
    //#
    typename T::SizeType;
    requires std::integral<typename T::SizeType>;

    //#
    //# The iterator type that the allocator is going to use.
    //# The iterator must be instantiable with const T. But may also be constructed with T
    //# or any other type U if applicable.
    //#
    //# We here just check if there is even an iterator that can be constructed with const LSzie.
    //# It is not the responsibility for traits to provide concrete data types.
    //#
    typename T::template Iterator<const LSize>;
};

//#
//# The traits a trait of a strong allocator must implement to qualify as a strong array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptStrong = TArrayBaseAllocatorTraitsConceptBase<T> && requires
{
    //#
    //# The growth function that returns the new size of the array given its current size.
    //#
    { T::GetGrowSize(std::declval<typename T::SizeType>()) } -> std::convertible_to<typename T::SizeType>;
};

//#
//# The traits a static allocator must implement to qualify as a static array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptWeak = TArrayBaseAllocatorTraitsConceptBase<T>;

template <typename T>
concept TArrayBaseCappedAllocatorTraitsConceptWeak = TArrayBaseAllocatorTraitsConceptBase<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

template <typename T>
concept TArrayBaseCappedAllocatorTraitsConceptStrong = TArrayBaseAllocatorTraitsConceptStrong<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator.
//#
template <typename T>
concept TArrayBaseAllocatorConceptBase = requires(T Allocator)
{
    //#
    //# The actual underlying allocator type.
    //#
    typename T::T;

    //#
    //# The allocator traits that the allocator uses for its internal memory management.
    //#
    typename T::Traits;
    requires TArrayBaseAllocatorTraitsConceptBase<typename T::Traits>;

    //# @see #TArrayBaseAllocatorTraitsConceptBase.
    typename T::SizeType;
    requires std::integral<typename T::SizeType>;

    //#
    //# The iterator type that the allocator is going to use.
    //#
    typename T::Iterator;
    typename T::ConstIterator;

    //#
    //# A fully qualified pointer type.
    //#
    typename T::Pointer;
    typename T::ConstPointer;

    //#
    //# A fully qualified reference type.
    //#
    typename T::Reference;
    typename T::ConstReference;

    //#
    //# For internal use only. Do not call.
    //#
    //# Resets the allocator to its default state. It must assume that relevant data was exported and heap
    //# allocated memory was successfully freed or re-owned.
    //# Therefore it has to correctly implement a function #IsCurrentDataOnHeap if special handling is required.
    //#
    { Allocator._ResetToDefaultState() } -> std::same_as<void>;

    //#
    //# Whether this array owns the memory it uses for its data.
    //#
    { Allocator.IsOwningMemoryAllocator() } -> std::convertible_to<bool>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator with mutable data.
//#
template <typename T>
concept TArrayBaseMutableAllocatorConceptBase = TArrayBaseAllocatorConceptBase<T> && requires(T Allocator)
{
    //#
    //# A pointer to the beginning of the allocated memory or nullptr if no memory is allocated.
    //#
    { +Allocator.Data } -> std::same_as<typename T::T*>;

    //#
    //# A pointer to the first element of the slack or nullptr if no slack is allocated.
    //#
    { +Allocator.Slack } -> std::same_as<typename T::T*>;

    //#
    //# A pointer to the first element after the slack or nullptr if no memory is allocated.
    //#
    { +Allocator.End } -> std::same_as<typename T::T*>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator with const data.
//#
template <typename T>
concept TArrayBaseConstAllocatorConceptBase = TArrayBaseAllocatorConceptBase<T> && requires(T Allocator)
{
    //#
    //# A pointer to the beginning of the allocated memory or nullptr if no memory is allocated.
    //#
    { +Allocator.Data } -> std::same_as<const typename T::T*>;

    //#
    //# A pointer to the first element of the slack or nullptr if no slack is allocated.
    //#
    { +Allocator.Slack } -> std::same_as<const typename T::T*>;

    //#
    //# A pointer to the first element after the slack or nullptr if no memory is allocated.
    //#
    { +Allocator.End } -> std::same_as<const typename T::T*>;
};

template <typename T>
concept TArrayBaseMutableAllocatorConceptStrong = TArrayBaseMutableAllocatorConceptBase<T> && requires(T Allocator)
{
    { Allocator.Grow() };
    { Allocator.GrowTo(std::declval<typename T::SizeType>()) };

    { Allocator.Resize(std::declval<typename T::SizeType>()) } -> std::same_as<typename T::SizeType>;

    { Allocator.ShrinkToFit() };
    { Allocator.ShrinkTo(std::declval<typename T::SizeType>()) };

    { Allocator.Orphan() };
};

template <typename T>
concept TArrayBaseConstAllocatorConceptWeak = TArrayBaseConstAllocatorConceptBase<T> &&
(
    requires(T Allocator)
    {
        { Allocator.Orphan() };
    }
    ||
    requires(T Allocator)
    {
        { Allocator.Empty() };
    }
);

template <typename T>
concept TArrayBaseMutableAllocatorConceptWeak = TArrayBaseMutableAllocatorConceptBase<T> &&
(
    requires(T Allocator)
    {
        { Allocator.Orphan() };
    }
    ||
    requires(T Allocator)
    {
        { Allocator.Empty() };
    }
);

template <typename T>
concept TArrayBaseMutableCappedAllocatorConceptWeak = TArrayBaseMutableAllocatorConceptWeak<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

template <typename T>
concept TArrayBaseMutableCapacityAllocatorConceptStrong = TArrayBaseMutableAllocatorConceptStrong<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

///////////////////////////////////////////////////////////////////////////////
// Implementations.
///////////////////////////////////////////////////////////////////////////////

template <std::integral TSizeType, template <typename> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsWeak
{
    typedef TSizeType SizeType;

    template <typename T>
    using Iterator = TIterator<T>;
};

template <std::integral TSizeType, template <typename> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsStrong : public TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    template <SizeType Current>
    NODISCARD
    FORCEINLINE consteval static SizeType GetGrowSizeStatic() noexcept
    {
        return Current < 5 ? 5 : Current < 20 ? 20 : Current * 2;
    }

    NODISCARD
    FORCEINLINE constexpr static SizeType GetGrowSize(const SizeType Current) noexcept
    {
        return Current < 5 ? 5 : Current < 20 ? 20 : Current * 2;
    }
};

template <std::integral TSizeType, template <typename> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitsWeakImpl : public TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    static constexpr SizeType SizeCapacity { static_cast<SizeType>(TSizeCapacity) };
};

template <std::integral TSizeType, template <typename> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitStrongImpl : public TArrayBaseAllocatorDefaultTraitsStrong<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsStrong<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    static constexpr SizeType SizeCapacity { static_cast<SizeType>(TSizeCapacity) };
};

namespace Private
{

//#
//# The default implementation to check if an allocator has its current data on the heap.
//#
template <TArrayBaseAllocatorTraitsConceptBase TAllocator>
    requires
    (
        requires(TAllocator Allocator)
        {
            { Allocator.IsCurrentDataOnHeap() } -> std::convertible_to<bool>;
        }
    )
FORCEINLINE constexpr bool IsCurrentDataOnHeapDefaultAllocatorImpl(const TAllocator& Allocator) noexcept
{
    return Allocator.IsCurrentDataOnHeap();
}

//#
//# The default implementation to check if an allocator has its current data on the heap but does not
//# itself specify any #IsCurrentDataOnHeap function.
//#
template <TArrayBaseAllocatorTraitsConceptBase TAllocator>
    requires
    (
        requires(TAllocator Allocator)
        {
            { !Allocator.IsCurrentDataOnHeap() } -> std::convertible_to<bool>;

            { Allocator.Data };
            requires std::is_pointer_v<decltype(Allocator.Data)>;
        }
    )
FORCEINLINE constexpr bool IsCurrentDataOnHeapDefaultAllocatorImpl(const TAllocator& Allocator) noexcept
{
    return Allocator.Data != nullptr;
}

} /* ~Namespace Private */

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorBase
{
    typedef TTraits Traits;
    typedef TIn     T;

    typedef typename Traits::SizeType SizeType;

    FORCEINLINE explicit constexpr TArrayBaseDefaultAllocatorBase() = default;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return true; }
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseMutableDefaultAllocatorBase : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<T>       Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;

    typedef T*       Pointer;
    typedef const T* ConstPointer;
    typedef T&       Reference;
    typedef const T& ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorBase() noexcept
        : Data{nullptr}, Slack{nullptr}, End{nullptr}
    {
        return;
    }

    //#
    //# Default implementation to reset this allocator to its default state.
    //# If you have special rules that need to be applied, you can override this function.
    //#
    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;

        return;
    }

    Pointer Data;
    Pointer Slack;
    Pointer End;
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseConstDefaultAllocatorBase : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<const T> Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;

    typedef const T*  Pointer;
    typedef Pointer   ConstPointer;
    typedef const T&  Reference;
    typedef Reference ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return false; }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorBase() noexcept
        : Data{nullptr}, Slack{nullptr}, End{nullptr}
    {
        return;
    }

    //#
    //# Default implementation to reset this allocator to its default state.
    //# If you have special rules that need to be applied, you can override this function.
    //#
    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;

        return;
    }

    Pointer Data;
    Pointer Slack;
    Pointer End;
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseMutableDefaultAllocatorStrongImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits   Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return true; }

    using TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorBase;

    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);
            this->Slack = this->Data;

            Pointer Cursor { Other.Data };
            while (Cursor != Other.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            this->End = this->Slack;
            LAL_CHECK_ARRAY( this->End == this->Data + Used )
        }
        else
        {
            this->Data = nullptr;
            this->Slack = nullptr;
            this->End = nullptr;
        }

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            this->Destruct();
            this->ResizeEmtpy(Used);

            LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

            Pointer Cursor { Other.Data };
            while (Cursor != Other.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;

        Other._ResetToDefaultState();

        return *this;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        this->OrphanImpl();

        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const UAllocator& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);
            this->Slack = this->Data;

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            this->End = this->Slack;
            LAL_CHECK_ARRAY( this->End == this->Data + Used )
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const UAllocator& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            this->Destruct();
            this->ResizeEmtpy(Used);

            LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(UAllocator&& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.End };
                    }
                );

                this->Data = Allocator.Data;
                this->Slack = Allocator.Slack;
                this->End = Allocator.End;
            }
            else
            {
                this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);
                this->Slack = this->Data + Used;
                this->End = this->Slack;

                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }

            Allocator._ResetToDefaultState();
            LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(UAllocator&& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                this->OrphanImpl();

                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.End };
                    }
                );

                this->Data = Allocator.Data;
                this->Slack = Allocator.Slack;
                this->End = Allocator.End;
            }
            else
            {
                this->Destruct();
                this->ResizeEmtpy(Used);

                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"

                this->Slack = this->Data + Used;
                LAL_CHECK_ARRAY( this->Slack <= this->End )
            }

            Allocator._ResetToDefaultState();
            LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        if (LAL_LIKELY(List.size() > 0))
        {
            this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(List.size());
            this->Slack = this->Data;

            for (const U& Element : List)
            {
                std::construct_at(this->Slack++, Element);
            }

            this->End = this->Slack;
            LAL_CHECK_ARRAY( this->End == this->Data + List.size() )
        }
        else
        {
            this->Data = nullptr;
            this->Slack = nullptr;
            this->End = nullptr;
        }

        return;
    }

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

        if (LAL_LIKELY(List.size() > 0))
        {
            this->Destruct();
            this->ResizeEmtpy(List.size());

            LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

            for (const U& Element : List)
            {
                std::construct_at(this->Slack++, Element);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE ~TArrayBaseMutableDefaultAllocatorStrongImpl() noexcept
    {
        this->OrphanImpl();
        return;
    }

    //#
    //# Growths the allocator to the next size specified by its traits.
    //#
    FORCEINLINE void Grow() noexcept
    {
        const SizeType NewCapacity { Traits::GetGrowSize(this->End - this->Data) };
        LAL_CHECK_ARRAY( NewCapacity > 0ull && NewCapacity >= static_cast<SizeType>(this->End - this->Data) )

        this->GrowImpl(NewCapacity);

        return;
    }

    //#
    //# Growths the allocator so that it can hold at least the given number of elements.
    //# This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE void GrowTo(const SizeType Count) noexcept
    {
        if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }

        return;
    }

    //#
    //# Resizes the allocator so that it can hold at least the given number of elements.
    //# This operation may cause a shrink under the hood.
    //# If the allocator is bigger than the given count, it will be shrunk to the number of elements it currently holds.
    //#
    //# @return The number of elements reserved by this allocator.
    //#
    FORCEINLINE SizeType Resize(const SizeType Count) noexcept
    {
        if
        (
            const SizeType Diff { static_cast<SizeType>(this->Slack - this->Data) };
            Count <= Diff
        )
        {
            this->ShrinkToImpl(Diff);
        }
        else if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }
        else
        {
            this->ShrinkToImpl(Count);
        }

        return this->End - this->Data;
    }

    //#
    //# Shrinks the allocator to current number of elements it currently holds.
    //#
    FORCEINLINE void ShrinkToFit() noexcept
    {
        this->ShrinkToImpl(static_cast<SizeType>(this->Slack - this->Data));
        return;
    }

    //#
    //# Shrinks the allocator to the given number of elements.
    //# @param Count The number of elements to shrink the allocator to. If this is less than the current number
    //#              of elements it holds, the allocator will be shrunk to the current number of elements it holds.
    //#
    FORCEINLINE void ShrinkTo(const SizeType Count) noexcept
    {
        if (Count <= static_cast<SizeType>(this->Slack - this->Data))
        {
            this->ShrinkToFit();
        }
        else if (Count < static_cast<SizeType>(this->End - this->Data))
        {
            this->ShrinkToImpl(Count);
        }

        return;
    }

    //#
    //# Destructs all elements and orphans the current allocator.
    //#
    FORCEINLINE void Orphan() noexcept
    {
        this->OrphanImpl();

        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;

        return;
    }

    FORCEINLINE void SwapBuffers(TArrayBaseMutableDefaultAllocatorStrongImpl* Other) noexcept
    {
        LAL_CHECK_ARRAY( this != Other )

        Pointer TempData  { this->Data };
        Pointer TempSlack { this->Slack };
        Pointer TempEnd   { this->End };

        this->Data  = Other->Data;
        this->Slack = Other->Slack;
        this->End   = Other->End;

        Other->Data  = TempData;
        Other->Slack = TempSlack;
        Other->End   = TempEnd;

        return;
    }

private:

    FORCEINLINE void GrowImpl(const SizeType Count) noexcept
    {
        if (this->Data)
        {
            this->AlignedRealloc(Count);
        }
        else
        {
            this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Count);
            this->Slack = this->Data;
            this->End = this->Data + Count;
        }

        return;
    }

    FORCEINLINE void ShrinkToImpl(const SizeType Count) noexcept
    {
        if (Count == 0)
        {
            this->Orphan();
        }
        else if (Count < static_cast<SizeType>(this->End - this->Data))
        {
            this->AlignedRealloc(Count);
        }
        else
        {
            LAL_CHECK_ARRAY( Count == static_cast<SizeType>(this->End - this->Data) )
        }

        return;
    }

    FORCEINLINE void ResizeEmtpy(const SizeType Count) noexcept
    {
        LAL_CHECK_ARRAY( this->Slack == this->Data )

        if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }
        else
        {
            this->ShrinkToImpl(Count);
        }

        return;
    }

    FORCEINLINE static Pointer AlignedAlloc(const SizeType Count) noexcept
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
#if LAL_CHECK_CONTAINER_BOUNDS
        Pointer Out { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
        jassert( Out )
        return Out;
#else /* LAL_CHECK_CONTAINER_BOUNDS */
        return static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T)));
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */
        #include "Definitions/PopDiagnostics.h"
    }

    FORCEINLINE void AlignedRealloc(const SizeType Count) noexcept
    {
        LAL_CHECK_ARRAY( Count > 0 && Count > static_cast<SizeType>(this->Slack - this->Data) )

        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"

        Pointer NewData { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
        LAL_CHECK_ARRAY( NewData )

        const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };
        std::memcpy(NewData, this->Data, Used * sizeof(T));

        std::free(this->Data);

        #include "Definitions/PopDiagnostics.h"

        this->Data = NewData;
        this->Slack = this->Data + Used;
        this->End = this->Data + Count;

        return;
    }

    FORCEINLINE void OrphanImpl() noexcept
    {
        this->Destruct();
        std::free(this->Data);

        return;
    }

    FORCEINLINE constexpr void Destruct() noexcept
    {
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            Bulk->~T();
        }

        this->Slack = this->Data;

        return;
    }
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseMutableDefaultAllocatorWeakImpl : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<T>       Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;

    typedef T*       Pointer;
    typedef const T* ConstPointer;
    typedef T&       Reference;
    typedef const T& ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return false; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept { static_assert(false); }

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl() noexcept
        : Data{nullptr}, Slack{nullptr}
    {
        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const TArrayBaseMutableDefaultAllocatorWeakImpl& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultAllocatorWeakImpl& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(TArrayBaseMutableDefaultAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultAllocatorWeakImpl&& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            std::is_same_v<typename UAllocator::Pointer, const typename UAllocator::T*> == false
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            std::is_same_v<typename UAllocator::Pointer, const typename UAllocator::T*> == false
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
    requires
    (
        requires
        {
            requires std::same_as<T, typename UAllocator::T>;
        }
        &&
        (UAllocator::IsOwningMemoryAllocator() == false)
        &&
        requires(UAllocator Allocator)
        {
            { Allocator.Data };
            { Allocator.Slack };
        }
    )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept = delete;

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept
    {
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
    {
        return;
    }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->_ResetToDefaultState();

        return;
    }

    Pointer Data;
    union
    {
        Pointer Slack;
        Pointer End;
    };
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseConstDefaultAllocatorWeakImpl : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<const T> Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;

    typedef const T*  Pointer;
    typedef Pointer   ConstPointer;
    typedef const T&  Reference;
    typedef Reference ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return false; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept { static_assert(false); }

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl() noexcept
        : Data{nullptr}, Slack{nullptr}
    {
        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const TArrayBaseConstDefaultAllocatorWeakImpl& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return;
    }
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(const TArrayBaseConstDefaultAllocatorWeakImpl& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(TArrayBaseConstDefaultAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return;
    }
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(TArrayBaseConstDefaultAllocatorWeakImpl&& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept = delete;

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept
    {
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
    {
        return;
    }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->_ResetToDefaultState();

        return;
    }

    Pointer Data;
    union
    {
        Pointer Slack;
        Pointer End;
    };
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    static constexpr SizeType SizeCapacity { TTraits::SizeCapacity };
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultFixedAllocatorWeakImpl : public TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    static_assert(_Super::SizeCapacity > 0);

    using TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl;

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            this->AllocateNoCheck();

            Pointer Cursor { Other.Data };
            while (Cursor != Other.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Slack <= this->End )
        }
        else
        {
            this->Data = nullptr;
            this->Slack = nullptr;
            this->End = nullptr;
        }

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            if (this->Data == nullptr)
            {
                this->Allocate();
            }
            else
            {
                this->Destruct();
            }

            LAL_CHECK_ARRAY( this->Data == this->Slack )

            Pointer Cursor { Other.Data };
            while (Cursor != Other.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;

        Other._ResetToDefaultState();

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        this->OrphanImpl();

        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            this->AllocateNoCheck();

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (this->Data == nullptr)
            {
                this->Allocate();
            }
            else
            {
                this->Destruct();
            }

            LAL_CHECK_ARRAY( this->Data == this->Slack )

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            /* Assume that the using developer is not a JavaScript coder... */
            if (LAL_UNLIKELY(Used > _Super::SizeCapacity))
            {
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                    "Array overflow: Tried to allocate more elements than the array can hold; Capacity [{}], Allocated [{}].",
                    _Super::SizeCapacity, Used
                    )
            }

            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.End };
                    }
                );

                this->Data = Allocator.Data;
                this->Slack = Allocator.Slack;
                this->End = Allocator.End;
            }
            else
            {
                this->AllocateNoCheck();

                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"

                this->Slack = this->Data + Used;
                LAL_CHECK_ARRAY( this->Slack <= this->End )
            }

            Allocator._ResetToDefaultState();
            LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (Used > _Super::SizeCapacity)
            {
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                    "Array overflow: Tried to allocate more elements than the array can hold; Capacity [{}], Allocated [{}].",
                    _Super::SizeCapacity, Used
                    )
            }

            if (this->Data == nullptr)
            {
                this->Allocate();
            }
            else
            {
                this->Destruct();
            }

            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"

            this->Slack = this->Data + Used;
            LAL_CHECK_ARRAY( this->Slack <= this->End )

            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.Orphan() };
                    }
                );

                Allocator.Orphan();
            }
            else
            {
                Allocator._ResetToDefaultState();
            }

            LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    template <typename U>
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        if (LAL_LIKELY(List.size() > 0))
        {
            this->AllocateNoCheck();

            for (const U& Element : List)
            {
                /* Assume that the using developer is not a vibe coder... */
                if (LAL_UNLIKELY(this->Slack >= this->End))
                {
                    const SizeType ListSize { static_cast<SizeType>(List.size()) };
                    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                        "Array overflow: Tried to add more elements than the array can hold; Capacity [{}], std::initializer_list [{}].",
                        _Super::SizeCapacity, ListSize
                        )
                }

                std::construct_at(this->Slack++, Element);

                continue;
            }
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

        if (LAL_LIKELY(List.size() > 0))
        {
            if (this->Data == nullptr)
            {
                this->Allocate();
            }
            else
            {
                this->Destruct();
            }

            for (const U& Element : List)
            {
                /* Assume that the using developer is not a vibe coder... */
                if (LAL_UNLIKELY(this->Slack >= this->End))
                {
                    const SizeType ListSize { static_cast<SizeType>(List.size()) };
                    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                        "Array overflow: Tried to add more elements than the array can hold; Capacity [{}], std::initializer_list [{}].",
                        _Super::SizeCapacity, ListSize
                        )

                    break;
                }

                std::construct_at(this->Slack++, Element);

                continue;
            }
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE ~TArrayBaseMutableDefaultFixedAllocatorWeakImpl() noexcept
    {
        this->OrphanImpl();
        return;
    }

    //#
    //# Tries to allocate the memory for the array if it is not allocated yet.
    //# This allocator uses an allocating on demand strategy, so it will not allocate memory if it is unnecessary,
    //# e.g. during default construction.
    //#
    FORCEINLINE void Grow() noexcept
    {
        /* Assume that the using developer is not a vibe coder... */
        if (LAL_LIKELY(this->Data == nullptr))
        {
            this->Allocate();
        }
        else
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Array overflow: Tried to grow a fixed array that is already allocated; Capacity [{}].", _Super::SizeCapacity)
        }

        return;
    }

    //#
    //# Destructs all elements and orphans the current allocator.
    //#
    FORCEINLINE void Orphan() noexcept
    {
        this->OrphanImpl();
        this->_ResetToDefaultState();

        return;
    }

private:

    FORCEINLINE void Allocate() noexcept
    {
        LAL_CHECK_ARRAY( this->Data == nullptr && this->Slack == nullptr && this->End == nullptr )
        this->AllocateNoCheck();

        return;
    }

    FORCEINLINE void AllocateNoCheck() noexcept
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        this->Data = static_cast<Pointer>(std::aligned_alloc(alignof(T), _Super::SizeCapacity * sizeof(T)));
        #include "Definitions/PopDiagnostics.h"

        this->Slack = this->Data;
        this->End = this->Data + _Super::SizeCapacity;

        return;
    }

    FORCEINLINE void OrphanImpl() noexcept
    {
        this->Destruct();
        std::free(this->Data);

        return;
    }

    FORCEINLINE constexpr void Destruct() noexcept
    {
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            Bulk->~T();
        }

        this->Slack = this->Data;

        return;
    }
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultStackAllocatorWeakImpl : public TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval bool IsCurrentDataOnHeap() const noexcept { return false; }

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = reinterpret_cast<T*>(this->_Data);
        this->Slack = this->Data;
        this->End = this->Data + _Super::SizeCapacity;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl() noexcept
        : _Data{}
    {
        this->_ResetToDefaultState();

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept
        : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
    {
        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        this->Destruct();

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept
        : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Other.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"

            this->Slack = this->Data + Used;
            LAL_CHECK_ARRAY( this->Slack <= this->End )

            Other.Slack = Other.Data;
            LAL_CHECK_ARRAY( static_cast<const void*>(&Other) == static_cast<const void*>(Other.Slack) )
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        this->Destruct();

        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Other.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"

            this->Slack = this->Data + Used;
            LAL_CHECK_ARRAY( this->Slack <= this->End )

            Other.Slack = Other.Data;
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        return *this;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(const UAllocator& Allocator) noexcept
        : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > _Super::SizeCapacity)
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
                )
        }

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        this->Destruct();

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > _Super::SizeCapacity)
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
                )
        }

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(UAllocator&& Allocator) noexcept
        : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > _Super::SizeCapacity)
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
                )
        }

        if (Used > 0)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"

            if constexpr (requires(UAllocator Allocator) { Allocator.Orphan(); })
            {
                Allocator.Orphan();
            }
            else
            {
                static_assert
                (
                    requires(UAllocator Allocator)
                    {
                        { Allocator._ResetToDefaultState() };
                    }
                );
                Allocator.Empty();
            }
        }

        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )

        return;
    }

    template <typename U>
    FORCEINLINE explicit TArrayBaseMutableDefaultStackAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
        : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
    {
        LAL_CHECK_ARRAY( this->Data != nullptr )

        for (const U& Element : List)
        {
            /* Assume that the using developer has a brain... */
            if (LAL_UNLIKELY(this->Slack >= this->End))
            {
                const SizeType ListSize { static_cast<SizeType>(List.size()) };
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                    "Array overflow: Tried to add more elements than the array can hold: Capacity [{}], std::initializer_list [{}].",
                    _Super::SizeCapacity, ListSize
                    )

                break;
            }

            std::construct_at(this->Slack++, Element);

            continue;
        }

        return;
    }

    template <typename U>
    FORCEINLINE TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

        this->Destruct();

        for (const U& Element : List)
        {
            /* Assume that the using developer has a brain... */
            if (LAL_UNLIKELY(this->Slack >= this->End))
            {
                const SizeType ListSize { static_cast<SizeType>(List.size()) };
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                    "Array overflow: Tried to add more elements than the array can hold: Capacity [{}], std::initializer_list [{}].",
                    _Super::SizeCapacity, ListSize
                    )

                break;
            }

            std::construct_at(this->Slack++, Element);

            continue;
        }

        return *this;
    }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->Destruct();
        return;
    }

    alignas(T)
    std::byte _Data[sizeof(T) * _Super::SizeCapacity];

private:

    FORCEINLINE constexpr void Destruct() noexcept
    {
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            Bulk->~T();
        }

        this->Slack = this->Data;

        return;
    }
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return true; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept { LAL_CHECK_ARRAY( this->Data != nullptr ) return this->Data != reinterpret_cast<const T*>(this->_Data); }

    static constexpr SizeType SizeCapacity { TTraits::SizeCapacity };
    static_assert(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity > 0);

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data  = reinterpret_cast<Pointer>(this->_Data);
        this->Slack = this->Data;
        this->End   = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl() noexcept
        : _Data{}
    {
        this->_ResetToDefaultState();

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Other.IsCurrentDataOnHeap())
        {
            LAL_CHECK_ARRAY( Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

            this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
            this->Slack = this->Data;
            this->End = this->Data + Used;
        }
        else
        {
            this->_ResetToDefaultState();
        }

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        if (Used > 0)
        {
            this->Destruct();
            this->ResizeEmtpy(Used);

            LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

            Pointer Cursor { Other.Data };
            while (Cursor != Other.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept
    {
        if (Other.IsCurrentDataOnHeap())
        {
            this->Data = Other.Data;
            this->Slack = Other.Slack;
            this->End = Other.End;
        }
        else
        {
            const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

            std::memcpy(this->_Data, Other._Data, Used * sizeof(T));

            this->Data = reinterpret_cast<Pointer>(this->_Data);
            this->Slack = this->Data + Used;
            this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
        }

        Other._ResetToDefaultState();

        return *this;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

        this->DestructImpl();
        if (this->IsCurrentDataOnHeap())
        {
            std::free(this->Data);
        }

        if (Other.IsCurrentDataOnHeap())
        {
            this->Data = Other.Data;
            this->Slack = Other.Slack;
            this->End = Other.End;
        }
        else
        {
            const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

            std::memcpy(this->_Data, Other._Data, Used * sizeof(T));

            this->Data = reinterpret_cast<Pointer>(this->_Data);
            this->Slack = this->Data + Used;
            this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
        }

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const UAllocator& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
            {
                this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
                this->Slack = this->Data;

                typename UAllocator::Pointer Cursor { Allocator.Data };
                while (Cursor != Allocator.Slack)
                {
                    std::construct_at(this->Slack++, *Cursor++);
                }

                this->End = this->Slack;
                LAL_CHECK_ARRAY( this->End == this->Data + Used )
            }
            else
            {
                this->_ResetToDefaultState();

                typename UAllocator::Pointer Cursor { Allocator.Data };
                while (Cursor != Allocator.Slack)
                {
                    std::construct_at(this->Slack++, *Cursor++);
                }
            }
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const UAllocator& Allocator) noexcept
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            this->Destruct();
            this->ResizeEmtpy(Used);

            LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
    requires
    (
        requires
        {
            typename UAllocator::T;
            typename UAllocator::Pointer;
            requires std::constructible_from<T, typename UAllocator::T>;
        }
        &&
        requires(UAllocator Allocator)
        {
            { Allocator.Data };
            { Allocator.Slack };
        }
    )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(UAllocator&& Allocator) noexcept
    {
        const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

        if (Used > 0)
        {
            if (Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
            {
                if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
                {
                    static_assert
                    (
                        /* Only required for heap-based allocators. */
                        requires(UAllocator Allocator)
                        {
                            { Allocator.End };
                        }
                    );

                    this->Data = Allocator.Data;
                    this->Slack = Allocator.Slack;
                    this->End = Allocator.End;
                }
                else
                {
                    this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
                    this->Slack = this->Data + Used;
                    this->End = this->Slack;

                    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                    std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                    #include "Definitions/PopDiagnostics.h"
                }

                Allocator._ResetToDefaultState();
            }
            else
            {
                this->_ResetToDefaultState();
                this->Slack = this->Data + Used;

                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"

                if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
                {
                    static_assert
                    (
                        /* Only required for heap-based allocators. */
                        requires(UAllocator Allocator)
                        {
                            { Allocator.Orphan() };
                        }
                    );

                    Allocator.Slack = Allocator.Data;
                    Allocator.Orphan();
                }
                else
                {
                    Allocator._ResetToDefaultState();
                }
            }

            LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
        }
        else
        {
            this->_ResetToDefaultState();
        }

        return;
    }

    template <typename U>
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        if (List.size() > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
        {
            this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(List.size());
            this->Slack = this->Data;

            for (const U& Element : List)
            {
                std::construct_at(this->Slack++, Element);
            }

            this->End = this->Slack;
            LAL_CHECK_ARRAY(this->End == this->Data + List.size())
        }
        else
        {
            this->_ResetToDefaultState();

            for (const U& Element : List)
            {
                std::construct_at(this->Slack++, Element);
            }

            LAL_CHECK_ARRAY( this->Slack <= this->End )
        }

        return;
    }

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>)
    {
        LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

        if (LAL_LIKELY(List.size() > 0))
        {
            if (List.size() > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
            {
                this->Destruct();
                this->Resize(List.size());
            }
            else
            {
                if (this->IsCurrentDataOnHeap())
                {
                    this->OrphanImpl();
                    this->_ResetToDefaultState();
                }
                else
                {
                    this->Destruct();
                    LAL_CHECK_ARRAY( this->Data == reinterpret_cast<Pointer>(this->_Data) )
                    LAL_CHECK_ARRAY( this->Slack == this->Data )
                    LAL_CHECK_ARRAY( this->End == this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )
                }
            }

            for (const U& Element : List)
            {
                std::construct_at(this->Slack++, Element);
            }

            LAL_CHECK_ARRAY( this->Slack <= this->End )
        }
        else
        {
            this->Orphan();
        }

        return *this;
    }

    FORCEINLINE ~TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl() noexcept
    {
        if (this->IsCurrentDataOnHeap())
        {
            this->OrphanImpl();
        }

        return;
    }

    //#
    //# Growths the allocator to the next size specified by its traits.
    //#
    FORCEINLINE void Grow() noexcept
    {
        const SizeType NewCapacity { Traits::GetGrowSize(this->End - this->Data) };
        LAL_CHECK_ARRAY( NewCapacity > 0ull && NewCapacity >= static_cast<SizeType>(this->End - this->Data) )

        if (this->IsCurrentDataOnHeap())
        {
            this->AlignedRealloc(NewCapacity);
        }
        else
        {
            const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

            this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(NewCapacity);
            this->Slack = this->Data + Used;
            this->End = this->Data + NewCapacity;

            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, this->_Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }

        LAL_CHECK_ARRAY( this->Data )

        return;
    }

    //#
    //# Growths the allocator so that it can hold at least the given number of elements.
    //# This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE void GrowTo(const SizeType Count) noexcept
    {
        if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }

        return;
    }

    //#
    //# Resizes the allocator so that it can hold at least the given number of elements.
    //# This operation may cause a shrink under the hood.
    //# If the allocator is bigger than the given count, it will be shrunk to the number of elements it currently holds.
    //#
    //# @return The number of elements reserved by this allocator.
    //#
    FORCEINLINE SizeType Resize(const SizeType Count) noexcept
    {
        if
        (
            const SizeType Diff { static_cast<SizeType>(this->Slack - this->Data) };
            Count <= Diff
        )
        {
            this->ShrinkToImpl(Diff);
        }
        else if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }
        else
        {
            this->ShrinkToImpl(Count);
        }

        return this->End - this->Data;
    }

    //#
    //# Shrinks the allocator to current number of elements it currently holds.
    //#
    FORCEINLINE void ShrinkToFit() noexcept
    {
        this->ShrinkToImpl(static_cast<SizeType>(this->Slack - this->Data));
        return;
    }

    //#
    //# Shrinks the allocator to the given number of elements.
    //# @param Count The number of elements to shrink the allocator to. If this is less than the current number
    //#              of elements it holds, the allocator will be shrunk to the current number of elements it holds.
    //#
    FORCEINLINE void ShrinkTo(const SizeType Count) noexcept
    {
        if (Count <= static_cast<SizeType>(this->Slack - this->Data))
        {
            this->ShrinkToFit();
        }
        else if (Count < static_cast<SizeType>(this->End - this->Data))
        {
            this->ShrinkToImpl(Count);
        }

        return;
    }

    //#
    //# Destructs all elements and orphans the current allocated memory if it is on the heap.
    //#
    FORCEINLINE void Orphan() noexcept
    {
        if (this->IsCurrentDataOnHeap())
        {
            this->OrphanImpl();

            this->Data = reinterpret_cast<Pointer>(this->_Data);
            this->Slack = this->Data;
            this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
        }
        else
        {
            this->Destruct();
            LAL_CHECK_ARRAY( this->Data == reinterpret_cast<Pointer>(this->_Data) )
            LAL_CHECK_ARRAY( this->Slack == this->Data )
            LAL_CHECK_ARRAY( this->End == this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )
        }

        return;
    }

    alignas(T)
    std::byte _Data[sizeof(T) * TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity];

private:

    FORCEINLINE void GrowImpl(const SizeType Count) noexcept
    {
        if (this->IsCurrentDataOnHeap())
        {
            this->AlignedRealloc(Count);
        }
        else
        {
            LAL_CHECK_ARRAY( Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

            const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

            this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Count);
            this->Slack = this->Data + Used;
            this->End = this->Data + Count;

            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, this->_Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }

        return;
    }

    FORCEINLINE void ShrinkToImpl(const SizeType Count) noexcept
    {
        LAL_CHECK_ARRAY( Count >= static_cast<SizeType>(this->Slack - this->Data) )

        if (this->IsCurrentDataOnHeap() == false)
        {
            return;
        }

        if (Count == 0)
        {
            this->OrphanImpl();
            this->_ResetToDefaultState();
        }
        else if (Count < static_cast<SizeType>(this->End - this->Data))
        {
            if (Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
            {
                this->AlignedRealloc(Count);
            }
            else
            {
                const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->_Data, this->Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"

                std::free(this->Data);

                this->Data = reinterpret_cast<Pointer>(this->_Data);
                this->Slack = this->Data + Used;
                this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
            }
        }
        else
        {
            LAL_CHECK_ARRAY( Count == static_cast<SizeType>(this->End - this->Data) )
        }

        return;
    }

    FORCEINLINE void ResizeEmtpy(const SizeType Count) noexcept
    {
        LAL_CHECK_ARRAY( this->Slack == this->Data )

        if (Count > static_cast<SizeType>(this->End - this->Data))
        {
            this->GrowImpl(Count);
        }
        else
        {
            this->ShrinkToImpl(Count);
        }

        return;
    }

    FORCEINLINE static Pointer AlignedAlloc(const SizeType Count) noexcept
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
#if LAL_CHECK_CONTAINER_BOUNDS
        Pointer Out { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
        jassert( Out )
        return Out;
#else /* LAL_CHECK_CONTAINER_BOUNDS */
        return static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T)));
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */
        #include "Definitions/PopDiagnostics.h"
    }

    FORCEINLINE void AlignedRealloc(const SizeType Count) noexcept
    {
        LAL_CHECK_ARRAY( Count > 0 && Count > static_cast<SizeType>(this->End - this->Data) )
        LAL_CHECK_ARRAY( Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        Pointer NewData { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
        #include "Definitions/PopDiagnostics.h"
        LAL_CHECK_ARRAY( NewData )

        const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(NewData, this->Data, Used * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        LAL_CHECK_ARRAY( this->IsCurrentDataOnHeap() )
        std::free(this->Data);

        this->Data = NewData;
        this->Slack = this->Data + Used;
        this->End = this->Data + Count;

        return;
    }

    FORCEINLINE void OrphanImpl() noexcept
    {
        LAL_CHECK_ARRAY( this->IsCurrentDataOnHeap() )

        this->Destruct();
        std::free(this->Data);

        return;
    }

    FORCEINLINE constexpr void Destruct() noexcept
    {
        this->DestructImpl();
        this->Slack = this->Data;

        return;
    }

    FORCEINLINE constexpr void DestructImpl() noexcept
    {
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            Bulk->~T();
        }

        return;
    }
};

//#
//# An array container that may use any form of (un-)owned stack or heap allocated memory.
//#
//# This container checks OOB accesses and all inputs for correctness during debug and development builds
//# but will not check these in release builds.
//#
//# This array may not handle complex types that require move semantics as it was designed to be
//# a fast and simple container for extreme fast-paced memory read and write operations.
//#
//# What copy and move assignment semantics are called?
//# - Copy ctors / assignments from any variable: Always.
//# - Copy ctors / assignments from any other array: Always.
//# - Move assignments from any statement: Always
//# - Move ctors from another x- and pr-valued statement: Always.
//# - Move ctors from another x- and pr-valued array: Sometimes, depending on the allocator and their state.
//# - Move ctors during buffer reallocation: Never.
//# - Move ctors during reallocation inside the parent array: Mostly never.
//#
//# Of course, this only applies to the default allocators that Lal provides. You may define your own allocators
//# that follow rules differently.
//#
//# @note dtors will always be correctly called.
//#
template <TArrayBaseAllocatorConceptBase TAllocator>
class TArrayBase
{
public:

    typedef TAllocator                         Allocator;
    typedef typename Allocator::Traits         Traits;
    typedef typename Allocator::T              T;
    typedef typename Allocator::SizeType       SizeType;
    typedef typename Allocator::Iterator       Iterator;
    typedef typename Allocator::ConstIterator  ConstIterator;
    typedef typename Allocator::Pointer        Pointer;
    typedef typename Allocator::ConstPointer   ConstPointer;
    typedef typename Allocator::Reference      Reference;
    typedef typename Allocator::ConstReference ConstReference;

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArrayBase<TMemberField>* MemberField);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    friend class TArrayBase;

    NODISCARD
    FORCEINLINE consteval static bool IsStronglyAllocated() noexcept requires(requires { Allocator::IsStrong; }) { return Allocator::IsStrong(); }
    NODISCARD
    FORCEINLINE consteval static bool IsStronglyAllocated() noexcept requires(!requires { Allocator::IsStrong; }) { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsWeaklyAllocated() noexcept requires(requires { Allocator::IsWeak; }) { return Allocator::IsWeak(); }
    NODISCARD
    FORCEINLINE consteval static bool IsWeaklyAllocated() noexcept requires(!requires { Allocator::IsWeak; }) { return false; }
    static_assert(IsStronglyAllocated() != IsWeaklyAllocated());

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept requires(requires { Allocator::IsContentConst; }) { return Allocator::IsContentConst(); }
    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept requires(!requires { Allocator::IsContentConst; }) { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept requires(requires { Allocator::IsContentMutable; }) { return Allocator::IsContentMutable(); }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept requires(!requires { Allocator::IsContentMutable; }) { return false; }
    static_assert(IsContentConst() != IsContentMutable());

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept requires(requires { Allocator::IsAllowedToPushItems; }) { return Allocator::IsAllowedToPushItems(); }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept requires(!requires { Allocator::IsAllowedToPushItems; }) { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPopItemsInBetween() noexcept requires(requires { Allocator::IsAllowedToPopItemsInBetween; }) { return Allocator::IsAllowedToPopItemsInBetween(); }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPopItemsInBetween() noexcept requires(!requires { Allocator::IsAllowedToPopItemsInBetween; }) { return IsAllowedToPushItems(); }

    FORCEINLINE constexpr TArrayBase() noexcept = default;

    FORCEINLINE constexpr TArrayBase(const TArrayBase& Other) noexcept
        requires(std::is_constructible_v<Allocator, const Allocator&>);
    FORCEINLINE constexpr TArrayBase& operator=(const TArrayBase& Other) noexcept
        requires(std::assignable_from<Allocator&, const Allocator&>);

    FORCEINLINE constexpr TArrayBase(TArrayBase&& Other) noexcept
        requires(std::is_constructible_v<Allocator, Allocator&&>);
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase&& Other) noexcept
        requires(std::assignable_from<Allocator&, Allocator&&>);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(const TArrayBase<UAllocator>& Other) noexcept
        requires(std::is_constructible_v<Allocator, const UAllocator&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(const TArrayBase<UAllocator>& Other) noexcept
        requires(std::assignable_from<Allocator&, const UAllocator&>);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::assignable_from<Allocator&, UAllocator&&>);

    //#
    //# Delete any r-value move conversion constructors and assignment operators that would otherwise implicitly
    //# convert to l-value references of the allocator type, which may cause double implicit conversions of the
    //# underlying allocator type. Notably, moves stack pointers to the heap or double owned heap pointers from
    //# extraordinary weak allocators.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&> == false) = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase<UAllocator>&& Other) noexcept
       requires(std::assignable_from<Allocator&, UAllocator&&> == false) = delete;

    //# Non templated ctor for recursive implicit bracket initialization.
    FORCEINLINE constexpr TArrayBase(const std::initializer_list<T> List) noexcept
        requires(std::is_constructible_v<Allocator, std::initializer_list<T>>);
    template <typename U>
    FORCEINLINE constexpr TArrayBase(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<Allocator, std::initializer_list<U>>);
    template <typename U>
    FORCEINLINE constexpr TArrayBase& operator=(const std::initializer_list<U> List) noexcept
        requires(std::assignable_from<Allocator&, std::initializer_list<U>>);

    FORCEINLINE constexpr ~TArrayBase() noexcept = default;

    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { return this->Impl.Slack - this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr bool     IsEmpty() const noexcept { return this->Impl.Data == this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr SizeType GetSizeInBytes() const noexcept { return this->GetSize() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE constexpr SizeType GetCapacity() const noexcept { return this->Impl.End - this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr SizeType GetAllocatedByteSize() const noexcept { return this->GetCapacity() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE constexpr bool IsDataValid() const noexcept { return this->Impl.Data != nullptr; }
    NODISCARD FORCEINLINE constexpr bool IsSlackValid() const noexcept { return this->Impl.Slack != nullptr; }
    NODISCARD FORCEINLINE constexpr bool IsCapacityValid() const noexcept { return this->Impl.End != nullptr; }
    NODISCARD FORCEINLINE constexpr bool HasReachedCapacity() const noexcept { return this->Impl.Slack == this->Impl.End; }
    NODISCARD FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept requires(requires(Allocator _Allocator) { _Allocator.IsCurrentDataOnHeap(); }) { return this->Impl.IsCurrentDataOnHeap(); }
    NODISCARD FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept requires(!requires(Allocator _Allocator) { _Allocator.IsCurrentDataOnHeap(); }) { return this->GetDataPointer() != nullptr; }

    NODISCARD FORCEINLINE constexpr ConstPointer GetDataPointer() const noexcept { return this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr Pointer      GetDataPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetSlackPointer() const noexcept { return this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr Pointer      GetSlackPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetCapacityPointer() const noexcept { return this->Impl.End; }
    NODISCARD FORCEINLINE constexpr Pointer      GetCapacityPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.End; }

    NODISCARD FORCEINLINE constexpr bool IsValidPointer(const ConstPointer Ptr)  const noexcept { return Ptr >= this->GetDataPointer() && Ptr < this->GetSlackPointer(); }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsValidIterator(const TIterator It) const noexcept { return It.Cursor >= this->GetDataPointer() && It.Cursor < this->GetSlackPointer(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires( std::is_signed_v<SizeType>) { return Index >= 0 && Index < this->GetSize(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires(!std::is_signed_v<SizeType>) { return Index < this->GetSize(); }

    NODISCARD FORCEINLINE constexpr bool IsPointerInCapacityRange(const ConstPointer Ptr) const noexcept { return Ptr >= this->GetDataPointer() && Ptr < this->GetCapacityPointer(); }
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr bool IsIteratorInCapacityRange(const TIterator It) const noexcept { return It.Cursor >= this->GetDataPointer() && It.Cursor < this->GetCapacityPointer(); }

    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr typename TIterator::Reference operator[](const TIterator It) noexcept requires(TArrayBase::IsContentMutable());
    template <TIteratorConcept TIterator>
    NODISCARD FORCEINLINE constexpr typename TIterator::Reference operator[](const TIterator It) const noexcept;
    NODISCARD FORCEINLINE constexpr Reference operator[](const SizeType Index) noexcept requires(TArrayBase::IsContentMutable());
    NODISCARD FORCEINLINE constexpr const T& operator[](const SizeType Index) const noexcept;

    FORCEINLINE constexpr Iterator begin() noexcept requires(TArrayBase::IsContentMutable()) { return Iterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr Iterator end() noexcept requires(TArrayBase::IsContentMutable()) { return Iterator{ this->Impl.Slack }; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return ConstIterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr ConstIterator end()const noexcept { return ConstIterator{ this->Impl.Slack }; }
    FORCEINLINE constexpr ConstIterator cbegin() const noexcept { return ConstIterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr ConstIterator cend() const noexcept { return ConstIterator{ this->Impl.Slack }; }

    FORCEINLINE constexpr SizeType end_idx() const noexcept requires(TArrayBase::IsContentMutable()) { return this->GetSize(); }

    FORCEINLINE constexpr auto Iter() noexcept requires(requires { typename Iterator::Factory; });
    FORCEINLINE constexpr auto CIter() const noexcept requires(requires { typename ConstIterator::Factory; });

    //# @return First element or nullptr.
    NODISCARD FORCEINLINE constexpr Pointer GetFirst()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetSize() > 0 ? this->GetDataPointer() : nullptr; }
    NODISCARD FORCEINLINE constexpr Pointer GetFirstChecked()  noexcept requires(TArrayBase::IsContentMutable()) { check( this->GetSize() > 0 ); return this->GetDataPointer(); }
    NODISCARD FORCEINLINE constexpr Pointer GetFirstAsserted() noexcept requires(TArrayBase::IsContentMutable()) { jassert( this->GetSize() > 0 ); return this->GetDataPointer(); }
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirst()         const noexcept { return this->GetSize() > 0 ? this->GetDataPointer() : nullptr; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirstChecked()  const noexcept { check( this->GetSize() > 0 ) return this->GetDataPointer(); }
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirstAsserted() const noexcept { jassert( this->GetSize() > 0 ) return this->GetDataPointer(); }

    //# @return Last element or nullptr.
    NODISCARD FORCEINLINE constexpr Pointer GetLast()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetSize() > 0 ? this->GetSlackPointer() - 1 : nullptr; }
    NODISCARD FORCEINLINE constexpr Pointer GetLastChecked()  noexcept requires(TArrayBase::IsContentMutable()) { check( this->GetSize() > 0 ); return this->GetSlackPointer() - 1; }
    NODISCARD FORCEINLINE constexpr Pointer GetLastAsserted() noexcept requires(TArrayBase::IsContentMutable()) { jassert( this->GetSize() > 0 ); return this->GetSlackPointer() - 1; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetLast()         const noexcept { return this->GetSize() > 0 ? this->GetSlackPointer() - 1 : nullptr; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetLastChecked()  const noexcept { check( this->GetSize() > 0 ) return this->GetSlackPointer() - 1; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetLastAsserted() const noexcept { jassert( this->GetSize() > 0 ) return this->GetSlackPointer() - 1; }

    //#
    //# Growths the array so that it can hold at least the given number of elements.
    //# This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE constexpr void Reserve(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Will clear out all elements in the array and set the size to zero.
    //# Then it will try to grow or shrink the array to the given number of elements.
    //#
    FORCEINLINE constexpr void Reset(const SizeType Count, const bool bAllowShrink = true) noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Resizes the array so that it can hold at least the given number of elements.
    //# This operation may cause a shrink under the hood.
    //# If the array is bigger than the given count, it will be shrunk to the number of elements it currently holds.
    //#
    //# @return The number of elements reserved by this array.
    //#
    FORCEINLINE constexpr SizeType Resize(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Shrinks the array to current number of elements it currently holds.
    //#
    FORCEINLINE constexpr void ShrinkToFit() noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Completely empties the array, sets its size to zero, and if possible, orphans the underlying memory.
    //#
    FORCEINLINE constexpr void Empty() noexcept;

    //#
    //# Swaps the underlying buffers of this array with the other array. If the buffers are stack allocated, then this
    //# operation may cause a cross-copy of the data.
    //#
    FORCEINLINE void SwapBuffers(TArrayBase* Other) noexcept requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); });

    //#
    //# Swap two indices in the array.
    //#
    FORCEINLINE void SwapIndices(const Iterator InA, const Iterator InB) noexcept requires(TAllocator::IsContentMutable());
    FORCEINLINE void SwapIndices(const SizeType InA, const SizeType InB) noexcept requires(TAllocator::IsContentMutable());

    //#
    //# Do not use std operators as...
    //#    - they are ambiguous in terms of meaning (compare by size, value or reference?).
    //#    - we might accidentally do comparisons inside templated paths of arrays that are very expensive in terms of
    //#      runtime performance.
    //# Better be explicit about it with the named functions #EqualInSizeTo, #IsSameArray and #IsDataEqual.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE bool operator==(const TArrayBase<UAllocator>& Other) const noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE bool operator!=(const TArrayBase<UAllocator>& Other) const noexcept = delete;

    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TArrayBase<UAllocator>& Other) const noexcept { return this->GetSize() <=> Other.GetSize(); }

    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool EqualInSizeTo(const TArrayBase<UAllocator>& Other) const noexcept { return this->GetSize() == Other.GetSize(); }

    //#
    //# Checks if both instances point to the same memory location.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool IsSameArray(const TArrayBase<UAllocator>& Other) const noexcept { return this->GetDataPointer() != nullptr && this->GetDataPointer() == Other.GetDataPointer(); }

    //#
    //# Checks if both instances have the same meaningful data. The capacity is not checked.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD FORCEINLINE constexpr bool IsDataEqual(const TArrayBase<UAllocator>& Other) const noexcept;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD FORCEINLINE constexpr bool IsDataUnequal(const TArrayBase<UAllocator>& Other) const noexcept { return this->IsDataEqual(Other) == false; }

    //# Pushes the specified element to the array.
    constexpr void Add(const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void Add(T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    // Pushes the default constructed element to the array.
    constexpr void AddDefaulted() noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<T>);
    constexpr void AddDefaulted(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<T>);

    //# Pushes an element to the array that is zeroed out.
    constexpr void AddZeroed() noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void AddZeroed(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# Pushes an uninitialized element to the array. The memory's content is undefined and must be initialized by hand afterward.
    constexpr void AddUninitialized() noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void AddUninitialized(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# @return The iterator of the newly added element.
    FORCEINLINE constexpr Iterator AddAt(const ConstIterator It, const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr Iterator AddAt(const SizeType Index, const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# @return The iterator of the newly added element.
    FORCEINLINE constexpr Iterator AddAt(const ConstIterator It, T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr Iterator AddAt(const SizeType Index, T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //#
    //# Adds a new element to the array and constructs it in place while potentially reallocating the whole
    //# array to fit.
    //#
    template <typename... TArgs>
    void Emplace(TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Adds a new element to the array and constructs it in place while potentially reallocating the whole
    //# array to fit.
    //# @return The iterator of the newly added element.
    //#
    template <typename... TArgs>
    FORCEINLINE Iterator EmplaceAt(const ConstIterator It, TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    Iterator EmplaceAt(const SizeType Index, TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Appends new elements to the array while potentially reallocating the whole array to fit.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    void Append(const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    void Append(TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <typename U>
    void Append(const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept TIterator>
    void Append(TIterator Begin, const TIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);
    template <TIteratorConcept TIterator>
    FORCEINLINE void Append(const TIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);

    //#
    //# Inserts new elements to the array at the specified location, while potentially reallocating the whole
    //# array to fit.
    //# @return The iterator to the first newly inserted element. If the range is empty, the iterator will point
    //#         to the slack.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    Iterator AppendAt(const ConstIterator It, const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    Iterator AppendAt(const ConstIterator It, TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <typename U>
    Iterator AppendAt(const ConstIterator It, const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept TIterator>
    Iterator AppendAt(const ConstIterator It, const TIterator Begin, const TIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);
    template <TIteratorConcept TIterator>
    FORCEINLINE Iterator AppendAt(const ConstIterator It, const TIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    Iterator AppendAt(SizeType Index, const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    Iterator AppendAt(SizeType Index, TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>);
    template <typename U>
    Iterator AppendAt(SizeType Index, const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept TIterator>
    Iterator AppendAt(SizeType Index, TIterator Begin, const TIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);
    template <TIteratorConcept TIterator>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const TIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>);

    template <TIteratorConcept TIterator>
    FORCEINLINE void RemoveAt(const TIterator It) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    //# Inclusive Begin and exclusive End.
    template <TIteratorConcept TIterator>
    FORCEINLINE void RemoveAt(const TIterator Begin, const TIterator End) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    FORCEINLINE void RemoveAt(const SizeType Index) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    //# Inclusive Begin and exclusive End.
    FORCEINLINE void RemoveAt(const SizeType Begin, const SizeType End) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    //# @return The number of elements removed.
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType Remove(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U>
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween() && Lal::TEqualityComparableLeft<T, U>) { const SizeType Count { this->Remove(What) }; check( Count > 0 ) return Count; }
    template <typename U> requires(TArrayBase::IsAllowedToPopItemsInBetween() && Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const U& What) noexcept { const SizeType Count { this->Remove(What) }; jassert( Count > 0 ) return Count; }

    template <std::predicate<T> TPredicate>
    SizeType RemoveByPredicate(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType RemoveByPredicateAtLeastOnceChecked(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType RemoveByPredicateAtLeastOnceAsserted(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    //#
    //# Removes the first occurrence of the specified element from the array.
    //# @return True if the element was found and removed, false otherwise.
    //#
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    bool RemoveOnce(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool RemoveOnceChecked(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween()) { const bool bRemoved { this->RemoveOnce(What) }; check( bRemoved ) return bRemoved; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool RemoveOnceAsserted(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween()) { const bool bRemoved { this->RemoveOnce(What) }; jassert( bRemoved ) return bRemoved; }

    template <std::predicate<T> TPredicate>
    bool RemoveOnceByPredicate(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool RemoveOnceByPredicateChecked(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool RemoveOnceByPredicateAsserted(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    Iterator Find(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    ConstIterator Find(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindChecked(const U& What) noexcept { const Iterator It { this->Find(What) }; check( It != this->end() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindChecked(const U& What) const noexcept { const ConstIterator It { this->Find(What) }; check( It != this->cend() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindAsserted(const U& What) noexcept { const Iterator It { this->Find(What) }; jassert( It != this->end() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindAsserted(const U& What) const noexcept { const ConstIterator It { this->Find(What) }; jassert( It != this->cend() ) return It; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndex(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndex(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexChecked(const U& What) noexcept { const SizeType Index { this->FindIndex(What) }; check( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexChecked(const U& What) const noexcept { const SizeType Index { this->FindIndex(What) }; check( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexAsserted(const U& What) noexcept { const SizeType Index { this->FindIndex(What) }; jassert( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexAsserted(const U& What) const noexcept { const SizeType Index { this->FindIndex(What) }; jassert( Index != this->GetSize() ) return Index; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRef(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRef(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRefChecked(const U& What) noexcept { const Pointer Ptr { this->FindRef(What) }; check( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRefChecked(const U& What) const noexcept { const ConstPointer Ptr { this->FindRef(What) }; check( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRefAsserted(const U& What) noexcept { const Pointer Ptr { this->FindRef(What) }; jassert( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRefAsserted(const U& What) const noexcept { const ConstPointer Ptr { this->FindRef(What) }; jassert( Ptr ) return Ptr; }

    template <std::predicate<T> TPredicate>
    Iterator FindByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    ConstIterator FindByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindByPredicateChecked(const TPredicate& Predicate) noexcept { const Iterator It { this->FindByPredicate(Predicate) }; check( It != this->end() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicate(Predicate) }; check( It != this->cend() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindByPredicateAsserted(const TPredicate& Predicate) noexcept { const Iterator It { this->FindByPredicate(Predicate) }; jassert( It != this->end() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicate(Predicate) }; jassert( It != this->cend() ) return It; }

    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateChecked(const TPredicate& Predicate) noexcept { const SizeType Index { this->FindIndexByPredicate(Predicate) }; check( Index != this->end() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateChecked(const TPredicate& Predicate) const noexcept { const SizeType Index { this->FindIndexByPredicate(Predicate) }; check( Index != this->cend() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateAsserted(const TPredicate& Predicate) noexcept { const SizeType Index { this->FindIndexByPredicate(Predicate) }; jassert( Index != this->end() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept { const SizeType Index { this->FindIndexByPredicate(Predicate) }; jassert( Index != this->cend() ) return Index; }

    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicateChecked(const TPredicate& Predicate) noexcept { const Pointer Ptr { this->FindRefByPredicate(Predicate) }; check( Ptr) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstPointer Ptr { this->FindRefByPredicate(Predicate) }; check( Ptr ) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicateAsserted(const TPredicate& Predicate) noexcept { const Pointer Ptr { this->FindRefByPredicate(Predicate) }; jassert( Ptr ) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstPointer Ptr { this->FindRefByPredicate(Predicate) }; jassert( Ptr ) return Ptr; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    Iterator FindLast(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    ConstIterator FindLast(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindLastChecked(const U& What) noexcept { const Iterator It { this->FindLast(What) }; check( It != this->end() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindLastChecked(const U& What) const noexcept { const ConstIterator It { this->FindLast(What) }; check( It != this->cend() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindLastAsserted(const U& What) noexcept { const Iterator It { this->FindLast(What) }; jassert( It != this->end() ) return It; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindLastAsserted(const U& What) const noexcept { const ConstIterator It { this->FindLast(What) }; jassert( It != this->cend() ) return It; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndex(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndex(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexChecked(const U& What) noexcept { const SizeType Index { this->FindLastIndex(What) }; check( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexChecked(const U& What) const noexcept { const SizeType Index { this->FindLastIndex(What) }; check( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexAsserted(const U& What) noexcept { const SizeType Index { this->FindLastIndex(What) }; jassert( Index != this->GetSize() ) return Index; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexAsserted(const U& What) const noexcept { const SizeType Index { this->FindLastIndex(What) }; jassert( Index != this->GetSize() ) return Index; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRef(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRef(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRefChecked(const U& What) noexcept { const Pointer Ptr { this->FindLastRef(What) }; check( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRefChecked(const U& What) const noexcept { const ConstPointer Ptr { this->FindLastRef(What) }; check( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRefAsserted(const U& What) noexcept { const Pointer Ptr { this->FindLastRef(What) }; jassert( Ptr ) return Ptr; }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRefAsserted(const U& What) const noexcept { const ConstPointer Ptr { this->FindLastRef(What) }; jassert( Ptr ) return Ptr; }

    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicateChecked(const TPredicate& Predicate) noexcept { const Iterator It { this->FindLastByPredicate(Predicate) }; check( It != this->end() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindLastByPredicate(Predicate) }; check( It != this->cend() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicateAsserted(const TPredicate& Predicate) noexcept { const Iterator It { this->FindLastByPredicate(Predicate) }; jassert( It != this->end() ) return It; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindLastByPredicate(Predicate) }; jassert( It != this->cend() ) return It; }

    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateChecked(const TPredicate& Predicate) noexcept { const SizeType Index { this->FindLastIndexByPredicate(Predicate) }; check( Index != this->end() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateChecked(const TPredicate& Predicate) const noexcept { const SizeType Index { this->FindLastIndexByPredicate(Predicate) }; check( Index != this->cend() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateAsserted(const TPredicate& Predicate) noexcept { const SizeType Index { this->FindLastIndexByPredicate(Predicate) }; jassert( Index != this->end() ) return Index; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept { const SizeType Index { this->FindLastIndexByPredicate(Predicate) }; jassert( Index != this->cend() ) return Index; }

    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicateChecked(const TPredicate& Predicate) noexcept { const Pointer Ptr { this->FindLastRefByPredicate(Predicate) }; check( Ptr ) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstPointer Ptr { this->FindLastRefByPredicate(Predicate) }; check( Ptr ) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicateAsserted(const TPredicate& Predicate) noexcept { const Pointer Ptr { this->FindLastRefByPredicate(Predicate) }; jassert( Ptr ) return Ptr; }
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstPointer Ptr { this->FindLastRefByPredicate(Predicate) }; jassert( Ptr ) return Ptr; }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool Contains(const U& What) const noexcept { return this->Find(What) != this->cend(); }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool ContainsChecked(const U& What) const noexcept { const ConstIterator It { this->FindChecked(What) }; return It != this->cend(); }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool ContainsAsserted(const U& What) const noexcept { const ConstIterator It { this->FindAsserted(What) }; return It != this->cend(); }

    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicate(const TPredicate& Predicate) const noexcept { return this->FindByPredicate(Predicate) != this->cend(); }
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicateChecked(Predicate) }; return It != this->cend(); }
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicateAsserted(Predicate) }; return It != this->cend(); }

    template <typename TPredicate> requires(std::invocable<TPredicate, T> && std::is_void_v<std::invoke_result_t<TPredicate, T>>)
    FORCEINLINE void ForEach(const TPredicate& Predicate) noexcept requires(TArrayBase::IsContentMutable());
    template <typename TPredicate> requires(std::invocable<TPredicate, T> && std::is_void_v<std::invoke_result_t<TPredicate, T>>)
    FORCEINLINE void ForEach(const TPredicate& Predicate) const noexcept;

    //# @return The number of elements replaced.
    template <typename U, typename V> requires(Lal::TEqualityComparableLeft<T, U> && std::assignable_from<T, V>)
    FORCEINLINE SizeType Replace(const U& What, const V& Replacement) noexcept requires(TArrayBase::IsContentMutable());
    template <std::predicate<T> TPredicate, typename V> requires(std::assignable_from<T, V>)
    FORCEINLINE SizeType ReplaceByPredicate(const TPredicate& Predicate, const V& Replacement) noexcept requires(TArrayBase::IsContentMutable());

    ///////////////////////////////////////////////////////////////////////////////
    // Stack Operations

    //# Peeks at the last element in the array. Returns nullptr if the array is empty.
    NODISCARD FORCEINLINE constexpr Pointer Peek()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLast(); }
    NODISCARD FORCEINLINE constexpr Pointer PeekChecked()  noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLastChecked(); }
    NODISCARD FORCEINLINE constexpr Pointer PeekAsserted() noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLastAsserted(); }
    NODISCARD FORCEINLINE constexpr ConstPointer Peek()         const noexcept { return this->GetLast(); }
    NODISCARD FORCEINLINE constexpr ConstPointer PeekChecked()  const noexcept { return this->GetLastChecked(); }
    NODISCARD FORCEINLINE constexpr ConstPointer PeekAsserted() const noexcept { return this->GetLastAsserted(); }

    template <typename... TArgs>
    FORCEINLINE TArrayBase& Push(TArgs&&... Args) & noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    FORCEINLINE TArrayBase&& Push(TArgs&&... Args) && noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Removes the last element from the array.
    //# @return True if an element was popped, false if the array was empty.
    //#
    FORCEINLINE bool Pop() noexcept;
    //# Pops #Count elements from the end of the array.
    FORCEINLINE void Pop(SizeType Count) noexcept;

    // ~Stack Operations
    ///////////////////////////////////////////////////////////////////////////////

    NODISCARD
    FORCEINLINE const Allocator& GetAllocator() const noexcept { return this->Impl; }
    NODISCARD
    FORCEINLINE Allocator& GetMutableAllocator() noexcept { return this->Impl; }

private:

    //#
    //# We could use the TArrayBase::IsStronglyAllocated required clause here, but we want to give weakly allocated
    //# arrays also the chance to grow if they hunt a creation on demand allocation strategy.
    //#
    FORCEINLINE constexpr void GrowImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Grow(); })
    {
        this->Impl.Grow();
        return;
    }
    FORCEINLINE constexpr void GrowImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Grow(); })
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
    }

    FORCEINLINE constexpr void GrowToImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.GrowTo(Count);
        return;
    }
    FORCEINLINE constexpr void GrowToImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated())
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
    }

    FORCEINLINE constexpr SizeType ResizeImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        return this->Impl.Resize(Count);
    }
    FORCEINLINE constexpr SizeType ResizeImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated())
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
    }

    FORCEINLINE constexpr void ShrinkToFitImpl() noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.ShrinkToFit();
        return;
    }
    FORCEINLINE constexpr void ShrinkToFitImpl() noexcept requires(!TArrayBase::IsStronglyAllocated())
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
    }

    FORCEINLINE constexpr void ShrinkToImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.ShrinkTo(Count);
        return;
    }
    FORCEINLINE constexpr void ShrinkToImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated())
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
    }

    FORCEINLINE constexpr void OrphanImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Orphan();
        return;
    }
    FORCEINLINE constexpr void OrphanImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on non orphanable array.")
    }

    FORCEINLINE constexpr void EmptyImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Orphan();
        return;
    }
    FORCEINLINE constexpr void EmptyImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Empty();
        return;
    }

    FORCEINLINE constexpr void DestroyAt(const Iterator It) noexcept
        requires(requires(Allocator _Allocator) { _Allocator.DestroyAt(It.Cursor); })
    {
        this->Impl.DestroyAt(It.Cursor);
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const Iterator It) noexcept
        requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(It.Cursor); } && TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
        requires(requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); })
    {
        this->Impl.DestroyAt(Index);
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
        requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); } && TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr void Destruct() noexcept requires(TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated());

    Allocator Impl;
};

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const TArrayBase& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const TAllocator&>)
    : Impl{Other.Impl}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const TArrayBase& Other) noexcept
    requires(std::assignable_from<TAllocator&, const TAllocator&>)
{
    this->Impl = Other.Impl;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(TArrayBase&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, TAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(TArrayBase&& Other) noexcept
    requires(std::assignable_from<TAllocator&, TAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const TArrayBase<UAllocator>& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const UAllocator&>)
    : Impl{Other.Impl}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const TArrayBase<UAllocator>& Other) noexcept
    requires(std::assignable_from<TAllocator&, const UAllocator&>)
{
    this->Impl = Other.Impl;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, UAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(TArrayBase<UAllocator>&& Other) noexcept
    requires(std::assignable_from<TAllocator&, UAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const std::initializer_list<T> List) noexcept
    requires(std::is_constructible_v<TAllocator, std::initializer_list<typename TAllocator::T>>)
    : Impl{List}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<TAllocator, std::initializer_list<U>>)
    : Impl{List}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::assignable_from<TAllocator&, std::initializer_list<U>>)
{
    this->Impl = List;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
FORCEINLINE constexpr typename TIterator::Reference TArrayBase<TAllocator>::operator[](const TIterator It) noexcept
    requires(TArrayBase::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIterator(It) )
    return *It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
FORCEINLINE constexpr typename TIterator::Reference TArrayBase<TAllocator>::operator[](const TIterator It) const noexcept
{
    LAL_CHECK_ARRAY( this->IsValidIterator(It) )
    return *It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Reference TArrayBase<TAllocator>::operator[](const SizeType Index) noexcept
    requires(TArrayBase::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )
    return this->Impl.Data[Index];
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr const typename TArrayBase<TAllocator>::T& TArrayBase<TAllocator>::operator[](const SizeType Index) const noexcept
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )
    return this->Impl.Data[Index];
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr auto TArrayBase<TAllocator>::Iter() noexcept
    requires(requires { typename Iterator::Factory; })
{
    return typename Iterator::Factory{ this->Impl.Data, this->Impl.Slack };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr auto TArrayBase<TAllocator>::CIter() const noexcept
    requires(requires { typename ConstIterator::Factory; })
{
    return typename ConstIterator::Factory{ this->Impl.Data, this->Impl.Slack };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Reserve(const SizeType Count) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->GrowToImpl(Count);
    LAL_CHECK_ARRAY( Count == 0 || (this->Impl.Data && this->Impl.Slack <= this->Impl.End) )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Reset(const SizeType Count, const bool bAllowShrink) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->Destruct();

    if (LAL_LIKELY(bAllowShrink))
    {
        this->Resize(Count);
    }
    else
    {
        this->Reserve(Count);
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Resize(const SizeType Count) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    return this->ResizeImpl(Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::ShrinkToFit() noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->ShrinkToFitImpl();
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Empty() noexcept
{
    this->EmptyImpl();
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::SwapBuffers(TArrayBase* Other) noexcept
    requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); })
{
    check( Other )

    this->Impl.SwapBuffers(&Other->Impl);
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::SwapIndices(const Iterator InA, const Iterator InB) noexcept
    requires(TAllocator::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIterator(InA) && this->IsValidIterator(InB) )

    /* Assume the correct input. */
    if (LAL_UNLIKELY(InA == InB))
    {
        return;
    }

    alignas(T)
    u8 Temp[sizeof(T)];

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memcpy(Temp,       InA.Cursor, sizeof(T));
    ::memcpy(InA.Cursor, InB.Cursor, sizeof(T));
    ::memcpy(InB.Cursor, Temp,       sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::SwapIndices(const SizeType InA, const SizeType InB) noexcept
    requires(TAllocator::IsContentMutable())
{
    this->SwapIndices(this->begin() + InA, this->begin() + InB);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr bool TArrayBase<TAllocator>::IsDataEqual(const TArrayBase<UAllocator>& Other) const noexcept
{
    if (this->GetSize() != Other.GetSize())
    {
        return false;
    }

    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk != Other[Bulk - this->GetDataPointer()])
        {
            return false;
        }

        continue;
    }

    return true;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::Add(const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, Element);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::Add(T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, std::move(Element));

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddDefaulted() noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<typename TAllocator::T>)
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddDefaulted(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<typename TAllocator::T>)
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    for (SizeType Index { 0 }; Index < Count; ++Index)
    {
        std::construct_at(this->Impl.Slack++);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddZeroed() noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memset(this->Impl.Slack++, 0, sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddZeroed(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memset(this->Impl.Slack, 0, Count * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += Count;
    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddUninitialized() noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    ++this->Impl.Slack;

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr void TArrayBase<TAllocator>::AddUninitialized(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    this->Impl.Slack += Count;
    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const ConstIterator It, const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    return this->AddAt(It.Cursor - this->Impl.Data, Element);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const SizeType Index, const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (Index == this->GetSize())
    {
        this->Add(Element);
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    std::construct_at(this->Impl.Data + Index, Element);

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const ConstIterator It, T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    return this->AddAt(It.Cursor - this->Impl.Data, std::move(Element));
}

template <TArrayBaseAllocatorConceptBase TAllocator>
constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const SizeType Index, T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (Index == this->GetSize())
    {
        this->Add(std::move(Element));
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    std::construct_at(this->Impl.Data + Index, std::move(Element));

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
void TArrayBase<TAllocator>::Emplace(TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, std::forward<TArgs>(Args)...);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::EmplaceAt(const ConstIterator It, TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    return this->EmplaceAt(It.Cursor - this->Impl.Data, std::forward<TArgs>(Args)...);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::EmplaceAt(const SizeType Index, TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    if (Index == this->GetSize())
    {
        this->Emplace(std::forward<TArgs>(Args)...);
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    std::construct_at(this->Impl.Data + Index, std::forward<TArgs>(Args)...);

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
void TArrayBase<TAllocator>::Append(const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    this->Reserve(this->GetSize() + Other.GetSize());

    LAL_CHECK_ARRAY( this->Impl.Data )

    for (const typename UAllocator::T& Element : Other)
    {
        std::construct_at(this->Impl.Slack++, Element);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
void TArrayBase<TAllocator>::Append(TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    if (Other.GetSize() == 0)
    {
        return;
    }

    this->Reserve(this->GetSize() + Other.GetSize());

    LAL_CHECK_ARRAY( this->Impl.Data )

    if constexpr (std::is_same_v<T, typename UAllocator::T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(this->Impl.Slack, Other.Impl.Data, Other.GetSize() * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
        this->Impl.Slack += Other.GetSize();

        Other.GetMutableAllocator().Slack = Other.GetMutableAllocator().Data;
    }
    else
    {
        for (typename UAllocator::T& Element : Other)
        {
            std::construct_at(this->Impl.Slack++, std::move(Element));
        }
    }

    Other.Empty();

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
void TArrayBase<TAllocator>::Append(const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    this->Reserve(this->GetSize() + List.size());

    for (const U& Element : List)
    {
        std::construct_at(this->Impl.Slack++, Element);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
void TArrayBase<TAllocator>::Append(TIterator Begin, const TIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    LAL_CHECK_ARRAY( Begin.Cursor <= End.Cursor )
    LAL_CHECK_ARRAY( (this->IsIteratorInCapacityRange(Begin) || this->IsIteratorInCapacityRange(End)) == false )

    this->Reserve(this->GetSize() + (End - Begin));

    while (Begin != End)
    {
        std::construct_at(this->Impl.Slack++, *Begin++);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
FORCEINLINE void TArrayBase<TAllocator>::Append(const TIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    this->Append(Begin, Begin + Count);
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const ConstIterator It, const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    return this->AppendAt(It.Cursor - this->Impl.Data, Other);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const ConstIterator It, TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    return this->AppendAt(It.Cursor - this->Impl.Data, std::move(Other));
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const ConstIterator It, const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    return this->AppendAt(It.Cursor - this->Impl.Data, List);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const ConstIterator It, const TIterator Begin, const TIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    return this->AppendAt(It.Cursor - this->Impl.Data, Begin, End);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const ConstIterator It, const TIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    return this->AppendAt(It, Begin, Begin + Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(SizeType Index, const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    if (Other.GetSize() == 0)
    {
        return this->end();
    }

    if (Index == this->GetSize())
    {
        this->Append(Other);
        return this->end() - Other.GetSize();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->Reserve(this->GetSize() + Other.GetSize());

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + Other.GetSize(), this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += Other.GetSize();

    for (const typename UAllocator::T& Element : Other)
    {
        std::construct_at(this->Impl.Data + Index++, Element);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return Iterator{ this->Impl.Data + Index - Other.GetSize() };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(SizeType Index, TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T>)
{
    const SizeType OtherSize { Other.GetSize() };

    if (OtherSize == 0)
    {
        return this->end();
    }

    if (Index == this->GetSize())
    {
        this->Append(std::move(Other));
        return this->end() - OtherSize;
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->Reserve(this->GetSize() + OtherSize);

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + OtherSize, this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += OtherSize;

    if constexpr (std::is_same_v<T, typename UAllocator::T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(this->Impl.Data + Index, Other.Impl.Data, OtherSize * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        Other.GetMutableAllocator().Slack = Other.GetMutableAllocator().Data;
    }
    else
    {
        for (typename UAllocator::T& Element : Other)
        {
            std::construct_at(this->Impl.Data + Index++, std::move(Element));
        }
    }

    Other.Empty();

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    Iterator Out { this->Impl.Data + Index - OtherSize };
    return Out;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(SizeType Index, const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    if (Index == this->GetSize())
    {
        this->Append(List);
        return this->end() - List.size();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->Reserve(this->GetSize() + List.size());

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + List.size(), this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += List.size();

    for (const U& Element : List)
    {
        std::construct_at(this->Impl.Data + Index++, Element);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return Iterator{ this->Impl.Data + Index - List.size() };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(SizeType Index, TIterator Begin, const TIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    const SizeType Count { End - Begin };

    if (Count == 0)
    {
        return this->end();
    }

    if (Index == this->GetSize())
    {
        this->Append(Begin, End);
        return this->end() - Count;
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->Reserve(this->GetSize() + Count);

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    ::memmove(this->Impl.Data + Index + Count, this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += Count;

    while (Begin != End)
    {
        std::construct_at(this->Impl.Data + Index++, *Begin++);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return Iterator{ this->Impl.Data + Index - Count };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const SizeType Index, const TIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIterator::Reference>)
{
    return this->AppendAt(Index, Begin, Begin + Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
void TArrayBase<TAllocator>::RemoveAt(const TIterator It) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    this->RemoveAt(It.Cursor - this->Impl.Data);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept TIterator>
void TArrayBase<TAllocator>::RemoveAt(const TIterator Begin, const TIterator End) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    this->RemoveAt(Begin.Cursor - this->Impl.Data, End.Cursor - this->Impl.Data);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
void TArrayBase<TAllocator>::RemoveAt(const SizeType Index) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->DestroyAt(Index);

    if (Index < this->GetSize() - 1)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        ::memmove(this->Impl.Data + Index, this->Impl.Data + Index + 1, (this->GetSize() - Index - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }

    --this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
void TArrayBase<TAllocator>::RemoveAt(const SizeType Begin, const SizeType End) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Begin) && Begin <= End && (this->IsValidIndex(End - 1) || Begin == End ) )

    if (Begin == End)
    {
        return;
    }

    for (SizeType Index { Begin }; Index < End; ++Index)
    {
        this->DestroyAt(Index);
    }

    if (End <= this->GetSize())
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        ::memmove(this->Impl.Data + Begin, this->Impl.Data + End, (this->GetSize() - (End - 1)) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }

    this->Impl.Slack -= End - Begin;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Remove(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    SizeType Removed { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer();)
    {
        if (*Bulk == What)
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            ++Removed;
        }
        else
        {
            ++Bulk;
        }

        continue;
    }

    return Removed;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicate(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    SizeType Removed { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer();)
    {
        if (Predicate(*Bulk))
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            ++Removed;
        }
        else
        {
            ++Bulk;
        }

        continue;
    }

    return Removed;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicateAtLeastOnceChecked(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->RemoveByPredicate<TPredicate>(Predicate) };
    check( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicateAtLeastOnceAsserted(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->RemoveByPredicate<TPredicate>(Predicate) };
    jassert( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
bool TArrayBase<TAllocator>::RemoveOnce(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            return true;
        }

        continue;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
bool TArrayBase<TAllocator>::RemoveOnceByPredicate(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            return true;
        }

        continue;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
bool TArrayBase<TAllocator>::RemoveOnceByPredicateChecked(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnceByPredicate<TPredicate>(Predicate) };
    check( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
bool TArrayBase<TAllocator>::RemoveOnceByPredicateAsserted(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnceByPredicate<TPredicate>(Predicate) };
    jassert( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::Find(const U& What) noexcept
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::Find(const U& What) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndex(const U& What) noexcept
{
    if (Iterator It { this->Find(What) }; It != this->end())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndex(const U& What) const noexcept
{
    if (ConstIterator It { this->Find(What) }; It != this->cend())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRef(const U& What) noexcept
{
    if (Iterator It { this->Find(What) }; It != this->end())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRef(const U& What) const noexcept
{
    if (ConstIterator It { this->Find(What) }; It != this->cend())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindByPredicate(const TPredicate& Predicate) noexcept
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindByPredicate(const TPredicate& Predicate) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindByPredicate(Predicate) }; It != this->end())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindByPredicate(Predicate) }; It != this->cend())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindByPredicate(Predicate) }; It != this->end())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindByPredicate(Predicate) }; It != this->cend())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLast(const U& What) noexcept
{
    for (T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (*Bulk == What)
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLast(const U& What) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (*Bulk == What)
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndex(const U& What) noexcept
{
    if (Iterator It { this->FindLast(What) }; It != this->end())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndex(const U& What) const noexcept
{
    if (ConstIterator It { this->FindLast(What) }; It != this->cend())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRef(const U& What) noexcept
{
    if (Iterator It { this->FindLast(What) }; It != this->end())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRef(const U& What) const noexcept
{
    if (ConstIterator It { this->FindLast(What) }; It != this->cend())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastByPredicate(const TPredicate& Predicate) noexcept
{
    for (T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (Predicate(*Bulk))
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastByPredicate(const TPredicate& Predicate) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (Predicate(*Bulk))
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindLastByPredicate(Predicate) }; It != this->end())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindLastByPredicate(Predicate) }; It != this->cend())
    {
        return It.Cursor - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindLastByPredicate(Predicate) }; It != this->end())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindLastByPredicate(Predicate) }; It != this->cend())
    {
        return It.Cursor;
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename TPredicate> requires(std::invocable<TPredicate, typename TAllocator::T> && std::is_void_v<std::invoke_result_t<TPredicate, typename TAllocator::T>>)
FORCEINLINE void TArrayBase<TAllocator>::ForEach(const TPredicate& Predicate) noexcept requires(TArrayBase::IsContentMutable())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Predicate(*Bulk);

        continue;
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename TPredicate> requires(std::invocable<TPredicate, typename TAllocator::T> && std::is_void_v<std::invoke_result_t<TPredicate, typename TAllocator::T>>)
FORCEINLINE void TArrayBase<TAllocator>::ForEach(const TPredicate& Predicate) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Predicate(*Bulk);

        continue;
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U, typename V> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U> && std::assignable_from<typename TAllocator::T, V>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Replace(const U& What, const V& Replacement) noexcept
    requires(TArrayBase::IsContentMutable())
{
    SizeType Replaced { 0 };

    this->ForEach([&Replaced, &What, &Replacement](T& Element)
    {
        if (Element == What)
        {
            Element = Replacement;
            ++Replaced;
        }

        return;
    });

    return Replaced;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate, typename V> requires(std::assignable_from<typename TAllocator::T, V>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::ReplaceByPredicate(const TPredicate& Predicate, const V& Replacement) noexcept
    requires(TArrayBase::IsContentMutable())
{
    SizeType Replaced { 0 };

    this->ForEach([&Replaced, &Predicate, &Replacement](T& Element)
    {
        if (Predicate(Element))
        {
            Element = Replacement;
            ++Replaced;
        }

        return;
    });

    return Replaced;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename ... TArgs>
FORCEINLINE TArrayBase<TAllocator>& TArrayBase<TAllocator>::Push(TArgs&&... Args) & noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->Emplace(std::forward<TArgs>(Args)...);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename ... TArgs>
FORCEINLINE TArrayBase<TAllocator>&& TArrayBase<TAllocator>::Push(TArgs&&... Args) && noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->Emplace(std::forward<TArgs>(Args)...);
    return std::move(*this);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE bool TArrayBase<TAllocator>::Pop() noexcept
{
    if (this->GetSize() > 0)
    {
        this->DestroyAt(this->GetSize() - 1);
        --this->Impl.Slack;

        LAL_CHECK_ARRAY( this->Impl.Slack >= this->Impl.Data && this->Impl.Slack <= this->Impl.End )

        return true;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::Pop(SizeType Count) noexcept
{
    while (this->GetSize() > 0 && Count > 0)
    {
        this->DestroyAt(this->Impl.Slack - 1);
        --this->Impl.Slack;
        --Count;

        continue;
    }

    LAL_CHECK_ARRAY( this->Impl.Slack >= this->Impl.Data && this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::DestroyAt(const Iterator It) noexcept
    requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(It.Cursor); } && TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated())
{
    LAL_CHECK_ARRAY( this->IsValidIterator(It) )

    It.Cursor->~T();

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::DestroyAt(const SizeType Index) noexcept
    requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); } && TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    (this->GetDataPointer() + Index)->~T();

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Destruct() noexcept
    requires(TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Bulk->~T();

        continue;
    }

    this->Impl.Slack = this->Impl.Data;

    return;
}

typedef TArrayBaseAllocatorDefaultTraitsStrong<LSize, Lal::TDefaultIterator> LArrayBaseAllocatorDefaultTraitsStrong;
typedef TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator> LArrayBaseAllocatorDefaultTraitsWeak;
template <LSize TSizeCapacity>
using TArrayBaseCappedAllocatorDefaultTraitsWeak
    = TArrayBaseCappedAllocatorDefaultTraitsWeakImpl<LSize, Lal::TDefaultIterator, TSizeCapacity>;
template <LSize TSizeCapacity>
using TArrayBaseCappedAllocatorDefaultTraitsStrong
    = TArrayBaseCappedAllocatorDefaultTraitStrongImpl<LSize, Lal::TDefaultIterator, TSizeCapacity>;
static_assert(TArrayBaseAllocatorTraitsConceptStrong<LArrayBaseAllocatorDefaultTraitsStrong>);
static_assert(TArrayBaseAllocatorTraitsConceptWeak<LArrayBaseAllocatorDefaultTraitsWeak>);
static_assert(TArrayBaseCappedAllocatorTraitsConceptWeak<TArrayBaseCappedAllocatorDefaultTraitsWeak<64>>);
static_assert(TArrayBaseCappedAllocatorTraitsConceptStrong<TArrayBaseCappedAllocatorDefaultTraitsStrong<64>>);

template <typename T>
using TArrayBaseMutableDefaultAllocatorStrong
    = TArrayBaseMutableDefaultAllocatorStrongImpl<T, LArrayBaseAllocatorDefaultTraitsStrong>;
template <typename T>
using TArrayBaseConstDefaultAllocatorWeak
    = TArrayBaseConstDefaultAllocatorWeakImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;
template <typename T>
using TArrayBaseMutableDefaultAllocatorWeak
    = TArrayBaseMutableDefaultAllocatorWeakImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;
template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TSizeCapacity>
using TArrayBaseMutableDefaultFixedAllocatorWeak
    = TArrayBaseMutableDefaultFixedAllocatorWeakImpl<T, TArrayBaseCappedAllocatorDefaultTraitsWeak<TSizeCapacity>>;
template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TSizeCapacity>
using TArrayBaseMutableDefaultStackAllocatorWeak
    = TArrayBaseMutableDefaultStackAllocatorWeakImpl<T, TArrayBaseCappedAllocatorDefaultTraitsWeak<TSizeCapacity>>;
template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TStackSizeCapacity>
using TArrayBaseMutableDefaultStackOptimizedAllocatorStrong
    = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<T, TArrayBaseCappedAllocatorDefaultTraitsStrong<TStackSizeCapacity>>;

static_assert(TArrayBaseMutableAllocatorConceptStrong<TArrayBaseMutableDefaultAllocatorStrong<LPlatformTypes::LSize>>);
static_assert(TArrayBaseConstAllocatorConceptWeak<TArrayBaseConstDefaultAllocatorWeak<LPlatformTypes::LSize>>);
static_assert(TArrayBaseMutableAllocatorConceptWeak<TArrayBaseMutableDefaultAllocatorWeak<LPlatformTypes::LSize>>);
static_assert(TArrayBaseMutableCappedAllocatorConceptWeak<TArrayBaseMutableDefaultFixedAllocatorWeak<LPlatformTypes::LSize, 64>>);
static_assert(TArrayBaseMutableCappedAllocatorConceptWeak<TArrayBaseMutableDefaultStackAllocatorWeak<LPlatformTypes::LSize, 64>>);
static_assert(TArrayBaseMutableCapacityAllocatorConceptStrong<TArrayBaseMutableDefaultStackOptimizedAllocatorStrong<LPlatformTypes::LSize, 64>>);

} /* ~Namespace Lal */

#undef LAL_CHECK_ARRAY

//#
//# An array that lives on the heap and own the memory it uses.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultAllocatorStrong<T>>;

//#
//# An array that has a view on some memory of any other array.
//# It may resize itself, but this will not affect the original array.
//# It does not own the memory it uses and cannot modify it.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TArrayView = Lal::TArrayBase<Lal::TArrayBaseConstDefaultAllocatorWeak<T>>;

//#
//# An array that has a view on some memory of any other array.
//# It may resize itself, but this will not affect the original array.
//# It does not own the memory it uses but can modify it.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TMutableArrayView = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultAllocatorWeak<T>>;

//#
//# An array that pre-allocates a specific number of elements and lives on the heap.
//# This array does not grow or shrink, it is fixed in its maximal size.
//#
//# @tparam T             The type of the elements in this array.
//# @tparam TSizeCapacity The size of the heap buffer in number of elements. This is the maximal size this array
//#                       can ever hold.
//#
template <typename T, LSize TSizeCapacity>
using TFixedArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultFixedAllocatorWeak<T, TSizeCapacity>>;

//#
//# An array that pre-allocates a specific number of elements and lives on the stack.
//# It does therefore not allocate any memory in the heap.
//# This array does not grow or shrink, it is fixed in its maximal size.
//#
//# @tparam T             The type of the elements in this array.
//# @tparam TSizeCapacity The size of the stack buffer in number of elements. This is the maximal size this array
//#                       can ever hold.
//#
template <typename T, LSize TSizeCapacity>
using TStackArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultStackAllocatorWeak<T, TSizeCapacity>>;

//#
//# An array that has an internal buffer on the stack and uses the heap for larger arrays.
//# If a small array is used it will use the stack buffer, if it gets larger it will automatically switch to the heap;
//# and if it gets smaller again, it will switch back to the stack buffer and orphan its heap memory.
//#
//# @tparam T                  The type of the elements in this array.
//# @tparam TStackSizeCapacity The size of the stack buffer in number of elements.
//#
template <typename T, Lal::TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TStackSizeCapacity>
using TStackOptimizedArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultStackOptimizedAllocatorStrong<T, TStackSizeCapacity>>;
