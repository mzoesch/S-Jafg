// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "ScrollRegion.generated.h"

namespace Jafg
{

struct LFactoryScrollRegion;

struct LScrollRegionBehavior
{
    //# Whether to always show the vertical scroll bar.
    bool bAlwaysShowVScrollbar:1{};
    //# Requires that #bAlwaysShowVScrollbar is false.
    bool bAlwaysHideVScrollbar:1{};

    //# Whether to always show the horizontal scroll bar.
    bool bAlwaysShowHScrollbar:1{};
    //# Requires that #bAlwaysShowHScrollbar is false.
    bool bAlwaysHideHScrollbar:1{};
};

struct LScrollRegionBarBrush
{
    enum { NoScrollBarBackground = 0 };

    LColor VBackgroundTint{ Colors::Black };
    LColor VTint{ Colors::White };
    //# The padding from the top right of the vertical scroll bar.
    LVec2F VScrollBarPadding{ maths::zero_vector<LVec2F> };
    //# The width of the vertical scroll bar.
    f32 VScrollBarWidth{ 5.0f };
    //# The width of the vertical scroll bar background. Zero means no background.
    f32 VScrollBarBackgroundWidth{ NoScrollBarBackground };

    LColor HBackgroundTint{ Colors::Black };
    LColor HTint{ Colors::White };
    //# The padding from the left bottom of the horizontal scroll bar.
    LVec2F HScrollBarPadding{ 5.0f, 0.0f };
    //# The height of the horizontal scroll bar.
    f32 HScrollBarHeight{ 5.0f };
    //# The height of the horizontal scroll bar background. Zero means no background.
    f32 HScrollBarBackgroundHeight{ NoScrollBarBackground };
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryScrollRegion)
class WScrollRegion final : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WScrollRegion)
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->Anchor = EAnchor::Fill;
    }

public:

    enum
    {
        //# The maximal position the scroll region can be up.
        MaxScrollUp     = 0,
        //# The maximal position the scroll region can be down.
        MaxScrollDown   = 1,
        //# The maximal position the scroll region can be left.
        MaxScrollLeft   = 0,
        //# The maximal position the scroll region can be right.
        MaxScrollRight  = 1,
    };

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    virtual LNodeReply SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location) override;
    virtual LNodeReply Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location) override;

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;
    virtual LNodeReply OnKeyEventUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    virtual void UpdateDesiredSize() const override;

    //# The size to use for the whole scroll region if it was not anchored.
    LNodeSize2 ScrollRegionSize;

    LScrollRegionBehavior Behavior;
    LScrollRegionBarBrush BarBrush;
    bool bCullNonVisible:1{ true };
    bool bUseChildrenDesiredSize:1{};

private:

    bool UserInterfaceTick();

    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBDownOnScrollbar(std::optional<LVec2F> const& CursorLocation);
    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBUpOnScrollbar();

    inline constexpr bool ShouldDrawVScrollbar(f32 InVisibleY) const noexcept
    {
        if (this->Behavior.bAlwaysShowVScrollbar) { return true; }
        if (this->Behavior.bAlwaysHideHScrollbar) { return false; }
        return maths::eq_e(InVisibleY, 1.0f) == false;
    }
    inline constexpr bool ShouldDrawHScrollbar(f32 InVisibleX) const noexcept
    {
        if (this->Behavior.bAlwaysShowHScrollbar) { return true; }
        if (this->Behavior.bAlwaysHideVScrollbar) { return false; }
        return maths::eq_e(InVisibleX, 1.0f) == false;
    }

    LVec2F GetVInteractiveAreaScrollSize() const;
    LVec2F GetVInteractiveAreaScrollPositionFromOuter() const;

    LVec2F GetHInteractiveAreaScrollSize() const;
    LVec2F GetHInteractiveAreaScrollPositionFromOuter() const;

    LVec2F GetVBackgroundScrollSize() const;
    LVec2F GetVBackgroundScrollPositionFromOuter() const;
    LVec2F GetVForegroundScrollSize() const;
    LVec2F GetVForegroundScrollSize(f32 InVisibleY) const;
    LVec2F GetVForegroundScrollPositionFromOuter() const;
    LVec2F GetVForegroundScrollPositionFromOuter(f32 InScrollOffsetYPercent) const;

    LVec2F GetHBackgroundScrollSize() const;
    LVec2F GetHBackgroundScrollPositionFromOuter() const;
    LVec2F GetHForegroundScrollSize() const;
    LVec2F GetHForegroundScrollSize(f32 InVisibleX) const;
    LVec2F GetHForegroundScrollPositionFromOuter() const;
    LVec2F GetHForegroundScrollPositionFromOuter(f32 InScrollOffsetXPercent) const;

    void HandleMouseWheelUp(f32 Value);
    void HandleMouseWheelDown(f32 Value);

    //# The scroll-position in percent. Where 0.0 is the top / left and 1.0 is the bottom / right.
    LVec2F ScrollPosition{ maths::zero_vector<LVec2F> };

    mutable LVec2F DesiredSizeOfChildren{ maths::zero_vector<LVec2F> };

    LDelegateHandle UserInterfaceTickDelegateHandle;
    bool bUiTickV:1{};
    bool bUiTickH:1{};
    LVec2F MbOffset{ maths::zero_vector<LVec2F> };
    mutable LVec2D LastVisible{ maths::zero_vector<LVec2D> };
};

struct LFactoryScrollRegion : NODE_FACTORY_PARENT(WScrollRegion)
{
    NODE_FACTORY_BODY(WScrollRegion)

    decltype(auto) ScrollRegionSize(this auto&& Self, LNodeSize2 const& Size) noexcept
    {
        NODE_FACTORY_SELF().ScrollRegionSize = Size;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Behavior(this auto&& Self, LScrollRegionBehavior Behavior) noexcept
    {
        NODE_FACTORY_SELF().Behavior = Behavior;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysShowVScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().Behavior.bAlwaysShowVScrollbar = bValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysHideVScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().Behavior.bAlwaysHideVScrollbar = bValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysShowHScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().Behavior.bAlwaysShowHScrollbar = bValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysHideHScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().Behavior.bAlwaysHideHScrollbar = bValue;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) BarBrush(this auto&& Self, LScrollRegionBarBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().BarBrush = Brush;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) VBackgroundTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.VBackgroundTint = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.VTint = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarPadding(this auto&& Self, LVec2F InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.VScrollBarPadding = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.VScrollBarWidth = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarBackgroundWith(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.VScrollBarBackgroundWidth = InValue;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) HBackgroundTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.HBackgroundTint = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.HTint = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarPadding(this auto&& Self, LVec2F InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.HScrollBarPadding = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.HScrollBarHeight = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarBackgroundHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().BarBrush.HScrollBarBackgroundHeight = InValue;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CullNonVisible(this auto&& Self, bool bCull) noexcept
    {
        NODE_FACTORY_SELF().bCullNonVisible = bCull;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) UseChildrenDesiredSize(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().bUseChildrenDesiredSize = bValue;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
