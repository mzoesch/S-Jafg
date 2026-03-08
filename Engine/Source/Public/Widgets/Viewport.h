// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "User/Input/Replies.h"
#include "User/UserPreferencesForward.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/Material.h"

namespace Jafg
{

class WNode;
class WUserWidget;
class LWorld;
struct LRenderInfo;

//# @see #LViewport::ApplySweepTranslation.
struct LViewportSweepTranslation final
{
    LViewportSweepTranslation() = delete;
    explicit LViewportSweepTranslation(LViewport const& InViewport, LVec2F const& InOffset) noexcept;
    ~LViewportSweepTranslation() noexcept;

    const LViewport& Viewport;
    LVec2F Offset;
};

//#
//# Represents a viewport that can contain widgets.
//# A viewport has in most cases a handle to some sort of platform-specific window instance.
//#
class LViewport final
{
    friend WNode;

public:

    explicit LViewport(LSurface& Owner) noexcept : Surface(Owner) { this->Outer.SetUserData(this); }
    PROHIBIT_REALLOC_OF_ANY_FORM(LViewport)
    ~LViewport() { this->TearDown(); }

    void Vk_OnLateInit();

    void ClearInvalidWidgets();
    void DispatchInputs(LSurface& Surface, TOptional<LVec2F> const& CursorLocation);
    void OnMouseLeftViewport(LSurface& Context, bool bInvalidateAllInputs);
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

    //# Internal methods used by Jafg. Do not call yourself.
    ENGINE_API void _AddWidget(WUserWidget* Widget);
    ENGINE_API void _RemoveWidget(WUserWidget* Widget);

    //# The scale factor is based on the physical platform dpi in relation to the base dpi.
    FORCEINLINE EApplicationScale GetMaxAllowApplicationScale() const noexcept;
    FORCEINLINE f32  GetScaleFactor() const { return this->ScaleFactor; }
    FORCEINLINE void SetPlatformDpi(const f32 InDpi) { this->PlatformDpi = InDpi; }
    FORCEINLINE f32  GetPlatformDpi() const { return this->PlatformDpi; }
    FORCEINLINE f32  GetBaseDpi() const { return this->BaseDpi; }

    ENGINE_API LVec2u32 GetDimensions() const noexcept;
    FORCEINLINE i32     GetWidth() const noexcept { return this->GetDimensions().x; }
    FORCEINLINE i32     GetHeight() const noexcept { return this->GetDimensions().y; }
    FORCEINLINE LVec2F  GetDimensionsF() const { return {static_cast<f32>(this->GetDimensions().x), static_cast<f32>(this->GetDimensions().y)}; }
    FORCEINLINE f32     GetWidthF() const noexcept { return static_cast<f32>(this->GetDimensions().x); }
    FORCEINLINE f32     GetHeightF() const noexcept { return static_cast<f32>(this->GetDimensions().y); }
    FORCEINLINE LVec2F  GetDimensionsD() const noexcept { return {static_cast<f64>(this->GetDimensions().x), static_cast<f64>(this->GetDimensions().y)}; }
    FORCEINLINE f64     GetWidthD() const noexcept { return static_cast<f64>(this->GetDimensions().x); }
    FORCEINLINE f64     GetHeightD() const noexcept { return static_cast<f64>(this->GetDimensions().y); }

    FORCEINLINE constexpr auto const& GetTopLevelWidgets() const noexcept { return this->TopLevelWidgets; }
    ENGINE_API  WNode* GetTopLevelWidgetByClass(TSubclassOf<WNode> Class) const noexcept;
    FORCEINLINE WNode* GetTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const noexcept{ auto* Widget{this->GetTopLevelWidgetByClass(Class)}; check(Widget); return Widget; }
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClass() const noexcept { return StaticCast<TNode>(this->GetTopLevelWidgetByClass(TNode::StaticClass())); }
    template <typename TNode> requires std::is_base_of_v<WNode, TNode> TNode* GetTopLevelWidgetByClassChecked() const noexcept { return StaticCastChecked<TNode>(this->GetTopLevelWidgetByClassChecked(TNode::StaticClass())); }

    template<typename TNode>
    FORCEINLINE auto GetFocusedWidget() const -> const TNode* { return DynamicCast<TNode>(this->FocusedWidget.GetPointer()); }
    FORCEINLINE auto GetFocusedWidget() const -> const WNode* { return this->FocusedWidget.GetPointer(); }
    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    bool FocusWidgetNode(WNode* InNode);
    FORCEINLINE auto GetHoveredWidgets() const -> const TArray<TClassStorage<WNode>>& { return this->HoveredWidgets; }

    //# @return True if in the last frame, this node was not added.
    bool AddHoveredWidgetForFrame(WNode* Node);

    //#
    //# The translation that is recommended for children of a #WNode to use while drawing.
    //# This translation should be removed after said #WNode is finished drawing.
    //# This value is reset every frame.
    //#
    FORCEINLINE void ApplyFrameTranslation(LVec2F const& InTranslation) const noexcept { this->FrameTranslation += InTranslation; }
    FORCEINLINE LVec2F const& GetFrameTranslation() const noexcept { return this->FrameTranslation; }

    //#
    //# The translation that is recommended for children of a #WParentBase to use while sweeping.
    //# This translation should be removed if a parent widget finished its sweep logic.
    //# This value is reset every frame.
    //# @remark Use the #LViewportSweepTranslation for easy RAII style translation logic.
    //#
    FORCEINLINE void ApplySweepTranslation(const LVec2F& InTranslation) const noexcept { this->SweepTranslation += InTranslation; }
    FORCEINLINE LVec2F const& GetSweepTranslation() const noexcept { return this->SweepTranslation; }

    FORCEINLINE bool HasFrameCulls() const noexcept { return this->FrameCulls.empty() == false; }
    FORCEINLINE TArray<LVec4F> const& GetFrameCulls() const noexcept { return this->FrameCulls; }
    FORCEINLINE LVec2F GetFrameCullTopLeft() const noexcept
    {
        check( this->HasFrameCulls() )
        const LVec4F& CullDimensions{ this->FrameCulls.back() };
        return LVec2F{ CullDimensions.x, CullDimensions.y };
    }
    FORCEINLINE LVec2F GetFrameCullSize() const noexcept
    {
        check( this->HasFrameCulls() )
        const LVec4F& CullDimensions{ this->FrameCulls.back() };
        return LVec2F{ CullDimensions.z, CullDimensions.w };
    }
    FORCEINLINE void PushFrameCull(LVec4F const& CullDimensions) noexcept { this->FrameCulls.emplace_back(CullDimensions); }
    FORCEINLINE void PopFrameCull() noexcept { this->FrameCulls.pop_back(); }

    FORCEINLINE LSurface& GetSurface() noexcept { return this->Surface; }
    FORCEINLINE const LSurface& GetSurface() const noexcept { return this->Surface; }

    FORCEINLINE auto const& GetCachedCursorLocation() const noexcept { return this->CachedCursorLocation; }
    FORCEINLINE auto const& GetCachedCursorLocationChecked() const noexcept { check( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }
    FORCEINLINE auto const& GetCachedCursorLocationAsserted() const noexcept { jassert( this->CachedCursorLocation.has_value() ) return this->CachedCursorLocation; }

    //#
    //# Convert the argument from a top-left origin vector to a bottom-left origin vector.
    //#
    FORCEINLINE CONSTEXPR_CHECK_SLOW void ConvertTLToBLOrigin(LVec2F* Vector) const noexcept;

    //#
    //# @return True if the point is inside the bounds of the viewport.
    //#
    FORCEINLINE static constexpr bool IsInBounds(const LVec2F& InTopLeft, const LVec2F& InSize, const LVec2F& InPoint) noexcept;

private:

    void ChangeFocusUnsafe(WNode* InNode);

    void RecalculateScaleFactor();
    void HandleReply(LSurface& Context, const LCursorReply& Reply);
    void HandleReply(LSurface& Context, const LReply& Reply);

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

    mutable LVec2F FrameTranslation;
    mutable LVec2F SweepTranslation;
    TArray<LVec4F> FrameCulls;

    LSurface& Surface;
    TOptional<LVec2F> CachedCursorLocation;

    LClassOuter Outer{ "SurfaceViewport" };

    u64 MaxInstanceCount{ 128 };
    TFrameArray<LMappedDeviceBuffer> VisualBatches;
    TFrameArray<vk::raii::DescriptorSet> Vk_VisualSharedDescriptorSets JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    TFrameArray<LMappedDeviceBuffer> Vk_VisualSharedBuffers;

    LMaterialInstanceRef VisualBatchMaterial;
};

FORCEINLINE LViewportSweepTranslation::LViewportSweepTranslation(LViewport const& InViewport, LVec2F const& InOffset) noexcept
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

FORCEINLINE EApplicationScale LViewport::GetMaxAllowApplicationScale() const noexcept
{
    auto Dimensions{ this->GetDimensions() };

    if (Dimensions.x < 640 || Dimensions.y < 475)
    {
        return EApplicationScale::Single;
    }

    if (Dimensions.x < 960 || Dimensions.y < 720)
    {
        return EApplicationScale::Double;
    }

    return EApplicationScale::Triple;
}

FORCEINLINE constexpr bool LViewport::IsInBounds(const LVec2F& InTopLeft, const LVec2F& InSize, const LVec2F& InPoint) noexcept
{
    return
            InTopLeft.x <= InPoint.x
         && InPoint.x   <= InTopLeft.x + InSize.x
         && InTopLeft.y <= InPoint.y
         && InPoint.y   <= InTopLeft.y + InSize.y
         ;
}

FORCEINLINE CONSTEXPR_CHECK_SLOW void LViewport::ConvertTLToBLOrigin(LVec2F* Vector) const noexcept
{
    checkSlow( Vector )
    Vector->y = this->GetDimensions().y - Vector->y;
}

namespace Detail
{

inline constexpr LNodeDynamicInit LOuter2ViewportProj::operator()(LCxxDynamicInit const& Init) const noexcept
{
    check(Init.Outer.GetUserData())
    return {.Outer=*static_cast<LViewport*>(Init.Outer.GetUserData()),.Class=Init.Class};
}

} /* ~Namespace Detail */

inline WNode::WNode(LNodeDynamicInit const& Init) noexcept
    : Super{LCxxDynamicInit{.Outer=std::invoke(LNodeDynamicInit::Proj{}, Init.Outer),.Class=Init.Class}}
    , AttachedViewport{Init.Outer}
{
}

} /* ~Namespace Jafg */

