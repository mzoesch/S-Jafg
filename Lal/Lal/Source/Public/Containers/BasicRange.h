// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template<typename TRange, typename T>
concept ContainerCompatibleRange =
#if LAL_WITH_CLANG
    std::__detail::__container_compatible_range<TRange, T>;
#else /* LAL_WITH_CLANG */
    #error "Missing implementation for compiler."
#endif /* !LAL_WITH_CLANG */

template<typename TRange>
concept CRange = requires(TRange Range)
{
    Range.begin();
    { Range.end() } -> std::sentinel_for<decltype(Range.begin())>;
};

#define RANGE ::Lal::CRange auto&&

} /* ~Namespace Lal */
