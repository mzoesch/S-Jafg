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

typedef TFunction<void(WButton* Self, LKeyEvent const& InKeyEvent)>  LOnButtonKeyEvent;
typedef TFunction<void(WVButton* Self, LKeyEvent const& InKeyEvent)> LOnVButtonKeyEvent;
typedef TFunction<void(WHButton* Self, LKeyEvent const& InKeyEvent)> LOnHButtonKeyEvent;

struct LButtonStyle
{
    LRegionBrush NormalBrush   { .Type = ERegionBrush::OutlineBox, .Tint = Lal::LColor::Gray,          .OutlineTint = Lal::LColor::Black };
    LRegionBrush HoverBrush    { .Type = ERegionBrush::OutlineBox, .Tint = Lal::LColor::Gray,          .OutlineTint = Lal::LColor::White };
    LRegionBrush PressBrush    { .Type = ERegionBrush::OutlineBox, .Tint = Lal::LColor::Gray,          .OutlineTint = Lal::LColor::White };
    LRegionBrush DisabledBrush { .Type = ERegionBrush::OutlineBox, .Tint = Lal::LColor::NotSoDarkGray, .OutlineTint = Lal::LColor::Black };
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

    FORCEINLINE void SetNormalBrush(LRegionBrush const& InBrush) noexcept { this->Style.NormalBrush = InBrush; }
    FORCEINLINE void SetHoverBrush(LRegionBrush const& InBrush) noexcept { this->Style.HoverBrush = InBrush; }
    FORCEINLINE void SetPressBrush(LRegionBrush const& InBrush) noexcept { this->Style.PressBrush = InBrush; }
    FORCEINLINE void SetDisabledBrush(LRegionBrush const& InBrush) noexcept { this->Style.DisabledBrush = InBrush; }
    FORCEINLINE void SetOmniBrush(LRegionBrush const& InBrush) noexcept
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

    FORCEINLINE void SetNormalTint(Lal::LColor const& InTint) noexcept { this->Style.NormalBrush.Tint = InTint; }
    FORCEINLINE void SetHoverTint(Lal::LColor const& InTint) noexcept { this->Style.HoverBrush.Tint = InTint; }
    FORCEINLINE void SetPressTint(Lal::LColor const& InTint) noexcept { this->Style.PressBrush.Tint = InTint; }
    FORCEINLINE void SetDisabledTint(Lal::LColor const& InTint) noexcept { this->Style.DisabledBrush.Tint = InTint; }
    FORCEINLINE void SetOmniTint(Lal::LColor const& InTint) noexcept
    {
        this->Style.NormalBrush.Tint   = InTint;
        this->Style.HoverBrush.Tint    = InTint;
        this->Style.PressBrush.Tint    = InTint;
        this->Style.DisabledBrush.Tint = InTint;

        return;
    }

    FORCEINLINE void SetNormalImage(LImage const& InImage) noexcept { this->Style.NormalBrush.Image = InImage; }
    FORCEINLINE void SetHoverImage(LImage const& InImage) noexcept { this->Style.HoverBrush.Image = InImage; }
    FORCEINLINE void SetPressImage(LImage const& InImage) noexcept { this->Style.PressBrush.Image = InImage; }
    FORCEINLINE void SetDisabledImage(LImage const& InImage) noexcept { this->Style.DisabledBrush.Image = InImage; }
    FORCEINLINE void SetOmniImage(LImage const& InImage) noexcept
    {
        this->Style.NormalBrush.Image   = InImage;
        this->Style.HoverBrush.Image    = InImage;
        this->Style.PressBrush.Image    = InImage;
        this->Style.DisabledBrush.Image = InImage;

        return;
    }

    FORCEINLINE void SetNormalImageTint(Lal::LColor const& InImageTint) noexcept { this->Style.NormalBrush.Tint = InImageTint; }
    FORCEINLINE void SetHoverImageTint(Lal::LColor const& InImageTint) noexcept { this->Style.HoverBrush.Tint = InImageTint; }
    FORCEINLINE void SetPressImageTint(Lal::LColor const& InImageTint) noexcept { this->Style.PressBrush.Tint = InImageTint; }
    FORCEINLINE void SetDisabledImageTint(Lal::LColor const& InImageTint) noexcept { this->Style.DisabledBrush.Tint = InImageTint; }
    FORCEINLINE void SetOmniImageTint(Lal::LColor const& InImageTint) noexcept
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

    FORCEINLINE void SetNormalRadii(LVector4 const& InRadii) noexcept { this->Style.NormalBrush.Radii = InRadii; }
    FORCEINLINE void SetHoverRadii(LVector4 const& InRadii) noexcept { this->Style.HoverBrush.Radii = InRadii; }
    FORCEINLINE void SetPressRadii(LVector4 const& InRadii) noexcept { this->Style.PressBrush.Radii = InRadii; }
    FORCEINLINE void SetDisabledRadii(LVector4 const& InRadii) noexcept { this->Style.DisabledBrush.Radii = InRadii; }
    FORCEINLINE void SetOmniRadii(LVector4 const& InRadii) noexcept
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

    FORCEINLINE void SetNormalOutlineTint(Lal::LColor const& InOutlineTint) noexcept { this->Style.NormalBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetHoverOutlineTint(Lal::LColor const& InOutlineTint) noexcept { this->Style.HoverBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetPressOutlineTint(Lal::LColor const& InOutlineTint) noexcept { this->Style.PressBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetDisabledOutlineTint(Lal::LColor const& InOutlineTint) noexcept { this->Style.DisabledBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetOmniOutlineTint(Lal::LColor const& InOutlineTint) noexcept
    {
        this->Style.NormalBrush.OutlineTint   = InOutlineTint;
        this->Style.HoverBrush.OutlineTint    = InOutlineTint;
        this->Style.PressBrush.OutlineTint    = InOutlineTint;
        this->Style.DisabledBrush.OutlineTint = InOutlineTint;

        return;
    }

    FORCEINLINE void SetStyle(LButtonStyle const& InStyle) noexcept { this->Style = InStyle; }
    FORCEINLINE const LButtonStyle& GetStyle() const noexcept { return this->Style; }
    FORCEINLINE LButtonStyle& GetMutableStyle() noexcept { return this->Style; }

    FORCEINLINE LRegionBrush const& GetNormalBrush() const noexcept { return this->Style.NormalBrush; }
    FORCEINLINE LRegionBrush const& GetHoverBrush() const noexcept { return this->Style.HoverBrush; }
    FORCEINLINE LRegionBrush const& GetPressBrush() const noexcept { return this->Style.PressBrush; }
    FORCEINLINE LRegionBrush const& GetDisabledBrush() const noexcept { return this->Style.DisabledBrush; }

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
    FORCEINLINE TFactoryRetTy& DisabledBrush(const LRegionBrush& InBrush) { this->This()->SetDisabledBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniBrush(const LRegionBrush& InBrush) { this->This()->SetOmniBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalType(const ERegionBrush::Type InType) { this->This()->SetNormalType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverType(const ERegionBrush::Type InType) { this->This()->SetHoverType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressType(const ERegionBrush::Type InType) { this->This()->SetPressType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledType(const ERegionBrush::Type InType) { this->This()->SetDisabledType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniType(const ERegionBrush::Type InType) { this->This()->SetOmniType(InType); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalTint(const Lal::LColor& InTint) { this->This()->SetNormalTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverTint(const Lal::LColor& InTint) { this->This()->SetHoverTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressTint(const Lal::LColor& InTint) { this->This()->SetPressTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledTint(const Lal::LColor& InTint) { this->This()->SetDisabledTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniTint(const Lal::LColor& InTint) { this->This()->SetOmniTint(InTint); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImage(const LImage& InImage) { this->This()->SetNormalImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImage(const LImage& InImage) { this->This()->SetHoverImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImage(const LImage& InImage) { this->This()->SetPressImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImage(const LImage& InImage) { this->This()->SetDisabledImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImage(const LImage& InImage) { this->This()->SetOmniImage(InImage); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImageTint(const Lal::LColor& InImageTint) { this->This()->SetNormalImageTint(InImageTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImageTint(const Lal::LColor& InImageTint) { this->This()->SetHoverImageTint(InImageTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImageTint(const Lal::LColor& InImageTint) { this->This()->SetPressImageTint(InImageTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImageTint(const Lal::LColor& InImageTint) { this->This()->SetDisabledImageTint(InImageTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImageTint(const Lal::LColor& InImageTint) { this->This()->SetOmniImageTint(InImageTint); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImageScale(const f32 InImageScale) { this->This()->SetNormalImageScale(InImageScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImageScale(const f32 InImageScale) { this->This()->SetHoverImageScale(InImageScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImageScale(const f32 InImageScale) { this->This()->SetPressImageScale(InImageScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImageScale(const f32 InImageScale) { this->This()->SetDisabledImageScale(InImageScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImageScale(const f32 InImageScale) { this->This()->SetOmniImageScale(InImageScale); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetNormalImageBehavior(InImageBehavior); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetHoverImageBehavior(InImageBehavior); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetPressImageBehavior(InImageBehavior); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetDisabledImageBehavior(InImageBehavior); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetOmniImageBehavior(InImageBehavior); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetNormalImageOobm(InImageOobm); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetHoverImageOobm(InImageOobm); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetPressImageOobm(InImageOobm); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetDisabledImageOobm(InImageOobm); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetOmniImageOobm(InImageOobm); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalImagePadding(const f32 InImagePadding) { this->This()->SetNormalImagePadding(InImagePadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverImagePadding(const f32 InImagePadding) { this->This()->SetHoverImagePadding(InImagePadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressImagePadding(const f32 InImagePadding) { this->This()->SetPressImagePadding(InImagePadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledImagePadding(const f32 InImagePadding) { this->This()->SetDisabledImagePadding(InImagePadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniImagePadding(const f32 InImagePadding) { this->This()->SetOmniImagePadding(InImagePadding); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalRadii(const LVector4& InRadii) { this->This()->SetNormalRadii(InRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverRadii(const LVector4& InRadii) { this->This()->SetHoverRadii(InRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressRadii(const LVector4& InRadii) { this->This()->SetPressRadii(InRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledRadii(const LVector4& InRadii) { this->This()->SetDisabledRadii(InRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniRadii(const LVector4& InRadii) { this->This()->SetOmniRadii(InRadii); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalOutlineThickness(const f32 InOutlineThickness) { this->This()->SetNormalOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverOutlineThickness(const f32 InOutlineThickness) { this->This()->SetHoverOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressOutlineThickness(const f32 InOutlineThickness) { this->This()->SetPressOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledOutlineThickness(const f32 InOutlineThickness) { this->This()->SetDisabledOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniOutlineThickness(const f32 InOutlineThickness) { this->This()->SetOmniOutlineThickness(InOutlineThickness); return this->Self(); }

    FORCEINLINE TFactoryRetTy& NormalOutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetNormalOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HoverOutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetHoverOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& PressOutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetPressOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisabledOutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetDisabledOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OmniOutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetOmniOutlineTint(InOutlineTint); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Style(const LButtonStyle& InStyle) { this->This()->SetStyle(InStyle); return this->Self(); }
    FORCEINLINE TFactoryRetTy& UiResponsive(const bool bTrue) { this->This()->SetLetUiReactToEvents(bTrue); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Enabled(const bool bInEnabled) { this->This()->SetEnabled(bInEnabled); return this->Self(); }
};

template <typename TNode>
using TWidgetFactoryButton      = TWidgetFactoryButtonBase<TNode, TWidgetFactoryRegion<TNode>>;
template <typename TNode>
using TWidgetFactoryVButton     = TWidgetFactoryButtonBase<TNode, TWidgetFactoryVRegion<TNode>>;
template <typename TNode>
using TWidgetFactoryHButton     = TWidgetFactoryButtonBase<TNode, TWidgetFactoryHRegion<TNode>>;

} /* ~Namespace Jafg */
