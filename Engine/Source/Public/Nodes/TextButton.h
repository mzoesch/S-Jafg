// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "Nodes/ButtonBase.h"
#include "TextButton.generated.h"

namespace Jafg
{

struct LFactoryTextButton;

struct LTextButtonTextStyle
{
    LTextBrush NormalBrush{ ETextScale::Body };
    LTextBrush HoverBrush{ ETextScale::Body };
    LTextBrush PressBrush{ ETextScale::Body };
    LTextBrush SelectedBrush{ ETextScale::Body };
    LTextBrush DisabledBrush{ ETextScale::Body };
};

struct LTextButtonIconBrush
{
    u32 Scale{ 1 };
    LWidgetSize1 RightPadding{ 6_spt };
    LColor Tint{ Colors::White };
};

struct LTextButtonIconStyle
{
    LTextButtonIconBrush NormalBrush;
    LTextButtonIconBrush HoverBrush;
    LTextButtonIconBrush PressBrush;
    LTextButtonIconBrush SelectedBrush;
    LTextButtonIconBrush DisabledBrush;
};

//# A button with text content. Use this for buttons that only display text to save performance.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButton)
class ENGINE_API WTextButton : public WTextBox, public TButtonBase<WTextButton, LBoxBrush, decltype(&WTextBox::SetBrush)>
{
    GENERATED_CLASS_BODY()

    typedef TButtonBase<WTextButton, LBoxBrush, decltype(&WTextBox::SetBrush)> SuperButton;

protected:

    explicit WTextButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this, &WTextBox::SetBrush}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }
    template<typename TCxxClass>
    explicit WTextButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this, &WTextBox::SetBrush}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Construct() override
    {
        Super::Construct();
        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                if (this->bSelected)
                {
                    this->SetTextBrush(this->TextStyle.SelectedBrush);
                    this->SetIconBrush(this->IconStyle.SelectedBrush);
                }
                else
                {
                    this->SetTextBrush(this->TextStyle.NormalBrush);
                    this->SetIconBrush(this->IconStyle.NormalBrush);
                }
            }
            else
            {
                this->SetTextBrush(this->TextStyle.DisabledBrush);
                this->SetIconBrush(this->IconStyle.DisabledBrush);
            }
        }
        this->ButtonBase_Construct();
        return;
    }
    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    virtual LCursorReply OnCursorEnter() override
    {
        if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            this->SetTextBrush(this->TextStyle.HoverBrush);
            this->SetIconBrush(this->IconStyle.HoverBrush);
        }
        return this->ButtonBase_OnCursorEnter();
    }
    virtual LCursorReply OnCursorLeave() override
    {
        if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            this->SetTextBrush(this->TextStyle.NormalBrush);
            this->SetIconBrush(this->IconStyle.NormalBrush);
        }
        return this->ButtonBase_OnCursorLeave();
    }

    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override
    {
        if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            if (   Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton)
                || Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::RightMouseButton))
            {
                this->SetTextBrush(this->TextStyle.PressBrush);
                this->SetIconBrush(this->IconStyle.PressBrush);
            }
        }
        return this->ButtonBase_OnKeyDown(Data, Event);
    }
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override
    {
        if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            if (   Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton)
                || Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::RightMouseButton))
            {
                this->SetTextBrush(this->TextStyle.HoverBrush);
                this->SetIconBrush(this->IconStyle.HoverBrush);
            }
        }
        return this->ButtonBase_OnKeyUp(Data, Event);
    }

    virtual void OnEnabledStateChanged() override
    {
        SuperButton::OnEnabledStateChanged();
        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                this->SetTextBrush(this->TextStyle.NormalBrush);
                this->SetIconBrush(this->IconStyle.NormalBrush);
            }
            else
            {
                this->SetTextBrush(this->TextStyle.DisabledBrush);
                this->SetIconBrush(this->IconStyle.DisabledBrush);
            }
        }
        return;
    }
    virtual void OnSelectedStateChanged() override
    {
        SuperButton::OnSelectedStateChanged();
        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bSelected)
            {
                this->SetTextBrush(this->TextStyle.SelectedBrush);
                this->SetIconBrush(this->IconStyle.SelectedBrush);
            }
            else
            {
                this->SetTextBrush(this->TextStyle.NormalBrush);
                this->SetIconBrush(this->IconStyle.NormalBrush);
            }
        }
        return;
    }

    constexpr void SetNormalPadding(LPadding const& InPadding) noexcept { this->Style.NormalBrush.Padding = InPadding; }
    constexpr void SetHoverPadding(LPadding const& InPadding) noexcept { this->Style.HoverBrush.Padding = InPadding; }
    constexpr void SetPressPadding(LPadding const& InPadding) noexcept { this->Style.PressBrush.Padding = InPadding; }
    constexpr void SetSelectedPadding(LPadding const& InPadding) noexcept { this->Style.SelectedBrush.Padding = InPadding; }
    constexpr void SetDisabledPadding(LPadding const& InPadding) noexcept { this->Style.DisabledBrush.Padding = InPadding; }
    constexpr void SetOmniPadding(LPadding const& InPadding) noexcept
    {
        this->Style.NormalBrush.Padding   = InPadding;
        this->Style.HoverBrush.Padding    = InPadding;
        this->Style.PressBrush.Padding    = InPadding;
        this->Style.SelectedBrush.Padding = InPadding;
        this->Style.DisabledBrush.Padding = InPadding;
    }

    constexpr bool IsIconValid() const noexcept { return this->Icon.get(); }
    constexpr void SetIcon(LTexture2Ref InIcon) noexcept { this->Icon = std::move(InIcon); }
    constexpr auto const& GetIcon() const noexcept { return this->Icon; }

    constexpr void SetIconBrush(LTextButtonIconBrush const& InBrush) noexcept { this->IconBrush = InBrush; }
    NODISCARD constexpr LTextButtonIconBrush const& GetIconBrush() const noexcept { return this->IconBrush; }

    constexpr void SetIconScale(u32 InScale) noexcept { this->IconBrush.Scale = InScale; }
    constexpr void SetIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconBrush.RightPadding = InRightPadding; }
    constexpr void SetIconTint(LColor const& InTint) noexcept { this->IconBrush.Tint = InTint; }
    NODISCARD constexpr u32 GetIconScale() const noexcept { return this->IconBrush.Scale; }
    NODISCARD constexpr LWidgetSize1 GetIconRightPadding() const noexcept { return this->IconBrush.RightPadding; }
    NODISCARD constexpr LColor GetIconTint() const noexcept { return this->IconBrush.Tint; }

    constexpr void SetNormalIconScale(u32 InScale) noexcept { this->IconStyle.NormalBrush.Scale = InScale; }
    constexpr void SetHoverIconScale(u32 InScale) noexcept { this->IconStyle.HoverBrush.Scale = InScale; }
    constexpr void SetPressIconScale(u32 InScale) noexcept { this->IconStyle.PressBrush.Scale = InScale; }
    constexpr void SetSelectedIconScale(u32 InScale) noexcept { this->IconStyle.SelectedBrush.Scale = InScale; }
    constexpr void SetDisabledIconScale(u32 InScale) noexcept { this->IconStyle.DisabledBrush.Scale = InScale; }
    constexpr void SetOmniIconScale(u32 InScale) noexcept
    {
        this->IconStyle.NormalBrush.Scale   = InScale;
        this->IconStyle.HoverBrush.Scale    = InScale;
        this->IconStyle.PressBrush.Scale    = InScale;
        this->IconStyle.SelectedBrush.Scale = InScale;
        this->IconStyle.DisabledBrush.Scale = InScale;
    }

    constexpr void SetNormalIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconStyle.NormalBrush.RightPadding = InRightPadding; }
    constexpr void SetHoverIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconStyle.HoverBrush.RightPadding = InRightPadding; }
    constexpr void SetPressIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconStyle.PressBrush.RightPadding = InRightPadding; }
    constexpr void SetSelectedIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconStyle.SelectedBrush.RightPadding = InRightPadding; }
    constexpr void SetDisabledIconRightPadding(LWidgetSize1 InRightPadding) noexcept { this->IconStyle.DisabledBrush.RightPadding = InRightPadding; }
    constexpr void SetOmniIconRightPadding(LWidgetSize1 InRightPadding) noexcept
    {
        this->IconStyle.NormalBrush.RightPadding   = InRightPadding;
        this->IconStyle.HoverBrush.RightPadding    = InRightPadding;
        this->IconStyle.PressBrush.RightPadding    = InRightPadding;
        this->IconStyle.SelectedBrush.RightPadding = InRightPadding;
        this->IconStyle.DisabledBrush.RightPadding = InRightPadding;
    }

    constexpr void SetNormalIconTint(LColor const& InTint) noexcept { this->IconStyle.NormalBrush.Tint = InTint; }
    constexpr void SetHoverIconTint(LColor const& InTint) noexcept { this->IconStyle.HoverBrush.Tint = InTint; }
    constexpr void SetPressIconTint(LColor const& InTint) noexcept { this->IconStyle.PressBrush.Tint = InTint; }
    constexpr void SetSelectedIconTint(LColor const& InTint) noexcept { this->IconStyle.SelectedBrush.Tint = InTint; }
    constexpr void SetDisabledIconTint(LColor const& InTint) noexcept { this->IconStyle.DisabledBrush.Tint = InTint; }
    constexpr void SetOmniIconTint(LColor const& InTint) noexcept
    {
        this->IconStyle.NormalBrush.Tint   = InTint;
        this->IconStyle.HoverBrush.Tint    = InTint;
        this->IconStyle.PressBrush.Tint    = InTint;
        this->IconStyle.SelectedBrush.Tint = InTint;
        this->IconStyle.DisabledBrush.Tint = InTint;
    }

    constexpr void SetIconStyle(LTextButtonIconStyle const& InStyle) noexcept { this->IconStyle = InStyle; }
    NODISCARD constexpr LTextButtonIconStyle& GetMutableIconStyle() noexcept { return this->IconStyle; }
    NODISCARD constexpr LTextButtonIconStyle const& GetIconStyle() const noexcept { return this->IconStyle; }

    constexpr void SetNormalTextScale(LTextScale InScale) noexcept { this->TextStyle.NormalBrush.TextScale = InScale; }
    constexpr void SetHoverTextScale(LTextScale InScale) noexcept { this->TextStyle.HoverBrush.TextScale = InScale; }
    constexpr void SetPressTextScale(LTextScale InScale) noexcept { this->TextStyle.PressBrush.TextScale = InScale; }
    constexpr void SetSelectedTextScale(LTextScale InScale) noexcept { this->TextStyle.SelectedBrush.TextScale = InScale; }
    constexpr void SetDisabledTextScale(LTextScale InScale) noexcept { this->TextStyle.DisabledBrush.TextScale = InScale; }
    constexpr void SetOmniTextScale(LTextScale InScale) noexcept
    {
        this->TextStyle.NormalBrush.TextScale   = InScale;
        this->TextStyle.HoverBrush.TextScale    = InScale;
        this->TextStyle.PressBrush.TextScale    = InScale;
        this->TextStyle.SelectedBrush.TextScale = InScale;
        this->TextStyle.DisabledBrush.TextScale = InScale;
    }

    constexpr void SetNormalTextTint(LColor const& InTint) noexcept { this->TextStyle.NormalBrush.Tint = InTint; }
    constexpr void SetHoverTextTint(LColor const& InTint) noexcept { this->TextStyle.HoverBrush.Tint = InTint; }
    constexpr void SetPressTextTint(LColor const& InTint) noexcept { this->TextStyle.PressBrush.Tint = InTint; }
    constexpr void SetSelectedTextTint(LColor const& InTint) noexcept { this->TextStyle.SelectedBrush.Tint = InTint; }
    constexpr void SetDisabledTextTint(LColor const& InTint) noexcept { this->TextStyle.DisabledBrush.Tint = InTint; }
    constexpr void SetOmniTextTint(LColor const& InTint) noexcept
    {
        this->TextStyle.NormalBrush.Tint   = InTint;
        this->TextStyle.HoverBrush.Tint    = InTint;
        this->TextStyle.PressBrush.Tint    = InTint;
        this->TextStyle.SelectedBrush.Tint = InTint;
        this->TextStyle.DisabledBrush.Tint = InTint;
    }

    constexpr void SetNormalSkipTextBrushDraw(bool bInSkip) noexcept { this->TextStyle.NormalBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetHoverSkipTextBrushDraw(bool bInSkip) noexcept { this->TextStyle.HoverBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetPressSkipTextBrushDraw(bool bInSkip) noexcept { this->TextStyle.PressBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetSelectedSkipTextBrushDraw(bool bInSkip) noexcept { this->TextStyle.SelectedBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetDisabledSkipTextBrushDraw(bool bInSkip) noexcept { this->TextStyle.DisabledBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetOmniSkipTextBrushDraw(bool bInSkip) noexcept
    {
        this->TextStyle.NormalBrush.bSkipBrushDraw   = bInSkip;
        this->TextStyle.HoverBrush.bSkipBrushDraw    = bInSkip;
        this->TextStyle.PressBrush.bSkipBrushDraw    = bInSkip;
        this->TextStyle.SelectedBrush.bSkipBrushDraw = bInSkip;
        this->TextStyle.DisabledBrush.bSkipBrushDraw = bInSkip;
    }

    constexpr void SetNormalTextOutlineThickness(f32 InThickness) noexcept { this->TextStyle.NormalBrush.OutlineThickness = InThickness; }
    constexpr void SetHoverTextOutlineThickness(f32 InThickness) noexcept { this->TextStyle.HoverBrush.OutlineThickness = InThickness; }
    constexpr void SetPressTextOutlineThickness(f32 InThickness) noexcept { this->TextStyle.PressBrush.OutlineThickness = InThickness; }
    constexpr void SetSelectedTextOutlineThickness(f32 InThickness) noexcept { this->TextStyle.SelectedBrush.OutlineThickness = InThickness; }
    constexpr void SetDisabledTextOutlineThickness(f32 InThickness) noexcept { this->TextStyle.DisabledBrush.OutlineThickness = InThickness; }
    constexpr void SetOmniTextOutlineThickness(f32 InThickness) noexcept
    {
        this->TextStyle.NormalBrush.OutlineThickness   = InThickness;
        this->TextStyle.HoverBrush.OutlineThickness    = InThickness;
        this->TextStyle.PressBrush.OutlineThickness    = InThickness;
        this->TextStyle.SelectedBrush.OutlineThickness = InThickness;
        this->TextStyle.DisabledBrush.OutlineThickness = InThickness;
    }

    constexpr void SetNormalTextOutlineTint(LColor const& InTint) noexcept { this->TextStyle.NormalBrush.OutlineTint = InTint; }
    constexpr void SetHoverTextOutlineTint(LColor const& InTint) noexcept { this->TextStyle.HoverBrush.OutlineTint = InTint; }
    constexpr void SetPressTextOutlineTint(LColor const& InTint) noexcept { this->TextStyle.PressBrush.OutlineTint = InTint; }
    constexpr void SetSelectedTextOutlineTint(LColor const& InTint) noexcept { this->TextStyle.SelectedBrush.OutlineTint = InTint; }
    constexpr void SetDisabledTextOutlineTint(LColor const& InTint) noexcept { this->TextStyle.DisabledBrush.OutlineTint = InTint; }
    constexpr void SetOmniTextOutlineTint(LColor const& InTint) noexcept
    {
        this->TextStyle.NormalBrush.OutlineTint   = InTint;
        this->TextStyle.HoverBrush.OutlineTint    = InTint;
        this->TextStyle.PressBrush.OutlineTint    = InTint;
        this->TextStyle.SelectedBrush.OutlineTint = InTint;
        this->TextStyle.DisabledBrush.OutlineTint = InTint;
    }

    constexpr void SetNormalTextHAlign(ETextHAlign InAlign) noexcept { this->TextStyle.NormalBrush.TextHAlign = InAlign; }
    constexpr void SetHoverTextHAlign(ETextHAlign InAlign) noexcept { this->TextStyle.HoverBrush.TextHAlign = InAlign; }
    constexpr void SetPressTextHAlign(ETextHAlign InAlign) noexcept { this->TextStyle.PressBrush.TextHAlign = InAlign; }
    constexpr void SetSelectedTextHAlign(ETextHAlign InAlign) noexcept { this->TextStyle.SelectedBrush.TextHAlign = InAlign; }
    constexpr void SetDisabledTextHAlign(ETextHAlign InAlign) noexcept { this->TextStyle.DisabledBrush.TextHAlign = InAlign; }
    constexpr void SetOmniTextHAlign(ETextHAlign InAlign) noexcept
    {
        this->TextStyle.NormalBrush.TextHAlign   = InAlign;
        this->TextStyle.HoverBrush.TextHAlign    = InAlign;
        this->TextStyle.PressBrush.TextHAlign    = InAlign;
        this->TextStyle.SelectedBrush.TextHAlign = InAlign;
        this->TextStyle.DisabledBrush.TextHAlign = InAlign;
    }

    constexpr void SetNormalTextVAlign(ETextVAlign InAlign) noexcept { this->TextStyle.NormalBrush.TextVAlign = InAlign; }
    constexpr void SetHoverTextVAlign(ETextVAlign InAlign) noexcept { this->TextStyle.HoverBrush.TextVAlign = InAlign; }
    constexpr void SetPressTextVAlign(ETextVAlign InAlign) noexcept { this->TextStyle.PressBrush.TextVAlign = InAlign; }
    constexpr void SetSelectedTextVAlign(ETextVAlign InAlign) noexcept { this->TextStyle.SelectedBrush.TextVAlign = InAlign; }
    constexpr void SetDisabledTextVAlign(ETextVAlign InAlign) noexcept { this->TextStyle.DisabledBrush.TextVAlign = InAlign; }
    constexpr void SetOmniTextVAlign(ETextVAlign InAlign) noexcept
    {
        this->TextStyle.NormalBrush.TextVAlign   = InAlign;
        this->TextStyle.HoverBrush.TextVAlign    = InAlign;
        this->TextStyle.PressBrush.TextVAlign    = InAlign;
        this->TextStyle.SelectedBrush.TextVAlign = InAlign;
        this->TextStyle.DisabledBrush.TextVAlign = InAlign;
    }

    constexpr void SetTextStyle(LTextButtonTextStyle const& InStyle) noexcept { this->TextStyle = InStyle; }
    NODISCARD constexpr LTextButtonTextStyle& GetMutableTextStyle() noexcept { return this->TextStyle; }
    NODISCARD constexpr LTextButtonTextStyle const& GetTextStyle() const noexcept { return this->TextStyle; }

private:

    LTexture2Ref Icon;
    LTextButtonIconBrush IconBrush;
    LTextButtonIconStyle IconStyle;
    LTextButtonTextStyle TextStyle;
};

struct LFactoryTextButton : public TFactoryButtonBase<WTextButton>
{
    NODE_FACTORY_BODY(WTextButton)

    decltype(auto) NormalPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetNormalPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetHoverPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetPressPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniPadding(this auto&& Self, LPadding const& InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetOmniPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Icon(this auto&& Self, LTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().SetIcon(std::move(InIcon));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetNormalIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetHoverIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetPressIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetOmniIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetNormalIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetHoverIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetPressIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniIconRightPadding(this auto&& Self, LWidgetSize1 InRightPadding) noexcept
    {
        NODE_FACTORY_SELF().SetOmniIconRightPadding(InRightPadding);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetNormalSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetHoverSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetPressSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniSkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetOmniSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetNormalTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetHoverTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetPressTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniTextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetOmniTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
