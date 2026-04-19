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
    bool bAlwaysShowVScrollbar{};
    //# Requires that #bAlwaysShowVScrollbar is false.
    bool bAlwaysHideVScrollbar{};

    //# Whether to always show the horizontal scroll bar.
    bool bAlwaysShowHScrollbar{};
    //# Requires that #bAlwaysShowHScrollbar is false.
    bool bAlwaysHideHScrollbar{};
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
        this->SetAnchor(EAnchor::Fill);
        this->SetVisibility(ENodeVisibility::Visible);
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

    virtual LCursorReply SweepMouse(LNodeSweepData const& Data, LVec2F const& Location) override;
    virtual LReply SweepFocusTest(LNodeSweepData const& Data, LVec2F const& Location) override;

    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent) override;

    virtual void UpdateDesiredSize() const override;

    constexpr void SetScrollRegionSize(LWidgetSize2 const& InSize) noexcept { this->ScrollRegionSize = InSize; }
    constexpr LWidgetSize2 const& GetScrollRegionSize() const noexcept { return this->ScrollRegionSize; }

    constexpr void SetBehavior(LScrollRegionBehavior const& InBehavior) noexcept { this->Behavior = InBehavior; }
    constexpr LScrollRegionBehavior const& GetBehavior() const noexcept { return this->Behavior; }
    constexpr LScrollRegionBehavior& GetMutableBehavior() noexcept { return this->Behavior; }
    constexpr bool GetAlwaysShowVScrollbar() const noexcept { return this->Behavior.bAlwaysShowVScrollbar; }
    constexpr bool GetAlwaysHideVScrollbar() const noexcept { return this->Behavior.bAlwaysHideVScrollbar; }
    constexpr void SetAlwaysShowVScrollbar(bool InValue) noexcept { this->Behavior.bAlwaysShowVScrollbar = InValue; }
    constexpr void SetAlwaysHideVScrollbar(bool InValue) noexcept { this->Behavior.bAlwaysHideVScrollbar = InValue; }
    constexpr bool GetAlwaysShowHScrollbar() const noexcept { return this->Behavior.bAlwaysShowHScrollbar; }
    constexpr bool GetAlwaysHideHScrollbar() const noexcept { return this->Behavior.bAlwaysHideHScrollbar; }
    constexpr void SetAlwaysShowHScrollbar(bool InValue) noexcept { this->Behavior.bAlwaysShowHScrollbar = InValue; }
    constexpr void SetAlwaysHideHScrollbar(bool InValue) noexcept { this->Behavior.bAlwaysHideHScrollbar = InValue; }

    constexpr void SetBarBrush(LScrollRegionBarBrush const& InBrush) noexcept { this->BarBrush = InBrush; }
    constexpr LScrollRegionBarBrush const& GetBarBrush() const noexcept { return this->BarBrush; }
    constexpr LScrollRegionBarBrush& GetMutableBarBrush() noexcept { return this->BarBrush; }

    constexpr void SetVBackgroundTint(LColor const& InValue) noexcept { this->BarBrush.VBackgroundTint = InValue; }
    constexpr void SetVTint(LColor const& InValue) noexcept { this->BarBrush.VTint = InValue; }
    constexpr void SetVScrollBarPadding(LVec2F InValue) noexcept { this->BarBrush.VScrollBarPadding = InValue; }
    constexpr void SetVScrollBarWidth(f32 InValue) noexcept { this->BarBrush.VScrollBarWidth = InValue; }
    constexpr void SetVScrollBarBackgroundWidth(f32 InValue) noexcept { this->BarBrush.VScrollBarBackgroundWidth = InValue; }
    constexpr LColor GetVBackgroundTint() const noexcept { return this->BarBrush.VBackgroundTint; }
    constexpr LColor GetVTint() const noexcept { return this->BarBrush.VTint; }
    constexpr LVec2F GetVScrollBarPadding() const noexcept { return this->BarBrush.VScrollBarPadding; }
    constexpr f32 GetVScrollBarWidth() const noexcept { return this->BarBrush.VScrollBarWidth; }
    constexpr f32 GetVScrollBarBackgroundWidth() const noexcept { return this->BarBrush.VScrollBarBackgroundWidth; }

    constexpr void SetHBackgroundTint(const LColor& InValue) noexcept { this->BarBrush.HBackgroundTint = InValue; }
    constexpr void SetHTint(LColor const& InValue) noexcept { this->BarBrush.HTint = InValue; }
    constexpr void SetHScrollBarPadding(LVec2F InValue) noexcept { this->BarBrush.HScrollBarPadding = InValue; }
    constexpr void SetHScrollBarHeight(f32 InValue) noexcept { this->BarBrush.HScrollBarHeight = InValue; }
    constexpr void SetHScrollBarBackgroundHeight(f32 InValue) noexcept { this->BarBrush.HScrollBarBackgroundHeight = InValue; }
    constexpr LColor GetHBackgroundTint() const noexcept { return this->BarBrush.HBackgroundTint; }
    constexpr LColor GetHTint() const noexcept { return this->BarBrush.HTint; }
    constexpr LVec2F GetHScrollBarPadding() const noexcept { return this->BarBrush.HScrollBarPadding; }
    constexpr f32 GetHScrollBarHeight() const noexcept { return this->BarBrush.HScrollBarHeight; }
    constexpr f32 GetHScrollBarBackgroundHeight() const noexcept { return this->BarBrush.HScrollBarBackgroundHeight; }

    constexpr void SetCullNonVisible(bool InValue) noexcept { this->bCullNonVisible = InValue; }
    constexpr bool GetCullNonVisible() const noexcept { return this->bCullNonVisible; }
    constexpr void SetUseChildrenDesiredSize(bool InValue) noexcept { this->bUseChildrenDesiredSize = InValue; }
    constexpr bool GetUseChildrenDesiredSize() const noexcept { return this->bUseChildrenDesiredSize; }

private:

    bool UserInterfaceTick();

    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBDownOnScrollbar(LViewport const& InViewport);
    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBUpOnScrollbar(LViewport const& InViewport);

    inline constexpr bool ShouldDrawVScrollbar(f32 InVisibleY) const noexcept
    {
        if (this->Behavior.bAlwaysShowVScrollbar) { return true; }
        if (this->Behavior.bAlwaysHideHScrollbar) { return false; }
        return maths::eq_e(InVisibleY, 1.0) == false;
    }
    inline constexpr bool ShouldDrawHScrollbar(f32 InVisibleX) const noexcept
    {
        if (this->Behavior.bAlwaysShowHScrollbar) { return true; }
        if (this->Behavior.bAlwaysHideVScrollbar) { return false; }
        return maths::eq_e(InVisibleX, 1.0) == false;
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

    LScrollRegionBehavior Behavior;
    LScrollRegionBarBrush BarBrush;

    //# The size to use for the whole scroll region if it was not anchored.
    LWidgetSize2 ScrollRegionSize;

    //# The scroll-position in percent. Where 0.0 is the top / left and 1.0 is the bottom / right.
    LVec2F ScrollPosition{ maths::zero_vector<LVec2F> };

    bool bCullNonVisible:1{ true };
    bool bUseChildrenDesiredSize:1{};
    mutable LVec2F DesiredSizeOfChildren{ maths::zero_vector<LVec2F> };

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };
    bool bUiTickV:1{};
    bool bUiTickH:1{};
    LVec2F MbOffset{ maths::zero_vector<LVec2F> };
    mutable LVec2D LastVisible{ maths::zero_vector<LVec2D> };
};

struct LFactoryScrollRegion : NODE_FACTORY_PARENT(WScrollRegion)
{
    NODE_FACTORY_BODY(WScrollRegion)

    decltype(auto) ScrollRegionSize(this auto&& Self, LWidgetSize2 const& InSize) noexcept
    {
        NODE_FACTORY_SELF().SetScrollRegionSize(InSize);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Behavior(this auto&& Self, LScrollRegionBehavior const& InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysShowVScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetAlwaysShowVScrollbar(bValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysHideVScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetAlwaysHideVScrollbar(bValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysShowHScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetAlwaysShowHScrollbar(bValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlwaysHideHScrollbar(this auto&& Self, bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetAlwaysHideHScrollbar(bValue);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) BarBrush(this auto&& Self, LScrollRegionBarBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetBarBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) VBackgroundTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().SetVBackgroundTint(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().SetVTint(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarPadding(this auto&& Self, LVec2F InValue) noexcept
    {
        NODE_FACTORY_SELF().SetVScrollBarPadding(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().SetVScrollBarWidth(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) VScrollBarBackgroundWith(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().SetVScrollBarBackgroundWidth(InValue);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) HBackgroundTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().SetHBackgroundTint(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HTint(this auto&& Self, LColor const& InValue) noexcept
    {
        NODE_FACTORY_SELF().SetHTint(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarPadding(this auto&& Self, LVec2F InValue) noexcept
    {
        NODE_FACTORY_SELF().SetHScrollBarPadding(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().SetHScrollBarHeight(InValue);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) HScrollBarBackgroundHeight(this auto&& Self, f32 InValue) noexcept
    {
        NODE_FACTORY_SELF().SetHScrollBarBackgroundHeight(InValue);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CullNonVisible(this auto&& Self, bool bCull) noexcept
    {
        NODE_FACTORY_SELF().SetCullNonVisible(bCull);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) UseChildrenDesiredSize(this auto&& Self, const bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetUseChildrenDesiredSize(bValue);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
