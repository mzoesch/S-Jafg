// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Framework/Lackey.h"
#include "Nodes/GenericTabInfos.h"
#include "Rhi/RenderTarget.h"
#include "Platform/Surface.h"
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

//# A widget to perspectively view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget, public LLocalLackey
{
    GENERATED_CLASS_BODY()

    friend WWorldViewerHierarchy;

    inline static constexpr rhi::extent2 DefaultExtent{640,480};

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

    inline static constexpr rhi::extent2 MinViewportExtent{128,128};

    virtual ~WWorldViewer() override;

    JAFG_DEFAULT_TAB_CANDIDATE("World Viewer", "Icons/Jafg.Sphere")

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

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

private:

    void InitializeRenderTarget();
    bool OnPreDraw(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info);

    void DispatchInputDelegates();

    void CreateMenuDropDown(LVec2F Where);

    void _ctor_SetBackgroundTint();

    WNode* Placeholder{};

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

    WWorldViewerHierarchy* Hierarchy{};
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

    void OnWorldViewerUpdate();

private:

    WWorldViewer* FindSmart() const noexcept;
    WWorldViewer* FindInViewport(LViewport const& Viewport) const noexcept;
    WWorldViewer* FindInNode(WNode& Node) const noexcept;

    static inline constexpr LNodeSize2 TypeSize{128_spt, 0.0f};
    static inline constexpr LWhitespace ListPadding{20_spt, 0.0f};
    WParent* Container;

    WWorldViewer* WorldViewer{};
    WText* ConnectedText{};
    void UpdateConnectedArea();
    void UpdateWorldObjectList();
    void OnWorldObjectListKeyEventFocus(WHButton& Self, AActor& Actor, LNodeKeyEventInfo const& Info, LKeyEvent const& Event);

    WTextBox* SelectedWorldObjectText{};
    void UpdateSelectedWorldObjectText(std::size_t Count, std::size_t Selected);

    inline LWorld* GetWorld() const noexcept
    {
        if (this->WorldViewer && this->WorldViewer->IsOwnedPersonaControllerValid())
        {
            return &this->WorldViewer->GetOwnedPersonaControllerChecked()->GetWorld();
        }
        return nullptr;
    }
};

} /* ~Namespace Jafg */
