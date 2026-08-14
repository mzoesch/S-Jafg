// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Nodes/Text.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();
    this->RenderTargetViewport.Vk_OnLateInit();
    BeginStyling(*this).StaticRoot<WText>().SaveTo(&this->Placeholder)
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::Fill)
        .TextScale(ETextScale::Header)
        .TextAlign(ETextHAlign::Center)
        .TextAlign(ETextVAlign::Center)
        .Content("No World Loaded")
        ;
}

void Jafg::WWorldViewer::Tick()
{
    Super::Tick();

    auto& Surface{this->GetViewport().GetSurface()};

    if (this->bDatedRenderTarget)
    {
        LOG_VERBOSE(LogWidgets, "[{}]: Render target is dated. Recreating render target.", this->GetNameAsString())
        Surface.GetMutableFrontend()._Vk_WaitIdle();
        this->InitializeRenderTarget();
    }
    else if (this->DesiredViewportExtent)
    {
        if (*this->DesiredViewportExtent != this->RenderTarget.GetExtent())
        {
            LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), *this->DesiredViewportExtent)
            Surface.GetMutableFrontend()._Vk_WaitIdle();
            this->InitializeRenderTarget();
        }
        this->LastUnstableExtent.reset();
    }
    else if (rhi::extent2 UnstableSize{rhi::extent2::from_vec(this->GetAnchoredSize_v2())}; UnstableSize.width > 0 && UnstableSize.height > 0)
    {
        if (!this->RenderTarget.IsInitialized())
        {
            this->InitializeRenderTarget();
        }
        else
        {
            constexpr f32 Threshold{0.2f};
            if (this->RenderTarget.GetExtent() == UnstableSize)
            {
                this->LastUnstableExtent.reset();
            }
            else if (!this->LastUnstableExtent || *this->LastUnstableExtent != UnstableSize)
            {
                this->LastUnstableDiff = algo::now();
                this->LastUnstableExtent = UnstableSize;
            }
            else if (algo::time_diff(this->LastUnstableDiff, algo::now()) > Threshold)
            {
                LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), UnstableSize)
                Surface.GetMutableFrontend()._Vk_WaitIdle();
                this->InitializeRenderTarget();
                this->LastUnstableExtent.reset();
            }
        }
    }

    if (this->UserInput._bCurrentlyConsuming && !this->ConsumeHandle.IsValid())
    {
        this->BindConsumeHandle();
        check(this->ConsumeHandle.IsValid())
    }

    this->RenderTargetViewport.Tick();
}

void Jafg::WWorldViewer::Draw(LNodeRenderInfo const& Info) const
{
    if (this->RenderTarget.IsInitialized())
    {
        check(!!*this->RenderTarget.GetMsaa().Image)
        if (!this->RenderTarget.IsResolvedBindless())
        {
            this->RenderTarget._SetResolvedBindlessIndex(
                Info.Frontend.GetSubsystemChecked<JTextureSubsystem>()->_Vk_AddTransientImageToGlobalBindlessArray(this->RenderTarget.GetResolved().ImageView)
                .value_or(INDEX_NONE)
                );
            if (!this->RenderTarget.IsResolvedBindless())
            {
                LOG_FATAL(LogVulkan, "[{}]: Failed to make render target texture bindless. Out of binding points."
                    , this->GetNameAsString())
            }
        }

        LRect2F Rect;
        if (this->IsManual())
        {
            Rect = {.offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
                    .extent=this->GetAnchoredSize_v2()};
        }
        else
        {
            Rect = {.offset=maths::round(this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)),
                    .extent=this->RenderTarget.GetExtent().ToVec<f32>()};
        }

        Info.AddInstance({
            .Rect = Rect,
            .TexCoordRect = {rhi::uv::fit(rhi::uv::identity
                , this->RenderTarget.GetExtent().ToVec<f32>(), this->GetAnchoredSize_v2()
                , rhi::tex_coord_behavior::FitAspect)},
            .Tint = Colors::White,
            .BorderTint = this->BorderTint,
            .TextureIndex = this->RenderTarget.GetResolvedBindlessIndex(),
            .SamplerIndex = UBO::Bindless::LinearClampToBorderSamplerIdx,
            .Flags = SSBO::EVisualInstanceFlagBits::IgnoreAlpha,
            });
    }

    Super::Draw(Info);
}

void Jafg::WWorldViewer::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->ConsumeHandle.IsValid())
    {
        LOG_TRACE(LogWidgetFramework, "[{}]: Escaping user input capture due to focus loss.", this->GetNameAsString())
        check(this->UserInput._bCurrentlyConsuming)
        this->UserInput._bCurrentlyConsuming = false;
        this->ConsumeHandle.Unbind();
        check(!this->ConsumeHandle.IsValid())
        if (this->GetViewport().GetSurface()._GetNativeHandleDangerous())
        {
            this->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
        }
    }
    check(!this->ConsumeHandle.IsValid())
}

void Jafg::WWorldViewer::TravelTo(LWorld& World)
{
    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        if (&Ctrl->GetWorld() == &World)
        {
            LOG_VERBOSE(LogWorld, "[{}]: Already in world [{}]. Rejecting travel request."
                , this->GetNameAsString(), World.GetDetailedHumanReadableName())
            return;
        }

        this->Disconnect();
        check(!this->IsOwnedPersonaControllerValid())
    }

    LOG_VERBOSE(LogWorld, "[{}]: Traveling to world [{}]."
        , this->GetNameAsString(), World.GetDetailedHumanReadableName())

    if (auto Result{World.Login({this->GetTransientPersona()})})
    {
        check(this->IsOwnedPersonaControllerValid())
    }
    else
    {
        LOG_FATAL(LogWidgetFramework, "[{}]: Failed to login to world [{}]. Reason: {}"
            , this->GetNameAsString() , World.GetDetailedHumanReadableName(), Result.error())
    }

    check(this->Placeholder)
    this->Placeholder->SetVisibility(ENodeVisibility::Collapsed);

    this->OnConnect();
}

void Jafg::WWorldViewer::Disconnect()
{
    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        LOG_VERBOSE(LogWidgetFramework, "[{}]: Disconnecting from world [{}]."
            , this->GetNameAsString(), Ctrl->GetWorld().GetHumanReadableName())

        this->OnDisconnect();

        if (!this->GetOwnedPersonaController())
        {
            check(this->Placeholder)
            this->Placeholder->SetVisibility(ENodeVisibility::Visible);
        }
    }
}

void Jafg::WWorldViewer::OnPerspectiveDepthTestChanged()
{
    if (this->RenderTarget.IsInitialized())
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};
        LOG_VERBOSE(LogWidgets, "Perspective depth test changed to [{}].", *Prefs.EditorPerspectiveDepthTestHint)
        this->bDatedRenderTarget = true;
    }
}

void Jafg::WWorldViewer::InitializeRenderTarget()
{
    this->bDatedRenderTarget = false;

    this->RenderTarget.Initialize({
        .Frontend = this->GetViewport().GetSurface().GetFrontend(),
        .Extent = this->DesiredViewportExtent.has_value() ? *this->DesiredViewportExtent : rhi::extent2::from_vec(this->GetAnchoredSize_v2()),
        .SampleCount = this->GetViewport().GetSurface().GetFrontend().Vk_GetMaxMsaaSampleCount(),
        .ResolveMode = vk::ResolveModeFlagBits::eAverage,
        .ClearColor = LinearColors::CornflowerBlue,
        .bDepthTest = *GetSingleton<JUserPreferences>().EditorPerspectiveDepthTestHint,
        .bAllowSelection = true,
        });

    if (!this->OnPreDrawHandle)
    {
        this->OnPreDrawHandle = LRaiiPreDrawHandle::Make(this->GetViewport().GetSurface().OnPreRender,
            std::bind(&WWorldViewer::OnPreDraw, this, std::placeholders::_1));
    }
}

bool Jafg::WWorldViewer::OnPreDrawImpl(LRenderInfo const& Info)
{
    this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    return {};
}

Jafg::LTransientPersona::Local Jafg::WWorldViewer::GetTransientPersona() noexcept
{
    return LTransientPersona::Local{
        .Lackey=*this,
        .bEditor=false,
        };
}

void Jafg::WWorldViewer::OnDisconnect()
{
    this->PossessPersonaController(nullptr);
}

void Jafg::WWorldViewer::BindConsumeHandle()
{
    this->ConsumeHandle = LRaiiViewportHandle::Make(this->GetViewport().OnEarlyTick, [this]
    {
        this->DispatchInputDelegates();
        return false;
    });
}

void Jafg::WWorldViewer::DispatchInputDelegates()
{
    auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
    this->UserInput._DispatchInputDelegates(Ctrl);
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        return this->OnPreDrawImpl(Info);
    }
    return {};
}

void Jafg::WWorldViewer::PreDraw(LRenderInfo const& Info)
{
    if (this->IsOwnedPersonaControllerValid())
    {
        if (auto& Ctrl{*this->GetOwnedPersonaControllerChecked()}; Ctrl.IsOwnedPawnValid())
        {
            auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
            auto Eye{Pawn.GetEye()};
            Pawn.GetWorld().Draw(Info, Eye, nullptr, {}, {});

            this->OnPostWorldDrawImpl(Info, Ctrl, Pawn, Eye);
        }
    }
    this->RenderTargetViewport.Draw(Info);
}

void Jafg::WWorldViewer::_ctor_SetBackgroundTint()
{
    this->BorderTint = *GetSingleton<JUserPreferences>().ViewportBackgroundTint;
}
