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

//# A widget to view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget, public LLocalLackey
{
    GENERATED_CLASS_BODY()

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
};

} /* ~Namespace Jafg */
