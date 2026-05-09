// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "Rhi/RenderTarget.h"
#include "WorldViewer.generated.h"

namespace Jafg
{

//# A widget to view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WWorldViewer(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
    }
    template<typename TCxxClass>
    explicit WWorldViewer(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
    }

public:

    virtual ~WWorldViewer() override;

    JAFG_DEFAULT_TAB_CANDIDATE("World Viewer", "Icons/Jafg.Sphere")

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

    NODISCARD FORCEINLINE constexpr LRenderTarget const& GetWorldRenderTarget() const noexcept { return this->RenderTarget; }
    NODISCARD FORCEINLINE constexpr LViewport& GetWorldRenderTargetViewport() noexcept { return this->RenderTargetViewport; }
    NODISCARD FORCEINLINE constexpr LViewport const& GetWorldRenderTargetViewport() const noexcept { return this->RenderTargetViewport; }

    LColor BackgroundTint{ Colors::Black };
    rhi::tex_coord_behavior TexCoordBehavior{ rhi::tex_coord_behavior::FitAspect };

    rhi::extent2 DesiredViewportExtent{ 300, 300 };

private:

    void InitializeRenderTarget();
    bool OnPreDraw(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info);

    LRenderTarget RenderTarget;
    LViewport RenderTargetViewport;
    LDelegateHandle OnPreDrawHandle{ nullptr };
};

} /* ~Namespace Jafg */
