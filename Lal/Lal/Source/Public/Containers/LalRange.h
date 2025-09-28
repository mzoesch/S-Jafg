// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename T> concept CRange            = std::ranges::range<T>;
template <typename T> concept CBorrowedRange    = std::ranges::borrowed_range<T>;

} /* ~Namespace Lal */

namespace Ranges
{

template <Lal::CRange _Range> using Difference_t    = std::ranges::range_difference_t<_Range>;
template <Lal::CRange _Range> using Value_t         = std::ranges::range_value_t<_Range>;
template <Lal::CRange _Range> using Reference_t     = std::ranges::range_reference_t<_Range>;

//#
//# Checks whether the given range is empty.
//#
template <typename Container> requires requires (Container C) { C.empty(); }
NODISCARD
FORCEINLINE constexpr auto IsEmpty(Container const& C) noexcept(noexcept(C.empty())) -> decltype(C.empty())
{
    return C.empty();
}
template <typename Container> requires requires (Container C) { !C.empty() && C.IsEmpty(); }
NODISCARD
FORCEINLINE constexpr auto IsEmpty(Container const& C) noexcept(noexcept(C.IsEmpty())) -> decltype(C.IsEmpty())
{
    return C.IsEmpty();
}
template <typename T, LSize N>
NODISCARD
FORCEINLINE bool IsEmpty(T const(&)[N]) noexcept
{
    return N == 0;
}
template <typename T>
NODISCARD
FORCEINLINE bool IsEmpty(const std::initializer_list<T> List) noexcept
{
    return List.size() == 0;
}

//#
//# Will check if the container is emptieable.
//#
template <typename Container>
struct HasEmpty : Lal::FalseType { };
template <typename Container> requires requires (Container C) { C.clear(); }
struct HasEmpty<Container> : Lal::TrueType { };
template <typename Container> requires requires (Container C) { !C.clear() && C.Empty(); }
struct HasEmpty<Container> : Lal::TrueType { };
template <typename Container>
inline constexpr bool HasEmpty_v = HasEmpty<Container>::value;

//#
//# Will empty the given container.
//#
template <typename Container> requires requires (Container C) { C.clear(); }
FORCEINLINE constexpr auto Empty(Container& C) noexcept(noexcept(C.clear())) -> decltype(C.clear())
{
    return C.clear();
}
template <typename Container> requires requires (Container C) { !C.clear() && C.Empty(); }
FORCEINLINE constexpr auto Empty(Container& C) noexcept(noexcept(C.Empty())) -> decltype(C.Empty())
{
    return C.Empty();
}

} /* ~Namespace Range */

namespace Lal
{

template <typename T> concept CContainer = CRange<T> && Ranges::HasEmpty_v<T>;

} /* ~Namespace Lal */
