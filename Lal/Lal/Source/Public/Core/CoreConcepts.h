// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename T, typename U>
concept TEqualityComparableLeft = requires(const T& t, const U& u)
{
    { t == u } -> std::convertible_to<bool>;
};

} /* ~Namespace Lal */
