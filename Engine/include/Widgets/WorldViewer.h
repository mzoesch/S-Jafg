// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Framework/Lackey.h"
#include "Nodes/GenericTabInfos.h"
#include "Rhi/RenderTarget.h"
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
        : Super{Init}, LLocalLackey{Init.Outer}
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
    virtual void Destruct() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

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

private:

    void InitializeRenderTarget();
    bool OnPreDraw(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info) { this->RenderTargetViewport.Draw(Info); }

    void CreateMenuDropDown(LVec2F Where);

    void _ctor_SetBackgroundTint();

    algo::clock::time_point LastUnstableDiff;
    std::optional<rhi::extent2> LastUnstableExtent;

    LRenderTarget RenderTarget;
    LViewport RenderTargetViewport;
    LDelegateHandle OnPreDrawHandle{ nullptr };
};

} /* ~Namespace Jafg */
