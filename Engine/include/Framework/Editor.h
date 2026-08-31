// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Nodes/HButton.h"
#include "Widgets/WorldViewer.h"
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
class WText;
class WTextButton;
class WEditorWorldViewer;
class WEditorCategorySeparator;
class WEditorWorldViewerHierarchy;
class WEditorWorldViewerInspector;
class WEditableTextButton;
struct LFactoryEditorCategorySeparator;
struct LEditorLayout;

namespace Detail
{

class WEditorWorldViewerHierarchyObjectHButton;

} /* ~Namespace Detail */

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
SERDE_JSON_TYPE(LEditorLayout::LNodes, Dist, Children)
SERDE_JSON_TYPE(LEditorLayout::LFlow, Controlflow, Children)
SERDE_JSON_TYPE(LEditorLayout::LSurface, bFullscreen, bBorderless, Dimensions, Layout)
SERDE_JSON_TYPE(LEditorLayout, Surfaces)
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

DECLARE_JAFG_WIDGET()
class ENGINE_API WSecondaryEditor final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WSecondaryEditor(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->SetShouldTick(true);
    }
    template<typename TCxxClass> explicit WSecondaryEditor(TNodeStaticInit<TCxxClass> const& Init
        , LTabOverlayPossibilities& Possibilities) noexcept
        : Super{Init}, Possibilities{&Possibilities}
    {
        this->SetShouldTick(true);
    }

public:

    virtual void Construct() override;

    void SetPossibilitiesLate(LTabOverlayPossibilities& Possibilities) noexcept
    {
        check(!this->Possibilities)
        this->Possibilities = &Possibilities;
    }

    NODISCARD LFactoryTabOverlay GetNewOverlay();
    NODISCARD WTabOverlay& FindNewOverlay(f32 Dist = {});

private:

    LTabOverlayPossibilities* Possibilities{};
    NODISCARD WTabOverlay* GetSelected() noexcept { check(this->Possibilities) return this->Possibilities->Selected; }
    NODISCARD WTabOverlay const* GetSelected() const noexcept { check(this->Possibilities) return this->Possibilities->Selected; }
    NODISCARD auto const& GetOverlays() const { check(this->Possibilities) return this->Possibilities->Overlays; }
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorWorldViewer : public WWorldViewer
{
    GENERATED_CLASS_BODY()

    friend WEditorWorldViewerHierarchy;
    friend WEditorWorldViewerInspector;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditorWorldViewer)

public:

    enum struct EStartType: u8
    {
        Pie,
        NewPie,
        FollowPie,
        Spectate,
        Standalone,
    };

    enum struct EPawnSart: u8
    {
        Default,
        EditorEye,
    };

    enum struct ENetMode: u8
    {
        Standalone,
        Client,
        Listen,
    };

    enum struct EGizmo: u32{ Select,Translate,Rotate,Scale,Gizmo, };

    JAFG_DEFAULT_TAB_CANDIDATE("World Viewer", "Icons/Jafg.Sphere")

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    NODISCARD FORCEINLINE constexpr bool HasHierarchy() const noexcept { return this->Hierarchy != nullptr; }
    NODISCARD FORCEINLINE constexpr WEditorWorldViewerHierarchy* GetHierarchy() const noexcept { return this->Hierarchy; }
    NODISCARD FORCEINLINE constexpr bool HasInspector() const noexcept { return this->Inspector != nullptr; }
    NODISCARD FORCEINLINE constexpr WEditorWorldViewerInspector* GetInspector() const noexcept { return this->Inspector; }

    //# Select new actors. All actors must be valid.
    void SelectActors(TArray<std::pair<AActor*,AActorComponent*>> Actors, bool bForce = false);
    //# Jafg guarantees that all listed actors are valid.
    MULTI_EVENT_DECL(OnActorsSelected, TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New)
    NODISCARD FORCEINLINE constexpr auto const& GetSelectedActors() const noexcept { return this->SelectedActors; }

    void SelectGizmo(EGizmo Gizmo);
    void UpdateClientGizmo();
    NODISCARD constexpr EGizmo GetSelectedGizmo() const noexcept { return this->CurrentGizmo; }

    void FocusActors();

    NODISCARD bool IsGridSpaceLocal() const noexcept;
    NODISCARD bool IsTranslationGridSnapEnabled() const noexcept;
    NODISCARD bool IsRotationGridSnapEnabled() const noexcept;
    NODISCARD bool IsScaleGridSnapEnabled() const noexcept;
    NODISCARD constexpr f32 GetTranslationGridSnap() const noexcept { return this->SnapTranslation; }
    NODISCARD constexpr f32 GetRotationGridSnap() const noexcept { return this->SnapRotation; }
    NODISCARD constexpr f32 GetScaleGridSnap() const noexcept { return this->SnapScale; }

protected:

    virtual void InitializeRenderTarget() override;
    virtual bool OnPreDrawImpl(LRenderInfo const& Info) override;
    virtual void OnPostWorldDrawImpl(LRenderInfo const& Info, APersonaController& Ctrl, APawn& Pawn, LWorldEye const& Eye) override;

    virtual LTransientPersona::Local GetTransientPersona() noexcept override;
    virtual void OnConnect() override;
    virtual void OnDisconnect() override;

private:

    static constexpr rhi::extent2 DefaultExtent{640,480};

    void PreDrawSelected(LRenderInfo const& Info);

    void CreateMenuDropDown(LVec2F Where);
    void CreateStartDropDown(LVec2F Where);

    //# TODO: Not a good solution. We have to think of something more permanent maybe in the fufute??
    void InitializeJustSummoned(LWorld& World);
    void TravelToJustSummoned(LWorld& World);

    EStartType StartType{EStartType::Pie};
    EPawnSart PawnStart{EPawnSart::Default};
    ENetMode NetMode{ENetMode::Standalone};

    struct LEditorReconnectionData final
    {
        LWorld* World{};
        LWorldTrans EyeTrans{ maths::identity<LWorldTrans> };
    };
    std::optional<LEditorReconnectionData> EditorReconnectionData;
    WWorldViewer* RunningInstance{};
    bool bIsNextPersonaEditorControlled{};
    NODISCARD constexpr bool IsRunning() const noexcept { return !!this->RunningInstance; }

    NODISCARD LTexture2Ref GetStartTexture() const;

    WButton* StartButton{};
    WButton* StepButton{};
    WButton* StopButton{};
    WButton* DetachButton{};
    WButton* MoreButton{};
    void UpdateStartStopButtons();
    void OnLaunchAll();

    EGizmo CurrentGizmo{EGizmo::Select};

    WButton* SelectButton{};
    WButton* TranslateGizmoButton{};
    WButton* RotateGizmoButton{};
    WButton* ScaleGizmoButton{};
    WButton* GizmoButton{};

    //# Toggle between local and world grid space.
    WButton* ToggleGridSpace{};

    WButton* ToggleTranslationGridSnapButton{};
    WTextButton* TranslationGridSnapButton{};
    WButton* ToggleRotationGridSnapButton{};
    WTextButton* RotationGridSnapButton{};
    WButton* ToggleScaleGridSnapButton{};
    WTextButton* ScaleGridSnapButton{};
    f32 SnapTranslation{ 0.1f };
    f32 SnapRotation{ 5.0f };
    f32 SnapScale{ 0.1f };

    WTextButtonIconizedDouble* CameraButton{};
    std::optional<f32> RequestedCameraSpeed;
    f32 CameraSpeed{ 10.0f };
    f32 CameraAcceleration{ 1.0f };
    NODISCARD LString GetCameraSpeedString() const noexcept
    {
        std::ostringstream ss; ss << std::fixed << std::setprecision(2) << this->CameraSpeed;
        auto String{ss.str()};
        if (!String.empty() && String.back() == '.')
        {
            String.push_back('0');
        }
        return String;
    }

    struct LFocusTransition final
    {
        LWorldVec3 Origin;
        LWorldVec3 Destination;
        algo::clock::time_point Start;
        f64 Duration;
    };
    std::optional<LFocusTransition> FocusTransition;

    WText* DebugLocationText{};

    TArray<std::pair<AActor*,AActorComponent*>> SelectedActors;
    LMaterialInstanceRef SelectionMaterialInstance;
    LMaterialInstanceRef PostSelectionMaterialInstance;

    WEditorWorldViewerHierarchy* Hierarchy{};
    WEditorWorldViewerInspector* Inspector{};
};

//#
//# A widget to view a world in a hierarchical manner.
//# Each viewer can have exact one hierarchy widget and each hierarchical widget can only connect to one world viewer.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorWorldViewerHierarchy : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditorWorldViewerHierarchy)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Hierarchy", "Icons/Jafg.Hierarchy")

    virtual void Construct() override;
    virtual void Destruct() override;

    void _OnWorldViewerDestruct();
    void OnWorldViewerUpdate();

private:

    WEditorWorldViewer* FindSmart() const noexcept;
    WEditorWorldViewer* FindInViewport(LViewport const& Viewport) const noexcept;
    WEditorWorldViewer* FindInNode(WNode& Node) const noexcept;

    static constexpr LNodeSize2 TypeSize{128_spt, 0.0f};
    static constexpr LWhitespace ListPadding{20_spt, 0.0f};

    WEditorWorldViewer* WorldViewer{};
    LDelegateHandle OnActorsSelectedHandle;
    void DisconnectFromViewer();
    bool OnActorsSelected(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New);

    WText* ConnectedText{};
    void UpdateConnectedArea();

    WParent* Container{};
    void* LastContainerElemSelected{};
    void UpdateWorldObjectList(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New);
    WTextBox* SelectedWorldObjectText{};
    void UpdateSelectedWorldObjectText(std::size_t Count, std::size_t Shown, std::size_t Selected);

    LNodeReply OnWorldObjectListKeyEventFocus(Detail::WEditorWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event);
    LNodeReply OnWorldObjectListKeyEventUnfocus(Detail::WEditorWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event);

    LWorld* GetWorld() const noexcept
    {
        if (this->WorldViewer && this->WorldViewer->IsOwnedPersonaControllerValid())
        {
            return &this->WorldViewer->GetOwnedPersonaControllerChecked()->GetWorld();
        }
        return nullptr;
    }
};

//#
//# A widget that displays details about a selected object.
//# Just like #WEditorWorldViewerHierarchy this widget thas a one-to-one relation to the world viewer.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorWorldViewerInspector : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditorWorldViewerInspector)
    {
        this->SetShouldTick(true);
    }

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Details", "Icons/Jafg.Information")

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;

    void _OnWorldViewerDestruct();
    void OnWorldViewerUpdate();

private:

    WEditorWorldViewer* FindSmart() const noexcept;
    WEditorWorldViewer* FindInViewport(LViewport const& Viewport) const noexcept;
    WEditorWorldViewer* FindInNode(WNode& Node) const noexcept;

    WEditorWorldViewer* WorldViewer{};
    LDelegateHandle OnActorsSelectedHandle;
    void DisconnectFromViewer();
    bool OnActorsSelected(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New);

    WText* ConnectedText{};
    void UpdateConnectedArea();

    WEditableTextButton* EditableObjectDisplayName{};
    WParent* Container{};
    WEditableTextButton* ContainerSearch{};
    void UpdateObjectDisplayName();

    WParent* ComponentContainerWrapper{};
    WParent* ComponentContainer{};
    TArray<TFunction2<void()>> ComponentUpdateFunctions;
    AActorComponent* SelectedComponent{};
    void UpdateObjectDetails();
    void ReloadInnerComponents();
    void SelectComponent(AActorComponent* Component);
};

namespace Detail
{

struct LFactoryEditorWorldViewerHierarchyObjectHButton;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryEditorWorldViewerHierarchyObjectHButton)
class WEditorWorldViewerHierarchyObjectHButton : public WHButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WEditorWorldViewerHierarchyObjectHButton)

public:

    AActor* Actor{};
};

struct LFactoryEditorWorldViewerHierarchyObjectHButton: NODE_FACTORY_PARENT(WEditorWorldViewerHierarchyObjectHButton)
{
    NODE_FACTORY_BODY(WEditorWorldViewerHierarchyObjectHButton)

    constexpr decltype(auto) Actor(this auto&& Self, AActor* Actor) noexcept
    {
        NODE_FACTORY_SELF().Actor = Actor;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Detail */

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
    WEditorWorldViewer& Node;
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

    static constexpr u64 MaxLineCount{16'384};
    static constexpr f32 OneTimeDraw{0.0f};

    static constexpr u64 ShaderSpace{0uz};
    static constexpr u64 DebugLinesIndex{0uz};
    static constexpr u64 ViewProjIndex{1uz};

    static constexpr u64 GizmoSolidColorInputSpace{0uz};
    static constexpr u64 GizmoSolidColorInputIndex{0uz};

    virtual void OnAttach(AActor& InOwner) override;
    virtual void ParentTick(f32 Dt) override;
    virtual void Render(LActorRenderInfo const& Info) const override;

    WEditorWorldViewer::EGizmo SetSelectedGizmo(WEditorWorldViewer::EGizmo Gizmo) noexcept;
    std::pair<AActor*,AActorComponent*> SetSelectedActor(WEditorWorldViewer* Origin, std::pair<AActor*,AActorComponent*> Actor) noexcept;

    struct LineCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldLine3 Value;
        LWorldLine3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddLine(LineCreateInfo Line) noexcept { this->Lines.emplace_back(std::move(Line)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddLineNextTick(LineCreateInfo Line) noexcept { this->NextLines.emplace_back(std::move(Line)); }

    struct RayCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldMagRay3 Value;
        LWorldMagRay3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddRay(RayCreateInfo Ray) noexcept { this->Rays.emplace_back(std::move(Ray)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddRayNextTick(RayCreateInfo Ray) noexcept { this->NextRays.emplace_back(std::move(Ray)); }

    struct AabbCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldAabb3 Value;
        LWorldAabb3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddAabb(AabbCreateInfo Aabb) noexcept { this->Aabbs.emplace_back(std::move(Aabb)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddAabbNextTick(AabbCreateInfo Aabb) noexcept { this->NextAabbs.emplace_back(std::move(Aabb)); }

    struct TextCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        f32 Height;
        LWorldVec3 Location;
        LString Text;
        LWorldVec3 const* operator->() const noexcept { return &this->Location; }
    };
    void AddText(TextCreateInfo Text) noexcept { this->Texts.emplace_back(std::move(Text)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddTextNextTick(TextCreateInfo Text) noexcept { this->NextTexts.emplace_back(std::move(Text)); }

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
    void RenderTexts(LActorRenderInfo const& Info) const;

    WEditorWorldViewer::EGizmo Gizmo{};
    struct LSelectedActor final
    {
        WEditorWorldViewer* Viewer{};
        std::pair<AActor*,AActorComponent*> Actor{nullptr,nullptr};
        std::optional<LWorldQuat> Quaternion;
    };
    LSelectedActor SelectedActor;
    NODISCARD constexpr bool IsSelectedActorValid() const noexcept
    {
        bool b{this->SelectedActor.Actor.first && this->SelectedActor.Actor.second};
        check(!b || this->SelectedActor.Viewer)
        return b;
    }

    mutable TArray<LineCreateInfo> Lines;
    TArray<LineCreateInfo> NextLines;
    mutable TArray<RayCreateInfo> Rays;
    TArray<RayCreateInfo> NextRays;
    mutable TArray<AabbCreateInfo> Aabbs;
    TArray<AabbCreateInfo> NextAabbs;
    mutable TArray<TextCreateInfo> Texts;
    TArray<TextCreateInfo> NextTexts;

    LMaterialInstanceRef RayInstance;
    rhi::frame_array<rhi::mapped_device_buffer> RayBuffers;
    rhi::frame_array<rhi::mapped_device_buffer> RayViewBuffers;

    LMaterialInstanceRef TextMaterialInstance;

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
    bool ActivateUserInputContext() noexcept;

    void OnHighlightTrace(WEditorWorldViewer& Viewer, rhi::extent2 Extent, LVec2F Location);
    void OnTrace(WEditorWorldViewer& Viewer, bool bMultiselect, rhi::extent2 Extent, LVec2F Location, std::optional<LEditorTraceOrigin> Origin = {});
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
