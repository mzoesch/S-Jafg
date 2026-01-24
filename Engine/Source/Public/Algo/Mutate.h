// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

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

        bool bFirst{true};
        while (First != Sent)
        {
            if (*First >= 'A' && *First <= 'Z')
            {
                if (bFirst == false)
                {
                    Result.push_back(' ');
                }
            }

            Result.push_back(*First);

            bFirst = false;
            continue;
        }

        return Result;
    }

    template<std::ranges::input_range TRange, typename TOut = std::remove_reference_t<TRange>>
    NODISCARD FORCEINLINE constexpr auto
    operator()(TRange&& R) const -> TOut
    {
        return (*this).template operator()<decltype(algo::begin(R)), decltype(algo::end(R)), TOut>(algo::begin(R), algo::end(R));
    }
};

} /* ~Namespace detail */
inline constexpr detail::add_spaces_to_camel_case_fn add_spaces_to_camel_case{};

} /* ~Namespace algo */
