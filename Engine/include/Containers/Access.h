// Copyright mzoesch. All rights reserved.

#pragma once

#if JAFG_WITH_CLANG
    #define JAFG_RANGE_VAL_T(It, Proj) // _GLIBCXX26_RANGE_ALGO_DEF_VAL_T(It, Proj)
#else /* JAFG_WITH_CLANG */
    //# TODO: Add security for non clang toolchains.
    #define JAFG_RANGE_VAL_T(It, Proj)
#endif /* !JAFG_WITH_CLANG */

namespace algo
{

//# The maximal aligned type.
typedef f64 max_align_t;

template<typename...> struct always_false : std::false_type {};
template<typename... T> inline constexpr bool always_false_v{ always_false<T...>::value };

inline constexpr bool is_u64_size_t_v{ std::is_same_v<u64, std::size_t> };
inline constexpr bool is_u32_size_t_v{ std::is_same_v<u32, std::size_t> };

inline constexpr bool is_little_endian{ std::endian::native == std::endian::little };
inline constexpr bool is_big_endian{ std::endian::native == std::endian::big };

inline constexpr std::size_t wchar_size{ sizeof(wchar_t) };
inline constexpr std::size_t ptr_size{ sizeof(void*) };

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
struct unique_raw_t final
{
    template<typename T, typename Deleter> NODISCARD FORCEINLINE constexpr T*
    operator()(TUnique<T,Deleter> const& Ptr) const noexcept { return Ptr.get(); }
};
inline constexpr unique_raw_t unique_raw{};
//# Projection to a dereferenced unique pointer const reference.
struct unique_deref_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr T&
    operator()(TUnique<T> const& Ptr) const noexcept { check(Ptr.get()) return *Ptr.get(); }
};
inline constexpr unique_deref_t unique_deref{};
//# Projection to a dereferenced raw pointer.
struct raw_pointer_deref_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr T&
    operator()(T* Ptr) const noexcept { check(Ptr) return *Ptr; }
};
inline constexpr raw_pointer_deref_t raw_pointer_deref{};
//# Projection to the first item in a pair.
struct pair_first_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Pair) const noexcept { return std::forward<T>(Pair).first; }
};
inline constexpr pair_first_t pair_first{};
//# Projection to the second item in a pair.
struct pair_second_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Pair) const noexcept { return std::forward<T>(Pair).second; }
};
inline constexpr pair_second_t pair_second{};
//# The record from a projection to a member of said record.
template<typename T> struct proj_record;
template<typename T, typename U> struct proj_record<U T::*> { typedef T type; };
template<typename T> using proj_record_t = typename proj_record<T>::type;
//# The member from a projection to this member of any record.
template<typename T> struct proj_member;
template<typename T, typename U> struct proj_member<U T::*> { typedef U type; };
template<typename T> using proj_member_t = typename proj_member<T>::type;
struct universal_deref_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return *Value; }
};
inline constexpr universal_deref_t universal_deref{};
struct universal_ref_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return &Value; }
};
inline constexpr universal_ref_t universal_ref{};
struct universal_ptr_noop_t final
{
    template<typename T> NODISCARD FORCEINLINE constexpr decltype(auto)
    operator()(T&& Value) const noexcept { return &*Value; }
};
inline constexpr universal_ptr_noop_t universal_ptr_noop{};

//# Whether a container can reserve memory.
template<typename T>
inline constexpr bool is_reservable_v = sized_range<T>
    && requires(T& t, range_size_t<T> n)
    {
        t.reserve(n);
        { t.capacity() } -> std::same_as<decltype(n)>;
        { t.max_size() } -> std::same_as<decltype(n)>;
    };

#if JAFG_WITH_CLANG
namespace detail
{

template<typename T>
struct access_protected_members_of_vector: std::vector<T>
{
    UTILITY_STRUCT(access_protected_members_of_vector)

    NODISCARD
    consteval
    static typename std::vector<T>::size_type begin() noexcept
    {
        return offsetof(access_protected_members_of_vector, _M_impl._M_start);
    }

    NODISCARD
    consteval
    static typename std::vector<T>::size_type slack() noexcept
    {
        return offsetof(access_protected_members_of_vector, _M_impl._M_finish);
    }

    NODISCARD
    consteval
    static typename std::vector<T>::size_type end() noexcept
    {
        return offsetof(access_protected_members_of_vector, _M_impl._M_end_of_storage);
    }
};

} /* detail */
#elif JAFG_WITH_MSVC
    //# ... private and not protected :( We prob. want something like a reinterpret cast.
#else /* JAFG_WITH_MSVC */
#endif /* !JAFG_WITH_MSVC */

template<typename T>
#if !JAFG_WITH_CLANG
    //#
    //# If this hits, we have to decide for another impl...
    //# Currently this is not needed anymore, yea :D
    //#
    requires std::is_default_constructible_v<typename T::value_type>
#endif /* !JAFG_WITH_CLANG */
//#
//# Allows one to add uninitialized elements to a std::vector<T> without calling the default constructor of T
//# with is usually forbidden as it leads to UB (as if the std actually cares about eliminating UB...).
//#
FORCEINLINE void add_uninitialized(std::vector<T>* Container, std::size_t Count = 1) noexcept
{
    check(Container)

    typedef typename std::vector<T>::value_type value_type;
    typedef typename std::vector<T>::size_type size_type;
    typedef typename std::vector<T>::pointer pointer;

#if JAFG_WITH_CLANG

#if JAFG_DO_CHECKS
    size_type OldSize{Container->size()};
#endif /* JAFG_DO_CHECKS */

    Container->reserve(Container->size() + Count);

    pointer& Slack{*reinterpret_cast<pointer*>
        (
            reinterpret_cast<u8*>(Container) + detail::access_protected_members_of_vector<value_type>::slack()
        )};
    check(Slack)
    check(static_cast<void const*>(std::to_address(Container->end())) == static_cast<void const*>(Slack))
    Slack += Count;

    check(Container->size() == OldSize + Count)
    check(Container->size() <= Container->capacity())
#else /* JAFG_WITH_CLANG */

    Container->resize(Container->size() + Count);

#endif /* !JAFG_WITH_CLANG */
}

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
    NODISCARD FORCEINLINE constexpr std::iter_difference_t<TIter>
    operator()(TIter Begin, TSent Sent, T const& Value, TProj Proj = {}) const
    {
        std::iter_difference_t<TIter> N{};
        while (Begin != Sent)
        {
            if (std::invoke(Proj, *Begin++) == Value)
            {
                break;
            }
            ++N;
        }
        return N;
    }
    template<input_range TRange, typename TProj = identity, typename T>
        requires std::indirect_binary_predicate<equal_to, std::projected<iterator_t<TRange>, TProj>, T const*>
    NODISCARD FORCEINLINE constexpr range_difference_t<TRange>
    operator()(TRange&& Range, T const& Value, TProj Proj = {}) const
    {
        return (*this)(begin(Range), end(Range), Value, std::move(Proj));
    }
};

struct valid_index_fn final
{
    template<forward_range TRange, typename T>
        requires(std::is_integral_v<T> || std::is_enum_v<T>)
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
        const auto OldSize{size(*Container)};
        Container->erase(It, End);
        return OldSize - size(*Container);
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

#if JAFG_PLATFORM_USES_UTF16
struct utf8_to_utf16_fn final
{
    template<std::contiguous_iterator TIt, std::unsigned_integral TSize>
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
    template<std::contiguous_iterator TIt, std::unsigned_integral TSize>
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
#endif /* JAFG_PLATFORM_USES_UTF16 */

struct utf8_to_native_fn final
{
    template<std::contiguous_iterator TIt, std::unsigned_integral TSize>
    NODISCARD FORCEINLINE LNativeString operator()(TIt It, TSize Size) const noexcept
    {
#if JAFG_PLATFORM_USES_UTF16
        return utf8_to_utf16_fn{}(It, Size);
#else /* JAFG_PLATFORM_USES_UTF16 */
        LNativeString Result; Result.reserve(Size);
        for (; Size > 0; --Size)
        {
            Result.push_back(*It++);
        }
        return Result;
#endif /* !JAFG_PLATFORM_USES_UTF16 */
    }
    template<contiguous_range TRange>
    NODISCARD FORCEINLINE LNativeString operator()(TRange&& Range) const noexcept
    {
#if JAFG_PLATFORM_USES_UTF16
        return (*this)(algo::begin(Range), algo::size(Range));
#else /* JAFG_PLATFORM_USES_UTF16 */
        return std::forward<TRange>(Range);
#endif /* !JAFG_PLATFORM_USES_UTF16 */
    }
};

struct utf8_char_length_fn final
{
    template<typename T> requires(std::is_same_v<T, char> || std::is_same_v<T, char8_t>
        || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> || std::is_same_v<T, wchar_t>)
    NODISCARD FORCEINLINE constexpr std::size_t operator()(T Char) const noexcept
    {
        if ((Char & 0x80) == 0) { return 1; } /* ASCII byte. */
        if ((Char & 0xE0) == 0xC0) { return 2; } /* 2-byte sequence. */
        if ((Char & 0xF0) == 0xE0) { return 3; } /* 3-byte sequence. */
        if ((Char & 0xF8) == 0xF0) { return 4; } /* 4-byte sequence. */
        checkNoEntry() return 1; /* Invalid UTF-8; We return one so we do not hang indefinitely. */
    }
};

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
#if JAFG_PLATFORM_WINDOWS
    inline constexpr detail::utf8_to_utf16_fn utf8_to_utf16{};
    inline constexpr detail::utf16_to_utf8_fn utf16_to_utf8{};
#endif /* JAFG_PLATFORM_WINDOWS */
//#
//# A function that consumes the UTF-8, and converts it to the native string type.
//# If the native string type is already provided, then this results in a noop.
//#
inline constexpr detail::utf8_to_native_fn utf8_to_native{};
inline constexpr detail::utf8_char_length_fn utf8_char_length{};

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
//# Subs from N until the right end.
template<random_access_range T, typename U = T>
    requires std::constructible_from<U, iterator_t<T>, sentinel_t<T>>
FORCEINLINE U right_sub(T const& Container, range_size_t<T> N) noexcept
{
    return sub<T,U>(Container, N, size(Container));
}
//# Subs from the left until N.
template<random_access_range T, typename U = T>
    requires std::constructible_from<U, iterator_t<T>, sentinel_t<T>>
FORCEINLINE U left_sub(T const& Container, range_size_t<T> N) noexcept
{
    return sub<T,U>(Container, 0, N);
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

struct to_array_fn
{
    template <input_range TRange>
    auto operator()(TRange&& Range) const noexcept
    {
        typedef range_value_t<TRange> T;
        TArray<T> Result; Result.reserve(this->Reservation);
        for (auto&& Element: Range)
        {
            Result.emplace_back(std::forward<decltype(Element)>(Element));
        }
        return Result;
    }

    std::size_t Reservation{};
};
//# Transform a range into TArray. You may use to_array_fn{N} to reserve N elements.
inline constexpr to_array_fn to_array{};

template<input_range TRange>
NODISCARD FORCEINLINE decltype(auto) operator|(TRange&& Range, to_array_fn const& F) noexcept
{
    return F(std::forward<decltype(Range)>(Range));
}

///////////////////////////////////////////////////////////////////////////////
// Weak - weak implementations of algorithms that require less correctly specified traits -- as the standard really
// tries to force one to define literally mathematical perfect reflexive, symmetric, and transitive type comparisons
// for all types involved in any way imaginable forming a total order even for finding elements!??... why
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_v{requires(TLhs const& Lhs, TRhs const& Rhs){ {Lhs==Rhs} -> std::convertible_to<bool>; }};
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_three_way_v{requires(TLhs const& Lhs, TRhs const& Rhs){
    {Lhs<=>Rhs} -> std::convertible_to<std::partial_ordering>;
    {Lhs<=>Rhs} -> std::convertible_to<std::weak_ordering>;
    {Lhs<=>Rhs} -> std::convertible_to<std::strong_ordering>;
    }};
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_lt_v{requires(TLhs const& Lhs, TRhs const& Rhs){ {Lhs<Rhs} -> std::convertible_to<bool>; }};
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_lteq_v{requires(TLhs const& Lhs, TRhs const& Rhs){ {Lhs<=Rhs} -> std::convertible_to<bool>; }};
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_gt_v{requires(TLhs const& Lhs, TRhs const& Rhs){ {Lhs>Rhs} -> std::convertible_to<bool>; }};
template<typename TLhs, typename TRhs>
inline constexpr bool is_weak_eq_gteq_v{requires(TLhs const& Lhs, TRhs const& Rhs){ {Lhs>=Rhs} -> std::convertible_to<bool>; }};

template<typename TIter, typename TSent, typename TComp, typename TProj> requires std::sentinel_for<TSent, TIter>
inline constexpr bool is_it_comparable_v{requires(TIter Begin, TSent Sent, TComp Comp, TProj Proj)
{
    {Comp(Proj(*Begin), Proj(*Sent))} -> std::convertible_to<bool>;
}};
template<typename TRange, typename TComp, typename TProj>
inline constexpr bool is_range_comparable_v{requires(iterator_t<TRange> Begin, sentinel_t<TRange> Sent, TComp Comp, TProj Proj)
{
    {Comp(Proj(*Begin), Proj(*Sent))} -> std::convertible_to<bool>;
}};

struct equal_to_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs == Rhs;
    }
};
inline constexpr equal_to_weak_fn equal_to_weak{};
struct three_way_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_three_way_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr decltype(auto) operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs <=> Rhs;
    }
};
inline constexpr three_way_weak_fn three_way_weak{};
struct less_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_lt_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs < Rhs;
    }
};
inline constexpr less_weak_fn less_weak{};
struct less_equal_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_lteq_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs <= Rhs;
    }
};
inline constexpr less_equal_weak_fn less_equal_weak{};
struct greater_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_gt_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs > Rhs;
    }
};
inline constexpr greater_weak_fn greater_weak{};
struct greater_equal_weak_fn final
{
    template<typename TLhs, typename TRhs>
        requires is_weak_eq_gteq_v<TLhs, TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs const& Lhs, TRhs const& Rhs) const
    {
        return Lhs >= Rhs;
    }
};
inline constexpr greater_equal_weak_fn greater_equal_weak{};

struct lexicographical_string_compare_fn
{
    template<range TLhs, range TRhs>
    NODISCARD FORCEINLINE constexpr bool operator()(TLhs&& Lhs, TRhs&& Rhs) const
    {
        auto N{std::min(algo::size(Lhs), algo::size(Rhs))};
        for (auto Idx{0uz}; Idx < N; ++Idx)
        {
            auto LhsLetter{Lhs[Idx]};
            auto RhsLetter{Rhs[Idx]};

            auto LhsLower{std::tolower(LhsLetter)};
            auto RhsLower{std::tolower(RhsLetter)};

            if (LhsLower != RhsLower)
            {
                return LhsLower < RhsLower;
            }

            if (LhsLetter != RhsLetter)
            {
                return LhsLetter < RhsLetter;
            }
        }
        return algo::size(Lhs) < algo::size(Rhs);
    }
};
inline constexpr lexicographical_string_compare_fn lexicographical_string_compare{};

namespace detail
{

template<typename TProj, typename TComp>
struct reflexive_proj_for_weak_stl final
{
    TComp Comp;
    TProj Proj;
    template<typename T>
    NODISCARD FORCEINLINE bool operator()(T&& Lhs, T&& Rhs) const noexcept
    {
        return this->Comp(std::invoke(this->Proj, std::forward<T>(Lhs)), std::invoke(this->Proj, std::forward<T>(Rhs)));
    }
};

struct sort_weak_fn
{
    template<std::random_access_iterator TIter, std::sentinel_for<TIter> TSent, typename TComp=less_weak_fn, typename TProj=identity>
        requires is_it_comparable_v<TIter, TSent, TComp, TProj>
    constexpr void
    operator()(TIter Begin, TSent Sent, TComp Comp={}, TProj Proj={}) const noexcept
    {
        std::sort(Begin, Sent, reflexive_proj_for_weak_stl{.Comp=std::move(Comp), .Proj=std::move(Proj)});
    }
    template<random_access_range TRange, typename TComp=less_weak_fn, typename TProj=identity>
        requires is_range_comparable_v<TRange, TComp, TProj>
    constexpr void
    operator()(TRange&& Range, TComp Comp={}, TProj Proj={}) const noexcept
    {
        (*this)(begin(Range), end(Range), std::move(Comp), std::move(Proj));
    }
};
} /* ~Namespace detail */

//# Weak implementation of the std::ranges::sort that does not require mathematically perfect comparisons.
inline constexpr detail::sort_weak_fn sort_weak{};

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

//#
//# Clock used by jafg.
//# Warning: Do not use high resolution clocks as they are affected by timezones and user preferences
//# which could lead to irreversible broken engine states.
//#
typedef std::chrono::steady_clock clock;

inline decltype(auto) now() noexcept
{
    return clock::now();
}

template<typename T = f64, typename TRatio = std::chrono::seconds::period>
inline T time_diff(clock::time_point A, clock::time_point B) noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<T, TRatio>>(B - A).count();
}

///////////////////////////////////////////////////////////////////////////////
// Misc
template<typename T> concept bool_testable = requires(T&& t) { static_cast<bool>(t); };

struct dynamic_bit_set final
{
    typedef std::size_t size_type;
    static constexpr size_type npos{std::numeric_limits<size_type>::max()};

    constexpr dynamic_bit_set() noexcept = default;
    explicit dynamic_bit_set(size_type Count) noexcept : BitCount{Count}, Words((BitCount + 63) / 64, 0) {}

    //#
    //# Set the first free bit.
    //# @return The index of the set bit, or npos if no free found.
    //#
    NODISCARD size_type allocate() noexcept
    {
        for (auto WordIdx{0uz}; WordIdx < this->Words.size(); ++WordIdx)
        {
            size_type Word{this->Words[WordIdx]};
            if (Word != std::numeric_limits<u64>::max())
            {
                size_type FreeBits{~Word};
                size_type Bit{static_cast<size_type>(std::countr_zero(FreeBits))};
                size_type Idx{WordIdx * 64 + Bit};

                if (Idx >= this->BitCount)
                {
                    return npos;
                }

                this->Words[WordIdx] |= (1ull << Bit);
                return Idx;
            }
        }
        return npos;
    }

    void free(size_type Idx) noexcept
    {
        size_type Word{Idx / 64};
        size_type Bit{Idx % 64};
        this->Words[Word] &= ~(1ull << Bit);
    }

    NODISCARD bool allocated(size_type Idx) const noexcept
    {
        size_type Word{Idx / 64};
        size_type Bit{Idx % 64};
        return (this->Words[Word] >> Bit) & 1;
    }

    NODISCARD constexpr size_type bit_count() const noexcept { return this->BitCount; }
    NODISCARD constexpr auto const& words() const noexcept { return this->Words; }

private:

    size_type BitCount{};
    std::vector<u64> Words;
};

//#
//# Stores trivial non-copyable data in a non-dynamic bulk container. Relies heavily on UB to enable optimizations.
//# @note This container intentionally does not support copy operations (even though they are trivial), as it is
//#       meant for very large binary data that should not be copied around.
//#       If you still need to copy in bulk, you may use #serialize.
//#
template<typename T> requires std::is_trivially_copyable_v<T>
struct bulk_data
{
    typedef T value_type;

    FORCEINLINE constexpr bulk_data() noexcept : m_size{}, m_bulk{} {}
    PROHIBIT_COPY(bulk_data)
    constexpr bulk_data(bulk_data&& Other) noexcept
        : m_size{std::exchange(Other.m_size, 0)}, m_bulk{std::exchange(Other.m_bulk, nullptr)} {}
    constexpr bulk_data& operator=(bulk_data&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            if (this->allocated())
            {
                this->free_impl();
            }
            this->m_size = std::exchange(Rhs.m_size, 0);
            this->m_bulk = std::exchange(Rhs.m_bulk, nullptr);
        }
        return *this;
    }
    constexpr ~bulk_data() noexcept
    {
        if (this->allocated())
        {
            this->free_impl();
        }
    }

    NODISCARD FORCEINLINE constexpr bool allocated() const noexcept { return !!this->m_bulk; }

    NODISCARD FORCEINLINE constexpr std::size_t size() const noexcept { return this->m_size; }
    NODISCARD FORCEINLINE constexpr std::size_t byte_size() const noexcept { return this->size() * sizeof(value_type); }

    NODISCARD FORCEINLINE constexpr value_type* data() noexcept { return this->m_bulk; }
    NODISCARD FORCEINLINE constexpr value_type const* data() const noexcept { return this->m_bulk; }
    NODISCARD FORCEINLINE constexpr value_type& operator*() noexcept { return this->m_bulk; }
    NODISCARD FORCEINLINE constexpr value_type const& operator*() const noexcept { return this->m_bulk; }

    NODISCARD FORCEINLINE constexpr auto begin() noexcept { return this->m_bulk; }
    NODISCARD FORCEINLINE constexpr auto begin() const noexcept { return this->m_bulk; }
    NODISCARD FORCEINLINE constexpr auto end() noexcept { return this->m_bulk + this->size(); }
    NODISCARD FORCEINLINE constexpr auto end() const noexcept { return this->m_bulk + this->size(); }

    NODISCARD FORCEINLINE constexpr value_type& operator[](std::size_t Idx) noexcept
    {
        check(this->m_bulk)
        check(Idx < this->size())
        return this->m_bulk[Idx];
    }
    NODISCARD FORCEINLINE constexpr value_type const& operator[](std::size_t Idx) const noexcept
    {
        check(this->m_bulk)
        check(Idx < this->size())
        return this->m_bulk[Idx];
    }

    FORCEINLINE constexpr void allocate(std::size_t Size) noexcept
    {
        check(!this->allocated())
        check(Size > 0)
        this->m_size = Size;
        this->m_bulk = new value_type[this->m_size];
    }

    FORCEINLINE void allocate_zeroed(std::size_t Size) noexcept
    {
        this->allocate(Size);
        std::memset(this->m_bulk, 0, this->byte_size());
    }

    FORCEINLINE void serialize(value_type const* Bulk, std::size_t Size, std::size_t Offset = 0)
    {
        check(!this->allocated())
        check(Bulk && Size > 0)
        this->allocate(Size);
        std::memcpy(this->m_bulk, Bulk + Offset, this->byte_size());
    }

    constexpr void free() noexcept
    {
        this->free_impl();
        this->release();
    }

    constexpr void release() noexcept
    {
        this->m_size = 0;
        this->m_bulk = nullptr;
    }

private:

    constexpr void free_impl() noexcept
    {
        check(this->m_bulk)
        delete[] this->m_bulk;
    }

    std::size_t m_size;
    value_type* m_bulk;
};

typedef bulk_data<std::byte> byte_bulk;

struct transparent_string_hash final
{
    typedef void is_transparent;

    decltype(auto) operator()(LStringView sv) const noexcept
    {
        return std::hash<LStringView>{}(sv);
    }

    decltype(auto) operator()(LString const& s) const noexcept
    {
        return std::hash<LStringView>{}(s);
    }

    decltype(auto) operator()(char const* s) const noexcept
    {
        return std::hash<LStringView>{}(s);
    }
};

struct transparent_string_eq final
{
    typedef void is_transparent;

    constexpr bool operator()(std::string_view Lhs, std::string_view Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
    constexpr bool operator()(LString const& Lhs, LString const& Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
    constexpr bool operator()(LString const& Lhs, char const* Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
    constexpr bool operator()(char const* Lhs, LString const& Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
    constexpr bool operator()(char const* Lhs, char const* Rhs) const noexcept
    {
        return std::strcmp(Lhs, Rhs) == 0;
    }
    constexpr bool operator()(std::string_view Lhs, char const* Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
    constexpr bool operator()(char const* Lhs, std::string_view Rhs) const noexcept
    {
        return Lhs == Rhs;
    }
};

//#
//# Transparent string map which allows looking up elements with a numerus number of string-like types
//# without expensive constructions of transient string objects.
//#
template<typename T>
using transparent_unordered_string_map = std::unordered_map<LString, T, transparent_string_hash, transparent_string_eq>;

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

template<typename T>
FORCEINLINE constexpr std::string_view type_name() noexcept
{
    constexpr std::string_view Signature{ JAFG_FUNCTION_SIG };
#if JAFG_WITH_MSVC
    constexpr std::string_view StructPrefix{ "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl algo::type_name<struct " };
    constexpr std::string_view ClassPrefix{ "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl algo::type_name<class " };
    constexpr std::string_view Suffix{ ">(void) noexcept" };

    if (Signature.starts_with(StructPrefix))
    {
        return Signature.substr(StructPrefix.size(), Signature.size() - StructPrefix.size() - Suffix.size());
    }
    return Signature.substr(ClassPrefix.size(), Signature.size() - ClassPrefix.size() - Suffix.size());
#elif JAFG_WITH_CLANG
    constexpr std::string_view Prefix{ "std::string_view algo::type_name() [T = " };
    constexpr std::string_view Suffix{ "]" };
    return Signature.substr(Prefix.size(), Signature.size() - Prefix.size() - Suffix.size());
#elif JAFG_WITH_GCC
    constexpr std::string_view Prefix{ "constexpr std::string_view algo::type_name() [with T = " };
    constexpr std::string_view Suffix{ "; std::string_view = std::basic_string_view<char>]" };
    return Signature.substr(Prefix.size(), Signature.size() - Prefix.size() - Suffix.size());
#else /* JAFG_WITH_GCC */
    #error "Missing compiler implementation."
#endif /* !JAFG_WITH_GCC */
}

struct raii_leave final
{
    std::move_only_function<void()> Delegate;

    raii_leave() = delete;
    raii_leave(std::move_only_function<void()> Delegate) noexcept : Delegate{std::move(Delegate)} {}
    PROHIBIT_COPY(raii_leave)
    raii_leave(raii_leave&&) noexcept = default;
    raii_leave& operator=(raii_leave&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            if (this->Delegate)
            {
                this->Delegate();
            }
            this->Delegate = std::move(Rhs.Delegate);
            check(!Rhs.Delegate)
        }
        return *this;
    }
    ~raii_leave()
    {
        if (this->Delegate)
        {
            this->Delegate();
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

template<typename... TArgs>
FORCEINLINE LString sprintf(char const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}
template<typename... TArgs>
FORCEINLINE LWString wsprintf(wchar_t const* Format, TArgs&&... Args) noexcept
{
    return std::vformat(Format, std::make_format_args(Args...));
}
template<typename... TArgs>
FORCEINLINE auto native_sprintf(LChar const* Format, TArgs&&... Args) noexcept
{
#if JAFG_PLATFORM_USES_UTF8
    return std::vformat(Format, std::make_format_args(Args...));
#else /* JAFG_PLATFORM_USES_UTF8 */
    return std::vformat(Format, std::make_wformat_args(Args...));
#endif /* !JAFG_PLATFORM_USES_UTF8 */
}

template<typename T, typename TDeleter>
FORCEINLINE constexpr std::unique_ptr<T,TDeleter>::pointer leak(std::unique_ptr<T, TDeleter>&& Ptr) noexcept
{
    return Ptr.release();
}

//# A storage that exchanges the value on move.
template<typename T, T Default = T{}>
struct exchange_storage
{
    typedef T value_type;

    FORCEINLINE constexpr exchange_storage() noexcept : Value{Default} {}
    template<typename... TArgs> requires std::constructible_from<T, TArgs...>
    FORCEINLINE constexpr exchange_storage(TArgs&&... Args) noexcept : Value{std::forward<TArgs>(Args)...} {}

    FORCEINLINE constexpr exchange_storage(exchange_storage const&) noexcept = default;
    FORCEINLINE constexpr exchange_storage(exchange_storage&& Other) noexcept : Value{std::exchange(Other.Value, Default)} {}
    FORCEINLINE exchange_storage& operator=(exchange_storage const&) noexcept = default;
    FORCEINLINE exchange_storage& operator=(exchange_storage&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            this->Value = std::exchange(Rhs.Value, Default);
        }
        return *this;
    }
    FORCEINLINE constexpr ~exchange_storage() noexcept = default;

    FORCEINLINE constexpr T exchange(T NewValue) noexcept { return std::exchange(this->Value, NewValue); }

    FORCEINLINE constexpr T& get() noexcept { return this->Value; }
    FORCEINLINE constexpr T const& get() const noexcept { return this->Value; }
    FORCEINLINE constexpr T& operator*() noexcept { return this->Value; }
    FORCEINLINE constexpr T const& operator*() const noexcept { return this->Value; }
    FORCEINLINE constexpr T const* operator->() const noexcept { return &this->Value; }
    FORCEINLINE constexpr T* operator->() noexcept { return &this->Value; }

    T Value;
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
