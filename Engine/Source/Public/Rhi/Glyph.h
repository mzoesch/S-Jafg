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
    //# @param String A substring of the string that was previously used to get this collection. If this is
    //#               different, the result is garbage.
    //# @return The last glyph info for the given string.
    //#
    NODISCARD auto end_string(LStringView const& SubString) const noexcept
    {
        check(!this->GlyphInfos.empty())
        checkCode
        (
            check(SubString.size() <= this->_check_UsedString.size())
            for (auto Idx{0uz}; Idx < SubString.size(); ++Idx)
            {
                check(this->_check_UsedString[Idx] == SubString[Idx])
            }
        )
        auto It{this->GlyphInfos.begin()};
        for (;It != this->GlyphInfos.end(); ++It)
        {
            if (It->Cluster >= SubString.size())
            {
                break;
            }
        }
        return It;
    }
    NODISCARD auto end_location(LVec2F Location) const noexcept
    {
        check(!this->GlyphInfos.empty())
        auto It{this->GlyphInfos.begin()};
        for (;It != this->GlyphInfos.end(); ++It)
        {
            if (It->Pencil.x > Location.x)
            {
                break;
            }
        }
        return It;
    }
};

} /* ~Namespace Jafg */
