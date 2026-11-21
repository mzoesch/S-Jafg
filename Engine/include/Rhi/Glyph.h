// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

//# A collection of device-ready contiguous localized glyphs.
struct LGlyphCollection final
{
    struct Info final
    {
        // TODO: Replace with LRect2F
        LVec4F Rect{ maths::zero_vector<LVec4F> };
        LVec4F TexCoordRect{ maths::zero_vector<LVec4F> };
        u32 BindlessTextureIndex{};
        u32 SamplerIndex{};
        f32 MsdfPixelRange{};
        u32 Cluster{};
        LVec2F Pencil{ maths::zero_vector<LVec2F> };
    };

    TArray<Info> GlyphInfos;
    f32 Ascender{};
    f32 Descender{};
    f32 LineHeight{};

    LVec2F PencilBegin{ maths::zero_vector<LVec2F> };
    LVec2F PencilEnd{ maths::zero_vector<LVec2F> };

#if JAFG_DO_CHECKS
    LString _check_UsedString;
#endif /* JAFG_DO_CHECKS */

    FORCEINLINE constexpr decltype(auto) begin() const noexcept { return this->GlyphInfos.begin(); }
    FORCEINLINE constexpr decltype(auto) end() const noexcept { return this->GlyphInfos.end(); }

    //#
    //# Horizontal pencil location of a caret that is placed in front of the byte at the given index.
    //# A caret inside a cluster (e.g., a ligature like "fi") is linearly interpolated over the code points of that cluster.
    //# @param Text    The text that was used to shape this collection.
    //# @param ByteIdx Byte index into the text. Should be at the beginning of a code point.
    //#
    NODISCARD f32 caret_location(LStringView Text, std::size_t ByteIdx) const noexcept
    {
        f32 Result{this->PencilEnd.x};
        this->for_each_caret_stop(Text, [&](std::size_t Idx, f32 X) -> bool
        {
            if (Idx >= ByteIdx)
            {
                Result = X;
                return false;
            }
            return true;
        });
        return Result;
    }

    //#
    //# The byte index of the caret stop that is the closest to the given horizontal pencil location.
    //# @param Text The text that was used to shape this collection.
    //#
    NODISCARD std::size_t caret_index(LStringView Text, f32 Location) const noexcept
    {
        std::size_t Result{};
        f32 BestDistance{std::numeric_limits<f32>::max()};
        this->for_each_caret_stop(Text, [&](std::size_t Idx, f32 X) -> bool
        {
            if (f32 Distance{maths::abs(X - Location)}; Distance < BestDistance)
            {
                BestDistance = Distance;
                Result = Idx;
                return true;
            }
            /* Locations are monotonic for left to right text. Once we move away, we will never get closer again. */
            return false;
        });
        return Result;
    }

    //#
    //# Iterates over all possible caret stops (code point boundaries) in logical order, including the stop at the end
    //# of the text. The functor receives the byte index and the horizontal pencil location of the stop and returns
    //# false to stop the iteration.
    //# This is robust against glyphs that span multiple code points (ligatures) and multiple glyphs that share a
    //# code point (combining marks).
    //#
    template<typename TFunc>
    void for_each_caret_stop(LStringView Text, TFunc&& Func) const noexcept
    {
        auto NextCodePoint{[&Text](std::size_t Idx) noexcept -> std::size_t
        {
            ++Idx;
            while (Idx < Text.size() && (static_cast<u8>(Text[Idx]) & 0xC0) == 0x80)
            {
                ++Idx;
            }
            return Idx;
        }};

        auto It{this->GlyphInfos.begin()};
        while (It != this->GlyphInfos.end())
        {
            auto Next{It};
            std::size_t Glyphs{};
            while (Next != this->GlyphInfos.end() && Next->Cluster == It->Cluster)
            {
                ++Next;
                ++Glyphs;
            }

            const std::size_t Begin{maths::min(static_cast<std::size_t>(It->Cluster), Text.size())};
            const std::size_t End{Next == this->GlyphInfos.end() ? Text.size() : maths::min(static_cast<std::size_t>(Next->Cluster), Text.size())};
            const f32 BeginX{It->Pencil.x};
            const f32 EndX{Next == this->GlyphInfos.end() ? this->PencilEnd.x : Next->Pencil.x};

            if (End > Begin)
            {
                /*
                 * A single glyph for many code points is a ligature, and the caret may stop inside of it. Many glyphs
                 * in one cluster (e.g., a base and a combining mark) form one unit, and the caret only stops in front.
                 */
                std::size_t CodePoints{};
                for (auto Idx{Begin}; Idx < End; Idx = NextCodePoint(Idx))
                {
                    ++CodePoints;
                }
                const std::size_t Stops{Glyphs == 1uz ? CodePoints : 1uz};
                std::size_t Nth{};
                for (auto Idx{Begin}; Idx < End && Nth < Stops; Idx = NextCodePoint(Idx), ++Nth)
                {
                    const f32 X{BeginX + (EndX - BeginX) * (static_cast<f32>(Nth) / static_cast<f32>(Stops))};
                    if (!std::invoke(Func, Idx, X))
                    {
                        return;
                    }
                }
            }

            It = Next;
        }

        (void)std::invoke(Func, Text.size(), this->PencilEnd.x);
        return;
    }
};

} /* ~Namespace Jafg */
