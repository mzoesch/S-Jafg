// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Framework/Lackey.h"
#include "Nodes/GenericTabInfos.h"
#include "Rhi/RenderTarget.h"
#include "Platform/Surface.h"
#include "Nodes/HButton.h"
#include "WorldViewer.generated.h"

namespace Jafg
{

class WText;
class WTextBox;
class WTextButton;
class WHButton;
class WVButton;
class WWorldViewer;
class WWorldViewerHierarchy;
class WWorldViewerInspector;
class WEditableTextButton;

namespace Detail
{

class WWorldViewerHierarchyObjectHButton;

} /* ~Namespace Detail */

//# A widget to perspectively view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget, public LLocalLackey
{
    GENERATED_CLASS_BODY()

    friend WWorldViewerHierarchy;
    friend WWorldViewerInspector;

    static constexpr rhi::extent2 DefaultExtent{640,480};

protected:

    explicit WWorldViewer(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, LLocalLackey{this->RenderTargetViewport}
        , RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
        this->_ctor_SetBackgroundTint();
    }
    template<typename TCxxClass>
    explicit WWorldViewer(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, LLocalLackey{Init.Outer}
        , RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
        this->_ctor_SetBackgroundTint();
    }

public:

    static constexpr rhi::extent2 MinViewportExtent{128,128};

    virtual ~WWorldViewer() override;

    JAFG_DEFAULT_TAB_CANDIDATE("World Viewer", "Icons/Jafg.Sphere")

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void Destruct() override;

    virtual void OnFocusLost() override;

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    NODISCARD FORCEINLINE constexpr LRenderTarget const& GetWorldRenderTarget() const noexcept { return this->RenderTarget; }
    NODISCARD FORCEINLINE constexpr LViewport& GetWorldRenderTargetViewport() noexcept { return this->RenderTargetViewport; }
    NODISCARD FORCEINLINE constexpr LViewport const& GetWorldRenderTargetViewport() const noexcept { return this->RenderTargetViewport; }

    LColor BorderTint{ Colors::Black };

    //#
    //# The desired extent.
    //# If not set, then the extent of the render target will be determined by the size of this node.
    //#
    std::optional<rhi::extent2> DesiredViewportExtent;
    FORCEINLINE bool IsManual() const noexcept { return this->DesiredViewportExtent.has_value(); }

    void TravelTo(LWorld& World);
    void QueueTravelTo(LWorld& World);

    void OnPerspectiveDepthTestChanged();

    NODISCARD FORCEINLINE constexpr bool HasHierarchy() const noexcept { return this->Hierarchy != nullptr; }
    NODISCARD FORCEINLINE constexpr WWorldViewerHierarchy* GetHierarchy() const noexcept { return this->Hierarchy; }
    NODISCARD FORCEINLINE constexpr bool HasInspector() const noexcept { return this->Inspector != nullptr; }
    NODISCARD FORCEINLINE constexpr WWorldViewerInspector* GetInspector() const noexcept { return this->Inspector; }

    //# Select new actors. All actors must be valid.
    void SelectActors(TArray<AActor*> Actors, bool bForce = false);
    //# Jafg guarantees that all listed actors are valid.
    MULTI_EVENT_DECL(OnActorsSelected, TArray<AActor*> const& Old, TArray<AActor*> const& New)
    NODISCARD FORCEINLINE constexpr auto const& GetSelectedActors() const noexcept { return this->SelectedActors; }

private:

    void InitializeRenderTarget();
    bool OnPreDraw(LRenderInfo const& Info);
    void PreDrawSelected(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info);

    void DispatchInputDelegates();

    void CreateMenuDropDown(LVec2F Where);

    void _ctor_SetBackgroundTint();

    WNode* Placeholder{};

    WText* DebugLocationText{};

    algo::clock::time_point LastUnstableDiff;
    std::optional<rhi::extent2> LastUnstableExtent;

    bool bDatedRenderTarget{};
    LRenderTarget RenderTarget;
    LViewport RenderTargetViewport;
    LRaiiPreDrawHandle OnPreDrawHandle{this->GetViewport().GetSurface().OnPreRender};

    LWorld* QueuedTravelWorld{};
    LRaiiViewportHandle QueueHandle{this->GetViewport().OnLateTick};

    //
    // Even though early it says. It is after viewport delegate input dispatching. This is intended.
    // A viewport should always have priority for consumables so that a malicious input context cannot annoy
    // the user.
    //
    LRaiiViewportHandle ConsumeHandle{this->GetViewport().OnEarlyTick};

    TArray<AActor*> SelectedActors;
    LMaterialInstanceRef SelectionMaterialInstance;
    LMaterialInstanceRef PostSelectionMaterialInstance;

    WWorldViewerHierarchy* Hierarchy{};
    WWorldViewerInspector* Inspector{};
};

//#
//# A widget to view a world in a hierarchical manner.
//# Each viewer can have exact one hierarchy widget and each hierarchical widget can only connect to one world viewer.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewerHierarchy : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WWorldViewerHierarchy)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Hierarchy", "Icons/Jafg.Hierarchy")

    virtual void Construct() override;
    virtual void Destruct() override;

    void _OnWorldViewerDestruct();
    void OnWorldViewerUpdate();

private:

    WWorldViewer* FindSmart() const noexcept;
    WWorldViewer* FindInViewport(LViewport const& Viewport) const noexcept;
    WWorldViewer* FindInNode(WNode& Node) const noexcept;

    static constexpr LNodeSize2 TypeSize{128_spt, 0.0f};
    static constexpr LWhitespace ListPadding{20_spt, 0.0f};

    WWorldViewer* WorldViewer{};
    LDelegateHandle OnActorsSelectedHandle;
    void DisconnectFromViewer();
    bool OnActorsSelected(TArray<AActor*> const& Old, TArray<AActor*> const& New);

    WText* ConnectedText{};
    void UpdateConnectedArea();

    WParent* Container{};
    void* LastContainerElemSelected{};
    void UpdateWorldObjectList(TArray<AActor*> const& Old, TArray<AActor*> const& New);
    WTextBox* SelectedWorldObjectText{};
    void UpdateSelectedWorldObjectText(std::size_t Count, std::size_t Shown, std::size_t Selected);

    LNodeReply OnWorldObjectListKeyEventFocus(Detail::WWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event);
    LNodeReply OnWorldObjectListKeyEventUnfocus(Detail::WWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event);

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
//# Just like #WWorldViewerHierarchy this widget thas a one-to-one relation to the world viewer.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewerInspector : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WWorldViewerInspector)
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

    WWorldViewer* FindSmart() const noexcept;
    WWorldViewer* FindInViewport(LViewport const& Viewport) const noexcept;
    WWorldViewer* FindInNode(WNode& Node) const noexcept;

    WWorldViewer* WorldViewer{};
    LDelegateHandle OnActorsSelectedHandle;
    void DisconnectFromViewer();
    bool OnActorsSelected(TArray<AActor*> const& Old, TArray<AActor*> const& New);

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

struct LFactoryWorldViewerHierarchyObjectHButton;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryWorldViewerHierarchyObjectHButton)
class WWorldViewerHierarchyObjectHButton : public WHButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WWorldViewerHierarchyObjectHButton)

public:

    AActor* Actor{};
};

struct LFactoryWorldViewerHierarchyObjectHButton : NODE_FACTORY_PARENT(WWorldViewerHierarchyObjectHButton)
{
    NODE_FACTORY_BODY(WWorldViewerHierarchyObjectHButton)

    constexpr decltype(auto) Actor(this auto&& Self, AActor* Actor) noexcept
    {
        NODE_FACTORY_SELF().Actor = Actor;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Detail */

} /* ~Namespace Jafg */
