// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultIterator<TIn, bMove>::difference_type
TDefaultIterator<TIn, bMove>::operator-(const TDefaultIterator& Other) const noexcept
{
    check( this->Cursor >= Other.Cursor && "Underflow" )
    return this->Cursor - Other.Cursor;
}

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultIterator<TIn, bMove>::difference_type
TDefaultIterator<TIn, bMove>::operator-(const TDefaultIterator<std::remove_const_t<value_type>, bMove>& Other) const noexcept
    requires(std::is_const_v<value_type>)
{
    check( this->Cursor >= Other.Cursor && "Underflow" )
    return this->Cursor - Other.Cursor;
}

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultIterator<TIn, bMove>::difference_type
TDefaultIterator<TIn, bMove>::operator-(const TDefaultIterator<const value_type, bMove>& Other) const noexcept
    requires(!std::is_const_v<value_type>)
{
    check( this->Cursor >= Other.Cursor && "Underflow" )
    return this->Cursor - Other.Cursor;
}

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultReversedIterator<TIn, bMove>::difference_type
TDefaultReversedIterator<TIn, bMove>::operator-(const TDefaultReversedIterator& Other) const noexcept
{
    check( Other.base() >= this->base() && "Underflow" )
    return Other.base() - this->base();
}

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultReversedIterator<TIn, bMove>::difference_type
TDefaultReversedIterator<TIn, bMove>::operator-(const TDefaultReversedIterator<std::remove_const_t<value_type>, bMove>& Other) const noexcept
    requires(std::is_const_v<value_type>)
{
    check( Other.base() >= this->base() && "Underflow" )
    return Other.base() - this->base();
}

template <typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultReversedIterator<TIn, bMove>::difference_type
TDefaultReversedIterator<TIn, bMove>::operator-(const TDefaultReversedIterator<const value_type, bMove>& Other) const noexcept
    requires(!std::is_const_v<value_type>)
{
    check( Other.base() >= this->base() && "Underflow" )
    return Other.base() - this->base();
}

template <typename TPredicate, typename TIn, bool bMove>
FORCEINLINE constexpr TDefaultFilteredIterator<TPredicate, TIn, bMove>&
TDefaultFilteredIterator<TPredicate, TIn, bMove>::operator++() noexcept
{
    check( this->bFirstDereference == false )
    check( this->Cursor != this->Slack )

    ++this->Cursor;
    while (this->Cursor != this->Slack)
    {
        check( this->Cursor < this->Slack )
        if (this->Filter(*this->Cursor))
        {
            break;
        }

        ++this->Cursor;

        continue;
    }

    return *this;
}

template <typename TPredicate, typename TIn, bool bMove>
template <std::integral SizeType>
FORCEINLINE constexpr TDefaultFilteredIterator<TPredicate, TIn, bMove>&
TDefaultFilteredIterator<TPredicate, TIn, bMove>::operator+=(SizeType N) noexcept
{
    check( this->bFirstDereference == false )
    check( this->Cursor != this->Slack )

    ++this->Cursor;
    while (this->Cursor != this->Slack)
    {
        check( this->Cursor < this->Slack )
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

#if LAL_DO_CHECKS
template <typename TPredicate, typename TIn, bool bMove>
template <typename UPredicate>
void TDefaultFilteredIterator<TPredicate, TIn, bMove>::_check(UPredicate&& Predicate)
{
    check( Predicate() )
    return;
}
#endif /* ~LAL_DO_CHECKS */

template <typename TPredicate, typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultFilteredIterator<TPredicate, TIn, bMove>::pointer
TDefaultFilteredIterator<TPredicate, TIn, bMove>::GetThisOrNext() const noexcept
{
    if (this->bFirstDereference)
    {
        this->bFirstDereference = false;

        if (this->Cursor != this->Slack && this->Filter(*this->Cursor) == false)
        {
            checkSlow( this->Cursor != this->Slack )
            ++this->Cursor;
            while (this->Cursor != this->Slack)
            {
                check( this->Cursor < this->Slack )
                if (this->Filter(*this->Cursor))
                {
                    break;
                }

                ++this->Cursor;

                continue;
            }
        }
    }

    return this->Cursor;
}

template <typename TPredicate, typename TIn, bool bMove>
FORCEINLINE constexpr TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>&
TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>::operator++() noexcept
{
    check( this->bFirstDereference == false )
    check( this->Cursor != this->Begin )

    --this->Cursor;
    while (this->Cursor != this->Begin)
    {
        check( this->Cursor > this->Begin )
        if (this->Filter(*(this->Cursor - 1)))
        {
            break;
        }

        --this->Cursor;

        continue;
    }

    return *this;
}

template <typename TPredicate, typename TIn, bool bMove>
template <std::integral SizeType>
FORCEINLINE constexpr TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>&
TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>::operator+=(SizeType N) noexcept
{
    check( this->bFirstDereference == false )
    check( this->Cursor != this->Begin )

    --this->Cursor;
    while (this->Cursor != this->Begin)
    {
        check( this->Cursor > this->Begin )
        if (this->Filter(*(this->Cursor - 1)))
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

#if LAL_DO_CHECKS
template <typename TPredicate, typename TIn, bool bMove>
template <typename UPredicate>
void TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>::_check(UPredicate&& Predicate)
{
    check( Predicate() )
    return;
}
#endif /* ~LAL_DO_CHECKS */

template <typename TPredicate, typename TIn, bool bMove>
FORCEINLINE constexpr typename TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>::pointer
TDefaultReversedFilteredIterator<TPredicate, TIn, bMove>::GetThisOrPrevious() const noexcept
{
    if (this->bFirstDereference)
    {
        this->bFirstDereference = false;

        if (this->Cursor != this->Begin && this->Filter(*(this->Cursor - 1)) == false)
        {
            checkSlow( this->Cursor != this->Begin )
            --this->Cursor;
            while (this->Cursor != this->Begin)
            {
                check( this->Cursor > this->Begin )
                if (this->Filter(*(this->Cursor - 1)))
                {
                    break;
                }

                --this->Cursor;

                continue;
            }
        }
    }

    return this->Cursor;
}

} /* ~Namespace Lal */
