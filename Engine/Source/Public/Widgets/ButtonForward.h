// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LButtonStyle
{
    LRegionBrush NormalBrush   { .Type = ERegionBrush::OutlineBox, .Tint = Colors::Gray,          .OutlineTint = Colors::Black };
    LRegionBrush HoverBrush    { .Type = ERegionBrush::OutlineBox, .Tint = Colors::Gray,          .OutlineTint = Colors::White };
    LRegionBrush PressBrush    { .Type = ERegionBrush::OutlineBox, .Tint = Colors::Gray,          .OutlineTint = Colors::White };
    LRegionBrush DisabledBrush { .Type = ERegionBrush::OutlineBox, .Tint = Colors::NotSoDarkGray, .OutlineTint = Colors::Black };
};

//# Inherit from this to access common button logic.
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

    FORCEINLINE void SetNormalType(const ERegionBrush InType) noexcept { this->Style.NormalBrush.Type = InType; }
    FORCEINLINE void SetHoverType(const ERegionBrush InType) noexcept { this->Style.HoverBrush.Type = InType; }
    FORCEINLINE void SetPressType(const ERegionBrush InType) noexcept { this->Style.PressBrush.Type = InType; }
    FORCEINLINE void SetDisabledType(const ERegionBrush InType) noexcept { this->Style.DisabledBrush.Type = InType; }
    FORCEINLINE void SetOmniType(const ERegionBrush InType) noexcept
    {
        this->Style.NormalBrush.Type   = InType;
        this->Style.HoverBrush.Type    = InType;
        this->Style.PressBrush.Type    = InType;
        this->Style.DisabledBrush.Type = InType;

        return;
    }

    FORCEINLINE void SetNormalTint(LColor const& InTint) noexcept { this->Style.NormalBrush.Tint = InTint; }
    FORCEINLINE void SetHoverTint(LColor const& InTint) noexcept { this->Style.HoverBrush.Tint = InTint; }
    FORCEINLINE void SetPressTint(LColor const& InTint) noexcept { this->Style.PressBrush.Tint = InTint; }
    FORCEINLINE void SetDisabledTint(LColor const& InTint) noexcept { this->Style.DisabledBrush.Tint = InTint; }
    FORCEINLINE void SetOmniTint(LColor const& InTint) noexcept
    {
        this->Style.NormalBrush.Tint   = InTint;
        this->Style.HoverBrush.Tint    = InTint;
        this->Style.PressBrush.Tint    = InTint;
        this->Style.DisabledBrush.Tint = InTint;

        return;
    }

    FORCEINLINE void SetNormalTexture(LTexture2Ref InTexture) noexcept { this->Style.NormalBrush.Texture = std::move(InTexture); }
    FORCEINLINE void SetHoverTexture(LTexture2Ref InTexture) noexcept { this->Style.HoverBrush.Texture = std::move(InTexture); }
    FORCEINLINE void SetPressTexture(LTexture2Ref InTexture) noexcept { this->Style.PressBrush.Texture = std::move(InTexture); }
    FORCEINLINE void SetDisabledTexture(LTexture2Ref InTexture) noexcept { this->Style.DisabledBrush.Texture = std::move(InTexture); }
    FORCEINLINE void SetOmniTexture(LTexture2Ref InTexture) noexcept
    {
        this->Style.NormalBrush.Texture   = std::move(InTexture);
        this->Style.HoverBrush.Texture    = std::move(InTexture);
        this->Style.PressBrush.Texture    = std::move(InTexture);
        this->Style.DisabledBrush.Texture = std::move(InTexture);

        return;
    }

    FORCEINLINE void SetNormalImageTint(LColor const& InImageTint) noexcept { this->Style.NormalBrush.Tint = InImageTint; }
    FORCEINLINE void SetHoverImageTint(LColor const& InImageTint) noexcept { this->Style.HoverBrush.Tint = InImageTint; }
    FORCEINLINE void SetPressImageTint(LColor const& InImageTint) noexcept { this->Style.PressBrush.Tint = InImageTint; }
    FORCEINLINE void SetDisabledImageTint(LColor const& InImageTint) noexcept { this->Style.DisabledBrush.Tint = InImageTint; }
    FORCEINLINE void SetOmniImageTint(LColor const& InImageTint) noexcept
    {
        this->Style.NormalBrush.ImageTint   = InImageTint;
        this->Style.HoverBrush.ImageTint    = InImageTint;
        this->Style.PressBrush.ImageTint    = InImageTint;
        this->Style.DisabledBrush.ImageTint = InImageTint;

        return;
    }

    FORCEINLINE void SetNormalImageScale(const f64 InImageScale) noexcept { this->Style.NormalBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetHoverImageScale(const f64 InImageScale) noexcept { this->Style.HoverBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetPressImageScale(const f64 InImageScale) noexcept { this->Style.PressBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetDisabledImageScale(const f64 InImageScale) noexcept { this->Style.DisabledBrush.ImageScale = InImageScale; }
    FORCEINLINE void SetOmniImageScale(const f64 InImageScale) noexcept
    {
        this->Style.NormalBrush.ImageScale   = InImageScale;
        this->Style.HoverBrush.ImageScale    = InImageScale;
        this->Style.PressBrush.ImageScale    = InImageScale;
        this->Style.DisabledBrush.ImageScale = InImageScale;

        return;
    }

    FORCEINLINE void SetNormalImageBehavior(const EImageBehavior InImageBehavior) noexcept { this->Style.NormalBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetHoverImageBehavior(const EImageBehavior InImageBehavior) noexcept { this->Style.HoverBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetPressImageBehavior(const EImageBehavior InImageBehavior) noexcept { this->Style.PressBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetDisabledImageBehavior(const EImageBehavior InImageBehavior) noexcept { this->Style.DisabledBrush.ImageBehavior = InImageBehavior; }
    FORCEINLINE void SetOmniImageBehavior(const EImageBehavior InImageBehavior) noexcept
    {
        this->Style.NormalBrush.ImageBehavior   = InImageBehavior;
        this->Style.HoverBrush.ImageBehavior    = InImageBehavior;
        this->Style.PressBrush.ImageBehavior    = InImageBehavior;
        this->Style.DisabledBrush.ImageBehavior = InImageBehavior;

        return;
    }

    FORCEINLINE void SetNormalImageOobm(const EImageOobm InImageOobm) noexcept { this->Style.NormalBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetHoverImageOobm(const EImageOobm InImageOobm) noexcept { this->Style.HoverBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetPressImageOobm(const EImageOobm InImageOobm) noexcept { this->Style.PressBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetDisabledImageOobm(const EImageOobm InImageOobm) noexcept { this->Style.DisabledBrush.ImageOobm = InImageOobm; }
    FORCEINLINE void SetOmniImageOobm(const EImageOobm InImageOobm) noexcept
    {
        this->Style.NormalBrush.ImageOobm   = InImageOobm;
        this->Style.HoverBrush.ImageOobm    = InImageOobm;
        this->Style.PressBrush.ImageOobm    = InImageOobm;
        this->Style.DisabledBrush.ImageOobm = InImageOobm;

        return;
    }

    FORCEINLINE void SetNormalImagePadding(const f64 InImagePadding) noexcept { this->Style.NormalBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetHoverImagePadding(const f64 InImagePadding) noexcept { this->Style.HoverBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetPressImagePadding(const f64 InImagePadding) noexcept { this->Style.PressBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetDisabledImagePadding(const f64 InImagePadding) noexcept { this->Style.DisabledBrush.ImagePadding = InImagePadding; }
    FORCEINLINE void SetOmniImagePadding(const f64 InImagePadding) noexcept
    {
        this->Style.NormalBrush.ImagePadding   = InImagePadding;
        this->Style.HoverBrush.ImagePadding    = InImagePadding;
        this->Style.PressBrush.ImagePadding    = InImagePadding;
        this->Style.DisabledBrush.ImagePadding = InImagePadding;

        return;
    }

    FORCEINLINE void SetNormalRadii(LVec4F const& InRadii) noexcept { this->Style.NormalBrush.Radii = InRadii; }
    FORCEINLINE void SetHoverRadii(LVec4F const& InRadii) noexcept { this->Style.HoverBrush.Radii = InRadii; }
    FORCEINLINE void SetPressRadii(LVec4F const& InRadii) noexcept { this->Style.PressBrush.Radii = InRadii; }
    FORCEINLINE void SetDisabledRadii(LVec4F const& InRadii) noexcept { this->Style.DisabledBrush.Radii = InRadii; }
    FORCEINLINE void SetOmniRadii(LVec4F const& InRadii) noexcept
    {
        this->Style.NormalBrush.Radii   = InRadii;
        this->Style.HoverBrush.Radii    = InRadii;
        this->Style.PressBrush.Radii    = InRadii;
        this->Style.DisabledBrush.Radii = InRadii;

        return;
    }

    FORCEINLINE void SetNormalOutlineThickness(const f64 InOutlineThickness) noexcept { this->Style.NormalBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetHoverOutlineThickness(const f64 InOutlineThickness) noexcept { this->Style.HoverBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetPressOutlineThickness(const f64 InOutlineThickness) noexcept { this->Style.PressBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetDisabledOutlineThickness(const f64 InOutlineThickness) noexcept { this->Style.DisabledBrush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetOmniOutlineThickness(const f64 InOutlineThickness) noexcept
    {
        this->Style.NormalBrush.OutlineThickness   = InOutlineThickness;
        this->Style.HoverBrush.OutlineThickness    = InOutlineThickness;
        this->Style.PressBrush.OutlineThickness    = InOutlineThickness;
        this->Style.DisabledBrush.OutlineThickness = InOutlineThickness;

        return;
    }

    FORCEINLINE void SetNormalOutlineTint(LColor const& InOutlineTint) noexcept { this->Style.NormalBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetHoverOutlineTint(LColor const& InOutlineTint) noexcept { this->Style.HoverBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetPressOutlineTint(LColor const& InOutlineTint) noexcept { this->Style.PressBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetDisabledOutlineTint(LColor const& InOutlineTint) noexcept { this->Style.DisabledBrush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetOmniOutlineTint(LColor const& InOutlineTint) noexcept
    {
        this->Style.NormalBrush.OutlineTint   = InOutlineTint;
        this->Style.HoverBrush.OutlineTint    = InOutlineTint;
        this->Style.PressBrush.OutlineTint    = InOutlineTint;
        this->Style.DisabledBrush.OutlineTint = InOutlineTint;

        return;
    }

    FORCEINLINE void SetStyle(LButtonStyle const& InStyle) noexcept { this->Style = InStyle; }
    FORCEINLINE LButtonStyle const& GetStyle() const noexcept { return this->Style; }
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

    bool bLetUiReactToEvents:1{ true };
    bool bEnabled:1{ true };

    LButtonStyle Style;
};

template<typename TClass>
struct TFactoryButtonBase : NODE_FACTORY_PARENT(TClass)
{
    NODE_FACTORY_BODY(TClass)

    typedef TFunction<void(TClass* Self, LKeyEvent const& InKeyEvent)> _LOnButtonKeyEvent;

    decltype(auto) OnPrimaryPress(this auto&& Self, _LOnButtonKeyEvent&& InDelegate)
    {
        NODE_FACTORY_SELF().OnPrimaryPressDelegate = std::move(InDelegate);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OnPrimaryRelease(this auto&& Self, _LOnButtonKeyEvent&& InDelegate)
    {
        NODE_FACTORY_SELF().OnPrimaryReleaseDelegate = std::move(InDelegate);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OnSecondaryPress(this auto&& Self, _LOnButtonKeyEvent&& InDelegate)
    {
        NODE_FACTORY_SELF().OnSecondaryPressDelegate = std::move(InDelegate);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OnSecondaryRelease(this auto&& Self, _LOnButtonKeyEvent&& InDelegate)
    {
        NODE_FACTORY_SELF().OnSecondaryReleaseDelegate = std::move(InDelegate);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalBrush(this auto&& Self, const LRegionBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetNormalBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverBrush(this auto&& Self, const LRegionBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetHoverBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressBrush(this auto&& Self, const LRegionBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetPressBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledBrush(this auto&& Self, const LRegionBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetDisabledBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniBrush(this auto&& Self, const LRegionBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetOmniBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalType(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetNormalType(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverType(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetHoverType(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressType(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetPressType(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledType(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetDisabledType(InType);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniType(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetOmniType(InType);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetNormalTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetHoverTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetPressTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetDisabledTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetOmniTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalImage(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetNormalImage(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImage(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetHoverImage(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImage(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetPressImage(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImage(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetDisabledImage(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImage(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetOmniImage(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetNormalImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetHoverImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetPressImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetDisabledImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetOmniImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetNormalImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetHoverImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetPressImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetDisabledImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetOmniImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) NormalImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetNormalImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetHoverImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetPressImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetDisabledImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetOmniImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetNormalImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetHoverImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetPressImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetDisabledImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetOmniImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetNormalImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetHoverImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetPressImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetDisabledImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetOmniImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalRadii(this auto&& Self, const LVec4F& InRadii)
    {
        NODE_FACTORY_SELF().SetNormalRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverRadii(this auto&& Self, const LVec4F& InRadii)
    {
        NODE_FACTORY_SELF().SetHoverRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressRadii(this auto&& Self, const LVec4F& InRadii)
    {
        NODE_FACTORY_SELF().SetPressRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledRadii(this auto&& Self, const LVec4F& InRadii)
    {
        NODE_FACTORY_SELF().SetDisabledRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) OmniRadii(this auto&& Self, const LVec4F& InRadii)
    {
        NODE_FACTORY_SELF().SetOmniRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalOutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetNormalOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverOutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetHoverOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressOutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetPressOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledOutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetDisabledOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniOutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetOmniOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalOutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetNormalOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverOutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetHoverOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressOutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetPressOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledOutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetDisabledOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniOutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetOmniOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Style(this auto&& Self, const LButtonStyle& InStyle)
    {
        NODE_FACTORY_SELF().SetStyle(InStyle);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) UiResponsive(this auto&& Self, const bool bTrue)
    {
        NODE_FACTORY_SELF().SetLetUiReactToEvents(bTrue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Enabled(this auto&& Self, const bool bInEnabled)
    {
        NODE_FACTORY_SELF().SetEnabled(bInEnabled);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
