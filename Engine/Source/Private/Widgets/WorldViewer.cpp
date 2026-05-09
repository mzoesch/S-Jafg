// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Nodes/Viewport.h"
#include "Platform/Surface.h"
#include "Framework/Frontend.h"
#include "Nodes/Region.h"
#include "Framework/TextureSubsystem.h"
#include "Rhi/NodeRenderInfo.h"

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();

    this->RenderTargetViewport.Vk_OnLateInit();
    this->InitializeRenderTarget();

    check(!this->OnPreDrawHandle.IsValid())
    this->OnPreDrawHandle = this->GetViewport().GetSurface().OnPreRender.Emplace(this, &WWorldViewer::OnPreDraw);

    ConstructDeferredWidget(Jafg::TNodeStaticInit<WUserWidget>{this->RenderTargetViewport})
        .Style()
    [
        NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::Fill).Tint(Colors::White)
        [
              // NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Blue).MinDesiredSize(25_spt2)
              NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(6_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Red).MinDesiredSize(5_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(4_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Red).MinDesiredSize(3_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(2_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::White).MinDesiredSize(1_spt2)
            + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomRight).Tint(Colors::Red).MinDesiredSize(10_spt2)
            // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopRight).Tint(Colors::Green).MinDesiredSize(25_spt2)
            // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomRight).Tint(Colors::Green).MinDesiredSize(25_spt2)
            // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomLeft).Tint(Colors::Black).MinDesiredSize(25_spt2)
            // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::CenterCenter).Tint(Colors::RebeccaPurple).MinDesiredSize(25_spt2)
        ]
    ];

    return;
}

void Jafg::WWorldViewer::Tick()
{
    Super::Tick();

    // Just temp of course.
    if (auto& Surface{this->GetViewport().GetSurface()};
        Surface.HasPlatformKeyState(*Surface.GetFrontend().GetPhysicalKey(ELogicalKey::NumPadAdd), ERawInputStateBits::Press))
    {
        this->DesiredViewportExtent = rhi::extent2{this->DesiredViewportExtent.width + 50, this->DesiredViewportExtent.height + 50};
    }
    if (auto& Surface{this->GetViewport().GetSurface()};
        Surface.HasPlatformKeyState(*Surface.GetFrontend().GetPhysicalKey(ELogicalKey::NumPadSubtract), ERawInputStateBits::Press))
    {
        this->DesiredViewportExtent = rhi::extent2{
            maths::max(this->DesiredViewportExtent.width,  51u) - 50,
            maths::max(this->DesiredViewportExtent.height, 51u) - 50,
            };
    }
    if (this->DesiredViewportExtent != this->RenderTarget.GetExtent())
    {
        LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), this->DesiredViewportExtent)
        this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
        this->InitializeRenderTarget();
    }

    return;
}

void Jafg::WWorldViewer::Destruct()
{
    if (this->OnPreDrawHandle.IsValid())
    {
        this->GetViewport().GetSurface().OnPreRender.Remove(&this->OnPreDrawHandle);
    }
    Super::Destruct();
    return;
}

void Jafg::WWorldViewer::Draw(LNodeRenderInfo const& Info) const
{
    check(!!this->RenderTarget.GetMsaa().Image.GetBuffer())

    if (!this->RenderTarget.IsResolvedBindless())
    {
        this->RenderTarget._SetResolvedBindlessIndex(
            Info.Frontend._VK_AddTransientImageToGlobalBindlessArray(this->RenderTarget.GetResolved().ImageView)
            .value_or(INDEX_NONE)
            );
        if (!this->RenderTarget.IsResolvedBindless())
        {
            LOG_FATAL(LogVulkan, "[{}]: Failed to make render target texture bindless. Out of binding points."
                , this->GetNameAsString())
        }
    }

    Info.AddInstance({
        .Rect = {.Offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
                 .Extent=this->GetAnchoredSize_v2()},
        .Tint = Colors::White,
        .BackgroundTint = this->BackgroundTint,
        .Radii = maths::zero_vector<LVec4F>,
        .OutlineTint = Colors::Black,
        .TexCoordRect = {rhi::uv::fit({0.0f, 0.0f, 1.0f, 1.0f}
            , this->RenderTarget.GetExtent().ToVec<f32>(), this->GetAnchoredSize_v2()
            , this->TexCoordBehavior)},
        .OutlineThickness = 0.0f,
        .TextureIndex = this->RenderTarget.GetResolvedBindlessIndex(),
        .SamplerIndex = UBO::BindlessTextureArray::LinearClampToBorderSamplerIdx,
        .MsdfPixelRange = 0.0f,
        });

    Super::Draw(Info);

    return;
}

void Jafg::WWorldViewer::InitializeRenderTarget()
{
    this->RenderTarget.Initialize({
        .Frontend = this->GetViewport().GetSurface().GetFrontend(),
        .Extent = this->DesiredViewportExtent,
        .SampleCount = this->GetViewport().GetSurface().GetFrontend().Vk_GetMaxMsaaSampleCount(),
        .ResolveMode = vk::ResolveModeFlagBits::eAverage,
        });
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    }
    return {};
}

void Jafg::WWorldViewer::PreDraw(LRenderInfo const& Info)
{
    this->RenderTargetViewport.Draw(Info);
}
