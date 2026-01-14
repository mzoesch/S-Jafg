// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "User/Input/Replies.h"
#include "User/UserPreferencesForward.h"

namespace Jafg
{

class WNode;
class WUserWidget;
class LEye;
class LWorld;
struct LRenderInfo;

struct LPerspectiveView final
{
    LEye* Eye{};
    LWorld* World{};
};

//# @see #LViewport::ApplySweepTranslation.
struct LViewportSweepTranslation final
{
    LViewportSweepTranslation() = delete;
    explicit LViewportSweepTranslation(const LViewport& InViewport, const LVector2D& InOffset) noexcept;
    ~LViewportSweepTranslation() noexcept;

    const LViewport& Viewport;
    LVector2D Offset;
};

//#
//# Represents a viewport that can contain widgets.
//# A viewport has in most cases a handle to some sort of platform-specific window instance.
//#
class LViewport final
{
    friend WNode;

public:

    explicit LViewport(LSurface& Owner) noexcept : Surface(Owner) { }
    PROHIBIT_REALLOC_OF_ANY_FORM(LViewport)
    ~LViewport() { this->TearDown(); }

    void ClearInvalidWidgets();
    void DispatchInputs(LSurface& Context, const LVector2D& InCursorLocation);
    void OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs);
    void OnClear();
    void Tick();
    void Draw(LRenderInfo const& Info);
    void TearDown();

    //# Outer for this viewport only.
    FORCEINLINE LClassOuter& GetOuter() noexcept { return this->Outer; }
    FORCEINLINE LClassOuter const& GetOuter() const noexcept { return this->Outer; }

    //#
    //# !!!DO NOT USE!!! - Please read carefully.
    //# This delegate is called after the viewport has ticked.
    //# This tick event is for widgets that need to be updated but are not ticked.
    //# Note that this delegate bypasses the superior power of the parent widget (that usually dictates which widgets
    //# are allowed to tick.).
    //# Therefore, you should ONLY use this delegate if your widget needs tick-based updates (this should usually ONLY
    //# be the case for user interface updates - but NEVER for content updates of a widget). If the parent does not
    //# allow tick inside the widget's #Tick, then you have to be satisfied with that.
    //# This event is for very, very few widgets - do not abuse its abilities to justify bad object structure design.
    //#
    mutable MULTI_EVENT_DECL_VERBOSE(LViewport, OnLateTick, LViewport const& InViewport)

    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
    FORCEINLINE TWidget* AddWidget()
    {
        TWidget* NewWidget{ ConstructDeferredWidgetNode<TWidget>(&this->Outer) };
        NewWidget->AddToViewport(this);
        MakeDeferredWidgetNodeFinal(NewWidget);
        return NewWidget;
    }
    void AddWidget(WUserWidget* Widget);
    void AddWidgetAt(const i32 Index, WUserWidget* Widget);
    ENGINE_API void RemoveWidget(WUserWidget* Widget);
    ENGINE_API bool TryRemoveWidget(WUserWidget* Widget);

    FORCEINLINE const LMatrix& GetCachedOrthographicProjectionMatrix() const noexcept { return this->CachedOrthographicProjectionMatrix; }

    //# The scale factor is based on the physical platform dpi in relation to the base dpi.
    FORCEINLINE EApplicationScale::Type GetMaxAllowApplicationScale() const noexcept;
    FORCEINLINE f32  GetScaleFactor() const { return this->ScaleFactor; }
    FORCEINLINE void SetPlatformDpi(const f32 InDpi) { this->PlatformDpi = InDpi; }
    FORCEINLINE f32  GetPlatformDpi() const { return this->PlatformDpi; }
    FORCEINLINE f32  GetBaseDpi() const { return this->BaseDpi; }

    ENGINE_API LUIntVector2 GetDimensions() const noexcept;
    FORCEINLINE i32         GetWidth() const noexcept { return this->GetDimensions().X; }
    FORCEINLINE i32         GetHeight() const noexcept { return this->GetDimensions().Y; }
    FORCEINLINE LVector2    GetDimensionsF() const { return {static_cast<f32>(this->GetDimensions().X), static_cast<f32>(this->GetDimensions().Y)}; }
    FORCEINLINE f32         GetWidthF() const noexcept { return static_cast<f32>(this->GetDimensions().X); }
    FORCEINLINE f32         GetHeightF() const noexcept { return static_cast<f32>(this->GetDimensions().Y); }
    FORCEINLINE LVector2D   GetDimensionsD() const noexcept { return {static_cast<f64>(this->GetDimensions().X), static_cast<f64>(this->GetDimensions().Y)}; }
    FORCEINLINE f64         GetWidthD() const noexcept { return static_cast<f64>(this->GetDimensions().X); }
    FORCEINLINE f64         GetHeightD() const noexcept { return static_cast<f64>(this->GetDimensions().Y); }

    ENGINE_API  WNode* GetTopLevelWidgetByClass(TSubclassOf<WNode> Class) const;
    FORCEINLINE WNode* GetTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const;
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClass() const;
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClassChecked() const;

    template<typename TNode>
    FORCEINLINE auto GetFocusedWidget() const -> const TNode* { return DynamicCast<TNode>(this->FocusedWidget.GetPointer()); }
    FORCEINLINE auto GetFocusedWidget() const -> const WNode* { return this->FocusedWidget.GetPointer(); }
    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    bool FocusWidgetNode(WNode* InNode);
    FORCEINLINE auto GetHoveredWidgets() const -> const TArray<TClassStorage<WNode>>& { return this->HoveredWidgets; }

    //# @return True if in the last frame, this node was not added.
    bool AddHoveredWidgetForFrame(WNode* Node);

    FORCEINLINE f32 GetFrameOrthoZLayerDepth() const noexcept { this->FrameZLayerDepth += 0.0001f; return this->FrameZLayerDepth; }

    //#
    //# The translation that is recommended for children of a #WNode to use while drawing.
    //# This translation should be removed after said #WNode is finished drawing.
    //# This value is reset every frame.
    //#
    FORCEINLINE void ApplyFrameTranslation(LVector2D const& InTranslation) const noexcept { this->FrameTranslation += InTranslation; }
    FORCEINLINE LVector2D const& GetFrameTranslation() const noexcept { return this->FrameTranslation; }

    //#
    //# The translation that is recommended for children of a #WParentBase to use while sweeping.
    //# This translation should be removed if a parent widget finished its sweep logic.
    //# This value is reset every frame.
    //# @remark Use the #LViewportSweepTranslation for easy RAII style translation logic.
    //#
    FORCEINLINE void ApplySweepTranslation(const LVector2D& InTranslation) const noexcept { this->SweepTranslation += InTranslation; }
    FORCEINLINE LVector2D const& GetSweepTranslation() const noexcept { return this->SweepTranslation; }

    FORCEINLINE bool HasFrameCulls() const noexcept { return this->FrameCulls.empty() == false; }
    FORCEINLINE TArray<LVector4D> const& GetFrameCulls() const noexcept { return this->FrameCulls; }
    FORCEINLINE LVector2D GetFrameCullTopLeft() const noexcept
    {
        check( this->HasFrameCulls() )
        const LVector4D& CullDimensions{ this->FrameCulls.back() };
        return LVector2D{ CullDimensions.X, CullDimensions.Y };
    }
    FORCEINLINE LVector2D GetFrameCullSize() const noexcept
    {
        check( this->HasFrameCulls() )
        const LVector4D& CullDimensions{ this->FrameCulls.back() };
        return LVector2D{ CullDimensions.Z, CullDimensions.W };
    }
    FORCEINLINE void PushFrameCull(LVector4D const& CullDimensions) noexcept { this->FrameCulls.emplace_back(CullDimensions); }
    FORCEINLINE void PopFrameCull() noexcept { this->FrameCulls.pop_back(); }

    TArray<LPerspectiveView> PerspectiveViews;

    FORCEINLINE LSurface& GetSurface() noexcept { return this->Surface; }
    FORCEINLINE const LSurface& GetSurface() const noexcept { return this->Surface; }

    FORCEINLINE auto const& GetCachedCursorLocation() const noexcept { return this->CachedCursorLocation; }
    FORCEINLINE auto const& GetCachedCursorLocationChecked() const noexcept { check( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }
    FORCEINLINE auto const& GetCachedCursorLocationAsserted() const noexcept { jassert( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }

    //#
    //# Convert the argument from a top-left origin vector to a bottom-left origin vector.
    //#
    FORCEINLINE constexpr void ConvertTLToBLOrigin(LVector2D* Vector) const noexcept;

    //#
    //# @return True if the point is inside the bounds of the viewport.
    //#
    FORCEINLINE static constexpr bool IsInBounds(const LVector2D& InTopLeft, const LVector2D& InSize, const LVector2D& InPoint) noexcept;

private:

    void ChangeFocusUnsafe(WNode* InNode);

    void RecalculateScaleFactor();
    void HandleReply(LSurface& Context, const LCursorReply& Reply);
    void HandleReply(LSurface& Context, const LReply& Reply);

    LMatrix CachedOrthographicProjectionMatrix;

    //# The factor with which the entire orthographic projection is scaled.
    f32 ScaleFactor{ 1.0f };
    //# The dpi fetched from the physical platform.
    f32 PlatformDpi{ 0.0f };
    //#
    //# The base dpi that the application was designed for.
    //# All scales are based and calculated from this value, and only for the drawing
    //# we use the platform dpi.
    //#
    f32 BaseDpi{ 96.0f };

    //# Top level widgets that this viewport owns.
    TArray<WUserWidget*> TopLevelWidgets;

    TClassStorage<WNode> FocusedWidget;
    TArray<TClassStorage<WNode>> HoveredWidgets;
    TArray<TClassStorage<WNode>> LastFrameHoveredWidgets;

    mutable f32 FrameZLayerDepth { 0.0f };
    mutable LVector2D FrameTranslation;
    mutable LVector2D SweepTranslation;
    TArray<LVector4D> FrameCulls;

    LSurface& Surface;
    TOptional<LVector2D> CachedCursorLocation;

    LClassOuter Outer{ "SurfaceViewport" };
};

FORCEINLINE LViewportSweepTranslation::LViewportSweepTranslation(const LViewport& InViewport, const LVector2D& InOffset) noexcept
    : Viewport(InViewport), Offset(InOffset)
{
    this->Viewport.ApplySweepTranslation(this->Offset);
    return;
}

FORCEINLINE LViewportSweepTranslation::~LViewportSweepTranslation() noexcept
{
    this->Viewport.ApplySweepTranslation(-this->Offset);
    return;
}

FORCEINLINE EApplicationScale::Type LViewport::GetMaxAllowApplicationScale() const noexcept
{
    auto Dimensions{ this->GetDimensions() };

    if (Dimensions.X < 640 || Dimensions.Y < 475)
    {
        return EApplicationScale::Single;
    }

    if (Dimensions.X < 960 || Dimensions.Y < 720)
    {
        return EApplicationScale::Double;
    }

    return EApplicationScale::Triple;
}

FORCEINLINE WNode* LViewport::GetTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const
{
    WNode* Widget{ this->GetTopLevelWidgetByClass(Class) };
    check( Widget )
    return Widget;
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClass() const
{
    return StaticCast<TNode>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClassChecked() const
{
    return StaticCastChecked<TNode>(this->GetTopLevelWidgetByClassChecked(TNode::StaticClass()));
}

FORCEINLINE constexpr bool LViewport::IsInBounds(const LVector2D& InTopLeft, const LVector2D& InSize, const LVector2D& InPoint) noexcept
{
    return
            InTopLeft.X <= InPoint.X
         && InPoint.X   <= InTopLeft.X + InSize.X
         && InTopLeft.Y <= InPoint.Y
         && InPoint.Y   <= InTopLeft.Y + InSize.Y
         ;
}

FORCEINLINE constexpr void LViewport::ConvertTLToBLOrigin(LVector2D* Vector) const noexcept
{
    checkSlow( Vector )
    Vector->Y = this->GetDimensions().Y - Vector->Y;
}

} /* ~Namespace Jafg */
