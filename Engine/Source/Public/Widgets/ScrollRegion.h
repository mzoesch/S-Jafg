// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "ScrollRegion.generated.h"

namespace Jafg
{

struct LScrollRegionBrush : public LRegionBrush
{
    enum { NoScrollBarBackground = 0 };

    bool bAlwaysShowVScrollbar{};
    //# Requires that #bAlwaysShowVScrollbar is false.
    bool bAlwaysHideVScrollbar{};

    bool bAlwaysShowHScrollbar{};
    //# Requires that #bAlwaysShowHScrollbar is false.
    bool bAlwaysHideHScrollbar{};

    LColor VBackgroundTint{ Colors::Black };
    LColor VTint{ Colors::White };
    LColor HBackgroundTint{ Colors::Black };
    LColor HTint{ Colors::White };

    //#
    //# The padding from the top right of the vertical scroll bar.
    //#
    LVec2F VScrollBarPadding{ maths::zero_vector<LVec2F> };

    //#
    //# The width of the vertical scroll bar.
    //#
    f32 VScrollBarWidth{ 5.0f };

    //#
    //# The width of the vertical scroll bar background. Zero means no background.
    //#
    f32 VScrollBarBackgroundWidth{ NoScrollBarBackground };

    //#
    //# The padding from the left bottom of the horizontal scroll bar.
    //#
    LVec2F HScrollBarPadding{ 5.0f, 0.0f };

    //#
    //# The height of the horizontal scroll bar.
    //#
    f32 HScrollBarHeight{ 5.0f };

    //#
    //# The height of the horizontal scroll bar background. Zero means no background.
    //#
    f32 HScrollBarBackgroundHeight{ NoScrollBarBackground };
};

struct LFactoryScrollRegion;

//#
//# A #WScrollRegion is a parent node that may have many children.
//# The children that can be individually anchored with the #EAnchor enum.
//# Children are, as the name suggests, overlaid on top of each other and are independent of each of their siblings.
//# The scroll overlay usually should be anchored to its parent, and for unanchored sides the #SetScrollRegionSize in
//# should be used to define the size of the scroll overlay.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryScrollRegion)
class WScrollRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WScrollRegion(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::Visible);
    }
    template<typename TCxxClass>
    explicit WScrollRegion(TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}
    {
        this->SetVisibility(ENodeVisibility::Visible);
    }

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

        LColor VBackgroundTint { Colors::Black };
        LColor VTint { Colors::White };
        LColor HBackgroundTint { Colors::Black };
        LColor HTint { Colors::White };

        LVec2F VScrollBarPadding { 0.0f };
        f32 VScrollBarWidth { 5.0f };
        f32 VScrollBarBackgroundWidth { LScrollRegionBrush::NoScrollBarBackground };

        LVec2F HScrollBarPadding { 5.0f, 0.0f };
        f32 HScrollBarHeight { 5.0f };
        f32 HScrollBarBackgroundHeight { LScrollRegionBrush::NoScrollBarBackground };
    };

    // virtual void Draw(LNodeRenderInfo const& Info) const override; // TODO

    virtual LCursorReply SweepMouse(LViewport& Context, const LVec2F& InLocation) override;

    virtual LReply SweepFocusTest(const LViewport& Context, const LVec2F& InLocation) override;

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

    FORCEINLINE void ApplyScroll(const LVec2F& InScroll) { this->ScrollPosition = InScroll; }
    FORCEINLINE void ApplyVScroll(const f32 InScroll) { this->ScrollPosition.y = maths::clamp(InScroll, static_cast<f32>(MaxScrollUp), static_cast<f32>(MaxScrollDown)); }
    FORCEINLINE void ApplyHScroll(const f32 InScroll) { this->ScrollPosition.x = maths::clamp(InScroll, static_cast<f32>(MaxScrollLeft), static_cast<f32>(MaxScrollRight)); }

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

    FORCEINLINE constexpr LColor GetVBackgroundTint() const noexcept { return this->Brush.VBackgroundTint; }
    FORCEINLINE constexpr LColor GetVTint() const noexcept { return this->Brush.VTint; }
    FORCEINLINE constexpr LColor GetHBackgroundTint() const noexcept { return this->Brush.HBackgroundTint; }
    FORCEINLINE constexpr LColor GetHTint() const noexcept { return this->Brush.HTint; }
    FORCEINLINE constexpr void SetVBackgroundTint(const LColor& InValue) noexcept { this->Brush.VBackgroundTint = InValue; }
    FORCEINLINE constexpr void SetVTint(const LColor& InValue) noexcept { this->Brush.VTint = InValue; }
    FORCEINLINE constexpr void SetHBackgroundTint(const LColor& InValue) noexcept { this->Brush.HBackgroundTint = InValue; }
    FORCEINLINE constexpr void SetHTint(const LColor& InValue) noexcept { this->Brush.HTint = InValue; }

    FORCEINLINE constexpr LVec2F GetVScrollBarPadding() const noexcept { return this->Brush.VScrollBarPadding; }
    FORCEINLINE constexpr f32 GetVScrollBarWidth() const noexcept { return this->Brush.VScrollBarWidth; }
    FORCEINLINE constexpr f32 GetVScrollBarBackgroundWidth() const noexcept { return this->Brush.VScrollBarBackgroundWidth; }
    FORCEINLINE constexpr void SetVScrollBarPadding(const LVec2F& InValue) noexcept { this->Brush.VScrollBarPadding = InValue; }
    FORCEINLINE constexpr void SetVScrollBarWidth(const f32 InValue) noexcept { this->Brush.VScrollBarWidth = InValue; }
    FORCEINLINE constexpr void SetVScrollBarBackgroundWidth(const f32 InValue) noexcept { this->Brush.VScrollBarBackgroundWidth = InValue; }

    FORCEINLINE constexpr LVec2F GetHScrollBarPadding() const noexcept { return this->Brush.HScrollBarPadding; }
    FORCEINLINE constexpr f32 GetHScrollBarHeight() const noexcept { return this->Brush.HScrollBarHeight; }
    FORCEINLINE constexpr f32 GetHScrollBarBackgroundHeight() const noexcept { return this->Brush.HScrollBarBackgroundHeight; }
    FORCEINLINE constexpr void SetHScrollBarPadding(const LVec2F& InValue) noexcept { this->Brush.HScrollBarPadding = InValue; }
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

    bool DrawVScrollbar(const f32 InVisibleY) const;
    bool DrawHScrollbar(const f32 InVisibleX) const;

    LVec2F GetVInteractiveAreaScrollSize() const;
    LVec2F GetVInteractiveAreaScrollPositionFromOuter() const;

    LVec2F GetHInteractiveAreaScrollSize() const;
    LVec2F GetHInteractiveAreaScrollPositionFromOuter() const;

    LVec2F GetVBackgroundScrollSize() const;
    LVec2F GetVBackgroundScrollPositionFromOuter() const;
    LVec2F GetVForegroundScrollSize() const;
    LVec2F GetVForegroundScrollSize(const f32 InVisibleY) const;
    LVec2F GetVForegroundScrollPositionFromOuter() const;
    LVec2F GetVForegroundScrollPositionFromOuter(const f32 InScrollOffsetYPercent) const;

    LVec2F GetHBackgroundScrollSize() const;
    LVec2F GetHBackgroundScrollPositionFromOuter() const;
    LVec2F GetHForegroundScrollSize() const;
    LVec2F GetHForegroundScrollSize(const f32 InVisibleX) const;
    LVec2F GetHForegroundScrollPositionFromOuter() const;
    LVec2F GetHForegroundScrollPositionFromOuter(const f32 InScrollOffsetXPercent) const;

private:

    LScrollRegionBrushImpl Brush;

    //#
    //# The size to use for the whole scroll region if it was not anchored.
    //#
    LWidgetSize2 ScrollRegionSize;

    //#
    //# The scroll-position in percent. Where 0.0 is the top / left and 1.0 is the bottom / right.
    //#
    LVec2F ScrollPosition;

    mutable LVec2F DesiredSizeOfChildren;

    LDelegateHandle UserInterfaceTickDelegateHandle { nullptr };

    bool bCullNonVisible:1{ true };
    bool bUiTickV:1{ false };
    bool bUiTickH:1{ false };
    bool bUseChildrenDesiredSize:1{ false };
    f32 MbVOffset { 0.0 };
    f32 MbHOffset { 0.0 };
};

struct LFactoryScrollRegion : NODE_FACTORY_PARENT(WScrollRegion)
{
    NODE_FACTORY_BODY(WScrollRegion)

    decltype(auto) Brush(this auto&& Self, LScrollRegionBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) CullNonVisible(this auto&& Self, const bool bCull) noexcept
    {
        NODE_FACTORY_SELF().SetCullNonVisible(bCull);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) ScrollRegionSize(this auto&& Self, LWidgetSize2 const& InSize) noexcept
    {
        NODE_FACTORY_SELF().SetScrollRegionSize(InSize);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) UseChildrenDesiredSize(this auto&& Self, const bool bValue) noexcept
    {
        NODE_FACTORY_SELF().SetUseChildrenDesiredSize(bValue);
        return NODE_FACTORY_RESULT();
    }
};

FORCEINLINE void WScrollRegion::SetBrush(const LScrollRegionBrush& InBrush) noexcept
{
    this->Super::SetBrush(InBrush);
    this->Brush.Copy(InBrush);
}

FORCEINLINE void WScrollRegion::SetScrollRegionBrushOnly(const LScrollRegionBrush& InBrush) noexcept
{
    this->Brush.Copy(InBrush);
}

FORCEINLINE LScrollRegionBrush WScrollRegion::GetBrush() const noexcept
{
    static_assert(sizeof(LRegionBrush) == 72, "LRegionBrush has changed. Please modify this function.");

    LScrollRegionBrush Result;

    Result.Tint = this->Super::GetBrush().Tint;
    Result.Texture = this->Super::GetBrush().Texture;
    Result.TextureScale = this->Super::GetBrush().TextureScale;
    Result.TexCoordBehavior = this->Super::GetBrush().TexCoordBehavior;
    Result.SamplerAddressMode = this->Super::GetBrush().SamplerAddressMode;
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
