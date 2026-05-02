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

using namespace std::ranges;

using std::swap;
template<typename T> requires std::is_default_constructible_v<T> && std::is_swappable_v<T>
FORCEINLINE constexpr void swap_default(T* Element)
    noexcept(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_swappable_v<T>)
{
    check(Element)
    T Default; swap(*Element, Default);
}
template<typename T> FORCEINLINE constexpr void orphan(T const* Container) noexcept = delete;
template<typename T> FORCEINLINE constexpr void orphan(T* Container) noexcept { swap_default(Container); }

inline constexpr decltype(LString::npos) npos{ LString::npos };

using std::to_address;
using std::shift_left;
using std::shift_right;

using std::predicate;
template<typename F, typename... TArgs>
concept void_predicate = std::regular_invocable<F, TArgs...> && std::same_as<std::invoke_result_t<F, TArgs...>, void>;
template<typename TFn, typename TWhat>
concept void_mutable_predicate = requires(TFn&& Fn, TWhat What) { std::invoke(std::forward<TFn>(Fn), What); };

//# Identity projection.
using std::identity;
//# Projection to the raw underlying value of a unique pointer.
struct unique_raw final
{
    template<typename T, typename Deleter> NODISCARD FORCEINLINE constexpr T*
    operator()(TUnique<T,Deleter> const& Ptr) const noexcept { return Ptr.get(); }
};
//# Projection to a dereferenced unique pointer const reference.
struct unique_deref final
{
    template<typename T> NODISCARD FORCEINLINE constexpr T&
    operator()(TUnique<T> const& Ptr) const noexcept { check(Ptr.get()) return *Ptr.get(); }
};
//# Projection to a dereferenced raw pointer.
struct raw_pointer_deref final
{
    template<typename T> NODISCARD FORCEINLINE constexpr T&
    operator()(T* Ptr) const noexcept { check(Ptr) return *Ptr; }
};
//# Projection to the first item in a pair.
struct pair_first final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Pair) const noexcept { return std::forward<T>(Pair).first; }
};
//# Projection to the second item in a pair.
struct pair_second final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Pair) const noexcept { return std::forward<T>(Pair).second; }
};
//# The record from a projection to a member of said record.
template<typename T> struct proj_record;
template<typename T, typename U> struct proj_record<U T::*> { typedef T type; };
template<typename T> using proj_record_t = typename proj_record<T>::type;
//# The member from a projection to this member of any record.
template<typename T> struct proj_member;
template<typename T, typename U> struct proj_member<U T::*> { typedef U type; };
template<typename T> using proj_member_t = typename proj_member<T>::type;
struct universal_deref final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return *Value; }
};
struct universal_ref final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return &Value; }
};
struct universal_ptr_noop final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return &*Value; }
};

//# Whether a container can reserve memory.
template<typename T>
inline constexpr bool is_reservable_v = sized_range<T>
    && requires(T& t, range_size_t<T> n)
    {
        t.reserve(n);
        { t.capacity() } -> std::same_as<decltype(n)>;
        { t.max_size() } -> std::same_as<decltype(n)>;
    };

namespace detail
{

struct reserve_fn final
{
    template<forward_range TRange> requires is_reservable_v<TRange>
    FORCEINLINE decltype(auto) operator()(TRange* Range, range_size_t<TRange> N) const
        noexcept(noexcept(Range->reserve(N)))
    {
        return Range->reserve(N);
    }
};

struct distance_to_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<TIter, TProj>, T const*>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, T const& Value, TProj Proj = {}) const
    {
        std::iter_difference_t<TIter> N{};
        while (Begin != Sent)
        {
            if (std::invoke(Proj, *Begin) == Value)
            {
                break;
            }
            ++N;
        }
        return N;
    }
    template<input_range TRange, typename TProj = identity, typename T>
        requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<TRange>, TProj>, T const*>
    NODISCARD FORCEINLINE constexpr std::ranges::range_difference_t<TRange>
    operator()(TRange&& Range, T const& Value, TProj Proj = {}) const
    {
        return (*this)(begin(Range), end(Range), Value, std::move(Proj));
    }
};

struct valid_index_fn final
{
    template<forward_range TRange, typename T>
        requires (std::is_integral_v<T> || std::is_enum_v<T>)
    NODISCARD FORCEINLINE bool
    operator()(TRange&& Range, T Index) const noexcept
    {
        if constexpr (std::is_enum_v<T>)
        {
            if constexpr (std::is_signed_v<std::underlying_type_t<T>>)
            {
                if (std::to_underlying(Index) < 0)
                {
                    return false;
                }
            }
            return static_cast<std::make_unsigned_t<T>>(std::to_underlying(Index)) < size(Range);
        }
        if constexpr (std::is_signed_v<T>)
        {
            if (Index < 0)
            {
                return false;
            }
        }
        return static_cast<std::make_unsigned_t<T>>(Index) < size(Range);
    }
};

struct contains_if_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const
    {
        return std::ranges::find_if(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj)) != Sent;
    }
    template<input_range TRange, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};

struct contains_if_checked_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const
    {
        bool Result{contains_if_fn{}(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj))};
        check(Result)
        return Result;
    }
    template<input_range TRange, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};

struct contains_if_asserted_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Begin, TSent Sent, TPred&& Pred, TProj Proj = {}) const
    {
        bool Result{contains_if_fn{}(Begin, Sent, std::forward<TPred>(Pred), std::move(Proj))};
        jassert(Result)
        return Result;
    }
    template<input_range TRange, typename TProj = algo::identity,
         std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};

struct find_checked_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = identity, typename T JAFG_RANGE_VAL_T(TIter, TProj)>
          requires std::indirect_binary_predicate<equal_to, std::projected<TIter, TProj>, T const*>
    constexpr TIter
    operator()(TIter Begin, TSent Sent, T const& Value, TProj Proj = {}) const
    {
        auto It{algo::find(Begin, Sent, Value, std::move(Proj))};
        check(It != Sent)
        return It;
    }
    template<input_range TRange, typename TProj = identity, typename T JAFG_RANGE_VAL_T(iterator_t<_Range>, _Proj)>
      requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TRange>, TProj>, T const*>
    constexpr borrowed_iterator_t<TRange>
    operator()(TRange&& Range, T const& Value, TProj Proj = {}) const
    {
        return (*this)(begin(Range), end(Range), Value, std::move(Proj));
    }
};

struct find_if_checked_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = identity
        , std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    FORCEINLINE constexpr TIter
    operator()(TIter Begin, TSent Sent, TPred Pred, TProj Proj = {}) const
    {
        auto Result{find_if(Begin, Sent, std::move(Pred), std::move(Proj))};
        check(Result != Sent)
        return Result;
    }
    template<input_range TRange, typename TProj = identity
        , std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    FORCEINLINE constexpr borrowed_iterator_t<TRange>
    operator()(TRange&& Range, TPred Pred, TProj Proj = {}) const
    {
        return (*this)(begin(Range), end(Range), std::move(Pred), std::move(Proj));
    }
};

struct find_if_asserted_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = identity
        , std::indirect_unary_predicate<std::projected<TIter, TProj>> TPred>
    FORCEINLINE constexpr TIter
    operator()(TIter Begin, TSent Sent, TPred Pred, TProj Proj = {}) const
    {
        auto Result{find_if(Begin, Sent, std::move(Pred), std::move(Proj))};
        jassert(Result != Sent)
        return Result;
    }
    template<input_range TRange, typename TProj = identity
        , std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    FORCEINLINE constexpr borrowed_iterator_t<TRange>
    operator()(TRange&& Range, TPred Pred, TProj Proj = {}) const
    {
        return (*this)(begin(Range), end(Range), std::move(Pred), std::move(Proj));
    }
};
struct find_pointer_fn final
{
    template<std::input_iterator _Iter, std::sentinel_for<_Iter> _Sent, typename _Proj = algo::identity, typename _Tp JAFG_RANGE_VAL_T(_Iter, _Proj)>
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
    template<input_range _Range, typename _Proj = identity, typename _Tp JAFG_RANGE_VAL_T(std::ranges::iterator_t<_Range>, _Proj)>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<_Range>, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Range&& __r, const _Tp& __value, _Proj __proj = {}) const -> decltype(algo::to_address(algo::begin(__r)))
    {
        return (*this)(algo::begin(__r), algo::end(__r), __value, std::move(__proj));
    }
};

struct find_pointer_checked_fn final
{
    template<std::input_iterator _Iter, std::sentinel_for<_Iter> _Sent, typename _Proj = identity, typename _Tp JAFG_RANGE_VAL_T(_Iter, _Proj)>
        requires std::indirect_binary_predicate<equal_to, std::projected<_Iter, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Iter __first, _Sent __last, const _Tp& __value, _Proj __proj = {}) const // -> decltype(algo::to_address(__first))
    {
        auto* Result{find_pointer_fn{}(__first, __last, __value, std::move(__proj))};
        check(Result)
        return Result;
    }
    template<input_range _Range, typename _Proj = identity, typename _Tp JAFG_RANGE_VAL_T(std::ranges::iterator_t<_Range>, _Proj)>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<_Range>, _Proj>, const _Tp*>
    NODISCARD FORCEINLINE constexpr auto
    operator()(_Range&& __r, const _Tp& __value, _Proj __proj = {}) const -> decltype(algo::to_address(algo::begin(__r)))
    {
        return (*this)(algo::begin(__r), algo::end(__r), __value, std::move(__proj));
    }
};

struct find_pointer_if_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = algo::identity,
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
    template<input_range TRange, typename TProj = identity,
        std::indirect_unary_predicate<std::projected<iterator_t<TRange>, TProj>> TPred>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& Range, TPred&& Pred, TProj Proj = {}) const -> decltype(algo::to_address(algo::begin(Range)))
    {
        return (*this)(algo::begin(Range), algo::end(Range), std::forward<TPred>(Pred), std::move(Proj));
    }
};

struct erase_fn final
{
    template<typename TContainer, typename TProj = std::identity, typename T JAFG_RANGE_VAL_T(std::ranges::iterator_t<TContainer>, TProj)>
        requires std::permutable<iterator_t<TContainer>> && std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TContainer>, TProj>, const T*>
    FORCEINLINE constexpr typename TContainer::size_type
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        auto [It, End]{std::ranges::remove(*Container, What, std::move(Proj))};
        const auto OldSize{Container->size()};
        Container->erase(It, End);
        return OldSize - Container->size();
    }
};

struct erase_if_fn final
{
    template<typename TContainer, typename TProj = std::identity, std::indirect_unary_predicate<std::projected<iterator_t<TContainer>, TProj>> TPred>
        requires std::permutable<iterator_t<TContainer>>
    FORCEINLINE constexpr typename TContainer::size_type
    operator()(TContainer* Container, TPred&& Pred, TProj Proj = {}) const
    {
        auto [It, End]{remove_if(*Container, std::forward<TPred>(Pred), std::move(Proj))};
        const auto OldSize{size(Container)};
        Container->erase(It, End);
        return OldSize - size(Container);
    }
};

struct erase_once_fn final
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        if (auto It{find(begin(*Container), end(*Container), What, std::move(Proj))}; It != end(*Container))
        {
            Container->erase(It);
            return true;
        }
        return false;
    }
};

struct erase_once_checked_fn final
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
        check(bRemoved)
        return bRemoved;
    }
};

struct erase_once_asserted_fn final
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
        jassert( bRemoved )
        return bRemoved;
    }
};

struct erase_exactly_once_checked_fn final
{
    template<typename TContainer, typename TProj = std::identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TContainer>, TProj>, T const*>
    FORCEINLINE constexpr bool
    operator()(TContainer* Container, T const& What, TProj Proj = {}) const
    {
#if JAFG_DO_CHECKS
        TProj Proj2{Proj};
#endif /* JAFG_DO_CHECKS */
        const bool bRemoved{erase_once_fn{}(Container, What, std::move(Proj))};
        check(bRemoved)
        check(erase_once_fn{}(Container, What, std::move(Proj2)) == false && "Encountered multiple elements that are equal to #What.")
        return bRemoved;
    }
};

struct join_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TProj = identity>
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
    template<input_range TRange, typename TProj = identity>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& R, TProj Proj = {}, LStringView Separator = ", ") const -> LString
    {
        return (*this)(begin(R), end(R), std::move(Proj), Separator);
    }
};

struct add_spaces_to_camel_case_fn final
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent, typename TOut>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TIter First, TSent Sent) const -> TOut
    {
        TOut Result;
        if constexpr (algo::is_reservable_v<TOut>)
        {
            Result.reserve(algo::distance(First, Sent));
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

#if PLATFORM_WINDOWS
struct utf8_to_utf16_fn final
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

struct utf16_to_utf8_fn final
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
#endif /* PLATFORM_WINDOWS */

} /* ~Namespace detail */

inline constexpr detail::reserve_fn reserve{};

//# Trivial distance.
using std::distance;
template<input_range TRange, typename TIter>
FORCEINLINE constexpr decltype(auto) distance(TRange const& Range, TIter It) noexcept
{
    return std::ranges::distance(begin(Range), It);
}
//# Distance to the first element that matches.
inline constexpr detail::distance_to_fn distance_to{};
inline constexpr detail::valid_index_fn valid_index{};

inline constexpr detail::contains_if_fn contains_if{};
inline constexpr detail::contains_if_checked_fn contains_checked_if{};
inline constexpr detail::contains_if_asserted_fn contains_asserted_if{};
inline constexpr detail::find_checked_fn find_checked{};
inline constexpr detail::find_if_checked_fn find_if_checked{};
inline constexpr detail::find_if_asserted_fn find_if_asserted{};
//# Get a pointer to the found element or nullptr.
inline constexpr detail::find_pointer_fn find_pointer{};
inline constexpr detail::find_pointer_checked_fn find_pointer_checked{};
//# Get a pointer to the found element or nullptr.
inline constexpr detail::find_pointer_if_fn find_pointer_if{};

//# Erases all matching elements. Returns number of removed elements.
inline constexpr detail::erase_fn erase{};
//# Erases all matching elements. Returns number of removed elements.
inline constexpr detail::erase_if_fn erase_if{};
//# Erase an element from a range if it exists. @return True, if successfully deleted.
inline constexpr detail::erase_once_fn erase_once{};
//# Checks that the element is removed. There may still be more elements that equal the #What.
inline constexpr detail::erase_once_checked_fn erase_once_checked{};
inline constexpr detail::erase_once_asserted_fn erase_once_asserted{};
//# Remove an element exactly once. If checks are enabled, unable to remove once or finding duplicate elements will result in a program panic.
inline constexpr detail::erase_exactly_once_checked_fn erase_exactly_once_checked{};

//# Recursive joins elements to a string.
inline constexpr detail::join_fn join{};
inline constexpr detail::add_spaces_to_camel_case_fn add_spaces_to_camel_case{};
#if PLATFORM_WINDOWS
    inline constexpr detail::utf8_to_utf16_fn utf8_to_utf16{};
    inline constexpr detail::utf16_to_utf8_fn utf16_to_utf8{};
#endif /* PLATFORM_WINDOWS */

struct case_insensitive_hash final
{
    inline std::size_t operator()(auto const& Key) const noexcept { return (*this)(Key.data(), Key.size(), 0); }
    inline std::size_t operator()(char const* Str, std::size_t Size, std::size_t H) const noexcept
    {
        if (Size == 0) { return H; }
        return (*this)(Str + 1, Size - 1, (((std::numeric_limits<std::size_t>::max)() >> 6) & H * 33) ^ static_cast<unsigned char>(std::tolower(*Str)));
    }
};
struct case_insensitive_equal_to final
{
    inline std::size_t operator()(auto const& Lhs, auto const& Rhs) const noexcept
    {
        return
               algo::size(Lhs) == algo::size(Rhs)
            && std::equal(algo::begin(Lhs), algo::end(Lhs), algo::begin(Rhs), [](auto A, auto B) { return std::tolower(A) == std::tolower(B); });
    }
};

//# Subs a container.
template<random_access_range T, typename U = T>
    requires std::constructible_from<U, iterator_t<T>, iterator_t<T>>
FORCEINLINE U sub(T const& Container, range_size_t<T> Begin, range_size_t<T> End) noexcept
{
    check(Begin <= End && End <= size(Container))
    return U{begin(Container) + Begin, begin(Container) + End};
}
//# Chops N elements from the left.
template<random_access_range T, typename U = T>
    requires std::constructible_from<U, iterator_t<T>, sentinel_t<T>>
FORCEINLINE U right_chop(T const& Container, range_size_t<T> N)
{
    check(N <= size(Container))
    return U{begin(Container) + N, end(Container)};
}
template<random_access_range T>
    requires requires(T& t){t.erase(begin(t), begin(t) + 0uz);}
FORCEINLINE void inline_right_chop(T* Container, range_size_t<T> N) noexcept
{
    check(Container)
    check(N <= size(*Container))
    Container->erase(begin(*Container), begin(*Container) + N);
    return;
}
template<random_access_range T>
    requires requires(T& t){t.erase(begin(t), begin(t) + 0uz); t.shrink_to_fit();}
FORCEINLINE void inline_right_chop_shrink(T* Container, range_size_t<T> N) noexcept
{
    inline_right_chop(Container, N);
    Container->shrink_to_fit();
    return;
}
//# Chops N elements from the right.
template<random_access_range T, typename U = T>
    requires std::constructible_from<U, iterator_t<T>, sentinel_t<T>>
FORCEINLINE U left_chop(T const& Container, range_size_t<T> N) noexcept
{
    check(N <= size(Container))
    return U{begin(Container), end(Container) - N};
}
template<random_access_range T>
    requires requires(T& t){t.erase(end(t) - 0uz, end(t));}
FORCEINLINE void inline_left_chop(T* Container, range_size_t<T> N) noexcept
{
    check(Container)
    check(N <= size(*Container))
    Container->erase(end(*Container) - N, end(*Container));
    return;
}
template<random_access_range T>
    requires requires(T& t){t.erase(end(t) - 0uz, end(t)); t.shrink_to_fit();}
FORCEINLINE void inline_left_chop_shrink(T* Container, range_size_t<T> N) noexcept
{
    inline_left_chop(Container, N);
    Container->shrink_to_fit();
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
template<typename TProj = identity>
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
template<typename TProj = identity>
NODISCARD FORCEINLINE constexpr auto wfind_pointer(RANGE Container, const auto& Value, TProj Proj = {}) noexcept
{
    return algo::wfind_pointer(algo::begin(Container), algo::end(Container), Value, std::move(Proj));
}

///////////////////////////////////////////////////////////////////////////////
// Time stuff
typedef std::chrono::high_resolution_clock clock;
inline decltype(auto) now() noexcept
{
    return clock::now();
}

inline f64 time_diff(clock::time_point A, clock::time_point B) noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<f64>>(B - A).count();
}

///////////////////////////////////////////////////////////////////////////////
// Misc
template<typename T> concept bool_testable = requires(T&& t) { static_cast<bool>(t); };

namespace detail
{
//# This is UB, use only when it does not really matter.
template<typename T, typename = void> struct is_complete_type : std::false_type {};
template<typename T> struct is_complete_type<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};
template<typename T> inline constexpr bool is_complete_type_v{is_complete_type<T>::value};
} /* ~Namespace detail */

template<typename TBase, typename TDerived, bool = detail::is_complete_type_v<TBase> && detail::is_complete_type_v<TDerived>>
struct is_base_of_weak : std::true_type{};
template<typename TBase, typename TDerived>
struct is_base_of_weak<TBase,TDerived,true> : std::bool_constant<std::is_base_of_v<TBase,TDerived>>{};
template<typename TBase, typename TDerived> inline constexpr bool is_base_of_weak_v{is_base_of_weak<TBase,TDerived>::value};

struct raii_leave final
{
    std::move_only_function<void()> Delegate;
    ~raii_leave()
    {
        if (Delegate)
        {
            Delegate();
        }
    }
};

struct reply_base
{
    FORCEINLINE constexpr reply_base() noexcept : bHandled{} {}
    FORCEINLINE constexpr reply_base(bool bInHandled) noexcept : bHandled{bInHandled} {}
    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(reply_base)
    FORCEINLINE constexpr ~reply_base() noexcept {}

    FORCEINLINE constexpr bool is_handled() const noexcept { return this->bHandled; }

private:

    bool bHandled;
};

struct reply final : public reply_base
{
    FORCEINLINE static constexpr reply handled() noexcept { return reply{true}; }
    FORCEINLINE static constexpr reply unhandled() noexcept { return {}; }
};

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
