// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Widgets/Node.h"
#include "Rhi/FrameBuffer.h"
#include "User/Input/Replies.h"

namespace Jafg
{

class WNode;
class WUserWidget;
class LEye;
class LWorld;

struct LBackgroundContext final
{
    LEye*   Eye   { nullptr };
    LWorld* World { nullptr };
};

MAKE_MULTICAST_SIGNATURE(LOnLateTick, const LViewport& InViewport)

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

    LViewport() = default;
    PROHIBIT_COPY(LViewport)
    DEFAULT_MOVE(LViewport)
    ~LViewport() = default;

    void Initialize() { }
    void ClearInvalidWidgets();
    void DispatchInputs(LSurface& Context, const LVector2& InCursorLocation);
    void OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs);
    void OnClear();
    void Tick();
    void Draw();
    void TearDown();

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
    mutable LOnLateTick OnLateTick;

    void AddWidget(WUserWidget* Widget);
    void AddWidgetAt(const i32 Index, WUserWidget* Widget);
    ENGINE_API void RemoveWidget(WUserWidget* Widget);
    ENGINE_API bool TryRemoveWidget(WUserWidget* Widget);

    FORCEINLINE const LMatrix& GetCachedOrthographicProjectionMatrix() const noexcept { return this->CachedOrthographicProjectionMatrix; }

    //# The scale factor is based on the physical platform dpi in relation to the base dpi.
    FORCEINLINE f32  GetScaleFactor() const { return this->ScaleFactor; }
    FORCEINLINE void SetPlatformDpi(const f32 InDpi) { this->PlatformDpi = InDpi; }
    FORCEINLINE f32  GetPlatformDpi() const { return this->PlatformDpi; }
    FORCEINLINE f32  GetBaseDpi() const { return this->BaseDpi; }

    ENGINE_API void ChangeDimensions(const LIntVector2& InDimensions);
    FORCEINLINE LIntVector2 GetDimensions() const { return this->Dimensions; }
    FORCEINLINE i32         GetWidth() const noexcept { return this->Dimensions.X; }
    FORCEINLINE i32         GetHeight() const noexcept { return this->Dimensions.Y; }
    FORCEINLINE LVector2    GetDimensionsF() const { return {static_cast<f32>(this->Dimensions.X), static_cast<f32>(this->Dimensions.Y)}; }
    FORCEINLINE f32         GetWidthF() const noexcept { return static_cast<f32>(this->Dimensions.X); }
    FORCEINLINE f32         GetHeightF() const noexcept { return static_cast<f32>(this->Dimensions.Y); }
    FORCEINLINE LVector2D   GetDimensionsD() const noexcept { return {static_cast<f64>(this->Dimensions.X), static_cast<f64>(this->Dimensions.Y)}; }
    FORCEINLINE f64         GetWidthD() const noexcept { return static_cast<f64>(this->Dimensions.X); }
    FORCEINLINE f64         GetHeightD() const noexcept { return static_cast<f64>(this->Dimensions.Y); }

    ENGINE_API  WNode* GetTopLevelWidgetByClass(TSubclassOf<WNode> Class) const;
    FORCEINLINE WNode* GetTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const;
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClass() const;
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClassChecked() const;

    template <typename TNode>
    FORCEINLINE auto GetFocusedWidget() const -> const TNode* { return DynamicCast<TNode>(this->FocusedWidget); }
    FORCEINLINE auto GetFocusedWidget() const -> const WNode* { return this->FocusedWidget; }
    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    bool FocusWidgetNode(WNode* InNode);
    FORCEINLINE auto GetHoveredWidgets() const -> const TArray<TClassStorage<WNode>>& { return this->HoveredWidgets; }

    //# @return True if in the last frame, this node was not added.
    bool AddHoveredWidgetForFrame(WNode* Node);

    FORCEINLINE f32 GetFrameOrthoZLayerDepth() const { this->FrameZLayerDepth += 0.0001f; return this->FrameZLayerDepth; }

    //#
    //# The translation that is recommended for children of a #WNode to use while drawing.
    //# This translation should be removed after said #WNode is finished drawing.
    //# This value is reset every frame.
    //#
    FORCEINLINE void ApplyFrameTranslation(const LVector2D& InTranslation) const { this->FrameTranslation += InTranslation; }
    FORCEINLINE auto GetFrameTranslation() const -> const LVector2D& { return this->FrameTranslation; }

    //#
    //# The translation that is recommended for children of a #WParentBase to use while sweeping.
    //# This translation should be removed if a parent widget finished its sweep logic.
    //# This value is reset every frame.
    //# @remark Use the #LViewportSweepTranslation for easy RAII style translation logic.
    //#
    FORCEINLINE void ApplySweepTranslation(const LVector2D& InTranslation) const noexcept { this->SweepTranslation += InTranslation; }
    FORCEINLINE auto GetSweepTranslation() const -> const LVector2D& { return this->SweepTranslation; }

    FORCEINLINE auto GetBackgroundContexts() const noexcept -> const TArray<LBackgroundContext>& { return this->BackgroundContexts; }
    FORCEINLINE auto GetMutableBackgroundContexts() noexcept -> TArray<LBackgroundContext>& { return this->BackgroundContexts; }
    FORCEINLINE bool IsIntermediateBufferValid() const noexcept { return this->IntermediateBuffer.IsValid(); }
    FORCEINLINE auto GetIntermediateBuffer() const noexcept -> const LFrameBuffer& { return this->IntermediateBuffer; }

    //#
    //# Get the most recent context that was used on this viewport. Might be null, so do not use without checking.
    //#
    FORCEINLINE       LSurface* GetCachedContext()       { return this->CachedContext; }
    FORCEINLINE const LSurface* GetCachedContext() const { return this->CachedContext; }
    FORCEINLINE       LSurface* GetCachedContextChecked()       { check( this->CachedContext ) return this->CachedContext; }
    FORCEINLINE const LSurface* GetCachedContextChecked() const { check( this->CachedContext ) return this->CachedContext; }
    FORCEINLINE       LSurface* GetCachedContextAsserted()       { jassert( this->CachedContext ) return this->CachedContext; }
    FORCEINLINE const LSurface* GetCachedContextAsserted() const { jassert( this->CachedContext ) return this->CachedContext; }

    FORCEINLINE const TOptional<LVector2>& GetCachedCursorLocation() const { return this->CachedCursorLocation; }
    FORCEINLINE const TOptional<LVector2>& GetCachedCursorLocationChecked() const { check( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }
    FORCEINLINE const TOptional<LVector2>& GetCachedCursorLocationAsserted() const { jassert( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }

    //#
    //# Convert the argument from a top-left origin vector to a bottom-left origin vector.
    //#
    FORCEINLINE constexpr void ConvertTLToBLOrigin(LVector2* Vector) const noexcept;

    //#
    //# @return True if the point is inside the bounds of the viewport.
    //#
    FORCEINLINE static constexpr bool IsInBounds(const LVector2& InTopLeft, const LVector2& InSize, const LVector2& InPoint) noexcept;

    FORCEINLINE const Lal::LLinearColor& GetBackgroundColor() { return this->BackgroundColor; }
    FORCEINLINE void SetBackgroundColor(const Lal::LLinearColor& InColor) { this->BackgroundColor = InColor; }

private:

    void ChangeFocusUnsafe(WNode* InNode);

    void RecalculateScaleFactor();
    void HandleReply(LSurface& Context, const LCursorReply& Reply);
    void HandleReply(LSurface& Context, const LReply& Reply);

    LMatrix CachedOrthographicProjectionMatrix;

    //# The factor with which the entire orthographic projection is scaled.
    f32 ScaleFactor { 1.0f };
    //# The dpi fetched from the physical platform.
    f32 PlatformDpi { 0.0f };
    //#
    //# The base dpi that the application was designed for.
    //# All scales are based and calculated from this value, and only for the drawing
    //# we use the platform dpi.
    //#
    f32 BaseDpi { 96.0f };

    //# The dimensions of the viewport in px.
    LIntVector2 Dimensions;
    //# Top level widgets that this viewport owns.
    TArray<WUserWidget*> TopLevelWidgets;

    TClassStorage<WNode> FocusedWidget;
    TArray<TClassStorage<WNode>> HoveredWidgets;
    TArray<TClassStorage<WNode>> LastFrameHoveredWidgets;

    mutable f32 FrameZLayerDepth { 0.0f };
    mutable LVector2D FrameTranslation;
    mutable LVector2D SweepTranslation;

    TArray<LBackgroundContext> BackgroundContexts;
    LFrameBuffer IntermediateBuffer;

    LSurface* CachedContext { nullptr };
    TOptional<LVector2> CachedCursorLocation;

    Lal::LLinearColor BackgroundColor;
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

FORCEINLINE WNode* LViewport::GetTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const
{
    WNode* Widget{ this->GetTopLevelWidgetByClass(Class) };
    check( Widget )
    return Widget;
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClass() const
{
    return StaticCastChecked<TNode>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClassChecked() const
{
    return StaticCastChecked<TNode>(this->GetTopLevelWidgetByClassChecked(TNode::StaticClass()));
}

FORCEINLINE constexpr bool LViewport::IsInBounds(const LVector2& InTopLeft, const LVector2& InSize, const LVector2& InPoint) noexcept
{
    return
            InTopLeft.X <= InPoint.X
         && InPoint.X   <= InTopLeft.X + InSize.X
         && InTopLeft.Y <= InPoint.Y
         && InPoint.Y   <= InTopLeft.Y + InSize.Y
         ;
}

FORCEINLINE constexpr void LViewport::ConvertTLToBLOrigin(LVector2* Vector) const noexcept
{
    checkSlow( Vector )
    Vector->Y = this->Dimensions.Y - Vector->Y;
}

} /* ~Namespace Jafg */
