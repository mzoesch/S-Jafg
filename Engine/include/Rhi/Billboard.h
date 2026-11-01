// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Texture2.h"
#include "Rhi/Material.h"

namespace Jafg
{

struct LActorRenderInfo;

//# A billboard.
struct LBillboard
{
    struct Config
    {
        //# The color tint of the billboard.
        LColor Tint{Colors::White};
        //# The maximal distance in at which the billboard is visible.
        f32 MaxDistance{200.0f};
        //# The minimal pixel size of the billboard on screen.
        LVec2F MinSize{3.0f};
    };

    struct RenderInfo final
    {
        LWorldVec3 Translation;
    };

    ENGINE_API void Render(LActorRenderInfo const& Info, RenderInfo const& RenderInfo) const;

    ENGINE_API void SetTexture(LTexture2Ref Texture);
    NODISCARD FORCEINLINE constexpr LTexture2Ref const& GetTexture() const noexcept
    {
        return this->Texture;
    }

    FORCEINLINE constexpr void SetSize(LVec2F Size) noexcept
    {
        this->Size = Size;
    }
    NODISCARD FORCEINLINE constexpr LVec2F const& GetSize() const noexcept
    {
        return this->Size;
    }

    FORCEINLINE constexpr void SetConfig(Config Config) noexcept
    {
        this->m_Config = std::move(Config);
    }
    NODISCARD FORCEINLINE constexpr Config const& GetConfig() const noexcept
    {
        return this->m_Config;
    }
    FORCEINLINE constexpr void SetTint(LColor Tint) noexcept
    {
        this->m_Config.Tint = Tint;
    }
    NODISCARD FORCEINLINE constexpr LColor const& GetTint() const noexcept
    {
        return this->m_Config.Tint;
    }
    FORCEINLINE constexpr void SetMaxDistance(f32 MaxDistance) noexcept
    {
        this->m_Config.MaxDistance = MaxDistance;
    }
    NODISCARD FORCEINLINE constexpr f32 GetMaxDistance() const noexcept
    {
        return this->m_Config.MaxDistance;
    }
    FORCEINLINE constexpr void SetMinSize(LVec2F MinSize) noexcept
    {
        this->m_Config.MinSize = MinSize;
    }
    NODISCARD FORCEINLINE constexpr LVec2F const& GetMinSize() const noexcept
    {
        return this->m_Config.MinSize;
    }

private:

    LVec2F Size{maths::zero_vector<LVec2F>};
    LTexture2Ref Texture;
    Config m_Config;
    mutable std::optional<LColor> NextColor;
    mutable LMaterialInstanceRef Instance;
};

} /* ~Namespace Jafg */
