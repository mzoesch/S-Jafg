// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "Nodes/ButtonBase.h"
#include "TextButton.generated.h"

namespace Jafg
{

struct LFactoryTextButton;

struct LTextButtonIconBrush
{
    enum struct Align{ Left, Center, Right, };

    u32 Scale{ 1 };
    LNodeSize1 InwardsPadding{ 4_spt };
    LNodeSize1 MinIconSize{ 20_spt };
    Align Alignment{ Align::Center };
    LColor Tint{ Colors::White };
};

namespace Detail
{

struct LTextButtonBaseTextStyle
{
    LTextBrush NormalBrush{ ETextScale::Body };
    LTextBrush HoverBrush{ ETextScale::Body };
    LTextBrush PressBrush{ ETextScale::Body };
    LTextBrush SelectedBrush{ ETextScale::Body };
    LTextBrush DisabledBrush{ ETextScale::Body };
};

struct LTextButtonBaseIconStyle
{
    LTextButtonIconBrush NormalBrush;
    LTextButtonIconBrush HoverBrush;
    LTextButtonIconBrush PressBrush;
    LTextButtonIconBrush SelectedBrush;
    LTextButtonIconBrush DisabledBrush;
    LTextButtonIconBrush DecoupledBrush;
};

} /* ~Namespace Detail */

enum struct EIconStyleBits
{
    Identity = 0x0 << 0,
    Normal = 0x1 << 0,
    Hover = 0x1 << 1,
    Press = 0x1 << 2,
    Selected = 0x1 << 3,
    Disabled = 0x1 << 4,
    Decoupled = 0x1 << 5,

    count = 6,
};
ENUM_STRUCT_FLAGS(EIconStyleBits, EIconStyleFlags)

struct LTextButtonTextStyle : Detail::LTextButtonBaseTextStyle
    , LStyleBase<
        EStyleFlags,
        &Detail::LTextButtonBaseTextStyle::NormalBrush,
        &Detail::LTextButtonBaseTextStyle::HoverBrush,
        &Detail::LTextButtonBaseTextStyle::PressBrush,
        &Detail::LTextButtonBaseTextStyle::SelectedBrush,
        &Detail::LTextButtonBaseTextStyle::DisabledBrush
        >
{
};

struct LTextButtonIconStyle : Detail::LTextButtonBaseIconStyle
    , LStyleBase<
        EIconStyleFlags,
        &Detail::LTextButtonBaseIconStyle::NormalBrush,
        &Detail::LTextButtonBaseIconStyle::HoverBrush,
        &Detail::LTextButtonBaseIconStyle::PressBrush,
        &Detail::LTextButtonBaseIconStyle::SelectedBrush,
        &Detail::LTextButtonBaseIconStyle::DisabledBrush,
        &Detail::LTextButtonBaseIconStyle::DecoupledBrush
        >
{
};

//# A button with text content and an optional icon on the left or right. Use this for buttons that only display text/icons to save performance.
//# TODO: Instead of a button make a WIconTextBox and then => WIconTextButton
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButton)
class ENGINE_API WTextButton : public WTextBox, public TButtonBase<WTextButton, LBoxBrush, &WTextBox::Brush>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }
    template<typename TCxxClass>
    explicit WTextButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Construct() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;
    virtual LNodeReply OnCursorEnter() override;
    virtual LNodeReply OnCursorMoved(const LVec2F& InLocation) override;
    virtual void OnCursorLeave() override;
    virtual LNodeReply OnKeyDownFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;
    virtual LNodeReply OnKeyUpFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;
    virtual void OnEnabledStateChanged() override;
    virtual void OnSelectedStateChanged() override;

    LTextButtonTextStyle TextStyle;

    LTexture2Ref LeftIcon;
    LTextButtonIconBrush LeftIconBrush;
    LTextButtonIconStyle LeftIconStyle;

    LTexture2Ref RightIcon;
    LTextButtonIconBrush RightIconBrush;
    LTextButtonIconStyle RightIconStyle;

    bool bDecoupledLeftIcon{};
    bool bDecoupledRightIcon{};
    TFunction2<LNodeReply(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)> DecoupledLeftKeyDown;
    TFunction2<LNodeReply(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)> DecoupledLeftKeyUp;
    TFunction2<LNodeReply(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)> DecoupledRightKeyDown;
    TFunction2<LNodeReply(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)> DecoupledRightKeyUp;

private:

    LVec2F GetLeftIconTopLeft(LVec2F Translation) const noexcept;
    LVec2F GetRightIconTopLeft(LVec2F Translation) const noexcept;

    std::size_t DecoupledLeftIconFrame{};
    std::size_t DecoupledRightIconFrame{};
    LDelegateHandle LeftHandle{ nullptr };
    LDelegateHandle RightHandle{ nullptr };
};

struct LFactoryTextButton : public TFactoryButtonBase<WTextButton>
{
    NODE_FACTORY_BODY(WTextButton)

    decltype(auto) LeftIcon(this auto&& Self, LTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().LeftIcon = std::move(InIcon);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconBrush(this auto&& Self, LTextButtonIconBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush = Brush;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconScale(this auto&& Self, u32 Scale) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.Scale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconInwardsPadding(this auto&& Self, LNodeSize1 Padding) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.InwardsPadding = Padding;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconMinSize(this auto&& Self, LNodeSize1 MinSize) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.MinIconSize = MinSize;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconAlignment(this auto&& Self, LTextButtonIconBrush::Align Alignment) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.Alignment = Alignment;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    JAFG_NODE_FACTORY_STYLE_BOILERPLATE(LeftIcon, LeftIconStyle)

    decltype(auto) RightIcon(this auto&& Self, LTexture2Ref InIcon) noexcept
    {
        NODE_FACTORY_SELF().RightIcon = std::move(InIcon);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconBrush(this auto&& Self, LTextButtonIconBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush = Brush;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconScale(this auto&& Self, u32 Scale) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.Scale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconInwardsPadding(this auto&& Self, LNodeSize1 Padding) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.InwardsPadding = Padding;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconMinSize(this auto&& Self, LNodeSize1 MinSize) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.MinIconSize = MinSize;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconAlignment(this auto&& Self, LTextButtonIconBrush::Align Alignment) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.Alignment = Alignment;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    JAFG_NODE_FACTORY_STYLE_BOILERPLATE(RightIcon, RightIconStyle)
};

} /* ~Namespace Jafg */
