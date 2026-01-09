// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "ScrollRegion.generated.h"

namespace Jafg
{

struct LScrollRegionBrush : public LRegionBrush
{
    enum { NoScrollBarBackground = 0 };

    bool bAlwaysShowVScrollbar { false  };
    //# Requires that #bAlwaysShowVScrollbar is false.
    bool bAlwaysHideVScrollbar { false };

    bool bAlwaysShowHScrollbar { false  };
    //# Requires that #bAlwaysShowHScrollbar is false.
    bool bAlwaysHideHScrollbar { false };

    Lal::LColor VBackgroundTint { Lal::LColor::Black };
    Lal::LColor VTint { Lal::LColor::White };
    Lal::LColor HBackgroundTint { Lal::LColor::Black };
    Lal::LColor HTint { Lal::LColor::White };

    //#
    //# The padding from the top right of the vertical scroll bar.
    //#
    LVector2 VScrollBarPadding { 0.0f };

    //#
    //# The width of the vertical scroll bar.
    //#
    f32 VScrollBarWidth { 5.0f };

    //#
    //# The width of the vertical scroll bar background. Zero means no background.
    //#
    f32 VScrollBarBackgroundWidth { NoScrollBarBackground };

    //#
    //# The padding from the left bottom of the horizontal scroll bar.
    //#
    LVector2 HScrollBarPadding { 5.0f, 0.0f };

    //#
    //# The height of the horizontal scroll bar.
    //#
    f32 HScrollBarHeight { 5.0f };

    //#
    //# The height of the horizontal scroll bar background. Zero means no background.
    //#
    f32 HScrollBarBackgroundHeight { NoScrollBarBackground };
};

template <typename TNode>
class TWidgetFactoryScrollRegion : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& Brush(LScrollRegionBrush const& InBrush) noexcept
    {
        this->This()->SetBrush(InBrush);
        return this->Self();
    }

    FORCEINLINE TFactoryRetTy& CullNonVisible(const bool bCull) noexcept
    {
        this->This()->SetCullNonVisible(bCull);
        return this->Self();
    }

    FORCEINLINE TFactoryRetTy& ScrollRegionSize(LWidgetSize2 const& InSize) noexcept
    {
        this->This()->SetScrollRegionSize(InSize);
        return this->Self();
    }

    FORCEINLINE TFactoryRetTy& UseChildrenDesiredSize(const bool bValue) noexcept
    {
        this->This()->SetUseChildrenDesiredSize(bValue);
        return this->Self();
    }
};

//#
//# A #WScrollRegion is a parent node that may have many children.
//# The children that can be individually anchored with the #EAnchor enum.
//# Children are, as the name suggests, overlaid on top of each other and are independent of each of their siblings.
//# The scroll overlay usually should be anchored to its parent, and for unanchored sides the #SetScrollRegionSize in
//# should be used to define the size of the scroll overlay.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryScrollRegion)
class WScrollRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WScrollRegion(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WScrollRegion)

public:

    enum : i32
    {
        //#
        //# The maximal position the scroll region can be up.
        //#
        MaxScrollUp     = 0,

        //#
        //# The maximal position the scroll region can be down.
        //#
        MaxScrollDown   = 1,

        //#
        //# The maximal position the scroll region can be left.
        //#
        MaxScrollLeft   = 0,

        //#
        //# The maximal position the scroll region can be right.
        //#
        MaxScrollRight  = 1,
    };

    struct LScrollRegionBrushImpl final
    {
        friend WScrollRegion;

    private:

        FORCEINLINE LScrollRegionBrushImpl() noexcept = default;

        DEFAULT_REALLOC_OF_ANY_FORM(LScrollRegionBrushImpl)

        FORCEINLINE void Copy(const LScrollRegionBrush& InBrush) noexcept;
        FORCEINLINE void Move(LScrollRegionBrush&& InBrush) noexcept;
        FORCEINLINE LScrollRegionBrushImpl(const LScrollRegionBrush& InBrush) noexcept;
        FORCEINLINE LScrollRegionBrushImpl(LScrollRegionBrush&& InBrush) noexcept;
        FORCEINLINE LScrollRegionBrushImpl& operator=(const LScrollRegionBrush& InBrush) noexcept;
        FORCEINLINE LScrollRegionBrushImpl& operator=(LScrollRegionBrush&& InBrush) noexcept;

    public:

        FORCEINLINE ~LScrollRegionBrushImpl() noexcept = default;

        bool bAlwaysShowVScrollbar { false };
        bool bAlwaysHideVScrollbar { false };

        bool bAlwaysShowHScrollbar { false };
        bool bAlwaysHideHScrollbar { false };

        Lal::LColor VBackgroundTint { Lal::LColor::Black };
        Lal::LColor VTint { Lal::LColor::White };
        Lal::LColor HBackgroundTint { Lal::LColor::Black };
        Lal::LColor HTint { Lal::LColor::White };

        LVector2 VScrollBarPadding { 0.0f };
        f32 VScrollBarWidth { 5.0f };
        f32 VScrollBarBackgroundWidth { LScrollRegionBrush::NoScrollBarBackground };

        LVector2 HScrollBarPadding { 5.0f, 0.0f };
        f32 HScrollBarHeight { 5.0f };
        f32 HScrollBarBackgroundHeight { LScrollRegionBrush::NoScrollBarBackground };
    };

    virtual void Draw(LViewport& Context) const override;

    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2D& InLocation) override;

    virtual LReply SweepFocusTest(const LViewport& Context, const LVector2D& InLocation) override;

    //# This only the user interface.
    void UserInterfaceTick(const LViewport& InViewport);

    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE void SetScrollRegionSize(const LWidgetSize2& InSize) { this->ScrollRegionSize = InSize; }
    FORCEINLINE const LWidgetSize2& GetScrollRegionSize() const { return this->ScrollRegionSize; }

    void ApplyScroll(const LKeyEvent& InKeyEvent);

    FORCEINLINE void ApplyScroll(const LVector2D& InScroll) { this->ScrollPosition = InScroll; }
    FORCEINLINE void ApplyVScroll(const f64 InScroll) { this->ScrollPosition.Y = Maths::Clamp(InScroll, MaxScrollUp, MaxScrollDown); }
    FORCEINLINE void ApplyHScroll(const f64 InScroll) { this->ScrollPosition.X = Maths::Clamp(InScroll, MaxScrollLeft, MaxScrollRight); }

    using Super::SetBrush;
    FORCEINLINE void SetBrush(const LScrollRegionBrush& InBrush) noexcept;
    FORCEINLINE void SetScrollRegionBrushOnly(const LScrollRegionBrush& InBrush) noexcept;
    //# WARNING: This returns a new brush and not a reference. Use #GetScrollRegionBrush for just vising items.
    FORCEINLINE LScrollRegionBrush GetBrush() const noexcept;
    FORCEINLINE const LScrollRegionBrushImpl& GetScrollRegionBrush() const noexcept { return this->Brush; }

    FORCEINLINE constexpr bool GetAlwaysShowVScrollbar() const noexcept { return this->Brush.bAlwaysShowVScrollbar; }
    FORCEINLINE constexpr bool GetAlwaysHideVScrollbar() const noexcept { return this->Brush.bAlwaysHideVScrollbar; }
    FORCEINLINE constexpr void SetAlwaysShowVScrollbar(const bool InValue) noexcept { this->Brush.bAlwaysShowVScrollbar = InValue; }
    FORCEINLINE constexpr void SetAlwaysHideVScrollbar(const bool InValue) noexcept { this->Brush.bAlwaysHideVScrollbar = InValue; }

    FORCEINLINE constexpr bool GetAlwaysShowHScrollbar() const noexcept { return this->Brush.bAlwaysShowHScrollbar; }
    FORCEINLINE constexpr bool GetAlwaysHideHScrollbar() const noexcept { return this->Brush.bAlwaysHideHScrollbar; }
    FORCEINLINE constexpr void SetAlwaysShowHScrollbar(const bool InValue) noexcept { this->Brush.bAlwaysShowHScrollbar = InValue; }
    FORCEINLINE constexpr void SetAlwaysHideHScrollbar(const bool InValue) noexcept { this->Brush.bAlwaysHideHScrollbar = InValue; }

    FORCEINLINE constexpr Lal::LColor GetVBackgroundTint() const noexcept { return this->Brush.VBackgroundTint; }
    FORCEINLINE constexpr Lal::LColor GetVTint() const noexcept { return this->Brush.VTint; }
    FORCEINLINE constexpr Lal::LColor GetHBackgroundTint() const noexcept { return this->Brush.HBackgroundTint; }
    FORCEINLINE constexpr Lal::LColor GetHTint() const noexcept { return this->Brush.HTint; }
    FORCEINLINE constexpr void SetVBackgroundTint(const Lal::LColor& InValue) noexcept { this->Brush.VBackgroundTint = InValue; }
    FORCEINLINE constexpr void SetVTint(const Lal::LColor& InValue) noexcept { this->Brush.VTint = InValue; }
    FORCEINLINE constexpr void SetHBackgroundTint(const Lal::LColor& InValue) noexcept { this->Brush.HBackgroundTint = InValue; }
    FORCEINLINE constexpr void SetHTint(const Lal::LColor& InValue) noexcept { this->Brush.HTint = InValue; }

    FORCEINLINE constexpr LVector2 GetVScrollBarPadding() const noexcept { return this->Brush.VScrollBarPadding; }
    FORCEINLINE constexpr f32 GetVScrollBarWidth() const noexcept { return this->Brush.VScrollBarWidth; }
    FORCEINLINE constexpr f32 GetVScrollBarBackgroundWidth() const noexcept { return this->Brush.VScrollBarBackgroundWidth; }
    FORCEINLINE constexpr void SetVScrollBarPadding(const LVector2& InValue) noexcept { this->Brush.VScrollBarPadding = InValue; }
    FORCEINLINE constexpr void SetVScrollBarWidth(const f32 InValue) noexcept { this->Brush.VScrollBarWidth = InValue; }
    FORCEINLINE constexpr void SetVScrollBarBackgroundWidth(const f32 InValue) noexcept { this->Brush.VScrollBarBackgroundWidth = InValue; }

    FORCEINLINE constexpr LVector2 GetHScrollBarPadding() const noexcept { return this->Brush.HScrollBarPadding; }
    FORCEINLINE constexpr f32 GetHScrollBarHeight() const noexcept { return this->Brush.HScrollBarHeight; }
    FORCEINLINE constexpr f32 GetHScrollBarBackgroundHeight() const noexcept { return this->Brush.HScrollBarBackgroundHeight; }
    FORCEINLINE constexpr void SetHScrollBarPadding(const LVector2& InValue) noexcept { this->Brush.HScrollBarPadding = InValue; }
    FORCEINLINE constexpr void SetHScrollBarHeight(const f32 InValue) noexcept { this->Brush.HScrollBarHeight = InValue; }
    FORCEINLINE constexpr void SetHScrollBarBackgroundHeight(const f32 InValue) noexcept { this->Brush.HScrollBarBackgroundHeight = InValue; }

    FORCEINLINE constexpr void SetCullNonVisible(const bool InValue) noexcept { this->bCullNonVisible = InValue; }
    FORCEINLINE constexpr bool GetCullNonVisible() const noexcept { return this->bCullNonVisible; }

    FORCEINLINE constexpr void SetUseChildrenDesiredSize(const bool InValue) noexcept { this->bUseChildrenDesiredSize = InValue; }
    FORCEINLINE constexpr bool GetUseChildrenDesiredSize() const noexcept { return this->bUseChildrenDesiredSize; }

protected:

    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBDownOnScrollbar(const LViewport& InViewport);
    //# @return Whether the event is meaning full or not for this #WScrollRegion.
    bool MBUpOnScrollbar(const LViewport& InViewport);

    bool DrawVScrollbar(const f64 InVisibleY) const;
    bool DrawHScrollbar(const f64 InVisibleX) const;

    LVector2D GetVInteractiveAreaScrollSize() const;
    LVector2D GetVInteractiveAreaScrollPositionFromOuter() const;

    LVector2D GetHInteractiveAreaScrollSize() const;
    LVector2D GetHInteractiveAreaScrollPositionFromOuter() const;

    LVector2D GetVBackgroundScrollSize() const;
    LVector2D GetVBackgroundScrollPositionFromOuter() const;
    LVector2D GetVForegroundScrollSize() const;
    LVector2D GetVForegroundScrollSize(const f64 InVisibleY) const;
    LVector2D GetVForegroundScrollPositionFromOuter() const;
    LVector2D GetVForegroundScrollPositionFromOuter(const f64 InScrollOffsetYPercent) const;

    LVector2D GetHBackgroundScrollSize() const;
    LVector2D GetHBackgroundScrollPositionFromOuter() const;
    LVector2D GetHForegroundScrollSize() const;
    LVector2D GetHForegroundScrollSize(const f64 InVisibleX) const;
    LVector2D GetHForegroundScrollPositionFromOuter() const;
    LVector2D GetHForegroundScrollPositionFromOuter(const f64 InScrollOffsetXPercent) const;

private:

    LScrollRegionBrushImpl Brush;

    //#
    //# The size to use for the whole scroll region if it was not anchored.
    //#
    LWidgetSize2 ScrollRegionSize;

    //#
    //# The scroll-position in percent. Where 0.0 is the top / left and 1.0 is the bottom / right.
    //#
    LVector2D ScrollPosition;

    mutable LVector2D DesiredSizeOfChildren;

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };

    bool bCullNonVisible:1{ true };
    bool bUiTickV:1{ false };
    bool bUiTickH:1{ false };
    bool bUseChildrenDesiredSize:1{ false };
    f64 MbVOffset { 0.0 };
    f64 MbHOffset { 0.0 };
};

FORCEINLINE void WScrollRegion::SetBrush(const LScrollRegionBrush& InBrush) noexcept
{
    this->Super::SetBrush(InBrush);
    this->Brush.Copy(InBrush);

    return;
}

FORCEINLINE void WScrollRegion::SetScrollRegionBrushOnly(const LScrollRegionBrush& InBrush) noexcept
{
    this->Brush.Copy(InBrush);
    return;
}

FORCEINLINE LScrollRegionBrush WScrollRegion::GetBrush() const noexcept
{
    static_assert(sizeof(LRegionBrush) == 96, "LRegionBrush has changed. Please modify this function.");

    LScrollRegionBrush Result;

    Result.Type = this->Super::GetBrush().Type;
    Result.Tint = this->Super::GetBrush().Tint;
    Result.Image = this->Super::GetBrush().Image;
    Result.ImageTint = this->Super::GetBrush().ImageTint;
    Result.ImageScale = this->Super::GetBrush().ImageScale;
    Result.ImageBehavior = this->Super::GetBrush().ImageBehavior;
    Result.ImageOobm = this->Super::GetBrush().ImageOobm;
    Result.ImagePadding = this->Super::GetBrush().ImagePadding;
    Result.Radii = this->Super::GetBrush().Radii;
    Result.OutlineThickness = this->Super::GetBrush().OutlineThickness;
    Result.OutlineTint = this->Super::GetBrush().OutlineTint;

    Result.bAlwaysShowVScrollbar = this->Brush.bAlwaysShowVScrollbar;
    Result.bAlwaysHideVScrollbar = this->Brush.bAlwaysHideVScrollbar;

    Result.bAlwaysShowHScrollbar = this->Brush.bAlwaysShowHScrollbar;
    Result.bAlwaysHideHScrollbar = this->Brush.bAlwaysHideHScrollbar;

    Result.VBackgroundTint = this->Brush.VBackgroundTint;
    Result.VTint = this->Brush.VTint;
    Result.HBackgroundTint = this->Brush.HBackgroundTint;
    Result.HTint = this->Brush.HTint;

    Result.VScrollBarPadding = this->Brush.VScrollBarPadding;
    Result.VScrollBarWidth = this->Brush.VScrollBarWidth;
    Result.VScrollBarBackgroundWidth = this->Brush.VScrollBarBackgroundWidth;

    Result.HScrollBarPadding = this->Brush.HScrollBarPadding;
    Result.HScrollBarHeight = this->Brush.HScrollBarHeight;
    Result.HScrollBarBackgroundHeight = this->Brush.HScrollBarBackgroundHeight;

    return Result;
}

FORCEINLINE void WScrollRegion::LScrollRegionBrushImpl::Copy(const LScrollRegionBrush& InBrush) noexcept
{
    this->bAlwaysShowVScrollbar = InBrush.bAlwaysShowVScrollbar;
    this->bAlwaysHideVScrollbar = InBrush.bAlwaysHideVScrollbar;

    this->bAlwaysShowHScrollbar = InBrush.bAlwaysShowHScrollbar;
    this->bAlwaysHideHScrollbar = InBrush.bAlwaysHideHScrollbar;

    this->VBackgroundTint = InBrush.VBackgroundTint;
    this->VTint = InBrush.VTint;
    this->HBackgroundTint = InBrush.HBackgroundTint;
    this->HTint = InBrush.HTint;

    this->VScrollBarPadding = InBrush.VScrollBarPadding;
    this->VScrollBarWidth = InBrush.VScrollBarWidth;
    this->VScrollBarBackgroundWidth = InBrush.VScrollBarBackgroundWidth;

    this->HScrollBarPadding = InBrush.HScrollBarPadding;
    this->HScrollBarHeight = InBrush.HScrollBarHeight;
    this->HScrollBarBackgroundHeight = InBrush.HScrollBarBackgroundHeight;

    return;
}

FORCEINLINE void WScrollRegion::LScrollRegionBrushImpl::Move(LScrollRegionBrush&& InBrush) noexcept
{
    this->bAlwaysShowVScrollbar = std::move(InBrush.bAlwaysShowVScrollbar);
    this->bAlwaysHideVScrollbar = std::move(InBrush.bAlwaysHideVScrollbar);

    this->bAlwaysShowHScrollbar = std::move(InBrush.bAlwaysShowHScrollbar);
    this->bAlwaysHideHScrollbar = std::move(InBrush.bAlwaysHideHScrollbar);

    this->VBackgroundTint = std::move(InBrush.VBackgroundTint);
    this->VTint = std::move(InBrush.VTint);
    this->HBackgroundTint = std::move(InBrush.HBackgroundTint);
    this->HTint = std::move(InBrush.HTint);

    this->VScrollBarPadding = std::move(InBrush.VScrollBarPadding);
    this->VScrollBarWidth = std::move(InBrush.VScrollBarWidth);
    this->VScrollBarBackgroundWidth = std::move(InBrush.VScrollBarBackgroundWidth);

    this->HScrollBarPadding = std::move(InBrush.HScrollBarPadding);
    this->HScrollBarHeight = std::move(InBrush.HScrollBarHeight);
    this->HScrollBarBackgroundHeight = std::move(InBrush.HScrollBarBackgroundHeight);

    return;
}

FORCEINLINE WScrollRegion::LScrollRegionBrushImpl::LScrollRegionBrushImpl(const LScrollRegionBrush& InBrush) noexcept
{
    this->Copy(InBrush);
}

FORCEINLINE WScrollRegion::LScrollRegionBrushImpl::LScrollRegionBrushImpl(LScrollRegionBrush&& InBrush) noexcept
{
    this->Move(std::move(InBrush));
}

FORCEINLINE WScrollRegion::LScrollRegionBrushImpl& WScrollRegion::LScrollRegionBrushImpl::operator=(const LScrollRegionBrush& InBrush) noexcept
{
    this->Copy(InBrush);
    return *this;
}

FORCEINLINE WScrollRegion::LScrollRegionBrushImpl& WScrollRegion::LScrollRegionBrushImpl::operator=(LScrollRegionBrush&& InBrush) noexcept
{
    this->Move(std::move(InBrush));
    return *this;
}

} /* ~Namespace Jafg */
