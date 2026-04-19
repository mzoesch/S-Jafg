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
    u32 Scale{ 1 };
    LWidgetSize1 InwardsPadding{ 6_spt };
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
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorMoved(const LVec2F& InLocation) override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
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
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledLeftKeyDown;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledLeftKeyUp;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledRightKeyDown;
    std::move_only_function<LReply(LNodeKeyEventData const& Data, LKeyEvent const& Event)> DecoupledRightKeyUp;

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
    decltype(auto) LeftIconInwardsPadding(this auto&& Self, LWidgetSize1 Padding) noexcept
    {
        NODE_FACTORY_SELF().LeftIconBrush.InwardsPadding = Padding;
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
    decltype(auto) RightIconInwardsPadding(this auto&& Self, LWidgetSize1 Padding) noexcept
    {
        NODE_FACTORY_SELF().RightIconBrush.InwardsPadding = Padding;
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
