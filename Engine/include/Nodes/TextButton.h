// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "Nodes/ButtonBase.h"
#include "Nodes/Icon.h"
#include "Nodes/Iconized.h"
#include "TextButton.generated.h"

namespace Jafg
{

struct LFactoryTextButton;
struct LFactoryTextButtonIconizedLeft;
struct LFactoryTextButtonIconizedRight;
struct LFactoryTextButtonIconizedDouble;

namespace Detail
{
struct LTextButtonIconStyleBase
{
    LIconBrush NormalBrush;
    LIconBrush HoverBrush;
    LIconBrush PressBrush;
    LIconBrush SelectedBrush;
    LIconBrush DisabledBrush;
    LIconBrush DecoupledBrush;
};
} /* ~Namespace Detail */
enum struct ETextButtonIconStyleBits : u32
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
ENUM_STRUCT_FLAGS(ETextButtonIconStyleBits, ETextButtonIconStyleFlags)
struct LTextButtonIconStyle : Detail::LTextButtonIconStyleBase
    , LStyleBase<
        ETextButtonIconStyleFlags,
        &Detail::LTextButtonIconStyleBase::NormalBrush,
        &Detail::LTextButtonIconStyleBase::HoverBrush,
        &Detail::LTextButtonIconStyleBase::PressBrush,
        &Detail::LTextButtonIconStyleBase::SelectedBrush,
        &Detail::LTextButtonIconStyleBase::DisabledBrush,
        &Detail::LTextButtonIconStyleBase::DecoupledBrush
        >
{
};

//# A button with text content.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButton)
class ENGINE_API WTextButton : public WTextBox, public TButtonBase<WTextButton, LBoxBrush, &WTextBox::Brush>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(TButtonBase::DefaultVisibility);
    }
    template<typename TCxxClass>
    explicit WTextButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this}
    {
        this->SetVisibility(TButtonBase::DefaultVisibility);
    }

public:

    JAFG_NODE_BUTTON_BOILERPLATE()
    virtual void OnBrushChanged(EStyleBits Bit) noexcept override;

    TButtonStyle<LTextBoxBrush> TextStyle;
};

//# A text button with an optional icon on the left or right which may be decupled form the button logic to serve as an extra input target.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButtonIconizedDouble)
class ENGINE_API WTextButtonIconizedDouble : public WTextButton, public LIconizedDouble
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextButtonIconizedDouble)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;
    virtual LNodeReply OnCursorMoved(const LVec2F& InLocation) override;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;
    virtual void OnBrushChanged(EStyleBits Bit) noexcept override;

    LTextButtonIconStyle LeftIconStyle;
    LTextButtonIconStyle RightIconStyle;

    //# Whether the respective icon has their own hitbox and can receive input independently of the button.
    EVENT_DECL(DecoupledLeftKeyEvent, LNodeReply(WTextButtonIconizedDouble& Self, LNodeKeyEventInfo const& Data, LKeyEvent const& Event))
    EVENT_DECL(DecoupledRightKeyEvent, LNodeReply(WTextButtonIconizedDouble& Self, LNodeKeyEventInfo const& Data, LKeyEvent const& Event))

private:

    // TODO: Remove.
    LVec2F GetLeftIconTopLeft(LVec2F Translation) const noexcept;
    LVec2F GetRightIconTopLeft(LVec2F Translation) const noexcept;

    std::size_t DecoupledLeftIconFrame{};
    std::size_t DecoupledRightIconFrame{};
    LDelegateHandle LeftHandle;
    LDelegateHandle RightHandle;
};

struct LFactoryTextButton : TFactoryButtonBase<WTextButton>
{
    NODE_FACTORY_BODY(WTextButton)

    JAFG_NODE_FACTORY_STYLE_BOILERPLATE(Text, TextStyle)
};

struct LFactoryTextButtonIconizedDouble : NODE_FACTORY_PARENT(WTextButtonIconizedDouble)
{
    NODE_FACTORY_BODY(WTextButtonIconizedDouble)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(DecoupledLeftKey, DecoupledLeftKeyEvent)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(DecoupledRightKey, DecoupledRightKeyEvent)

    decltype(auto) LeftIcon(this auto&& Self, LOptionalTexture2Ref Icon) noexcept
    {
        NODE_FACTORY_SELF().LeftIcon = Icon.GetResolved();
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) LeftIconBrush(this auto&& Self, LIconBrush const& Brush) noexcept
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
    decltype(auto) LeftIconAlignment(this auto&& Self, LIconBrush::Align Alignment) noexcept
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

    decltype(auto) RightIcon(this auto&& Self, LOptionalTexture2Ref Icon) noexcept
    {
        NODE_FACTORY_SELF().RightIcon = Icon.GetResolved();
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) RightIconBrush(this auto&& Self, LIconBrush const& Brush) noexcept
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
    decltype(auto) RightIconAlignment(this auto&& Self, LIconBrush::Align Alignment) noexcept
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
