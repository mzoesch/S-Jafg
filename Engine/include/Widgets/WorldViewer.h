// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Framework/Lackey.h"
#include "Rhi/RenderTarget.h"
#include "Platform/Surface.h"
#include "WorldViewer.generated.h"

namespace Jafg
{

//# A widget to perspectively view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget, public LLocalLackey
{
    GENERATED_CLASS_BODY()

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

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

    virtual void OnFocusLost() override;

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
    void Disconnect();

    void OnPerspectiveDepthTestChanged();

protected:

    virtual void InitializeRenderTarget();
    virtual bool OnPreDrawImpl(LRenderInfo const& Info);
    virtual void OnPostWorldDrawImpl(LRenderInfo const& Info, APersonaController& Ctrl, APawn& Pawn, LWorldEye const& Eye) {}

    virtual LTransientPersona::Local GetTransientPersona() noexcept;
    virtual void OnConnect() {}
    virtual void OnDisconnect();

    LRaiiViewportHandle& GetConsumeHandle() noexcept { return this->ConsumeHandle; }
    void BindConsumeHandle();
    void DispatchInputDelegates();

    NODISCARD constexpr auto GetLastUnstableExtent() const noexcept { return this->LastUnstableExtent; }

private:

    bool OnPreDraw(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info);

    void _ctor_SetBackgroundTint();

    WNode* Placeholder{};

    algo::clock::time_point LastUnstableDiff;
    std::optional<rhi::extent2> LastUnstableExtent;

    bool bDatedRenderTarget{};
    LRenderTarget RenderTarget;
    LViewport RenderTargetViewport;
    LRaiiPreDrawHandle OnPreDrawHandle{this->GetViewport().GetSurface().OnPreRender};

    LRaiiViewportHandle ConsumeHandle{this->GetViewport().OnEarlyTick};
};

} /* ~Namespace Jafg */
