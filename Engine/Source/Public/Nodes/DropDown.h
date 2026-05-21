// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Icon.h"
#include "Rhi/Texture2.h"
#include "Nodes/GenericTabInfos.h"

namespace Jafg
{

class WHButton;
class WDismissibleFloatingWidget;
struct LFactoryDropDown;

struct LDropDownNodeOption;
struct LDropDownNodeInformation;
struct LDropDownNodeCustom;
struct LDropDownNodeScratch;
struct LDropDownNodeSeparator;
struct LDropDownNodeSubmenu;
// struct LDropDownNodeDeferredSubMenu;

typedef std::variant<
      LDropDownNodeOption
    , LDropDownNodeInformation
    , LDropDownNodeCustom
    , LDropDownNodeScratch
    , LDropDownNodeSeparator
    , LDropDownNodeSubmenu
    > LDropDownNode;

//# A simple one-time action.
struct LDropDownNodeOption final
{
    LTabSelectorCreateInfo Selector;
    bool IsEnabled{ true };
    // TODO: Add shortcut hint
    //#
    //# Only invoked if the owning dropdown seemed to see it necessary to invoke it.
    //# If this fires, then you should handle that event.
    //# Letting it unhandled will cause the owning drop down to kill itself if allowed.
    //#
    TCopyableFunction<algo::reply(WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)> OnAction;
};

//# Non-interactable information.
struct LDropDownNodeInformation final
{
    LString What;
    //#
    //# The minimal padding that a node should have (even for non information nodes).
    //# This is absolutely not enforced and just a recommendation.
    //#
    struct MinPadding{};
    //# The padding additionally to #MinPadding so that text nicely aligns with always padded or iconized text.
    struct IconPadding{};
    std::variant<IconPadding, MinPadding, LPadding, std::monostate> Padding;
};

//#
//# Just as #LDropDownNodeOption but allows one to create a set of nodes instead.
//# The resulting nodes of #OnCreate will be wrapped inside a h-filled h-region.
//#
struct LDropDownNodeCustom final
{
    typedef WHButton Parent;

    struct reply : algo::reply_base
    {
        FORCEINLINE constexpr reply() noexcept : reply_base{}, bKill{} {}
        FORCEINLINE constexpr reply(bool bInHandled, bool bInKill) noexcept : reply_base{bInHandled}, bKill{bInKill} {}
        DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(reply)
        FORCEINLINE constexpr ~reply() noexcept {}

        FORCEINLINE static constexpr reply handled(bool bKill) noexcept { return reply{true, bKill}; }
        FORCEINLINE static constexpr reply unhandled() noexcept { return {}; }

        FORCEINLINE constexpr bool should_kill() const noexcept { return this->bKill; }
    private:
        bool bKill;
    };

    TCopyableFunction<LFactoryNode(LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)> OnCreate;
    bool IsEnabled{ true };
    // TODO: Add shortcut hint
    //# Always invoked if a key is released. Check yourself if you want to handle this.
    TCopyableFunction<reply(WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)> OnAction;
};

//#
//# Give absolutely no helpers. Unlike #LDropDownNodeCustom nodes submitted there will be added to the drop-down
//# without further transformations or wrapping. This is true customization.
//#
struct LDropDownNodeScratch final
{
    TCopyableFunction<LFactoryNode(LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)> OnCreate;
};

struct LDropDownNodeSeparator final
{
    LString DisplayName;
    LOptionalTexture2Ref Icon;
    u32 Thickness{ 1 };
};

struct LDropDownNodeSubmenu final
{
    LTabSelectorCreateInfo Selector;
    bool IsEnabled{ true };
    TArray<LDropDownNode> Children;
};

// struct LDropDownNodeDeferredSubMenu final
// {
// };


struct LDropDownMenuCreateInfo final
{
    inline static constexpr auto RecommendedPadding{8_spt};
    inline static constexpr auto RecommendedPaddedTextPadding{LIconBrush::DefaultMinIconSize + LIconBrush::DefaultInwardsPadding};
    inline static constexpr auto RecommendedTextPadding{RecommendedPadding + RecommendedPaddedTextPadding};

    //#
    //# If not handled or unbound.
    //# The window will destroy itself.
    //#
    TCopyableFunction<algo::reply(WDismissibleFloatingWidget& Self)> OnOptionCloseResult;
};
ENGINE_API WDismissibleFloatingWidget& CreateDropDownMenu(LViewport& Viewport, LVec2F Position, LDropDownMenuCreateInfo CreateInfo, LDropDownNodeSubmenu const& Submenu);

} /* ~Namespace Jafg */
