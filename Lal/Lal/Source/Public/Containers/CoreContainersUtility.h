// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

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

} /* ~Namespace algo */
