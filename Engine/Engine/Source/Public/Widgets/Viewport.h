// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

class LSurface;
class LObjectClass;
class WWidgetNode;
class WUserWidget;

/**
 * Represents a viewport that can contain widgets.
 * A viewport has in most cases a handle to some sort of platform-specific window instance.
 */
class ENGINE_API LViewport final
{
    friend WWidgetNode;

public:

    LViewport() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LViewport)
    ~LViewport() = default;

    void Initialize();
    void DispatchInputs(LSurface& Context, const LVector2& InLocation);
    void OnMouseLeftViewport(LSurface& Context);
    void Tick();
    void Draw();
    void TearDown();

    void AddWidget(WUserWidget* Widget);
    void RemoveWidget(WUserWidget* Widget);

    /** The scale factor is based on the physical platform dpi in relation to the base dpi. */
    FORCEINLINE auto GetScaleFactor() const -> float { return this->ScaleFactor; }
    FORCEINLINE auto SetPlatformDpi(const float InDpi) -> void { this->PlatformDpi = InDpi; }
    FORCEINLINE auto GetPlatformDpi() const -> float { return this->PlatformDpi; }
    FORCEINLINE auto GetBaseDpi() const -> float { return this->BaseDpi; }

    auto ChangeDimensions(const LIntVector2& InDimensions) -> void;
    FORCEINLINE auto GetDimensions() const -> LIntVector2 { return this->Dimensions; }

    FORCEINLINE auto GetFrameOrthoZLayerDepth() const -> float
    {
        this->FrameZLayerDepth += 0.0001f;
        return this->FrameZLayerDepth;
    }

    auto GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) -> WWidgetNode*;
    auto GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const -> const WWidgetNode*;
    FORCEINLINE auto GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) -> WWidgetNode*;
    FORCEINLINE auto GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) const -> const WWidgetNode*;
    template <typename TNode> FORCEINLINE auto GetTopLevelWidgetByClass() -> TNode*;
    template <typename TNode> FORCEINLINE auto GetTopLevelWidgetByClass() const -> const TNode*;
    template <typename TNode> FORCEINLINE auto GetCheckedTopLevelWidgetByClass() -> TNode*;
    template <typename TNode> FORCEINLINE auto GetCheckedTopLevelWidgetByClass() const -> const TNode*;

    template <typename TNode>
    FORCEINLINE auto GetFocusedWidget() const -> const TNode* { return DynamicCast<TNode>(this->FocusedWidget); }
    FORCEINLINE auto GetFocusedWidget() const -> const WWidgetNode* { return this->FocusedWidget; }
    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    FORCEINLINE auto GetHoveredWidgets() const -> const TdhArray<WWidgetNode*>& { return this->HoveredWidgets; }

    /** @return True if in the last frame, this node was not added. */
    bool AddHoveredWidgetForFrame(WWidgetNode* Node);

private:

    void RecalculateScaleFactor();

    /** The factor with which the entire orthographic projection is scaled. */
    float ScaleFactor =  1.0f;
    /** The dpi fetched from the physical platform. */
    float PlatformDpi =  0.0f;
    /**
     * The base dpi that the application was designed for.
     * All scales are based and calculated from this value, and only for the drawing
     * we use the platform dpi.
     */
    float BaseDpi     = 96.0f;

    /** The dimensions of the viewport in px. */
    LIntVector2            Dimensions;
    /** Top level widgets that this viewport owns. */
    TdhArray<WUserWidget*> TopLevelWidgets;

    WWidgetNode* FocusedWidget = nullptr;
    TdhArray<WWidgetNode*> HoveredWidgets = { };
    TdhArray<WWidgetNode*> LastFrameHoveredWidgets = { };

    mutable float FrameZLayerDepth = 0.0f;
};

FORCEINLINE auto LViewport::GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) -> WWidgetNode*
{
    WWidgetNode* Widget = this->GetTopLevelWidgetByClass(WidgetClass);
    check( Widget )
    return Widget;
}

FORCEINLINE auto LViewport::GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) const -> const WWidgetNode*
{
    const WWidgetNode* Widget = this->GetTopLevelWidgetByClass(WidgetClass);
    check( Widget )
    return Widget;
}

template <typename TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClass()
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode, true>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE const TNode* LViewport::GetTopLevelWidgetByClass() const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode, true>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE TNode* LViewport::GetCheckedTopLevelWidgetByClass()
{
    return CheckedStaticCast<TNode>(this->GetCheckedTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE const TNode* LViewport::GetCheckedTopLevelWidgetByClass() const
{
    return CheckedStaticCast<TNode>(this->GetCheckedTopLevelWidgetByClass(TNode::StaticClass()));
}

} /* ~Namespace Jafg */
