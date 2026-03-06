// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Rhi/Texture2.h"
#include "Region.generated.h"

namespace Jafg
{

//# How the #LRegionBrush behaves at a high level.
enum struct ERegionBrush
{
    //# Do not draw.
    None,
    //# Draw as a normal box.
    Box,
    //# Draw as a rounded box.
    RoundedBox,
    //# Draw as a box with an outline.
    OutlineBox,
    //# Draw as a box with a rounded outline.
    RoundedOutlineBox,
};

//# High-level image behavior.
enum struct EImageBehavior
{
    //# Scale the image to the size of the parent.
    Scale,
    //# Preserve the aspect ratio of the image.
    Aspect,
};

//# The image out of bounds mode. How the image should behave if its UVs are going out of bounds.
enum struct EImageOobm
{
    //# Wrap the image. The default behavior.
    Wrap,
    //# Clamp the image sides.
    Clamp,
    //# Discard the image channels.
    Discard,
};

struct LRegionBrush
{
    //# The type of the region brush.
    ERegionBrush Type{ ERegionBrush::None };

    //# The tint of the draw area from this region.
    LColor Tint{ Colors::White };

    //# An optional texture to use as a background.
    LTexture2Ref Texture;

    //# The tint of the image.
    LColor ImageTint{ Colors::White };

    //# The scale of the image.
    f32 ImageScale{ 1.0 };

    //# How the #Image should behave.
    EImageBehavior ImageBehavior{ EImageBehavior::Scale };

    //# The image out of bounds mode. @see #EImageOobm.
    EImageOobm ImageOobm{ EImageOobm::Wrap };

    //# How much padding to apply to the image.
    f32 ImagePadding{};

    //# The radii to use for the edges. TL => TR => BR => BL.
    LVec4F Radii{ maths::zero_vector<LVec4F> };

    //# The thickness of the outline.
    f32 OutlineThickness{};

    //# The outline color to use.
    LColor OutlineTint{ Colors::White };
};

struct LFactoryRegion;

//#
//# WRegion is an overlay node that can be customized with a #LRegionBrush.
//# A region might still draw outside these bounds.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryRegion)
class ENGINE_API WRegion : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WRegion)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    void SetBrush(LRegionBrush const& InBrush) { this->Brush = InBrush; }
    void SetBrush(LRegionBrush&& InBrush) { this->Brush = std::move(InBrush); }
    LRegionBrush& GetMutableBrush() { return this->Brush; }
    const LRegionBrush& GetBrush() const { return this->Brush; }

    FORCEINLINE void SetType(const ERegionBrush InType) noexcept { this->Brush.Type = InType; }
    FORCEINLINE void SetTint(const LColor& InTint) noexcept { this->Brush.Tint = InTint; }
    FORCEINLINE void SetTexture(LTexture2Ref InTexture) noexcept { this->Brush.Texture = std::move(InTexture); }
    FORCEINLINE void SetImageTint(const LColor& InColor) noexcept { this->Brush.ImageTint = InColor; }
    FORCEINLINE void SetImageScale(const f32 InScale) noexcept { this->Brush.ImageScale = InScale; }
    FORCEINLINE void SetImageBehavior(const EImageBehavior InType) noexcept { this->Brush.ImageBehavior = InType; }
    FORCEINLINE void SetImageOobm(const EImageOobm InType) noexcept { this->Brush.ImageOobm = InType; }
    FORCEINLINE void SetImagePadding(const f32 InPadding) noexcept { this->Brush.ImagePadding = InPadding; }
    FORCEINLINE void SetOutlineThickness(const f32 InOutlineThickness) noexcept { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetRadii(const LVec4F& InOutlineRadii) noexcept { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetOutlineTint(const LColor& InOutlineTint) noexcept { this->Brush.OutlineTint = InOutlineTint; }

private:

    LRegionBrush Brush;
};

struct LFactoryRegion : NODE_FACTORY_PARENT(WRegion)
{
    NODE_FACTORY_BODY(WRegion)

    decltype(auto) Brush(this auto&& Self, LRegionBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Type(this auto&& Self, ERegionBrush InType) noexcept
    {
        NODE_FACTORY_SELF().SetType(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Tint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImageTint(this auto&& Self, LColor const& InColor) noexcept
    {
        NODE_FACTORY_SELF().SetImageTint(InColor);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImageScale(this auto&& Self, const f32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetImageScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImageBehavior(this auto&& Self, const EImageBehavior InType) noexcept
    {
        NODE_FACTORY_SELF().SetImageBehavior(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImageOobm(this auto&& Self, const EImageOobm InType) noexcept
    {
        NODE_FACTORY_SELF().SetImageOobm(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImagePadding(this auto&& Self, const f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetImagePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineThickness(this auto&& Self, const f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Radii(this auto&& Self, LVec4F const& InOutlineRadii) noexcept
    {
        NODE_FACTORY_SELF().SetRadii(InOutlineRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, LColor const& InOutlineTint) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
