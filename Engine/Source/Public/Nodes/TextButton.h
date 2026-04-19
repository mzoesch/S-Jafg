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
    LWidgetSize1 InwardsPadding{ 6_spt };
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

//# A button with text content and an optional icon on the left or right. Use this for buttons that only display text/icons to save performance.
//# TODO: Instead of a button make a WIconTextBox and then => WIconTextButton
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

    virtual void Construct() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorMoved(const LVec2F& InLocation) override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual void OnEnabledStateChanged() override;
    virtual void OnSelectedStateChanged() override;

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

    constexpr bool IsLeftIconValid() const noexcept { return this->LeftIcon.get(); }
    constexpr void SetLeftIcon(LTexture2Ref InIcon) noexcept { this->LeftIcon = std::move(InIcon); }
    constexpr auto const& GetLeftIcon() const noexcept { return this->LeftIcon; }
    constexpr bool IsRightIconValid() const noexcept { return this->RightIcon.get(); }
    constexpr void SetRightIcon(LTexture2Ref InIcon) noexcept { this->RightIcon = std::move(InIcon); }
    constexpr auto const& GetRightIcon() const noexcept { return this->RightIcon; }

    constexpr void SetLeftIconBrush(LTextButtonIconBrush const& InBrush) noexcept { this->LeftIconBrush = InBrush; }
    NODISCARD constexpr LTextButtonIconBrush const& GetLeftIconBrush() const noexcept { return this->LeftIconBrush; }
    constexpr void SetRightIconBrush(LTextButtonIconBrush const& InBrush) noexcept { this->RightIconBrush = InBrush; }
    NODISCARD constexpr LTextButtonIconBrush const& GetRightIconBrush() const noexcept { return this->RightIconBrush; }

    constexpr void SetLeftIconScale(u32 InScale) noexcept { this->LeftIconBrush.Scale = InScale; }
    constexpr void SetLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetLeftIconTint(LColor const& InTint) noexcept { this->LeftIconBrush.Tint = InTint; }
    NODISCARD constexpr u32 GetLeftIconScale() const noexcept { return this->LeftIconBrush.Scale; }
    NODISCARD constexpr LWidgetSize1 GetLeftIconInwardsPadding() const noexcept { return this->LeftIconBrush.InwardsPadding; }
    NODISCARD constexpr LColor GetLeftIconTint() const noexcept { return this->LeftIconBrush.Tint; }
    constexpr void SetRightIconScale(u32 InScale) noexcept { this->RightIconBrush.Scale = InScale; }
    constexpr void SetRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetRightIconTint(LColor const& InTint) noexcept { this->RightIconBrush.Tint = InTint; }
    NODISCARD constexpr u32 GetRightIconScale() const noexcept { return this->RightIconBrush.Scale; }
    NODISCARD constexpr LWidgetSize1 GetRightIconInwardsPadding() const noexcept { return this->RightIconBrush.InwardsPadding; }
    NODISCARD constexpr LColor GetRightIconTint() const noexcept { return this->RightIconBrush.Tint; }

    constexpr void SetNormalLeftIconScale(u32 InScale) noexcept { this->LeftIconStyle.NormalBrush.Scale = InScale; }
    constexpr void SetHoverLeftIconScale(u32 InScale) noexcept { this->LeftIconStyle.HoverBrush.Scale = InScale; }
    constexpr void SetPressLeftIconScale(u32 InScale) noexcept { this->LeftIconStyle.PressBrush.Scale = InScale; }
    constexpr void SetSelectedLeftIconScale(u32 InScale) noexcept { this->LeftIconStyle.SelectedBrush.Scale = InScale; }
    constexpr void SetDisabledLeftIconScale(u32 InScale) noexcept { this->LeftIconStyle.DisabledBrush.Scale = InScale; }
    constexpr void SetDecoupledLeftIconScale(u32 InScale) noexcept { this->DecoupledLeftIconBrush.Scale = InScale; }
    constexpr void SetOmniLeftIconScale(u32 InScale) noexcept
    {
        this->LeftIconStyle.NormalBrush.Scale   = InScale;
        this->LeftIconStyle.HoverBrush.Scale    = InScale;
        this->LeftIconStyle.PressBrush.Scale    = InScale;
        this->LeftIconStyle.SelectedBrush.Scale = InScale;
        this->LeftIconStyle.DisabledBrush.Scale = InScale;
        this->DecoupledLeftIconBrush.Scale = InScale;
    }
    constexpr void SetNormalRightIconScale(u32 InScale) noexcept { this->RightIconStyle.NormalBrush.Scale = InScale; }
    constexpr void SetHoverRightIconScale(u32 InScale) noexcept { this->RightIconStyle.HoverBrush.Scale = InScale; }
    constexpr void SetPressRightIconScale(u32 InScale) noexcept { this->RightIconStyle.PressBrush.Scale = InScale; }
    constexpr void SetSelectedRightIconScale(u32 InScale) noexcept { this->RightIconStyle.SelectedBrush.Scale = InScale; }
    constexpr void SetDisabledRightIconScale(u32 InScale) noexcept { this->RightIconStyle.DisabledBrush.Scale = InScale; }
    constexpr void SetDecoupledRightIconScale(u32 InScale) noexcept { this->DecoupledRightIconBrush.Scale = InScale; }
    constexpr void SetOmniRightIconScale(u32 InScale) noexcept
    {
        this->RightIconStyle.NormalBrush.Scale   = InScale;
        this->RightIconStyle.HoverBrush.Scale    = InScale;
        this->RightIconStyle.PressBrush.Scale    = InScale;
        this->RightIconStyle.SelectedBrush.Scale = InScale;
        this->RightIconStyle.DisabledBrush.Scale = InScale;
        this->DecoupledRightIconBrush.Scale = InScale;
    }

    constexpr void SetNormalLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconStyle.NormalBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetHoverLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconStyle.HoverBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetPressLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconStyle.PressBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetSelectedLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconStyle.SelectedBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetDisabledLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->LeftIconStyle.DisabledBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetDecoupledLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->DecoupledLeftIconBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetOmniLeftIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept
    {
        this->LeftIconStyle.NormalBrush.InwardsPadding   = InInwardsPadding;
        this->LeftIconStyle.HoverBrush.InwardsPadding    = InInwardsPadding;
        this->LeftIconStyle.PressBrush.InwardsPadding    = InInwardsPadding;
        this->LeftIconStyle.SelectedBrush.InwardsPadding = InInwardsPadding;
        this->LeftIconStyle.DisabledBrush.InwardsPadding = InInwardsPadding;
        this->DecoupledLeftIconBrush.InwardsPadding = InInwardsPadding;
    }
    constexpr void SetNormalRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconStyle.NormalBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetHoverRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconStyle.HoverBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetPressRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconStyle.PressBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetSelectedRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconStyle.SelectedBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetDisabledRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->RightIconStyle.DisabledBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetDecoupledRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept { this->DecoupledRightIconBrush.InwardsPadding = InInwardsPadding; }
    constexpr void SetOmniRightIconInwardsPadding(LWidgetSize1 InInwardsPadding) noexcept
    {
        this->RightIconStyle.NormalBrush.InwardsPadding   = InInwardsPadding;
        this->RightIconStyle.HoverBrush.InwardsPadding    = InInwardsPadding;
        this->RightIconStyle.PressBrush.InwardsPadding    = InInwardsPadding;
        this->RightIconStyle.SelectedBrush.InwardsPadding = InInwardsPadding;
        this->RightIconStyle.DisabledBrush.InwardsPadding = InInwardsPadding;
        this->DecoupledRightIconBrush.InwardsPadding = InInwardsPadding;
    }

    constexpr void SetNormalLeftIconTint(LColor const& InTint) noexcept { this->LeftIconStyle.NormalBrush.Tint = InTint; }
    constexpr void SetHoverLeftIconTint(LColor const& InTint) noexcept { this->LeftIconStyle.HoverBrush.Tint = InTint; }
    constexpr void SetPressLeftIconTint(LColor const& InTint) noexcept { this->LeftIconStyle.PressBrush.Tint = InTint; }
    constexpr void SetSelectedLeftIconTint(LColor const& InTint) noexcept { this->LeftIconStyle.SelectedBrush.Tint = InTint; }
    constexpr void SetDisabledLeftIconTint(LColor const& InTint) noexcept { this->LeftIconStyle.DisabledBrush.Tint = InTint; }
    constexpr void SetDecoupledLeftIconTint(LColor const& InTint) noexcept { this->DecoupledLeftIconBrush.Tint = InTint; }
    constexpr void SetOmniLeftIconTint(LColor const& InTint) noexcept
    {
        this->LeftIconStyle.NormalBrush.Tint   = InTint;
        this->LeftIconStyle.HoverBrush.Tint    = InTint;
        this->LeftIconStyle.PressBrush.Tint    = InTint;
        this->LeftIconStyle.SelectedBrush.Tint = InTint;
        this->LeftIconStyle.DisabledBrush.Tint = InTint;
        this->DecoupledLeftIconBrush.Tint = InTint;
    }
    constexpr void SetNormalRightIconTint(LColor const& InTint) noexcept { this->RightIconStyle.NormalBrush.Tint = InTint; }
    constexpr void SetHoverRightIconTint(LColor const& InTint) noexcept { this->RightIconStyle.HoverBrush.Tint = InTint; }
    constexpr void SetPressRightIconTint(LColor const& InTint) noexcept { this->RightIconStyle.PressBrush.Tint = InTint; }
    constexpr void SetSelectedRightIconTint(LColor const& InTint) noexcept { this->RightIconStyle.SelectedBrush.Tint = InTint; }
    constexpr void SetDisabledRightIconTint(LColor const& InTint) noexcept { this->RightIconStyle.DisabledBrush.Tint = InTint; }
    constexpr void SetDecoupledRightIconTint(LColor const& InTint) noexcept { this->DecoupledRightIconBrush.Tint = InTint; }
    constexpr void SetOmniRightIconTint(LColor const& InTint) noexcept
    {
        this->RightIconStyle.NormalBrush.Tint   = InTint;
        this->RightIconStyle.HoverBrush.Tint    = InTint;
        this->RightIconStyle.PressBrush.Tint    = InTint;
        this->RightIconStyle.SelectedBrush.Tint = InTint;
        this->RightIconStyle.DisabledBrush.Tint = InTint;
        this->DecoupledRightIconBrush.Tint = InTint;
    }

    constexpr void SetIconRightStyle(LTextButtonIconStyle const& InStyle) noexcept { this->LeftIconStyle = InStyle; }
    NODISCARD constexpr LTextButtonIconStyle& GetMutableIconRightStyle() noexcept { return this->LeftIconStyle; }
    NODISCARD constexpr LTextButtonIconStyle const& GetIconRightStyle() const noexcept { return this->LeftIconStyle; }
    constexpr void SetRightIconStyle(LTextButtonIconStyle const& InStyle) noexcept { this->RightIconStyle = InStyle; }
    NODISCARD constexpr LTextButtonIconStyle& GetMutableRightIconStyle() noexcept { return this->RightIconStyle; }
    NODISCARD constexpr LTextButtonIconStyle const& GetRightIconStyle() const noexcept { return this->RightIconStyle; }

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

    constexpr void SetIsDecoupledLeftIcon(bool bInDecoupled) noexcept { this->bDecoupledLeftIcon = bInDecoupled; }
    constexpr bool IsLeftIconDecoupled() const noexcept { return this->bDecoupledLeftIcon; }
    constexpr void SetIsDecoupledRightIcon(bool bInDecoupled) noexcept { this->bDecoupledRightIcon = bInDecoupled; }
    constexpr bool IsRightIconDecoupled() const noexcept { return this->bDecoupledRightIcon; }

    constexpr void SetDecoupledLeftIconBrush(LTextButtonIconBrush const& InBrush) noexcept { this->DecoupledLeftIconBrush = InBrush; }
    NODISCARD constexpr LTextButtonIconBrush const& GetDecoupledLeftIconBrush() const noexcept { return this->DecoupledLeftIconBrush; }
    constexpr void SetDecoupledRightIconBrush(LTextButtonIconBrush const& InBrush) noexcept { this->DecoupledRightIconBrush = InBrush; }
    NODISCARD constexpr LTextButtonIconBrush const& GetDecoupledRightIconBrush() const noexcept { return this->DecoupledRightIconBrush; }
    void SetDecoupledLeftKeyDownHandler(std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> Handler) noexcept { this->DecoupledLeftKeyDown = std::move(Handler); }
    void SetDecoupledLeftKeyUpHandler(std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> Handler) noexcept { this->DecoupledLeftKeyUp = std::move(Handler); }
    void SetDecoupledRightKeyDownHandler(std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> Handler) noexcept { this->DecoupledRightKeyDown = std::move(Handler); }
    void SetDecoupledRightKeyUpHandler(std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> Handler) noexcept { this->DecoupledRightKeyUp = std::move(Handler); }

private:

    LVec2F GetLeftIconTopLeft(LVec2F Translation) const noexcept;
    LVec2F GetRightIconTopLeft(LVec2F Translation) const noexcept;

    LTexture2Ref LeftIcon;
    LTexture2Ref RightIcon;
    LTextButtonIconBrush LeftIconBrush;
    LTextButtonIconBrush RightIconBrush;
    LTextButtonIconStyle LeftIconStyle;
    LTextButtonIconStyle RightIconStyle;
    LTextButtonTextStyle TextStyle;
    bool bDecoupledLeftIcon{};
    bool bDecoupledRightIcon{};
    std::size_t DecoupledLeftIconFrame{};
    std::size_t DecoupledRightIconFrame{};
    LDelegateHandle LeftHandle{ nullptr };
    LDelegateHandle RightHandle{ nullptr };
    LTextButtonIconBrush DecoupledLeftIconBrush;
    LTextButtonIconBrush DecoupledRightIconBrush;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledLeftKeyDown;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledLeftKeyUp;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledRightKeyDown;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledRightKeyUp;
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

    decltype(auto) LeftIcon(this auto&& Self, LTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().SetLeftIcon(std::move(InIcon));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIcon(this auto&& Self, LTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().SetRightIcon(std::move(InIcon));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetNormalLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetHoverLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetPressLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniLeftIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetOmniLeftIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) NormalRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetNormalRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetHoverRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetPressRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniRightIconScale(this auto&& Self, u32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetOmniRightIconScale(InScale);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetNormalLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetHoverLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetPressLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniLeftIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetOmniLeftIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) NormalRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetNormalRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetHoverRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetPressRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniRightIconInwardsPadding(this auto&& Self, LWidgetSize1 InInwardsPadding) noexcept
    {
        NODE_FACTORY_SELF().SetOmniRightIconInwardsPadding(InInwardsPadding);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) NormalLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalLeftIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverLeftIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressLeftIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedLeftIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledLeftIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniLeftIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOmniIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) NormalRightIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetNormalRightIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HoverRightIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetHoverRightIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) PressRightIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetPressRightIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SelectedRightIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetSelectedRightIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisabledRightIconTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetDisabledRightIconTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OmniRightIconTint(this auto&& Self, LColor const& InTint) noexcept
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
