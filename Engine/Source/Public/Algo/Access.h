// Copyright mzoesch. All rights reserved.

#pragma once

#if JAFG_WITH_CLANG
    #define JAFG_RANGE_VAL_T(It, Proj) _GLIBCXX26_RANGE_ALGO_DEF_VAL_T(It, Proj)
#else /* JAFG_WITH_CLANG */
    //# TODO: Add security for non clang toolchains.
    #define JAFG_RANGE_VAL_T(It, Proj)
#endif /* !JAFG_WITH_CLANG */

namespace algo
{

///////////////////////////////////////////////////////////////////////////////
// FWD
template<typename TContainer> FORCEINLINE constexpr void orphan(TContainer* Container) noexcept;

///////////////////////////////////////////////////////////////////////////////
// Move
using std::swap;
template<typename T> requires std::is_default_constructible_v<T>
FORCEINLINE constexpr void swap_default(T* Element)
    noexcept(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_swappable_v<T>)
{
    check(Element)
    T Default;
    swap(*Element, Default);
}

///////////////////////////////////////////////////////////////////////////////
// Projection

//# Identity projection.
using std::identity;
//# Projection to the raw underlying value of a unique pointer.
struct unique_raw
{
    template<typename T, typename Deleter>
    NODISCARD FORCEINLINE constexpr auto
    operator()(const TUnique<T,Deleter>& Ptr) const noexcept -> T*
    {
        return Ptr.get();
    }
};
//# Projection to a dereferenced unique pointer const reference.
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
//# Projection to the first item in a pair.
struct pair_first
{
    template<typename T, typename U>
    NODISCARD FORCEINLINE constexpr auto
    operator()(std::pair<T,U>& Pair) -> T&
    {
        return Pair.first;
    }
};
//# Projection to the second item in a pair.
struct pair_second
{
    template<typename T, typename U>
    NODISCARD FORCEINLINE constexpr auto
    operator()(std::pair<T,U>& Pair) -> U&
    {
        return Pair.second;
    }
};

//# The record from a projection to a member of said record.
template<typename T> struct proj_record;
template<typename T, typename U> struct proj_record<U T::*> { typedef T type; };
template<typename T> using proj_record_t = typename proj_record<T>::type;
//# The member from a projection to this member of any record.
template<typename T> struct proj_member;
template<typename T, typename U> struct proj_member<U T::*> { typedef U type; };
template<typename T> using proj_member_t = typename proj_member<T>::type;

///////////////////////////////////////////////////////////////////////////////
// Core

//# Convert something to a raw pointer.
using std::to_address;

using std::ranges::range;
using std::ranges::borrowed_range;

using std::ranges::range_value_t;

using std::ranges::iterator_t;
using std::ranges::const_iterator_t;
using std::ranges::range_const_reference_t;

using std::ranges::sentinel_t;
using std::ranges::const_sentinel_t;

using std::ranges::begin;
using std::ranges::end;
using std::ranges::rbegin;
using std::ranges::rend;
using std::ranges::size;
using std::ranges::ssize;
using std::ranges::empty;
using std::ranges::data;

using std::ranges::sized_range;
using std::ranges::range_size_t;

using std::ranges::input_range;
using std::ranges::forward_range;
using std::ranges::random_access_range;
using std::ranges::contiguous_range;
using std::ranges::common_range;
using std::ranges::constant_range;

using std::input_iterator;
using std::output_iterator;
using std::forward_iterator;
using std::bidirectional_iterator;
using std::random_access_iterator;
using std::contiguous_iterator;
using std::sentinel_for;
using std::sized_sentinel_for;

//# Trivial distance.
using std::distance;
template<input_range TRange, typename TIt>
FORCEINLINE constexpr decltype(auto) distance(TRange const& Range, TIt It) noexcept
{
    return std::ranges::distance(begin(Range), It);
}
//# Distance between ranges.
#if JAFG_WITH_CLANG
inline constexpr std::ranges::__distance_fn ranged_distance{};
#endif /* JAFG_WITH_CLANG */

inline constexpr decltype(LString::npos) npos{ LString::npos };

NODISCARD FORCEINLINE constexpr bool is_valid_index(auto const& Container, const auto Index) noexcept
    requires (std::integral<decltype(Index)> || std::is_enum_v<decltype(Index)>)
{
    typedef decltype(Index) Index_t;
    if constexpr (std::is_enum_v<Index_t>)
    {
        if constexpr (std::is_signed_v<std::underlying_type_t<Index_t>>)
        {
            if (std::to_underlying(Index) < 0)
            {
                return false;
            }
        }
        return static_cast<std::make_unsigned_t<Index_t>>(std::to_underlying(Index)) < size(Container);
    }
    else if constexpr (std::is_signed_v<Index_t>)
    {
        if (Index < 0)
        {
            return false;
        }
    }
    return static_cast<std::make_unsigned_t<Index_t>>(Index) < size(Container);
}

///////////////////////////////////////////////////////////////////////////////
// Base
using std::ranges::next;
using std::ranges::prev;

///////////////////////////////////////////////////////////////////////////////
// Algo Base
using std::ranges::equal_to;
using std::ranges::not_equal_to;
using std::ranges::less;
using std::ranges::less_equal;
using std::ranges::greater;
using std::ranges::greater_equal;

using std::ranges::equal;
using std::ranges::out_value_result;
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
        auto [It, End]{std::ranges::remove_if(*Container, std::forward<TPred>(Pred), std::move(Proj))};
        const auto OldSize{Container->size()};
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
    template<typename TContainer, typename TProj = std::identity, typename T JAFG_RANGE_VAL_T(std::ranges::iterator_t<TContainer>, TProj)>
        requires std::permutable<std::ranges::iterator_t<TContainer>> && std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, const T*>
    FORCEINLINE constexpr typename TContainer::size_type
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        auto [It, End]{std::ranges::remove(*Container, What, std::move(Proj))};
        const auto OldSize{Container->size()};
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
        if (auto It{std::ranges::find(algo::begin(*Container), algo::end(*Container), What, std::move(Proj))}; It != algo::end(*Container))
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
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
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
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
        jassert( bRemoved )
        return bRemoved;
    }
};

struct erase_exactly_once_checked_fn
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
#if JAFG_DO_CHECKS
        TProj Proj2{Proj};
#endif /* JAFG_DO_CHECKS */
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
        check( bRemoved )
        check( erase_once_fn{}(Container, What, std::move(Proj2)) == false && "Encountered multiple elements that are equal to #What." )
        return bRemoved;
    }
};

} /* ~Namespace detail */
//#
//# Erase an element from a range if it exists.
//# @return True, if successfully deleted.
//#
inline constexpr detail::erase_once_fn          erase_once{};
//# Checks that the element is removed. There may still be more elements that equal the #What.
inline constexpr detail::erase_once_checked_fn  erase_once_checked{};
inline constexpr detail::erase_once_asserted_fn erase_once_asserted{};
//# Remove an element exactly once. If checks are enabled, unable to remove once or finding duplicate elements will result in a program panic.
inline constexpr detail::erase_exactly_once_checked_fn erase_exactly_once_checked{};

//# Subs a container.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer sub(TContainer const& Container, const LSize Begin, const LSize End)
{
    check( Begin <= End && End <= algo::size(Container) )
    UContainer Out{Container.begin() + Begin, Container.begin() + End};
    return Out;
}

//# Chops N elements from the left.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer right_chop(TContainer const& Container, const LSize N)
{
    check(N <= algo::size(Container))
    return UContainer{Container.begin() + N, Container.end()};
}
FORCEINLINE void inline_right_chop(auto* Container, const LSize N, const bool bAllowShrinking = true) noexcept
{
    check(N <= algo::size(*Container))
    Container->erase(Container->begin(), Container->begin() + N);
    if (bAllowShrinking) { Container->shrink_to_fit(); }
    return;
}

//# Chops N elements from the right.
template<typename TContainer, typename UContainer = TContainer>
FORCEINLINE UContainer left_chop(TContainer const& Container, const LSize N)
{
    check(N <= algo::size(Container))
    return UContainer{Container.begin(), Container.end() - N};
}
FORCEINLINE void inline_left_chop(auto* Container, const LSize N, const bool bAllowShrinking = true) noexcept
{
    check(N <= algo::size(*Container))
    Container->erase(Container->end() - N, Container->end());
    if (bAllowShrinking) { Container->shrink_to_fit(); }
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
    template<forward_iterator TIter, sentinel_for<TIter> TSent, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const
    {
        return std::ranges::find_if(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj)) != Sent;
    }

    template<forward_range TRange, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
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
    template<input_iterator _Iter, sentinel_for<_Iter> _Sent, typename _Proj = algo::identity, typename _Tp JAFG_RANGE_VAL_T(_Iter, _Proj)>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<_Iter, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Iter __first, _Sent __last, const _Tp& __value, _Proj __proj = {}) const // -> decltype(algo::to_address(__first))
    {
        if (_Iter It{algo::find(__first, __last, __value, std::move(__proj))}; It != __last)
        {
            return algo::to_address(It);
        }

        return static_cast<decltype(algo::to_address(__first))>(nullptr);
    }

    template<input_range _Range, typename _Proj = algo::identity, typename _Tp JAFG_RANGE_VAL_T(std::ranges::iterator_t<_Range>, _Proj)>
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

namespace detail
{
struct find_pointer_checked_fn
{
    template<input_iterator _Iter, sentinel_for<_Iter> _Sent, typename _Proj = algo::identity, typename _Tp JAFG_RANGE_VAL_T(_Iter, _Proj)>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<_Iter, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Iter __first, _Sent __last, const _Tp& __value, _Proj __proj = {}) const // -> decltype(algo::to_address(__first))
    {
        auto* Result{find_pointer(__first, __last, __value, std::move(__proj))};
        check(Result)
        return Result;
    }

    template<input_range _Range, typename _Proj = algo::identity, typename _Tp JAFG_RANGE_VAL_T(std::ranges::iterator_t<_Range>, _Proj)>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<_Range>, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Range&& __r, const _Tp& __value, _Proj __proj = {}) const -> decltype(algo::to_address(algo::begin(__r)))
    {
        return (*this)(algo::begin(__r), algo::end(__r), __value, std::move(__proj));
    }
};
} /* ~Namespace detail */
inline constexpr detail::find_pointer_checked_fn find_pointer_checked{};

using std::ranges::find_if;
namespace detail
{
struct find_pointer_if_fn
{
    template<input_iterator TIter, sentinel_for<TIter> TSent, typename TProj = algo::identity,
        std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const // -> decltype(algo::to_address(__first))
    {
        if (TIter It{algo::find_if(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj))}; It != Sent)
        {
            return algo::to_address(It);
        }

        return static_cast<decltype(algo::to_address(Begin))>(nullptr);
    }

    template<input_range TRange, typename TProj = algo::identity,
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
// Predicates
using std::predicate;
template<typename F, typename... TArgs>
concept void_predicate = std::regular_invocable<F, TArgs...> && std::same_as<std::invoke_result_t<F, TArgs...>, void>;
template<typename TFn, typename TWhat>
concept void_mutable_predicate = requires(TFn&& Fn, TWhat What) { std::invoke(std::forward<TFn>(Fn), What); };

///////////////////////////////////////////////////////////////////////////////
// Extended
template<typename TContainer>
FORCEINLINE constexpr void orphan(TContainer const* Container) noexcept = delete;
template<typename TContainer>
FORCEINLINE constexpr void orphan(TContainer* Container) noexcept
{
    swap_default(Container);
    check(Container->size() == Container->capacity())
}
template<>
FORCEINLINE constexpr void orphan<LString>(LString* Container) noexcept
{
    swap_default(Container);
    check(Container->size() == 0)
}

//# Whether a container can reserve memory.
template<typename T>
inline constexpr bool reservable = sized_range<T>
    && requires(T& t, range_size_t<T> n)
    {
        t.reserve(n);
        { t.capacity() } -> std::same_as<decltype(n)>;
        { t.max_size() } -> std::same_as<decltype(n)>;
    };
template<typename T> requires reservable<T>
NODISCARD FORCEINLINE constexpr auto reserve(T* t, range_size_t<T> n) -> decltype(t->reserve(n)) { return t->reserve(n); }

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
        if (auto It{std::find(Begin, End, Value)}; It != End)
        {
            return &*It;
        }
        return static_cast<decltype(&*Begin)>(nullptr);
    }

#if JAFG_WITH_GCC || JAFG_WITH_CLANG
    #pragma GCC unroll 4
#endif /* JAFG_WITH_GCC || JAFG_WITH_CLANG */
    while (Begin != End && !(std::invoke(Proj, *Begin) == Value))
    {
        ++Begin;
    }

    if (Begin != End)
    {
        return &*Begin;
    }

    return static_cast<decltype(&*Begin)>(nullptr);
}
template<typename TProj = algo::identity>
NODISCARD FORCEINLINE constexpr auto wfind_pointer(RANGE Container, const auto& Value, TProj Proj = {}) noexcept
{
    return algo::wfind_pointer(algo::begin(Container), algo::end(Container), Value, std::move(Proj));
}

///////////////////////////////////////////////////////////////////////////////
// Conversions
namespace detail
{
struct add_spaces_to_camel_case_fn
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TOut>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TIter First, TSent Sent) const -> TOut
    {
        TOut Result;
        if constexpr (algo::reservable<TOut>)
        {
            algo::reserve(&Result, algo::distance(First, Sent));
        }
        if (First != Sent)
        {
            Result.push_back(*First++);
            while (First != Sent)
            {
                if (*First >= 'A' && *First <= 'Z')
                {
                    Result.push_back(' ');
                }
                Result.push_back(*First++);
            }
        }
        return Result;
    }

    template<input_range TRange, typename TOut = std::remove_reference_t<TRange>>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& R) const -> TOut
    {
        return (*this).template operator()<decltype(algo::begin(R)), decltype(algo::end(R)), TOut>(algo::begin(R), algo::end(R));
    }
};
} /* ~Namespace detail */
inline constexpr detail::add_spaces_to_camel_case_fn add_spaces_to_camel_case{};


///////////////////////////////////////////////////////////////////////////////
// String utils
struct case_insensitive_hash
{
    inline LSize operator()(auto const& Key) const noexcept
    {
        return (*this)(Key.data(), Key.size(), 0);
    }
    inline LSize operator()(char const* Str, LSize Size, LSize H) const noexcept
    {
        if (Size == 0)
        {
            return H;
        }
        return (*this)(Str + 1, Size - 1, (((std::numeric_limits<LSize>::max)() >> 6) & H * 33) ^ static_cast<unsigned char>(std::tolower(*Str)));
    }
};

struct case_insensitive_equal_to
{
    inline LSize operator()(auto const& Lhs, auto const& Rhs) const noexcept
    {
        return
               algo::size(Lhs) == algo::size(Rhs)
            && std::equal(algo::begin(Lhs), algo::end(Lhs), algo::begin(Rhs), [](auto A, auto B) { return std::tolower(A) == std::tolower(B); });
    }
};

#if PLATFORM_WINDOWS
namespace detail
{

struct utf8_to_utf16_fn
{
    template<contiguous_iterator TIt, std::unsigned_integral TSize>
    NODISCARD FORCEINLINE LWString operator()(TIt It, TSize Size) const noexcept
    {
        if (Size == 0)
        {
            return {};
        }
        auto Required{::MultiByteToWideChar(CP_UTF8, 0, &*It, static_cast<int>(Size), NULL, 0)};
        LWString Utf16(Required, LITERAL_WIDE('\0'));
        ::MultiByteToWideChar(CP_UTF8, 0, &*It, Size, &Utf16[0], Required);
        return Utf16;
    }
    template<contiguous_range TRange>
    NODISCARD FORCEINLINE LWString operator()(TRange const& Range) const noexcept
    {
        return (*this)(algo::begin(Range), algo::size(Range));
    }
};

struct utf16_to_utf8_fn
{
    template<contiguous_iterator TIt, std::unsigned_integral TSize>
    NODISCARD FORCEINLINE LString operator()(TIt It, TSize Size) const noexcept
    {
        if (Size == 0)
        {
            return {};
        }
        auto Required{::WideCharToMultiByte(CP_UTF8, 0, &*It, static_cast<int>(Size), NULL, 0, NULL, NULL)};
        LString Utf8(Required, '\0');
        ::WideCharToMultiByte(CP_UTF8, 0, &*It, Size, &Utf8[0], Required, NULL, NULL);
        return Utf8;
    }

    template<input_range TRange>
    NODISCARD FORCEINLINE LString operator()(TRange const& Range) const noexcept
    {
        return (*this)(algo::begin(Range), algo::size(Range));
    }
};

} /* ~Namespace detail */

inline constexpr detail::utf8_to_utf16_fn utf8_to_utf16{};
inline constexpr detail::utf16_to_utf8_fn utf16_to_utf8{};

#endif /* PLATFORM_WINDOWS */

namespace detail
{

struct join_fn
{
    template<input_iterator TIter, sentinel_for<TIter> TSent, typename TProj = algo::identity>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TIter First, TSent Sent, TProj Proj, LStringView Separator = ", ") const -> LString
    {
        std::stringstream ss;
        if (First != Sent)
        {
            ss << std::invoke(Proj, *First++);
            for (;First != Sent; ++First)
            {
                ss << Separator << std::invoke(Proj, *First);
            }
        }
        return ss.str();
    }

    template<input_range TRange, typename TProj = algo::identity>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& R, TProj Proj = {}, LStringView Separator = ", ") const -> LString
    {
        return (*this)(begin(R), end(R), std::move(Proj), Separator);
    }
};

} /* ~Namespace detail */

inline constexpr detail::join_fn join{};

///////////////////////////////////////////////////////////////////////////////
// Time stuff
inline decltype(auto) now() noexcept
{
    return std::chrono::high_resolution_clock::now();
}

inline f64 time_diff(std::chrono::high_resolution_clock::time_point A, std::chrono::high_resolution_clock::time_point B) noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<f64>>(B - A).count();
}

///////////////////////////////////////////////////////////////////////////////
// Misc
template<typename T>
concept bool_testable = requires(T&& t) { static_cast<bool>(t); };

} /* ~Namespace algo */

static_assert(sizeof(algo::npos) == sizeof(std::string::npos));
static_assert(sizeof(algo::npos) == sizeof(LString::npos));
static_assert(sizeof(algo::npos) == sizeof(LStringView::npos));
static_assert(algo::npos == std::string::npos);
static_assert(algo::npos == LString::npos);
static_assert(algo::npos == LStringView::npos);
static_assert(std::is_same_v<decltype(algo::npos), decltype(std::string::npos)>);
static_assert(std::is_same_v<decltype(algo::npos), decltype(LString::npos)>);
static_assert(std::is_same_v<decltype(algo::npos), decltype(LStringView::npos)>);
