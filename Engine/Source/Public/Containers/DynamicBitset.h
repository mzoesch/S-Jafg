// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

struct LDynamicBitset final
{
    constexpr LDynamicBitset() noexcept = default;
    LDynamicBitset(u64 BitCount) noexcept : BitCount(BitCount), Words((BitCount + 63) / 64, 0) {}

    //#
    //# Set the first free bit.
    //# @return The index of the set bit, or size max if no free found.
    //#
    u64 Allocate() noexcept
    {
        for (auto WordIdx{0uz}; WordIdx < Words.size(); ++WordIdx)
        {
            u64 Word = Words[WordIdx];

            if (Word != std::numeric_limits<u64>::max())
            {
                u64 freeBits = ~Word;

                unsigned bit = std::countr_zero(freeBits);

                u64 index = WordIdx * 64 + bit;

                if (index >= BitCount)
                {
                    return std::numeric_limits<u64>::max();
                }

                Words[WordIdx] |= (1ull << bit);
                return index;
            }
        }

        return std::numeric_limits<u64>::max();
    }

    void Free(u64 Idx) noexcept
    {
        u64 Word = Idx / 64;
        u64 Bit  = Idx % 64;
        Words[Word] &= ~(1ull << Bit);
    }

    bool IsAllocated(u64 Index) const noexcept
    {
        u64 word = Index / 64;
        LSize bit  = Index % 64;
        return (Words[word] >> bit) & 1;
    }

    FORCEINLINE constexpr u64 GetBitCount() const noexcept { return this->BitCount; }
    FORCEINLINE constexpr auto& GetWords() noexcept { return this->Words; }
    FORCEINLINE constexpr auto const& GetWords() const noexcept { return this->Words; }

private:

    u64 BitCount{};
    std::vector<u64> Words;
};

} /* ~Namespace Jafg */
