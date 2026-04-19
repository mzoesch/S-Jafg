// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#define JAFG_NODE_BUTTON_BOILERPLATE() \
    virtual LCursorReply OnCursorEnter() override{ return this->ButtonBase_OnCursorEnter(); } \
    virtual LCursorReply OnCursorLeave() override{ return this->ButtonBase_OnCursorLeave(); } \
    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override { return this->ButtonBase_OnKeyDown(Data, Event); } \
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override { return this->ButtonBase_OnKeyUp(Data, Event); }

namespace Jafg
{

template<typename TBrush = LRegionBrush> requires std::is_base_of_v<LRegionBrush, TBrush>
struct TButtonBaseStyle
{
    TBrush NormalBrush   {LRegionBrush{.Tint = {0x15,0x15,0x15}, .OutlineTint = Colors::Black}};
    TBrush HoverBrush    {LRegionBrush{.Tint = {0x1C,0x1C,0x1C}, .OutlineTint = Colors::White}};
    TBrush PressBrush    {LRegionBrush{.Tint = {0x24,0x24,0x24}, .OutlineTint = Colors::White}};
    TBrush SelectedBrush {LRegionBrush{.Tint = {0x24,0x24,0x24}, .OutlineTint = Colors::White}};
    TBrush DisabledBrush {LRegionBrush{.Tint = {0x0F,0x0F,0x0F}, .OutlineTint = Colors::Black}};
};

//# Inherit from this to access common button logic.
template<typename TNode, typename TBrush, typename TBrushProj> requires std::is_base_of_v<LRegionBrush, TBrush>
class TButtonBase
{
    typedef TButtonBase Derived;

public:

    typedef TBrush _ButtonBaseBrush;

    constexpr TButtonBase(TNode& InOwner, TBrushProj&& BrushProj) noexcept
        : Owner(InOwner), BrushProj(std::move(BrushProj)) {}
    virtual ~TButtonBase() = default;

    EVENT_DECL(OnPrimaryPressDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnPrimaryReleaseDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnSecondaryPressDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnSecondaryReleaseDelegate, void(TNode& Self, LKeyEvent const& Event))
    //# These methods will only be called if the delegates are not bound.
    virtual void OnPrimaryPress() {}
    virtual void OnPrimaryRelease() {}
    virtual void OnSecondaryPress() {}
    virtual void OnSecondaryRelease() {}

    NODISCARD constexpr bool IsEnabled() const noexcept { return this->bEnabled; }
    constexpr void SetEnabled(bool bInEnabled) noexcept
    {
        check(this->bSelected == false)
        if (this->bEnabled == bInEnabled)
        {
            return;
        }
        this->bEnabled = bInEnabled;

        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.NormalBrush);
            }
            else
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.DisabledBrush);
            }
        }

        this->OnEnabledStateChanged();
        return;
    }
    //# Delegate that is called when the enabled state changed.
    virtual void OnEnabledStateChanged() {}

    NODISCARD constexpr bool IsSelectable() const noexcept { return this->bSelectable; }
    constexpr void SetSelectable(bool bInSelectable) noexcept { check(this->bSelected == false) this->bSelectable = bInSelectable; }

    NODISCARD constexpr bool IsSelected() const noexcept { return this->bSelected; }
    constexpr void SetSelected(bool bInSelected) noexcept
    {
        check(this->bEnabled)
        check(this->bSelectable)
        if (this->bSelected == bInSelected)
        {
            return;
        }
        this->bSelected = bInSelected;

        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bSelected)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.SelectedBrush);
            }
            else
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.NormalBrush);
            }
        }

        this->OnSelectedStateChanged();
        return;
    }
    //# Delegate that is called when the selected state changed.
    virtual void OnSelectedStateChanged() {}

    NODISCARD constexpr bool IsUpdateBrushOnStateChange() const noexcept { return this->bUpdateBrushOnStateChange; }
    constexpr void SetUpdateBrushOnStateChange(bool bInUpdate) noexcept { this->bUpdateBrushOnStateChange = bInUpdate; }

    constexpr void SetStyle(TButtonBaseStyle<TBrush> const& InStyle) noexcept { this->Style = InStyle; }
    constexpr TButtonBaseStyle<TBrush>& GetMutableStyle() noexcept { return this->Style; }
    constexpr TButtonBaseStyle<TBrush> const& GetStyle() const noexcept { return this->Style; }

    constexpr TBrush& GetMutableNormalBrush() noexcept { return this->Style.NormalBrush; }
    constexpr TBrush const& GetNormalBrush() const noexcept { return this->Style.NormalBrush; }
    constexpr TBrush& GetMutableHoverBrush() noexcept { return this->Style.HoverBrush; }
    constexpr TBrush const& GetHoverBrush() const noexcept { return this->Style.HoverBrush; }
    constexpr TBrush& GetMutablePressBrush() noexcept { return this->Style.PressBrush; }
    constexpr TBrush const& GetPressBrush() const noexcept { return this->Style.PressBrush; }
    constexpr TBrush& GetSelectedBrush() noexcept { return this->Style.SelectedBrush; }
    constexpr TBrush const& GetSelectedBrush() const noexcept { return this->Style.SelectedBrush; }
    constexpr TBrush& GetMutableDisabledBrush() noexcept { return this->Style.DisabledBrush; }
    constexpr TBrush const& GetDisabledBrush() const noexcept { return this->Style.DisabledBrush; }

    constexpr void SetNormalBrush(TBrush const& InBrush) noexcept { this->Style.NormalBrush = InBrush; }
    constexpr void SetHoverBrush(TBrush const& InBrush) noexcept { this->Style.HoverBrush = InBrush; }
    constexpr void SetPressBrush(TBrush const& InBrush) noexcept { this->Style.PressBrush = InBrush; }
    constexpr void SetSelectedBrush(TBrush const& InBrush) noexcept { this->Style.SelectedBrush = InBrush; }
    constexpr void SetDisabledBrush(TBrush const& InBrush) noexcept { this->Style.DisabledBrush = InBrush; }
    constexpr void SetOmniBrush(TBrush const& InBrush) noexcept
    {
        this->Style.NormalBrush   = InBrush;
        this->Style.HoverBrush    = InBrush;
        this->Style.PressBrush    = InBrush;
        this->Style.SelectedBrush = InBrush;
        this->Style.DisabledBrush = InBrush;
    }

    constexpr void SetNormalTint(LColor const& InTint) noexcept { this->Style.NormalBrush.Tint = InTint; }
    constexpr void SetHoverTint(LColor const& InTint) noexcept { this->Style.HoverBrush.Tint = InTint; }
    constexpr void SetPressTint(LColor const& InTint) noexcept { this->Style.PressBrush.Tint = InTint; }
    constexpr void SetSelectedTint(LColor const& InTint) noexcept { this->Style.SelectedBrush.Tint = InTint; }
    constexpr void SetDisabledTint(LColor const& InTint) noexcept { this->Style.DisabledBrush.Tint = InTint; }
    constexpr void SetOmniTint(LColor const& InTint) noexcept
    {
        this->Style.NormalBrush.Tint   = InTint;
        this->Style.HoverBrush.Tint    = InTint;
        this->Style.PressBrush.Tint    = InTint;
        this->Style.SelectedBrush.Tint = InTint;
        this->Style.DisabledBrush.Tint = InTint;
    }

    constexpr void SetNormalTexture(LTexture2Ref InTexture) noexcept { this->Style.NormalBrush.Texture = std::move(InTexture); }
    constexpr void SetHoverTexture(LTexture2Ref InTexture) noexcept { this->Style.HoverBrush.Texture = std::move(InTexture); }
    constexpr void SetPressTexture(LTexture2Ref InTexture) noexcept { this->Style.PressBrush.Texture = std::move(InTexture); }
    constexpr void SetSelectedTexture(LTexture2Ref InTexture) noexcept { this->Style.SelectedBrush.Texture = std::move(InTexture); }
    constexpr void SetDisabledTexture(LTexture2Ref InTexture) noexcept { this->Style.DisabledBrush.Texture = std::move(InTexture); }
    constexpr void SetOmniTexture(LTexture2Ref InTexture) noexcept
    {
        this->Style.NormalBrush.Texture   = InTexture;
        this->Style.HoverBrush.Texture    = InTexture;
        this->Style.PressBrush.Texture    = InTexture;
        this->Style.SelectedBrush.Texture = InTexture;
        this->Style.DisabledBrush.Texture = InTexture;
    }

    constexpr void SetNormalTextureScale(f32 InScale) noexcept { this->Style.NormalBrush.TextureScale = InScale; }
    constexpr void SetHoverTextureScale(f32 InScale) noexcept { this->Style.HoverBrush.TextureScale = InScale; }
    constexpr void SetPressTextureScale(f32 InScale) noexcept { this->Style.PressBrush.TextureScale = InScale; }
    constexpr void SetSelectedTextureScale(f32 InScale) noexcept { this->Style.SelectedBrush.TextureScale = InScale; }
    constexpr void SetDisabledTextureScale(f32 InScale) noexcept { this->Style.DisabledBrush.TextureScale = InScale; }
    constexpr void SetOmniTextureScale(f32 InScale) noexcept
    {
        this->Style.NormalBrush.TextureScale   = InScale;
        this->Style.HoverBrush.TextureScale    = InScale;
        this->Style.PressBrush.TextureScale    = InScale;
        this->Style.SelectedBrush.TextureScale = InScale;
        this->Style.DisabledBrush.TextureScale = InScale;
    }

    constexpr void SetNormalTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Style.NormalBrush.TexCoordBehavior = InBehavior; }
    constexpr void SetHoverTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Style.HoverBrush.TexCoordBehavior = InBehavior; }
    constexpr void SetPressTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Style.PressBrush.TexCoordBehavior = InBehavior; }
    constexpr void SetSelectedTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Style.SelectedBrush.TexCoordBehavior = InBehavior; }
    constexpr void SetDisabledTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Style.DisabledBrush.TexCoordBehavior = InBehavior; }
    constexpr void SetOmniTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept
    {
        this->Style.NormalBrush.TexCoordBehavior   = InBehavior;
        this->Style.HoverBrush.TexCoordBehavior    = InBehavior;
        this->Style.PressBrush.TexCoordBehavior    = InBehavior;
        this->Style.SelectedBrush.TexCoordBehavior = InBehavior;
        this->Style.DisabledBrush.TexCoordBehavior = InBehavior;
    }

    constexpr void SetNormalSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept { this->Style.NormalBrush.SamplerAddressMode = InAddressMode; }
    constexpr void SetHoverSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept { this->Style.HoverBrush.SamplerAddressMode = InAddressMode; }
    constexpr void SetPressSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept { this->Style.PressBrush.SamplerAddressMode = InAddressMode; }
    constexpr void SetSelectedSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept { this->Style.SelectedBrush.SamplerAddressMode = InAddressMode; }
    constexpr void SetDisabledSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept { this->Style.DisabledBrush.SamplerAddressMode = InAddressMode; }
    constexpr void SetOmniSamplerAddressMode(UBO::BindlessTextureArray::Sampler InAddressMode) noexcept
    {
        this->Style.NormalBrush.SamplerAddressMode   = InAddressMode;
        this->Style.HoverBrush.SamplerAddressMode    = InAddressMode;
        this->Style.PressBrush.SamplerAddressMode    = InAddressMode;
        this->Style.SelectedBrush.SamplerAddressMode = InAddressMode;
        this->Style.DisabledBrush.SamplerAddressMode = InAddressMode;
    }

    constexpr void SetNormalTexturePadding(f32 InPadding) noexcept { this->Style.NormalBrush.TexturePadding = InPadding; }
    constexpr void SetHoverTexturePadding(f32 InPadding) noexcept { this->Style.HoverBrush.TexturePadding = InPadding; }
    constexpr void SetPressTexturePadding(f32 InPadding) noexcept { this->Style.PressBrush.TexturePadding = InPadding; }
    constexpr void SetSelectedTexturePadding(f32 InPadding) noexcept { this->Style.SelectedBrush.TexturePadding = InPadding; }
    constexpr void SetDisabledTexturePadding(f32 InPadding) noexcept { this->Style.DisabledBrush.TexturePadding = InPadding; }
    constexpr void SetOmniTexturePadding(f32 InPadding) noexcept
    {
        this->Style.NormalBrush.TexturePadding   = InPadding;
        this->Style.HoverBrush.TexturePadding    = InPadding;
        this->Style.PressBrush.TexturePadding    = InPadding;
        this->Style.SelectedBrush.TexturePadding = InPadding;
        this->Style.DisabledBrush.TexturePadding = InPadding;
    }

    constexpr void SetNormalBackgroundTint(LColor const& InTint) noexcept { this->Style.NormalBrush.BackgroundTint = InTint; }
    constexpr void SetHoverBackgroundTint(LColor const& InTint) noexcept { this->Style.HoverBrush.BackgroundTint = InTint; }
    constexpr void SetPressBackgroundTint(LColor const& InTint) noexcept { this->Style.PressBrush.BackgroundTint = InTint; }
    constexpr void SetSelectedBackgroundTint(LColor const& InTint) noexcept { this->Style.SelectedBrush.BackgroundTint = InTint; }
    constexpr void SetDisabledBackgroundTint(LColor const& InTint) noexcept { this->Style.DisabledBrush.BackgroundTint = InTint; }
    constexpr void SetOmniBackgroundTint(LColor const& InTint) noexcept
    {
        this->Style.NormalBrush.BackgroundTint   = InTint;
        this->Style.HoverBrush.BackgroundTint    = InTint;
        this->Style.PressBrush.BackgroundTint    = InTint;
        this->Style.SelectedBrush.BackgroundTint = InTint;
        this->Style.DisabledBrush.BackgroundTint = InTint;
    }

    constexpr void SetNormalRadii(LVec4F const& InRadii) noexcept { this->Style.NormalBrush.Radii = InRadii; }
    constexpr void SetHoverRadii(LVec4F const& InRadii) noexcept { this->Style.HoverBrush.Radii = InRadii; }
    constexpr void SetPressRadii(LVec4F const& InRadii) noexcept { this->Style.PressBrush.Radii = InRadii; }
    constexpr void SetSelectedRadii(LVec4F const& InRadii) noexcept { this->Style.SelectedBrush.Radii = InRadii; }
    constexpr void SetDisabledRadii(LVec4F const& InRadii) noexcept { this->Style.DisabledBrush.Radii = InRadii; }
    constexpr void SetOmniRadii(LVec4F const& InRadii) noexcept
    {
        this->Style.NormalBrush.Radii   = InRadii;
        this->Style.HoverBrush.Radii    = InRadii;
        this->Style.PressBrush.Radii    = InRadii;
        this->Style.SelectedBrush.Radii = InRadii;
        this->Style.DisabledBrush.Radii = InRadii;
    }

    constexpr void SetNormalClampRadii(bool bInClamp) noexcept { this->Style.NormalBrush.bClampRadii = bInClamp; }
    constexpr void SetHoverClampRadii(bool bInClamp) noexcept { this->Style.HoverBrush.bClampRadii = bInClamp; }
    constexpr void SetPressClampRadii(bool bInClamp) noexcept { this->Style.PressBrush.bClampRadii = bInClamp; }
    constexpr void SetSelectedClampRadii(bool bInClamp) noexcept { this->Style.SelectedBrush.bClampRadii = bInClamp; }
    constexpr void SetDisabledClampRadii(bool bInClamp) noexcept { this->Style.DisabledBrush.bClampRadii = bInClamp; }
    constexpr void SetOmniClampRadii(bool bInClamp) noexcept
    {
        this->Style.NormalBrush.bClampRadii   = bInClamp;
        this->Style.HoverBrush.bClampRadii    = bInClamp;
        this->Style.PressBrush.bClampRadii    = bInClamp;
        this->Style.SelectedBrush.bClampRadii = bInClamp;
        this->Style.DisabledBrush.bClampRadii = bInClamp;
    }

    constexpr void SetNormalSkipBrushDraw(bool bInSkip) noexcept { this->Style.NormalBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetHoverSkipBrushDraw(bool bInSkip) noexcept { this->Style.HoverBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetPressSkipBrushDraw(bool bInSkip) noexcept { this->Style.PressBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetSelectedSkipBrushDraw(bool bInSkip) noexcept { this->Style.SelectedBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetDisabledSkipBrushDraw(bool bInSkip) noexcept { this->Style.DisabledBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetOmniSkipBrushDraw(bool bInSkip) noexcept
    {
        this->Style.NormalBrush.bSkipBrushDraw   = bInSkip;
        this->Style.HoverBrush.bSkipBrushDraw    = bInSkip;
        this->Style.PressBrush.bSkipBrushDraw    = bInSkip;
        this->Style.SelectedBrush.bSkipBrushDraw = bInSkip;
        this->Style.DisabledBrush.bSkipBrushDraw = bInSkip;
    }

    constexpr void SetNormalOutlineThickness(f32 InThickness) noexcept { this->Style.NormalBrush.OutlineThickness = InThickness; }
    constexpr void SetHoverOutlineThickness(f32 InThickness) noexcept { this->Style.HoverBrush.OutlineThickness = InThickness; }
    constexpr void SetPressOutlineThickness(f32 InThickness) noexcept { this->Style.PressBrush.OutlineThickness = InThickness; }
    constexpr void SetSelectedOutlineThickness(f32 InThickness) noexcept { this->Style.SelectedBrush.OutlineThickness = InThickness; }
    constexpr void SetDisabledOutlineThickness(f32 InThickness) noexcept { this->Style.DisabledBrush.OutlineThickness = InThickness; }
    constexpr void SetOmniOutlineThickness(f32 InThickness) noexcept
    {
        this->Style.NormalBrush.OutlineThickness   = InThickness;
        this->Style.HoverBrush.OutlineThickness    = InThickness;
        this->Style.PressBrush.OutlineThickness    = InThickness;
        this->Style.SelectedBrush.OutlineThickness = InThickness;
        this->Style.DisabledBrush.OutlineThickness = InThickness;
    }

    constexpr void SetNormalOutlineTint(LColor const& InTint) noexcept { this->Style.NormalBrush.OutlineTint = InTint; }
    constexpr void SetHoverOutlineTint(LColor const& InTint) noexcept { this->Style.HoverBrush.OutlineTint = InTint; }
    constexpr void SetPressOutlineTint(LColor const& InTint) noexcept { this->Style.PressBrush.OutlineTint = InTint; }
    constexpr void SetSelectedOutlineTint(LColor const& InTint) noexcept { this->Style.SelectedBrush.OutlineTint = InTint; }
    constexpr void SetDisabledOutlineTint(LColor const& InTint) noexcept { this->Style.DisabledBrush.OutlineTint = InTint; }
    constexpr void SetOmniOutlineTint(LColor const& InTint) noexcept
    {
        this->Style.NormalBrush.OutlineTint   = InTint;
        this->Style.HoverBrush.OutlineTint    = InTint;
        this->Style.PressBrush.OutlineTint    = InTint;
        this->Style.SelectedBrush.OutlineTint = InTint;
        this->Style.DisabledBrush.OutlineTint = InTint;
    }

protected:

    virtual void ButtonBase_Construct()
    {
        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                if (this->bSelected)
                {
                    std::invoke(this->BrushProj, this->Owner, this->Style.SelectedBrush);
                }
                else
                {
                    std::invoke(this->BrushProj, this->Owner, this->Style.NormalBrush);
                }
            }
            else
            {
                check(this->bSelected == false)
                std::invoke(this->BrushProj, this->Owner, this->Style.DisabledBrush);
            }
        }
        return;
    }

    LCursorReply ButtonBase_OnCursorEnter()
    {
        if (this->bEnabled == false)
        {
            return {};
        }
        if (this->bUpdateBrushOnStateChange && this->bSelected == false)
        {
            std ::invoke(this->BrushProj, this->Owner, this->Style.HoverBrush);
        }
        if (this->Owner.OnCursorEnterEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnCursorEnterEvent.Invoke(this->Owner)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        return LCursorReply::Handled();
    }

    LCursorReply ButtonBase_OnCursorLeave()
    {
        if (this->bEnabled == false)
        {
            return {};
        }
        if (this->bUpdateBrushOnStateChange && this->bSelected == false)
        {
            std::invoke(this->BrushProj, this->Owner, this->Style.NormalBrush);
        }
        if (this->Owner.OnCursorLeaveEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnCursorLeaveEvent.Invoke(this->Owner)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        return LCursorReply::Handled();
    }

    LReply ButtonBase_OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
    {
        if (this->bEnabled == false)
        {
            if (this->Owner.IsParentValid())
            {
                return this->Owner.GetParentChecked()->OnKeyDown(Data, Event);
            }
            return LReply::Unhandled();
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.PressBrush);
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyDownEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnPrimaryPressDelegate.IsValid())
            {
                this->OnPrimaryPressDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnPrimaryPress();
            }
            return Result;
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.PressBrush);
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyDownEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnSecondaryPressDelegate.IsValid())
            {
                this->OnSecondaryPressDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnSecondaryPress();
            }
            return Result;
        }

        if (this->Owner.OnKeyDownEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        if (this->Owner.IsParentValid())
        {
            return this->Owner.GetParentChecked()->OnKeyDown(Data, Event);
        }
        return LReply::Unhandled();
    }

    LReply ButtonBase_OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event)
    {
        if (this->bEnabled == false)
        {
            if (this->Owner.IsParentValid())
            {
                return this->Owner.GetParentChecked()->OnKeyUp(Data, Event);
            }
            return LReply::Unhandled();
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.HoverBrush);
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyUpEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnPrimaryReleaseDelegate.IsValid())
            {
                this->OnPrimaryReleaseDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnPrimaryRelease();
            }
            return Result;
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                std::invoke(this->BrushProj, this->Owner, this->Style.HoverBrush);
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyUpEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnSecondaryReleaseDelegate.IsValid())
            {
                this->OnSecondaryReleaseDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnSecondaryRelease();
            }
            return Result;
        }

        if (this->Owner.OnKeyUpEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        if (this->Owner.IsParentValid())
        {
            return this->Owner.GetParentChecked()->OnKeyUp(Data, Event);
        }
        return LReply::Unhandled();
    }

    TNode& Owner;
    TBrushProj BrushProj;
    bool bEnabled:1 { true };
    bool bSelectable:1 {};
    bool bSelected:1 {};
    //# TODO bool: Auto select on press.
    //# TODO bool: Call delegates if selected
    bool bUpdateBrushOnStateChange:1 { true };
    TButtonBaseStyle<TBrush> Style;
};

template<typename TClass>
struct TFactoryButtonBase : NODE_FACTORY_PARENT(TClass)
{
    NODE_FACTORY_BODY(TClass)

    typedef typename TClass::_ButtonBaseBrush Brush;

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnPrimaryPress, OnPrimaryPressDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnPrimaryRelease, OnPrimaryReleaseDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnSecondaryPress, OnSecondaryPressDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnSecondaryRelease, OnSecondaryReleaseDelegate)

    decltype(auto) Enabled(this auto&& Self, bool bInEnabled) noexcept
    {
        NODE_FACTORY_SELF().SetEnabled(bInEnabled);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Selectable(this auto&& Self, bool bInSelectable) noexcept
    {
        NODE_FACTORY_SELF().SetSelectable(bInSelectable);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Selected(this auto&& Self, bool bInSelected) noexcept
    {
        NODE_FACTORY_SELF().SetSelected(bInSelected);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) UpdateBrushOnStateChange(this auto&& Self, bool bInUpdate) noexcept
    {
        NODE_FACTORY_SELF().SetUpdateBrushOnStateChange(bInUpdate);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Style(this auto&& Self, TButtonBaseStyle<Brush> const& InStyle) noexcept
    {
        NODE_FACTORY_SELF().SetStyle(InStyle);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetNormalBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetHoverBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetPressBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniBrush(this auto&& Self, Brush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetOmniBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetPressTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTexture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextureScale(this auto&& Self, f32 InTextureScale) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetPressTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTexCoordBehavior(this auto&& Self, ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetNormalSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetHoverSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetPressSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniSamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetOmniSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetPressTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniBackgroundTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniBackgroundTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetNormalRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetHoverRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetPressRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniRadii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetOmniRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetNormalClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetHoverClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetPressClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetOmniClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetNormalSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetHoverSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetPressSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniSkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetOmniSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetNormalOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetHoverOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetPressOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniOutlineThickness(this auto&& Self, f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetOmniOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
