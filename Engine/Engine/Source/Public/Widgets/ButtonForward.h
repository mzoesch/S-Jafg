// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/RegionForward.h"
#include "Widgets/RegionFactory.h"
#include "Widgets/VRegionFactory.h"
#include "Widgets/HRegionFactory.h"

namespace Jafg
{

class WTextBox;
class WButton;
class WTextButton;
class WVButton;
class WHButton;
struct LTextBoxBrush;

MAKE_DELEGATE_SIGNATURE(LOnButtonKeyEvent,  void, WButton*  Self, const LKeyEvent& InKeyEvent)
MAKE_DELEGATE_SIGNATURE(LOnVButtonKeyEvent, void, WVButton* Self, const LKeyEvent& InKeyEvent)
MAKE_DELEGATE_SIGNATURE(LOnHButtonKeyEvent, void, WHButton* Self, const LKeyEvent& InKeyEvent)

struct LButtonStyle
{
    LRegionBrush NormalBrush   { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::Black };
    LRegionBrush HoverBrush    { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::White };
    LRegionBrush PressBrush    { .Type = ERegionBrush::OutlineBox, .Tint = LColor::Gray,          .OutlineTint = LColor::White };
    LRegionBrush DisabledBrush { .Type = ERegionBrush::OutlineBox, .Tint = LColor::NotSoDarkGray, .OutlineTint = LColor::Black };
};

class LButtonBase
{
public:

    virtual ~LButtonBase() = default;

    //# These virtual methods are called when the associated delegate is not bound.
    virtual void OnPrimaryPress()     { }
    virtual void OnPrimaryRelease()   { }
    virtual void OnSecondaryPress()   { }
    virtual void OnSecondaryRelease() { }

    FORCEINLINE void SetNormalBrush(const LRegionBrush& InBrush) noexcept { this->Style.NormalBrush = InBrush; }
    FORCEINLINE void SetHoverBrush(const LRegionBrush& InBrush) noexcept { this->Style.HoverBrush = InBrush; }
    FORCEINLINE void SetPressBrush(const LRegionBrush& InBrush) noexcept { this->Style.PressBrush = InBrush; }
    FORCEINLINE void SetDisabledBrush(const LRegionBrush& InBrush) noexcept { this->Style.DisabledBrush = InBrush; }
    FORCEINLINE void SetOmniBrush(const LRegionBrush& InBrush) noexcept
    {
        this->Style.NormalBrush   = InBrush;
        this->Style.HoverBrush    = InBrush;
        this->Style.PressBrush    = InBrush;
        this->Style.DisabledBrush = InBrush;

        return;
    }

    FORCEINLINE void SetNormalType(const ERegionBrush::Type InType) noexcept { this->Style.NormalBrush.Type = InType; }
    FORCEINLINE void SetHoverType(const ERegionBrush::Type InType) noexcept { this->Style.HoverBrush.Type = InType; }
    FORCEINLINE void SetPressType(const ERegionBrush::Type InType) noexcept { this->Style.PressBrush.Type = InType; }
    FORCEINLINE void SetDisabledType(const ERegionBrush::Type InType) noexcept { this->Style.DisabledBrush.Type = InType; }
    FORCEINLINE void SetOmniType(const ERegionBrush::Type InType) noexcept
    {
        this->Style.NormalBrush.Type   = InType;
        this->Style.HoverBrush.Type    = InType;
        this->Style.PressBrush.Type    = InType;
        this->Style.DisabledBrush.Type = InType;

        return;
    }

    FORCEINLINE void SetNormalTint(const LColor& InTint) noexcept { this->Style.NormalBrush.Tint = InTint; }
    FORCEINLINE void SetHoverTint(const LColor& InTint) noexcept { this->Style.HoverBrush.Tint = InTint; }
    FORCEINLINE void SetPressTint(const LColor& InTint) noexcept { this->Style.PressBrush.Tint = InTint; }
    FORCEINLINE void SetDisabledTint(const LColor& InTint) noexcept { this->Style.DisabledBrush.Tint = InTint; }
    FORCEINLINE void SetOmniTint(const LColor& InTint) noexcept
    {
        this->Style.NormalBrush.Tint   = InTint;
        this->Style.HoverBrush.Tint    = InTint;
        this->Style.PressBrush.Tint    = InTint;
        this->Style.DisabledBrush.Tint = InTint;

        return;
    }

    FORCEINLINE void SetNormalImage(const LImage& InImage) noexcept { this->Style.NormalBrush.Image = InImage; }
    FORCEINLINE void SetHoverImage(const LImage& InImage) noexcept { this->Style.HoverBrush.Image = InImage; }
    FORCEINLINE void SetPressImage(const LImage& InImage) noexcept { this->Style.PressBrush.Image = InImage; }
    FORCEINLINE void SetDisabledImage(const LImage& InImage) noexcept { this->Style.DisabledBrush.Image = InImage; }
    FORCEINLINE void SetOmniImage(const LImage& InImage) noexcept
    {
        this->Style.NormalBrush.Image   = InImage;
        this->Style.HoverBrush.Image    = InImage;
        this->Style.PressBrush.Image    = InImage;
        this->Style.DisabledBrush.Image = InImage;

        return;
    }

    FORCEINLINE void SetNormalImageTint(const LColor& InImageTint) noexcept { this->Style.NormalBrush.Tint = InImageTint; }
    FORCEINLINE void SetHoverImageTint(const LColor& InImageTint) noexcept { this->Style.HoverBrush.Tint = InImageTint; }
    FORCEINLINE void SetPressImageTint(const LColor& InImageTint) noexcept { this->Style.PressBrush.Tint = InImageTint; }
    FORCEINLINE void SetDisabledImageTint(const LColor& InImageTint) noexcept { this->Style.DisabledBrush.Tint = InImageTint; }
    FORCEINLINE void SetOmniImageTint(const LColor& InImageTint) noexcept
    {
        this->Style.NormalBrush.ImageTint   = InImageTint;
        this->Style.HoverBrush.ImageTint    = InImageTint;
        this->Style.PressBrush.ImageTint    = InImageTint;
        this->Style.DisabledBrush.ImageTint = InImageTint;

        return;
    }

    FORCEINLINE void SetNormalImageScale(const f32 InImageScale) noexcept { this->Style.NormalBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetHoverImageScale(const f32 InImageScale) noexcept { this->Style.HoverBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetPressImageScale(const f32 InImageScale) noexcept { this->Style.PressBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetDisabledImageScale(const f32 InImageScale) noexcept { this->Style.DisabledBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetOmniImageScale(const f32 InImageScale) noexcept
    {
        this->Style.NormalBrush.ImageScale   = InImageScale;
        this->Style.HoverBrush.ImageScale    = InImageScale;
        this->Style.PressBrush.ImageScale    = InImageScale;
        this->Style.DisabledBrush.ImageScale = InImageScale;

        return;
    }

    FORCEINLINE void SetNormalImageBehavior(const EImageBehavior::Type InImageBehavior) noexcept { this->Style.NormalBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetHoverImageBehavior(const EImageBehavior::Type InImageBehavior) noexcept { this->Style.HoverBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetPressImageBehavior(const EImageBehavior::Type InImageBehavior) noexcept { this->Style.PressBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetDisabledImageBehavior(const EImageBehavior::Type InImageBehavior) noexcept { this->Style.DisabledBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetOmniImageBehavior(const EImageBehavior::Type InImageBehavior) noexcept
    {
        this->Style.NormalBrush.ImageBehavior   = InImageBehavior;
        this->Style.HoverBrush.ImageBehavior    = InImageBehavior;
        this->Style.PressBrush.ImageBehavior    = InImageBehavior;
        this->Style.DisabledBrush.ImageBehavior = InImageBehavior;

        return;
    }

    FORCEINLINE void SetNormalImageOobm(const EImageOobm::Type InImageOobm) noexcept { this->Style.NormalBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetHoverImageOobm(const EImageOobm::Type InImageOobm) noexcept { this->Style.HoverBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetPressImageOobm(const EImageOobm::Type InImageOobm) noexcept { this->Style.PressBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetDisabledImageOobm(const EImageOobm::Type InImageOobm) noexcept { this->Style.DisabledBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetOmniImageOobm(const EImageOobm::Type InImageOobm) noexcept
    {
        this->Style.NormalBrush.ImageOobm   = InImageOobm;
        this->Style.HoverBrush.ImageOobm    = InImageOobm;
        this->Style.PressBrush.ImageOobm    = InImageOobm;
        this->Style.DisabledBrush.ImageOobm = InImageOobm;

        return;
    }

    FORCEINLINE void SetNormalImagePadding(const f32 InImagePadding) noexcept { this->Style.NormalBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetHoverImagePadding(const f32 InImagePadding) noexcept { this->Style.HoverBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetPressImagePadding(const f32 InImagePadding) noexcept { this->Style.PressBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetDisabledImagePadding(const f32 InImagePadding) noexcept { this->Style.DisabledBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetOmniImagePadding(const f32 InImagePadding) noexcept
    {
        this->Style.NormalBrush.ImagePadding   = InImagePadding;
        this->Style.HoverBrush.ImagePadding    = InImagePadding;
        this->Style.PressBrush.ImagePadding    = InImagePadding;
        this->Style.DisabledBrush.ImagePadding = InImagePadding;

        return;
    }

    FORCEINLINE void SetNormalRadii(const LVector4& InRadii) noexcept { this->Style.NormalBrush.Radii = InRadii; }
    FORCEINLINE void SetHoverRadii(const LVector4& InRadii) noexcept { this->Style.HoverBrush.Radii = InRadii; }
    FORCEINLINE void SetPressRadii(const LVector4& InRadii) noexcept { this->Style.PressBrush.Radii = InRadii; }
    FORCEINLINE void SetDisabledRadii(const LVector4& InRadii) noexcept { this->Style.DisabledBrush.Radii = InRadii; }
    FORCEINLINE void SetOmniRadii(const LVector4& InRadii) noexcept
    {
        this->Style.NormalBrush.Radii   = InRadii;
        this->Style.HoverBrush.Radii    = InRadii;
        this->Style.PressBrush.Radii    = InRadii;
        this->Style.DisabledBrush.Radii = InRadii;

        return;
    }

    FORCEINLINE void SetNormalOutlineThickness(const f32 InOutlineThickness) noexcept { this->Style.NormalBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetHoverOutlineThickness(const f32 InOutlineThickness) noexcept { this->Style.HoverBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetPressOutlineThickness(const f32 InOutlineThickness) noexcept { this->Style.PressBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetDisabledOutlineThickness(const f32 InOutlineThickness) noexcept { this->Style.DisabledBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetOmniOutlineThickness(const f32 InOutlineThickness) noexcept
    {
        this->Style.NormalBrush.OutlineThickness   = InOutlineThickness;
        this->Style.HoverBrush.OutlineThickness    = InOutlineThickness;
        this->Style.PressBrush.OutlineThickness    = InOutlineThickness;
        this->Style.DisabledBrush.OutlineThickness = InOutlineThickness;

        return;
    }

    FORCEINLINE void SetNormalOutlineTint(const LColor& InOutlineTint) noexcept { this->Style.NormalBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetHoverOutlineTint(const LColor& InOutlineTint) noexcept { this->Style.HoverBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetPressOutlineTint(const LColor& InOutlineTint) noexcept { this->Style.PressBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetDisabledOutlineTint(const LColor& InOutlineTint) noexcept { this->Style.DisabledBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetOmniOutlineTint(const LColor& InOutlineTint) noexcept
    {
        this->Style.NormalBrush.OutlineTint   = InOutlineTint;
        this->Style.HoverBrush.OutlineTint    = InOutlineTint;
        this->Style.PressBrush.OutlineTint    = InOutlineTint;
        this->Style.DisabledBrush.OutlineTint = InOutlineTint;

        return;
    }

    FORCEINLINE const LButtonStyle& GetStyle() const noexcept { return this->Style; }
    FORCEINLINE LButtonStyle& GetMutableStyle() noexcept { return this->Style; }

    FORCEINLINE const LRegionBrush& GetNormalBrush() const noexcept { return this->Style.NormalBrush; }
    FORCEINLINE const LRegionBrush& GetHoverBrush() const noexcept { return this->Style.HoverBrush; }
    FORCEINLINE const LRegionBrush& GetPressBrush() const noexcept { return this->Style.PressBrush; }
    FORCEINLINE const LRegionBrush& GetDisabledBrush() const noexcept { return this->Style.DisabledBrush; }

    FORCEINLINE LRegionBrush& GetMutableNormalBrush() noexcept { return this->Style.NormalBrush; }
    FORCEINLINE LRegionBrush& GetMutableHoverBrush() noexcept { return this->Style.HoverBrush; }
    FORCEINLINE LRegionBrush& GetMutablePressBrush() noexcept { return this->Style.PressBrush; }
    FORCEINLINE LRegionBrush& GetMutableDisabledBrush() noexcept { return this->Style.DisabledBrush; }

    FORCEINLINE void SetLetUiReactToEvents(const bool bInLetUiReactToEvents) { this->bLetUiReactToEvents = bInLetUiReactToEvents; }
    FORCEINLINE bool GetLetUiReactToEvents() const { return this->bLetUiReactToEvents; }

    virtual     void SetEnabled(const bool bInEnabled) = 0;
    FORCEINLINE bool IsEnabled() const noexcept { return this->bEnabled; }

protected:

    bool bLetUiReactToEvents : 1 { true };
    bool bEnabled : 1 { true };

    LButtonStyle Style;
};

template <typename TNode, typename TSuper>
class TWidgetFactoryButtonBase : public TSuper
{
public:

    static_assert(std::is_same_v<TNode, typename TSuper::TNodeTy>);

    using Super         = TSuper;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& OnPrimaryPress(LOnButtonKeyEvent&& InDelegate) { this->This()->OnPrimaryPressDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnPrimaryRelease(LOnButtonKeyEvent&& InDelegate) { this->This()->OnPrimaryReleaseDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnSecondaryPress(LOnButtonKeyEvent&& InDelegate) { this->This()->OnSecondaryPressDelegate = std::move(InDelegate); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OnSecondaryRelease(LOnButtonKeyEvent&& InDelegate) { this->This()->OnSecondaryReleaseDelegate = std::move(InDelegate); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalBrush(const LRegionBrush& InBrush) { this->This()->SetNormalBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverBrush(const LRegionBrush& InBrush) { this->This()->SetHoverBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressBrush(const LRegionBrush& InBrush) { this->This()->SetPressBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& UiResponsive(const bool bTrue) { this->This()->SetLetUiReactToEvents(bTrue); return this->Self(); }
};

template <typename TNode>
using TWidgetFactoryButton      = TWidgetFactoryButtonBase<TNode, TWidgetFactoryRegion<TNode>>;
template <typename TNode>
using TWidgetFactoryVButton     = TWidgetFactoryButtonBase<TNode, TWidgetFactoryVRegion<TNode>>;
template <typename TNode>
using TWidgetFactoryHButton     = TWidgetFactoryButtonBase<TNode, TWidgetFactoryHRegion<TNode>>;

} /* ~Namespace Jafg */
