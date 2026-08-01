// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Engine/Engine.h"
#include "Framework/PawnComponent.h"
#include "Framework/PersonaControllerComponent.h"
#include "Framework/SupremePolicies.h"
#include "Rhi/StaticMeshRenderable.h"
#include "Editor.generated.h"

namespace Jafg
{

class WTabOverlay;
class WEditor;
class WButton;
class WTextButton;
class WWorldViewer;
class WEditorCategorySeparator;
struct LFactoryEditorCategorySeparator;
struct LEditorLayout;

enum struct EGizmo: u32;

struct LEditorLayout final
{
    struct LNodes;
    struct LFlow;
    struct LSurface;
    typedef std::variant<LNodes, LFlow> LChild;

    struct LNodes final
    {
        f32 Dist{};
        TArray<LString> Children;
    };

    struct LFlow final
    {
        ENodePrimitiveControlflow Controlflow;
        TArray<LChild> Children;
    };

    struct LSurface final
    {
        bool bFullscreen{};
        bool bBorderless{};
        LVec2u32 Dimensions{1280, 720};
        LFlow Layout;
    };

    LPath Path;
    TArray<LSurface> Surfaces;
};
SERDE_JSON_TYPE_NON_INTRUSIVE(LEditorLayout::LNodes, Dist, Children)
SERDE_JSON_TYPE_NON_INTRUSIVE(LEditorLayout::LFlow, Controlflow, Children)
SERDE_JSON_TYPE_NON_INTRUSIVE(LEditorLayout::LSurface, bFullscreen, bBorderless, Dimensions, Layout)
SERDE_JSON_TYPE_NON_INTRUSIVE(LEditorLayout, Surfaces)
inline void to_json(json& j, LEditorLayout::LChild const& C){ std::visit([&j](auto&& Arg){ j = Arg; }, C); }
inline void from_json(json const& j, LEditorLayout::LChild& C)
{
    serde::JsonExpectType<LEditorLayout::LChild>(j, json::value_t::object);
    if (j.contains("Controlflow"))
    {
        if (j.contains("Dist"))
        {
            serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LFlow>();
    }
    else
    {
        if (j.contains("Controlflow"))
        {
            serde::JsonSink<LEditorLayout::LChild>(j, "Expected either a LFlow or LNodes but found keys for both. Failed to parse layout.");
        }
        C = j.get<LEditorLayout::LNodes>();
    }

    return;
}

ENGINE_API void ToggleEditorNodesTransitively(WNode& Node, bool bEnabled);

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorBackground final : public WVRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditorBackground)
    {
        this->Anchor = EAnchor::HFill;
        this->Brush.Tint = Colors::Black;
        this->Space = 1_spt;
        this->Padding = {0_spt, 0, 0, 1};
    }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditorCategorySeparator)
class ENGINE_API WEditorCategorySeparator final : public WTextButtonIconizedDouble
{
    GENERATED_CLASS_BODY()

    friend LFactoryEditorCategorySeparator;

protected:

    explicit WEditorCategorySeparator(LNodeDynamicInit const& Init) noexcept
        : Super{Init}
    {
        this->_ctor_Logic();
    }
    template<typename TCxxClass> explicit WEditorCategorySeparator(TNodeStaticInit<TCxxClass> const& Init, LString Category) noexcept
        : Super{Init}
    {
        this->SetContent(std::move(Category));
        this->_ctor_Logic();
    }

public:

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

private:

    void _ctor_Logic();
    TArray<std::pair<ENodeVisibility, WNode*>> Nodes;
};

struct LFactoryEditorCategorySeparator : NODE_FACTORY_PARENT(WEditorCategorySeparator)
{
    NODE_FACTORY_BODY(WEditorCategorySeparator)

    decltype(auto) operator[](this auto&& Self, LNodeFactoryBase&& F) noexcept
    {
        check(!F._IsReleased())
        auto& Node{DETAIL_JAFG_NODE_FACTORY_SELF()};

        Self.GetMutableSiblings().emplace_back(&F.GetRawNode());
        Node.Nodes.emplace_back(F.GetRawNode().GetVisibility(), &F.GetRawNode());
        for (auto* Sibling: F.GetSiblings())
        {
            check(Sibling)
            Node.Nodes.emplace_back(Sibling->GetVisibility(), Sibling);
            Self.GetMutableSiblings().emplace_back(Sibling);
        }
        algo::orphan(&F.GetMutableSiblings());
        checkCode(F._Release())
        checkCode(F._Decommission())

        return NODE_FACTORY_RESULT();
    }
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditor final : public WUserWidget, public LTabOverlayPossibilities
{
    GENERATED_CLASS_BODY()

protected:

    inline explicit WEditor(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
        this->SetShouldTick(true);
    }
    template<typename TCxxClass>
    inline explicit WEditor(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, LTabOverlayPossibilities{*static_cast<WUserWidget*>(this)}
    {
        this->SetShouldTick(true);
    }

public:

    static inline constexpr LPath GetUserLayoutsPath() noexcept
    {
        return finder::saved_dir() / "Layouts";
    }
    static void BeginClassLife(LBeginClassLifeInfo const& Info);

    // LTabOverlayPossibilities implementation
    virtual WParent& GetOverlayRoot() noexcept override;
    // ~LTabOverlayPossibilities implementation

    virtual void Construct() override;
    virtual void Tick() override;

    FORCEINLINE TArray<LPath> const& GetDiscoveredLayouts() const noexcept { return this->DiscoveredLayouts; }
    TArray<LPath> const& DiscoverLayouts();
    LEditorLayout LoadEditorLayout(LPath Path);
    void ApplyEditorLayout(LEditorLayout const& Layout);

private:

    TArray<LPath> DiscoveredLayouts;

    WParent* OverlayRoot;
    WParent* Bar{};

    algo::clock::time_point LastRatePoint;
    WTextBox* Rate{};
};

DECLARE_JAFG_CLASS()
class ENGINE_API AEditorSupremePolicies final : public ASupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AEditorSupremePolicies)

public:

    virtual void OnWorldPreInit() override;
    virtual void OnPersonaControllerCreated(APersonaController& Pc) override;
    virtual TJxxUnique<APawn> GetPawnForPersonaController(APersonaController const& Pc) override;
};

namespace SSBO
{

struct Ray: rhi::ssbo_template<Ray>
{
    constexpr Ray(LVec3F Origin, LVec3F End, LColor Tint) noexcept: Origin{Origin}, End{End}, Tint{Tint} {}

    LVec3F Origin;
    f32 _pad0{1.0f};
    LVec3F End;
    LColor Tint;
};
static_assert(rhi::ssbo<Ray>);

} /* ~Namespace SSBO */

struct LEditorTraceOrigin
{
    WWorldViewer& Node;
    LKeyEvent Event;
};

DECLARE_JAFG_CLASS()
class ENGINE_API AEditorPersonaControllerComponent final : public APersonaControllerComponent
{
GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AEditorPersonaControllerComponent)
    {
        this->bTick = true;
        this->SetShouldRender(true);
    }

public:

    static constexpr u64 MaxLineCount{1024};
    static constexpr f32 OneTimeDraw{0.0f};

    static constexpr u64 ShaderSpace{0uz};
    static constexpr u64 DebugLinesIndex{0uz};
    static constexpr u64 ViewProjIndex{1uz};

    static constexpr u64 GizmoSolidColorInputSpace{0uz};
    static constexpr u64 GizmoSolidColorInputIndex{0uz};

    virtual void OnAttach(AActor& InOwner) override;
    virtual void ParentTick(f32 Dt) override;
    virtual void Render(LActorRenderInfo const& Info) const override;

    EGizmo SetSelectedGizmo(EGizmo Gizmo) noexcept;
    AActor* SetSelectedActor(WWorldViewer* Origin, AActor* Actor) noexcept;

    struct RayCreateInfo
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldMagRay3 Value;
        LWorldMagRay3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddRay(RayCreateInfo Ray) noexcept { this->Rays.emplace_back(std::move(Ray)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddRayNextTick(RayCreateInfo Ray) noexcept { this->NextRays.emplace_back(std::move(Ray)); }

    struct AabbCreateInfo
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldAabb3 Value;
        LWorldAabb3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddAabb(AabbCreateInfo Aabb) noexcept { this->Aabbs.emplace_back(std::move(Aabb)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddAabbNextTick(AabbCreateInfo Aabb) noexcept { this->NextAabbs.emplace_back(std::move(Aabb)); }

    enum EGizmoMesh
    {
        TranslateX, TranslateY, TranslateZ,
        TranslateX_Var, TranslateY_Var, TranslateZ_Var,
        RotateR, RotateY, RotateP,
        ScaleX, ScaleY, ScaleZ,
        ScaleX_Var, ScaleY_Var, ScaleZ_Var,
        PlaneYZ, PlaneXY , PlaneXZ,

        GizmoCount, GizmoBegin = TranslateX,
    };

    void SetHighlightedGizmoMesh(EGizmoMesh Mesh) noexcept;
    EGizmoMesh GetHighlightedGizmoMesh() const noexcept;

    NODISCARD EGizmoMesh TraceGizmo(LWorldMagRay3 const& Ray, rhi::extent2 Extent, LWorldEye const& Eye) noexcept;
    void TraceForGizmo(EGizmoMesh Mesh, LWorldMagRay3 const& Ray, rhi::extent2 Extent, LEditorTraceOrigin const& Origin);

private:

    void RenderRays(LActorRenderInfo const& Info) const;
    void RenderGizmo(LActorRenderInfo const& Info) const;

    EGizmo Gizmo{};
    struct LSelectedActor final
    {
        WWorldViewer* Viewer{};
        AActor* Actor{};
        std::optional<LWorldQuat> Quaternion;
        NODISCARD constexpr AActor* operator->() noexcept { return this->Actor; }
        NODISCARD constexpr AActor const* operator->() const noexcept { return this->Actor; }
    };
    LSelectedActor SelectedActor;
    NODISCARD constexpr bool IsSelectedActorValid() const noexcept
    {
        bool b{this->SelectedActor.Actor && this->SelectedActor->HasRootComponent()};
        check(!b || this->SelectedActor.Viewer)
        return b;
    }

    mutable TArray<RayCreateInfo> Rays;
    TArray<RayCreateInfo> NextRays;
    mutable TArray<AabbCreateInfo> Aabbs;
    TArray<AabbCreateInfo> NextAabbs;

    LMaterialInstanceRef RayInstance;
    rhi::frame_array<rhi::mapped_device_buffer> RayBuffers;
    rhi::frame_array<rhi::mapped_device_buffer> RayViewBuffers;

    struct LHighlightedGizmoMesh final
    {
        u64 Frame{};
        mutable EGizmoMesh Mesh{GizmoCount};
        EGizmoMesh& operator*() noexcept { return this->Mesh; }
        EGizmoMesh const& operator*() const noexcept { return this->Mesh; }
    } HighlightedGizmoMesh;
    EGizmoMesh UsedGizmoMesh{GizmoCount};
    mutable std::array<LStaticMeshRenderable, GizmoCount> GizmoMeshes;
    void SetTranslationForGizmos(LWorldVec3 Translation) noexcept
    {
        for (auto Idx{std::to_underlying(GizmoBegin)}; Idx < std::to_underlying(GizmoCount); ++Idx)
        {
            this->GizmoMeshes[Idx].SetTranslation(Translation);
        }
    }
};

DECLARE_JAFG_CLASS()
class ENGINE_API AEditorCameraComponent final : public APawnComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AEditorCameraComponent)

public:

    virtual void OnAttach(AActor& InOwner) override
    {
        Super::OnAttach(InOwner);
        (void)this->ActivateUserInputContext();
    }
    virtual void OnNewPersonaController(APersonaController* New) override
    {
        Super::OnNewPersonaController(New);
        (void)this->ActivateUserInputContext();
    }

    //# @return Whether the context was activated successfully.
    bool ActivateUserInputContext() const noexcept;

    void OnHighlightTrace(WWorldViewer& Viewer, rhi::extent2 Extent, LVec2F Location);
    void OnTrace(WWorldViewer& Viewer, bool bMultiselect, rhi::extent2 Extent, LVec2F Location, std::optional<LEditorTraceOrigin> Origin = {});
    void OnMove(LInputActionValue const& Value);
    void OnRotate(LInputActionValue const& Value);
    void OnVelocityMultiplierChange(LInputActionValue const& Value);

    NODISCARD FORCEINLINE f32 GetVelocityMultiplier() const noexcept { return this->VelocityMultiplier; }
    FORCEINLINE f32 SetVelocityMultiplier(f32 NewVelocityMultiplier) noexcept
    {
        this->VelocityMultiplier = maths::clamp(NewVelocityMultiplier, MinVelocityMultiplier, MaxVelocityMultiplier);
        return this->VelocityMultiplier;
    }

    NODISCARD  FORCEINLINE constexpr f32 GetSensitivity() const noexcept { return this->Sensitivity; }
    FORCEINLINE void SetSensitivity(f32 Value) noexcept { this->Sensitivity = Value; }

    NODISCARD FORCEINLINE constexpr f32 GetVelocityMultiplierAcceleration() const noexcept { return this->VelocityMultiplierAcceleration; }
    FORCEINLINE void SetVelocityMultiplierAcceleration(f32 Value) noexcept
    {
        check(Value > 0.0f)
        this->VelocityMultiplierAcceleration = Value;
    }

private:

    //# Sensitivity in radians per pixel.
    f32 Sensitivity{0.1f};
    static constexpr f32 MinVelocityMultiplier{0.0f};
    static constexpr f32 MaxVelocityMultiplier{1'000.0f};
    f32 VelocityMultiplier{10.0f};
    f32 VelocityMultiplierAcceleration{1.0f};
    f32 CachedPitch{};
};

} /* ~Namespace Jafg */
