// Copyright mzoesch. All rights reserved.

#pragma once

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
    template <typename TPredicate>
    using _TDefaultFilteredIterator = TDefaultFilteredIterator<TPredicate, TIn>;
    template <typename TPredicate>
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
    template <typename TPredicate>
    using FilteredIterator = TFilteredIterator<TPredicate>;
    template <typename TPredicate>
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
    template <std::predicate<T> Predicate>
    FORCEINLINE constexpr TDefaultFilteredIteratorFactory<
          Predicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Filter(const Predicate& InPredicate) noexcept
    {
        return TDefaultFilteredIteratorFactory<
              Predicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            InPredicate, this->Data, this->Slack
        };
    }

    //#
    //# Reverses and applies a filter to the iterator, which will only return elements that match if the predicate is satisfied.
    //#
    template <std::predicate<T> Predicate>
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          Predicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > ReversedFilter(const Predicate& InPredicate) noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              Predicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            InPredicate, this->Data, this->Slack
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
    template <typename TPredicate>
    using FilteredIterator = TFilteredIterator<TPredicate>;
    template <typename TPredicate>
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
    template <std::predicate<T> Predicate>
    FORCEINLINE constexpr TDefaultReversedFilteredIteratorFactory<
          Predicate
        , Iterator
        , ReversedIterator
        , FilteredIterator
        , ReversedFilteredIterator
        > Filter(const Predicate& InPredicate) noexcept
    {
        return TDefaultReversedFilteredIteratorFactory<
              Predicate
            , Iterator
            , ReversedIterator
            , FilteredIterator
            , ReversedFilteredIterator
            >
        {
            InPredicate, this->Data, this->Slack
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

} /* Namespace Lal */
