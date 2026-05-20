// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Icon.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

struct LNodeRenderInfo;

namespace Detail
{
ENGINE_API void DrawIcon(LNodeRenderInfo const& Info, LVec2F TopLeft, LTexture2Ref const& Icon, LIconBrush const& Brush) noexcept;
} /* ~Namespace Detail */

enum struct EIconSide : u8 { Left, Right, };
struct LIconized
{
    LTexture2Ref Icon;
    LIconBrush IconBrush;

    NODISCARD FORCEINLINE LVec2F GetIconSize(LViewport const& Viewport) const noexcept
    {
        if (this->Icon.get() && this->IconBrush.Scale > 0)
        {
            return {
                maths::max(static_cast<f32>(this->Icon->GetExtent().width * this->IconBrush.Scale)
                    , this->IconBrush.MinIconSize.InStaticPoints(Viewport))
                    + this->IconBrush.InwardsPadding.InStaticPoints(Viewport)
                , 0.0f
                };
        }
        if (this->IconBrush.bAlwaysPad)
        {
            return {this->IconBrush.MinIconSize.InStaticPoints(Viewport) + IconBrush.InwardsPadding.InStaticPoints(Viewport), 0.0f};
        }
        return maths::zero_vector<LVec2F>;
    }
};

struct LIconizedDouble
{
    LTexture2Ref LeftIcon;
    LTexture2Ref RightIcon;
    LIconBrush LeftIconBrush;
    LIconBrush RightIconBrush;
};

template<typename TNode>
struct TFactoryIconized : NODE_FACTORY_PARENT(TNode)
{
    NODE_FACTORY_BODY(TNode)

    decltype(auto) Icon(this auto&& Self, LOptionalTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().Icon = InIcon.GetResolved();
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconBrush(this auto&& Self, LIconBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().IconBrush = Brush;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconScale(this auto&& Self, u32 Scale) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.Scale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysPadIcon(this auto&& Self, bool bPad) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.bAlwaysPad = bPad;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconInwardsPadding(this auto&& Self, LNodeSize1 Padding) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.InwardsPadding = Padding;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconMinSize(this auto&& Self, LNodeSize1 MinSize) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.MinIconSize = MinSize;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconAlignment(this auto&& Self, LIconBrush::Align Alignment) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.Alignment = Alignment;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().IconBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
};

namespace Detail
{
struct IconLocationFn final
{
    template<auto IconProj, auto IconBrushProj, typename T>
        requires std::is_base_of_v<WNode, T>
    NODISCARD FORCEINLINE LVec2F GetRelativeLeftIconTopLeft(T const& Who, LPadding const& Padding) const noexcept
    {
        check((Who.*IconProj).get())
        f32 TotalHeight{Who.GetAnchoredSize_v2().y - Padding.GetDesiredSize().InStaticPoints(Who.GetViewport()).y};

        f32 Offset{};
        if ((Who.*IconBrushProj).Scale > 0)
        {
            f32 IconSize{static_cast<f32>((Who.*IconProj)->GetExtent().width * (Who.*IconBrushProj).Scale)};
            f32 Playroom{maths::max((Who.*IconBrushProj).MinIconSize.InStaticPoints(Who.GetViewport()) - IconSize, 0.0f)};
            switch ((Who.*IconBrushProj).Alignment)
            {
            case LIconBrush::Align::Left: { break; }
            case LIconBrush::Align::Center: { Offset = maths::floor(Playroom * 0.5f); break; }
            case LIconBrush::Align::Right: { Offset = maths::floor(Playroom); break; }
            }
        }

        return Padding.GetTopLeftOffset().InStaticPoints(Who.GetViewport())
            + LVec2F{Offset, (TotalHeight - static_cast<f32>((Who.*IconProj)->GetExtent().height * (Who.*IconBrushProj).Scale)) * 0.5f};
    }
    template<auto IconProj, auto IconBrushProj, typename T>
        requires std::is_base_of_v<WNode, T>
    NODISCARD FORCEINLINE LVec2F GetRelativeRightIconTopLeft(T const& Who, LPadding const& Padding) const noexcept
    {
        check((Who.*IconProj).get())
        f32 TotalHeight{(Who.GetAnchoredSize_v2().y - Padding.GetDesiredSize().InStaticPoints(Who.GetViewport()).y)};

        f32 Offset{};
        if ((Who.*IconBrushProj).Scale > 0)
        {
            f32 IconSize{static_cast<f32>((Who.*IconProj)->GetExtent().width * (Who.*IconBrushProj).Scale)};
            f32 Playroom{maths::max((Who.*IconBrushProj).MinIconSize.InStaticPoints(Who.GetViewport()) - IconSize, 0.0f)};
            switch ((Who.*IconBrushProj).Alignment)
            {
            case LIconBrush::Align::Left: { break; }
            case LIconBrush::Align::Center: { Offset = maths::floor(Playroom * 0.5f); break; }
            case LIconBrush::Align::Right: { Offset = maths::floor(Playroom); break; }
            }
        }

        return LVec2F{Who.GetAnchoredSize_v2().x, 0.0f}
            - LVec2F{Padding.GetRightOffset().InStaticPoints(Who.GetViewport())
                , Padding.GetTopOffset().InStaticPoints(Who.GetViewport())}
            - LVec2F{maths::max(
                static_cast<f32>((Who.*IconProj)->GetExtent().width * (Who.*IconBrushProj).Scale)
                , (Who.*IconBrushProj).MinIconSize.InStaticPoints(Who.GetViewport())
                ), 0.0f}
            + LVec2F{Offset, (TotalHeight - static_cast<f32>((Who.*IconProj)->GetExtent().height * (Who.*IconBrushProj).Scale)) * 0.5f};
    }
};
} /* ~Namespace Detail */
inline constexpr Detail::IconLocationFn IconLocation;

} /* ~Namespace Jafg */
