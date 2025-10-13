// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

///////////////////////////////////////////////////////////////////////////////
// FWD
template<typename TContainer> FORCEINLINE constexpr void orphan(TContainer* Container) noexcept;
template<typename TContainer> FORCEINLINE constexpr void shrink_to_fit(TContainer* Container) noexcept;

///////////////////////////////////////////////////////////////////////////////
// Core

//# Convert something to a raw pointer.
using std::to_address;

//# Identity projection.
using std::identity;
//# Projection to a dereferenced unique pointer.
struct unique_raw
{
    template<typename T>
    NODISCARD FORCEINLINE constexpr auto
    operator()(const TUnique<T>& Ptr) const noexcept -> T*
    {
        return Ptr.get();
    }
};
struct unique_deref
{
    template<typename T>
    NODISCARD FORCEINLINE constexpr auto
    operator()(const TUnique<T>& Ptr) const noexcept -> T&
    {
        check( Ptr.get() )
        return *Ptr.get();
    }
};
//# Projection to a dereferenced raw pointer.
struct raw_pointer_deref
{
    template<typename T>
    NODISCARD FORCEINLINE constexpr auto
    operator()(T* Ptr) const noexcept -> T&
    {
        check( Ptr )
        return *Ptr;
    }
};

using std::ranges::begin;
using std::ranges::end;
using std::ranges::rbegin;
using std::ranges::rend;
using std::ranges::size;
using std::ranges::ssize;
using std::ranges::empty;
using std::ranges::data;

NODISCARD FORCEINLINE constexpr bool is_valid_index(auto const& Container, const auto Index) noexcept
{
    if constexpr (std::is_signed_v<decltype(Index)>)
    {
        if (Index < 0)
        {
            return false;
        }
    }

    return static_cast<std::make_unsigned_t<decltype(Index)>>(Index) < algo::size(Container);
}

///////////////////////////////////////////////////////////////////////////////
// Base
using std::ranges::next;
using std::ranges::prev;

///////////////////////////////////////////////////////////////////////////////
// Algo Base
using std::ranges::equal_to;
using std::ranges::greater_equal;
using std::ranges::less_equal;
using std::ranges::not_equal_to;
using std::ranges::out_value_result;
using std::ranges::equal;
using std::ranges::in_out_result;

using std::ranges::copy_result;
using std::ranges::copy;
using std::ranges::move_result;
using std::ranges::move;
using std::ranges::move_backward_result;
using std::ranges::copy_backward;
using std::ranges::copy_backward_result;
using std::ranges::move_backward;
using std::ranges::copy_n_result;
using std::ranges::copy_n;
using std::ranges::fill_n;
using std::ranges::fill;

///////////////////////////////////////////////////////////////////////////////
// Common
using std::ranges::all_of;
using std::ranges::any_of;
using std::ranges::none_of;

using std::ranges::in_fun_result;
using std::ranges::for_each_result;
using std::ranges::for_each;
using std::ranges::for_each_n_result;
using std::ranges::for_each_n;

using std::ranges::find_first_of;
using std::ranges::count;
using std::ranges::count_if;
using std::ranges::search_n;
using std::ranges::find_end;
using std::ranges::is_permutation;
using std::ranges::copy_if;
using std::ranges::swap_ranges;
using std::ranges::transform;

using std::ranges::replace;
using std::ranges::replace_if;
using std::ranges::replace_copy_result;
using std::ranges::replace_copy;
using std::ranges::replace_copy_if_result;
using std::ranges::replace_copy_if;

using std::ranges::generate_n;
using std::ranges::generate;

using std::ranges::remove_if;
namespace detail
{
struct erase_if_fn
{
    template<typename TContainer, typename TProj = std::identity, std::indirect_unary_predicate<std::projected<std::ranges::iterator_t<TContainer>, TProj>> TPred>
        requires std::permutable<std::ranges::iterator_t<TContainer>>
    FORCEINLINE constexpr typename TContainer::size_type
    operator()(TContainer* Container, TPred&& Pred, TProj Proj = {}) const
    {
        auto [It, End] { std::ranges::remove_if(*Container, std::forward<TPred>(Pred), std::move(Proj)) };
        const auto OldSize { Container->size() };
        Container->erase(It, End);
        return OldSize - Container->size();
    }
};
} /* ~Namespace detail */
//# Erases all matching elements. Returns number of removed elements.
inline constexpr detail::erase_if_fn erase_if{};

using std::ranges::remove;
namespace detail
{
struct erase_fn
{
    template<typename TContainer, typename TProj = std::identity, typename T _GLIBCXX26_RANGE_ALGO_DEF_VAL_T(std::ranges::iterator_t<TContainer>, TProj)>
        requires std::permutable<std::ranges::iterator_t<TContainer>> && std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, const T*>
    FORCEINLINE constexpr typename TContainer::size_type
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        auto [It, End] { std::ranges::remove(*Container, What, std::move(Proj)) };
        const auto OldSize { Container->size() };
        Container->erase(It, End);
        return OldSize - Container->size();
    }
};
} /* ~Namespace detail */
//# Erases all matching elements. Returns number of removed elements.
inline constexpr detail::erase_fn erase{};

using std::ranges::remove_copy_if_result;
using std::ranges::remove_copy_if;
using std::ranges::remove_copy_result;
using std::ranges::remove_copy;

namespace detail
{
struct erase_once_fn
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        if (auto It { std::ranges::find(algo::begin(*Container), algo::end(*Container), What, std::move(Proj)) }; It != algo::end(*Container))
        {
            Container->erase(It);
            return true;
        }

        return false;
    }
};

struct erase_once_checked_fn
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        const bool bRemoved{ erase_once_fn{}(Container, What, std::move(Proj)) };
        check( bRemoved )
        return bRemoved;
    }
};

struct erase_once_asserted_fn
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        const bool bRemoved{ erase_once_fn{}(Container, What, std::move(Proj)) };
        jassert( bRemoved )
        return bRemoved;
    }
};
} /* ~Namespace detail */
inline constexpr detail::erase_once_fn          erase_once{};
inline constexpr detail::erase_once_checked_fn  erase_once_checked{};
inline constexpr detail::erase_once_asserted_fn erase_once_asserted{};

//# Subs a container.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer sub(TContainer const& Container, const LSize Begin, const LSize End)
{
    check( Begin <= End && End <= algo::size(Container) )
    UContainer Out{ Container.begin() + Begin, Container.begin() + End };
    return Out;
}
//# Cuts the left.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer right_sub(TContainer const& Container, const LSize Begin)
{
    check( Begin <= algo::size(Container) )
    return UContainer{ Container.begin() + Begin, Container.end() };
}

//# Chops N elements from the left.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer right_chop(TContainer const& Container, const LSize N)
{
    check( N <= algo::size(Container) )
    return UContainer{ Container.begin() + N, Container.end() };
}
FORCEINLINE void inline_right_chop(auto* Container, const LSize N, const bool bAllowShrinking = true) noexcept
{
    check( N <= algo::size(*Container) )
    Container->erase(Container->begin() + N, Container->begin());
    if (bAllowShrinking) { algo::shrink_to_fit(Container); }
    return;
}

//# Chops N elements from the right.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer left_chop(TContainer const& Container, const LSize N)
{
    check( N <= algo::size(Container) )
    return UContainer{ Container.begin(), Container.end() - N };
}
FORCEINLINE void inline_left_chop(auto* Container, const LSize N, const bool bAllowShrinking = true) noexcept
{
    check( N <= algo::size(*Container) )
    Container->erase(Container->end() - N, Container->end());
    if (bAllowShrinking) { algo::shrink_to_fit(Container); }
    return;
}

using std::ranges::unique;
using std::ranges::unique_copy_result;
using std::ranges::unique_copy;
using std::ranges::reverse;
using std::ranges::reverse_copy_result;
using std::ranges::reverse_copy;
using std::ranges::rotate;
using std::ranges::rotate_copy_result;
using std::ranges::rotate_copy;
using std::ranges::sample;
using std::ranges::shuffle;

using std::ranges::push_heap;
using std::ranges::pop_heap;
using std::ranges::make_heap;
using std::ranges::sort_heap;
using std::ranges::is_heap_until;
using std::ranges::is_heap;

using std::ranges::sort;
using std::ranges::stable_sort;
using std::ranges::partial_sort;
using std::ranges::partial_sort_copy_result;
using std::ranges::partial_sort_copy;
using std::ranges::is_sorted_until;
using std::ranges::is_sorted;

using std::ranges::nth_element;
using std::ranges::lower_bound;
using std::ranges::upper_bound;
using std::ranges::equal_range;
using std::ranges::binary_search;
using std::ranges::is_partitioned;
using std::ranges::partition;
using std::ranges::stable_partition;

using std::ranges::in_out_result;
using std::ranges::partition_copy_result;
using std::ranges::partition_copy;
using std::ranges::partition_point;
using std::ranges::merge_result;
using std::ranges::merge;
using std::ranges::inplace_merge;

using std::ranges::includes;
using std::ranges::set_union;
using std::ranges::set_intersection;
using std::ranges::set_difference;
using std::ranges::set_symmetric_difference_result;
using std::ranges::set_symmetric_difference;

using std::ranges::max;
using std::ranges::clamp;
using std::ranges::minmax_result;
using std::ranges::minmax;
using std::ranges::min_element;
using std::ranges::max_element;
using std::ranges::minmax_element_result;
using std::ranges::minmax_element;
using std::ranges::lexicographical_compare;
using std::ranges::in_found_result;
using std::ranges::next_permutation_result;
using std::ranges::next_permutation;
using std::ranges::prev_permutation_result;
using std::ranges::prev_permutation;

using std::ranges::contains;
using std::ranges::contains_subrange;
namespace detail
{
struct contains_if_fn
{
    template<std::forward_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const
    {
        return std::ranges::find_if(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj)) != Sent;
    }

    template<std::ranges::forward_range TRange, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<std::ranges::iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};
} /* ~Namespace detail */
inline constexpr detail::contains_if_fn contains_if{};

using std::ranges::find_last;
using std::ranges::find_last_if;
using std::ranges::find_last_if_not;

using std::ranges::in_value_result;
using std::ranges::fold_left_with_iter_result;
using std::ranges::fold_left_with_iter;
using std::ranges::fold_left;
using std::ranges::fold_left_first_with_iter_result;
using std::ranges::fold_left_first_with_iter;
using std::ranges::fold_left_first;
using std::ranges::fold_right;
using std::ranges::fold_right_last;

using std::shift_left;
using std::shift_right;

///////////////////////////////////////////////////////////////////////////////
// Utility
using std::ranges::view_interface;
using std::ranges::subrange_kind;
using std::ranges::subrange;
using std::ranges::get;
using std::ranges::enable_borrowed_range;
using std::ranges::borrowed_subrange_t;

using std::ranges::find;
namespace detail
{
struct find_pointer_fn
{
    template<std::input_iterator _Iter, std::sentinel_for<_Iter> _Sent, typename _Proj = algo::identity, typename _Tp _GLIBCXX26_RANGE_ALGO_DEF_VAL_T(_Iter, _Proj)>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<_Iter, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Iter __first, _Sent __last, const _Tp& __value, _Proj __proj = {}) const // -> decltype(algo::to_address(__first))
    {
        if (_Iter It { algo::find(__first, __last, __value, std::move(__proj)) }; It != __last)
        {
            return algo::to_address(It);
        }

        return static_cast<decltype(algo::to_address(__first))>(nullptr);
    }

    template<std::ranges::input_range _Range, typename _Proj = algo::identity, typename _Tp _GLIBCXX26_RANGE_ALGO_DEF_VAL_T(std::ranges::iterator_t<_Range>, _Proj)>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<_Range>, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Range&& __r, const _Tp& __value, _Proj __proj = {}) const -> decltype(algo::to_address(algo::begin(__r)))
    {
        return (*this)(algo::begin(__r), algo::end(__r), __value, std::move(__proj));
    }
};
} /* ~Namespace detail */
//# Get a pointer to the found element or nullptr.
inline constexpr detail::find_pointer_fn find_pointer{};

using std::ranges::find_if;
namespace detail
{
struct find_pointer_if_fn
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
        std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const // -> decltype(algo::to_address(__first))
    {
        if (TIter It{ algo::find_if(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj)) }; It != Sent)
        {
            return algo::to_address(It);
        }

        return static_cast<decltype(algo::to_address(Begin))>(nullptr);
    }

    template<std::ranges::input_range TRange, typename TProj = algo::identity,
        std::indirect_unary_predicate<std::projected<std::ranges::iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const -> decltype(algo::to_address(algo::begin(Range)))
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};
} /* ~Namespace detail */
//# Get a pointer to the found element or nullptr.
inline constexpr detail::find_pointer_if_fn find_pointer_if{};

using std::ranges::find_if_not;

using std::ranges::in_in_result;
using std::ranges::mismatch_result;
using std::ranges::mismatch;
using std::ranges::search;
using std::ranges::min;
using std::ranges::adjacent_find;

///////////////////////////////////////////////////////////////////////////////
// Extended
template<typename TContainer>
FORCEINLINE constexpr void orphan(TContainer* Container) noexcept
{
    Container->clear();
    default_swap(Container);
    check( Container->size() == Container->capacity() )
    return;
}
template<>
FORCEINLINE constexpr void orphan<LString>(LString* Container) noexcept
{
    Container->clear();
    default_swap(Container);
    return;
}

template<typename TContainer>
FORCEINLINE constexpr void shrink_to_fit(TContainer* Container) noexcept
{
    Container->shrink_to_fit();
    check( Container->size() == Container->capacity() )
    return;
}

///////////////////////////////////////////////////////////////////////////////
// Weak
//# Weak find. Only requires left equality comparable.
NODISCARD FORCEINLINE constexpr auto wfind(ITERATOR Begin, ITERATOR End, const auto& Value) noexcept
{
    return std::find(Begin, End, Value);
}
NODISCARD FORCEINLINE constexpr auto wfind(RANGE Container, const auto& Value) noexcept
{
    return algo::wfind(algo::begin(Container), algo::end(Container), Value);
}

//# Get a pointer to the found element or nullptr.
template<typename TProj = algo::identity>
NODISCARD FORCEINLINE constexpr auto wfind_pointer(ITERATOR Begin, ITERATOR End, const auto& Value, TProj Proj = {}) noexcept -> decltype(&*Begin)
{
    if constexpr (std::is_same_v<TProj, algo::identity>)
    {
        if (auto It { std::find(Begin, End, Value) }; It != End)
        {
            return &*It;
        }
        return static_cast<decltype(&*Begin)>(nullptr);
    }

#if LAL_WITH_GCC || LAL_WITH_CLANG
    #pragma GCC unroll 4
#endif /* LAL_WITH_GCC || LAL_WITH_CLANG */
    while (Begin != End && !(std::invoke(Proj, *Begin) == Value))
    {
        ++Begin;
    }
    return &*Begin;
}
template<typename TProj = algo::identity>
NODISCARD FORCEINLINE constexpr auto wfind_pointer(RANGE Container, const auto& Value, TProj Proj = {}) noexcept
{
    return algo::wfind_pointer(algo::begin(Container), algo::end(Container), Value, std::move(Proj));
}

} /* ~Namespace algo */
