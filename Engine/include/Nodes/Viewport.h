// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/UserPreferencesForward.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/Material.h"
#include "Platform/SurfaceForward.h"
#include "Engine/Jxx.h"
#include "User/Input/RawInput.h"

namespace Jafg
{

class WNode;
class WUserWidget;
class LWorld;
struct LRenderInfo;
struct LNodeReply;

//#
//# Represents a viewport that can contain widgets.
//# A viewport has in most cases a handle to some sort of platform-specific window instance.
//#
class LViewport final
{
    friend WNode;

public:

    explicit LViewport(LSurface& Owner, rhi::extent2 const& Extent) noexcept
        : Surface{Owner}, Extent{Extent} { this->Outer.SetUserData(this); }
    PROHIBIT_REALLOC_OF_ANY_FORM(LViewport)
    ~LViewport() { this->TearDown(); }

    void Vk_OnLateInit();

    void DispatchInputs();
    void Tick();
    void Draw(LRenderInfo const& Info);
    void TearDown();

    FORCEINLINE constexpr LSurface& GetSurface() noexcept { return this->Surface; }

    FORCEINLINE constexpr LSurface const& GetSurface() const noexcept { return this->Surface; }
    FORCEINLINE constexpr rhi::extent2 GetExtent() const noexcept { return this->Extent; }
    //# The scale factor is based on the physical platform dpi in relation to the base dpi.
    FORCEINLINE constexpr EApplicationScale GetMaxAllowApplicationScale() const noexcept
    {
        if (this->Extent.width < 640 || this->Extent.height < 475) { return EApplicationScale::Single; }
        if (this->Extent.width < 960 || this->Extent.height < 720) { return EApplicationScale::Double; }
        return EApplicationScale::Triple;
    }

    FORCEINLINE constexpr f32  GetScaleFactor() const noexcept { return this->ScaleFactor; }
    FORCEINLINE constexpr void SetPlatformDpi(f32 Dpi) noexcept { this->PlatformDpi = Dpi; }
    FORCEINLINE constexpr f32  GetPlatformDpi() const noexcept { return this->PlatformDpi; }
    FORCEINLINE constexpr f32  GetBaseDpi() const noexcept { return this->BaseDpi; }

    //# Outer for this viewport only.
    FORCEINLINE LClassOuter& GetOuter() noexcept { return this->Outer; }
    FORCEINLINE LClassOuter const& GetOuter() const noexcept { return this->Outer; }

    //#
    //# Emplace two delegates until the state flag for the given key is met.
    //# F will be called each time the viewport processes its inputs and G will be called
    //# after the state flag is met. It is guaranteed that F will never be called after G.
    //# Both functors are optional.
    //# Both events will consume the key if available.
    //#
    template<ERawInputStateFlags Flags>
    //#
    FORCEINLINE void EmplaceUntil(LPhysicalKey Key, TFunction2<bool()> F, TFunction2<void()> G = {}) noexcept
    {
        this->KeyDelegates.emplace_back(Flags, Key, std::move(F), std::move(G));
    }

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
    mutable MULTI_EVENT_DECL_VERBOSE(LViewport, OnEarlyTick)
    mutable MULTI_EVENT_DECL_VERBOSE(LViewport, OnLateTick)

    //# Internal methods used by Jafg. Do not call yourself.
    ENGINE_API void _AddWidget(WUserWidget* Widget);
    ENGINE_API void _RemoveWidget(WUserWidget* Widget);

    NODISCARD FORCEINLINE constexpr auto const& GetTopLevelWidgets() const noexcept { return this->TopLevelWidgets; }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode* GetTopLevelWidgetByClass() noexcept
    {
        for (auto* Widget: this->TopLevelWidgets)
        {
            if (auto* Result{DynamicCast<TNode>(Widget)})
            {
                return Result;
            }
        }
        return nullptr;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode* GetTopLevelWidgetByClassChecked() noexcept
    {
        auto* Result{this->GetTopLevelWidgetByClass<TNode>()};
        check(Result)
        return Result;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode const* GetTopLevelWidgetByClass() const noexcept
    {
        for (auto* Widget: this->TopLevelWidgets)
        {
            if (auto* Result{DynamicCast<TNode>(Widget)})
            {
                return Result;
            }
        }
        return nullptr;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode const* GetTopLevelWidgetByClassChecked() const noexcept
    {
        auto* Result{this->GetTopLevelWidgetByClass<TNode>()};
        check(Result)
        return Result;
    }

    //#
    //# Clients should query for derived classes of #WUserWidget instead of individual nodes.
    //# If you find yourself quering for nodes, you are doing something wrong. This method is already
    //# highly controversial and should only be used with care.
    //#
    template<typename TNode> requires std::is_base_of_v<WUserWidget, TNode>
    NODISCARD FORCEINLINE TNode* FindVisibleWidgetByClass() noexcept;
    template<typename TNode> requires std::is_base_of_v<WUserWidget, TNode>
    NODISCARD FORCEINLINE TNode* FindVisibleWidgetByClassChecked() noexcept
    {
        auto* Result{this->FindVisibleWidgetByClass<TNode>()};
        check(Result)
        return Result;
    }
    template<typename TNode> requires std::is_base_of_v<WUserWidget, TNode>
    NODISCARD FORCEINLINE TNode const* FindVisibleWidgetByClass() const noexcept;
    template<typename TNode> requires std::is_base_of_v<WUserWidget, TNode>
    FORCEINLINE TNode const* FindVisibleWidgetByClassChecked() const noexcept
    {
        auto* Result{this->FindVisibleWidgetByClass<TNode>()};
        check(Result)
        return Result;
    }

    FORCEINLINE constexpr bool IsFocusedWidgetValid() const noexcept { return !!this->FocusedWidget; }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode const* GetFocusedWidget() const;
    FORCEINLINE constexpr WNode const* GetFocusedWidget() const;

    //# Convert the argument from a top-left origin vector to a bottom-left origin vector.
    FORCEINLINE constexpr void ConvertTLToBLOrigin(LVec2F* Vector) const noexcept
    {
        check(Vector) Vector->y = static_cast<f32>(this->Extent.height) - Vector->y;
    }

#if JAFG_DO_CHECKS
    NODISCARD FORCEINLINE constexpr bool _check_IsPrintTickTrace() const noexcept { return this->_check_PrintTickTrace; }
#endif /* JAFG_DO_CHECKS */

private:

    ENGINE_API void ChangeFocusImpl(TClassStorage<WNode> InNode);

    FORCEINLINE constexpr void RecalculateScaleFactor() noexcept { this->ScaleFactor = this->PlatformDpi / this->BaseDpi; }
    void HandleReply(LNodeReply&& Reply);

    LSurface& Surface;
    rhi::extent2 const& Extent;

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

    struct LKeyDelegate final
    {
        ERawInputStateFlags Flags;
        LPhysicalKey Key;
        TFunction2<bool()> F;
        TFunction2<void()> G;
    };
    TArray<LKeyDelegate> KeyDelegates;

    //# Top level widgets that this viewport owns.
    TArray<WUserWidget*> TopLevelWidgets;
    TClassStorage<WNode> FocusedWidget;

    LClassOuter Outer{ "SurfaceViewport" };

    LMaterialInstanceRef VisualBatchMaterialInstance;

#if JAFG_DO_CHECKS
    bool _check_PrintTickTrace{};
#endif /* JAFG_DO_CHECKS */
};

} /* ~Namespace Jafg */
