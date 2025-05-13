// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Node.h"
#include "Engine/ObjectBaseUtility.h"
#include "Rhi/FrameBuffer.h"
#include "User/Input/Replies.h"

namespace Jafg
{

class LObjectClass;
class WNode;
class WUserWidget;
class LEye;
class LWorld;

struct LBackgroundContext final
{
    LEye*   Eye   = nullptr;
    LWorld* World = nullptr;
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
    void DispatchInputs(LSurface& Context, const LVector2& InCursorLocation);
    void OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs);
    void OnClear();
    void Tick();
    void Draw();
    void TearDown();

    ENGINE_API void AddWidget(WUserWidget* Widget);
    ENGINE_API void RemoveWidget(WUserWidget* Widget);
    ENGINE_API bool TryRemoveWidget(WUserWidget* Widget);

    //# The scale factor is based on the physical platform dpi in relation to the base dpi.
    FORCEINLINE auto GetScaleFactor() const -> float { return this->ScaleFactor; }
    FORCEINLINE auto SetPlatformDpi(const float InDpi) -> void { this->PlatformDpi = InDpi; }
    FORCEINLINE auto GetPlatformDpi() const -> float { return this->PlatformDpi; }
    FORCEINLINE auto GetBaseDpi() const -> float { return this->BaseDpi; }

    auto ChangeDimensions(const LIntVector2& InDimensions) -> void;
    FORCEINLINE auto GetDimensions() const -> LIntVector2 { return this->Dimensions; }

    FORCEINLINE auto GetFrameOrthoZLayerDepth() const -> float { this->FrameZLayerDepth += 0.0001f; return this->FrameZLayerDepth; }

    ENGINE_API  WNode* GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const;
    FORCEINLINE WNode* GetTopLevelWidgetByClassChecked(const LObjectClass* WidgetClass) const;
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClass() const;
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClassChecked() const;

    template <typename TNode>
    FORCEINLINE auto GetFocusedWidget() const -> const TNode* { return DynamicCast<TNode>(this->FocusedWidget); }
    FORCEINLINE auto GetFocusedWidget() const -> const WNode* { return this->FocusedWidget; }
    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    bool FocusWidgetNode(const WNode* InNode);
    FORCEINLINE auto GetHoveredWidgets() const -> const TArray<TObjectStorage<WNode>>& { return this->HoveredWidgets; }

    //# @return True if in the last frame, this node was not added.
    bool AddHoveredWidgetForFrame(WNode* Node);

    FORCEINLINE auto GetBackgroundContexts() const -> const TArray<LBackgroundContext>& { return this->BackgroundContexts; }
    FORCEINLINE auto GetMutableBackgroundContexts() -> TArray<LBackgroundContext>& { return this->BackgroundContexts; }
    FORCEINLINE auto GetBackgroundBuffer() const -> const LFrameBuffer& { return this->BackgroundBuffer; }

    //#
    //# Get the most recent context that was used on this viewport. Might be null, so do not use without checking.
    //#
    FORCEINLINE LSurface* GetCachedContext() { return this->CachedContext; }
    FORCEINLINE LSurface* GetCachedContextChecked() { check( this->CachedContext ) return this->CachedContext; }
    FORCEINLINE LSurface* GetCachedContextAsserted() { jassert( this->CachedContext ) return this->CachedContext; }

private:

    void ChangeFocusUnsafe(const WNode* InNode);

    void RecalculateScaleFactor();
    void HandleReply(LSurface& Context, const LCursorReply& Reply);
    void HandleReply(LSurface& Context, const LReply& Reply);

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

    TObjectStorage<WNode> FocusedWidget;
    TArray<TObjectStorage<WNode>> HoveredWidgets;
    TArray<TObjectStorage<WNode>> LastFrameHoveredWidgets;

    mutable f32 FrameZLayerDepth { 0.0f };

    TArray<LBackgroundContext> BackgroundContexts;
    LFrameBuffer BackgroundBuffer;

    LSurface* CachedContext { nullptr };
};

FORCEINLINE WNode* LViewport::GetTopLevelWidgetByClassChecked(const LObjectClass* WidgetClass) const
{
    WNode* Widget = this->GetTopLevelWidgetByClass(WidgetClass);
    check( Widget )
    return Widget;
}

template <typename TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClass() const
{
    static_assert(std::derived_from<TNode, WNode>, "TNode must derive from WNode.");
    return CheckedStaticCast<TNode, true>(this->GetTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE TNode* LViewport::GetTopLevelWidgetByClassChecked() const
{
    static_assert(std::derived_from<TNode, WNode>, "TNode must derive from WNode.");
    return CheckedStaticCast<TNode>(this->GetTopLevelWidgetByClassChecked(TNode::StaticClass()));
}

} /* ~Namespace Jafg */
