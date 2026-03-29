// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "Nodes/ButtonBase.h"
#include "TextButton.generated.h"

namespace Jafg
{

struct LFactoryTextButton;

//# A button with text content. Use this for buttons that only display text to save performance.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextButton)
class ENGINE_API WTextButton : public WTextBox, public TButtonBase<WTextButton, LBoxBrush, decltype(&WTextBox::SetBrush)>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextButton(LNodeDynamicInit const& Init) noexcept : Super{Init}, TButtonBase{*this, &WTextBox::SetBrush} {}
    template<typename TCxxClass>
    explicit WTextButton(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}, TButtonBase{*this, &WTextBox::SetBrush} {}

public:

    virtual void Construct() override
    {
        Super::Construct();
        if (this->bEnabled)
        {
            this->SetBrush(this->Style.NormalBrush);
        }
    }

    JAFG_NODE_BUTTON_BOILERPLATE()

    constexpr void SetNormalPadding(LPadding const& InPadding) noexcept { this->Style.NormalBrush.Padding = InPadding; }
    constexpr void SetHoverPadding(LPadding const& InPadding) noexcept { this->Style.HoverBrush.Padding = InPadding; }
    constexpr void SetPressPadding(LPadding const& InPadding) noexcept { this->Style.PressBrush.Padding = InPadding; }
    constexpr void SetDisabledPadding(LPadding const& InPadding) noexcept { this->Style.DisabledBrush.Padding = InPadding; }
    constexpr void SetOmniPadding(LPadding const& InPadding) noexcept
    {
        this->Style.NormalBrush.Padding   = InPadding;
        this->Style.HoverBrush.Padding    = InPadding;
        this->Style.PressBrush.Padding    = InPadding;
        this->Style.DisabledBrush.Padding = InPadding;
    }
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
};

} /* ~Namespace Jafg */
