// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Nodes/Viewport.h"
#include "Platform/Surface.h"
#include "Framework/Frontend.h"
#include "Nodes/Button.h"
#include "Nodes/Region.h"
#include "Nodes/DropDown.h"
#include "Nodes/HParent.h"
#include "Nodes/TextButton.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Widgets/Input_Vector2.h"
#include "Nodes/HButton.h"

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();

    this->RenderTargetViewport.Vk_OnLateInit();

    // ConstructDeferredWidget(Jafg::TNodeStaticInit<WUserWidget>{this->RenderTargetViewport})
    //     .Style()
    // [
    //     NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::Fill).Tint(Colors::White)
    //     [
    //           // NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Blue).MinDesiredSize(25_spt2)
    //           NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(6_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Red).MinDesiredSize(5_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(4_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Red).MinDesiredSize(3_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::Green).MinDesiredSize(2_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopLeft).Tint(Colors::White).MinDesiredSize(1_spt2)
    //         + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomRight).Tint(Colors::Red).MinDesiredSize(10_spt2)
    //         // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::TopRight).Tint(Colors::Green).MinDesiredSize(25_spt2)
    //         // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomRight).Tint(Colors::Green).MinDesiredSize(25_spt2)
    //         // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::BottomLeft).Tint(Colors::Black).MinDesiredSize(25_spt2)
    //         // + NewNode(this->RenderTargetViewport).Class<WRegion>().Anchor(EAnchor::CenterCenter).Tint(Colors::RebeccaPurple).MinDesiredSize(25_spt2)
    //     ]
    // ];

    BeginStyling(*this).StaticRoot<WHParent>()
        .Padding(3_pt)
    [
        NewStaticNode(WButton)
            .MinDesiredSize(25_spt2)
            .InAllBrushesChained<&LRegionBrush::Tint, &LRegionBrush::Background>
                (Colors::White, LRegionBrush::Icon("Icons/Jafg.Menu"))
            .InAllBrushesChained<&LRegionBrush::Radii>(LVec4F{50.0f})
            .OnKeyDownFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event) -> LNodeReply
            {
                if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                {
                    this->CreateMenuDropDown(Info.CursorLocation.value_or(maths::zero_vector<LVec2F>));
                    return LNodeReply::Handled();
                }
                return {};
            })
    ];

    return;
}

void Jafg::WWorldViewer::Tick()
{
    Super::Tick();

    if (this->DesiredViewportExtent)
    {
        // Just temp of course.
        if (auto& Surface{this->GetViewport().GetSurface()};
            Surface.HasPlatformKeyState(*Surface.GetFrontend().GetPhysicalKey(ELogicalKey::NumPadAdd), ERawInputStateBits::Press))
        {
            this->DesiredViewportExtent = rhi::extent2{this->DesiredViewportExtent->width + 50, this->DesiredViewportExtent->height + 50};
        }
        if (auto& Surface{this->GetViewport().GetSurface()};
            Surface.HasPlatformKeyState(*Surface.GetFrontend().GetPhysicalKey(ELogicalKey::NumPadSubtract), ERawInputStateBits::Press))
        {
            this->DesiredViewportExtent = rhi::extent2{
                maths::max(this->DesiredViewportExtent->width,  51u) - 50,
                maths::max(this->DesiredViewportExtent->height, 51u) - 50,
                };
        }

        if (*this->DesiredViewportExtent != this->RenderTarget.GetExtent())
        {
            LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), *this->DesiredViewportExtent)
            this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
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
                this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
                this->InitializeRenderTarget();
                this->LastUnstableExtent.reset();
            }
        }
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
    if (this->RenderTarget.IsInitialized())
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
            .TexCoordRect = {rhi::uv::fit(rhi::uv::identity
                , this->RenderTarget.GetExtent().ToVec<f32>(), this->GetAnchoredSize_v2()
                , rhi::tex_coord_behavior::FitAspect)},
            .Tint = Colors::White,
            .BorderTint = this->BorderTint,
            .TextureIndex = this->RenderTarget.GetResolvedBindlessIndex(),
            .SamplerIndex = UBO::Bindless::LinearClampToBorderSamplerIdx,
            });
    }

    Super::Draw(Info);

    return;
}

void Jafg::WWorldViewer::InitializeRenderTarget()
{
    this->RenderTarget.Initialize({
        .Frontend = this->GetViewport().GetSurface().GetFrontend(),
        .Extent = this->DesiredViewportExtent.has_value() ? *this->DesiredViewportExtent : rhi::extent2::from_vec(this->GetAnchoredSize_v2()),
        .SampleCount = this->GetViewport().GetSurface().GetFrontend().Vk_GetMaxMsaaSampleCount(),
        .ResolveMode = vk::ResolveModeFlagBits::eAverage,
        });

    if (!this->OnPreDrawHandle)
    {
        this->OnPreDrawHandle = this->GetViewport().GetSurface().OnPreRender.Emplace(this, &WWorldViewer::OnPreDraw);
    }

    return;
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    }
    return {};
}

void Jafg::WWorldViewer::CreateMenuDropDown(LVec2F Where)
{
    struct LSharedNodes
    {
        WText* DisplayText{};
        WInput_Vector2* VectorInput{};
    };
    auto SharedNodes{std::make_shared<LSharedNodes>()};
    auto Res{std::make_shared<LVec2u64>(this->RenderTarget.GetExtent().width, this->RenderTarget.GetExtent().height)};
    CreateDropDownMenu(this->GetViewport(), Where, {}, {.Children={
        LDropDownNodeSeparator{.DisplayName = "VIEWPORT"},
        LDropDownNodeCustom{
            .OnCreate=[this, SharedNodes, Res](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget) -> LFactoryNode
            {
                return NewNode(Viewport).Class<WSpacer>().Width(2_spt)
                + NewNode(Viewport).Class<WButton>()
                    .Anchor(EAnchor::CenterLeft)
                    .MinDesiredSize(16_spt2)
                    .MaxDesiredSize(16_spt2)
                    .InAllBrushes<&LRegionBrush::Tint>(Colors::White)
                    .InBrush<EStyleBits::Normal, &LRegionBrush::BorderTint>(LColor{0x14})
                    .InBrush<EStyleBits::Hover, &LRegionBrush::BorderTint>(LColor{0x1C})
                    .InBrush<EStyleBits::Press, &LRegionBrush::BorderTint>(LColor{0x24})
                    .InBrush<EStyleBits::Selected, &LRegionBrush::BorderTint>(LColor{0x24})
                    .InBrush<EStyleBits::Disabled, &LRegionBrush::BorderTint>(LColor{0x0F})
                    .InAllBrushesChained<&LRegionBrush::OutlineTint, &LRegionBrush::OutlineThickness>(LColor{0x52}, 1)
                    .InAllBrushesChained<&LRegionBrush::Background>(LRegionBrush::LIcon{
                        .Texture=LOptionalTexture2Ref{"Icons/Jafg.Checkmark"}.GetResolved(),
                        .Scale=this->DesiredViewportExtent.has_value() ? 0u : 1u,
                        })
                    .OnKeyUpFocused([this, SharedNodes, Res](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            if (this->DesiredViewportExtent)
                            {
                                this->DesiredViewportExtent.reset();
                            }
                            else
                            {
                                this->DesiredViewportExtent = WWorldViewer::DefaultExtent;
                            }

                            auto& Btn{Self.AsStatic<WButton>()};
                            auto& Icon{std::get<LRegionBrush::LIcon>(Btn.Brush.Background)};
                            Icon.Scale = this->DesiredViewportExtent.has_value() ? 0u : 1u;
                            Btn.Style.SetEverywhere<&LRegionBrush::Background>(Icon);

                            check(SharedNodes->DisplayText && SharedNodes->VectorInput)
                            LVec2u64 ShownVector{this->DesiredViewportExtent.has_value()
                                ? this->DesiredViewportExtent->ToVec<u64>()
                                : this->LastUnstableExtent.has_value()
                                    ? this->LastUnstableExtent->ToVec<u64>()
                                    : this->RenderTargetViewport.GetExtent().ToVec<u64>()};
                            SharedNodes->VectorInput->Set(ShownVector);
                            check(*Res == ShownVector)
                            SharedNodes->VectorInput->SetInputEnabled(this->IsManual());
                            SharedNodes->DisplayText->TextBrush.Tint = this->IsManual()
                                ? Colors::White : Colors::Gray;
                            SharedNodes->DisplayText->GetParentChecked()->AsStatic<LDropDownNodeCustom::Parent>()
                                .SetEnabled(this->DesiredViewportExtent.has_value());

                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewNode(Viewport).Class<WText>() // Should not be a button. but more like WIconizedText
                    .Anchor(EAnchor::HFill)
                    .Visibility(ENodeVisibility::TransitiveHitTestInvisible)
                    .Padding({6_spt, 0.0f, 0.0f, 0.0f})
                    // .InAllBrushesChained<&LBoxBrush::bSkipBrushDraw, &LBoxBrush::Padding>(true, LPadding{6_spt, 0.0f, 0.0f, 0.0f})
                    .Content("Auto Resolve Resolution");
            },
            .OnAction=[this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                {
                    if (this->DesiredViewportExtent)
                    {
                        this->DesiredViewportExtent.reset();
                    }
                    else
                    {
                        this->DesiredViewportExtent = WWorldViewer::DefaultExtent;
                    }
                    return LDropDownNodeCustom::reply::handled(true);
                }
                return LDropDownNodeCustom::reply::unhandled();
            },},
        LDropDownNodeInformation{
            .What = SprintF("Min-Resolution: {}x{}", WWorldViewer::MinViewportExtent.width, WWorldViewer::MinViewportExtent.height),
            },
        LDropDownNodeCustom{
            .OnCreate=[this, SharedNodes, Res](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget) -> LFactoryNode
            {
                return
                    NewNode(Viewport).Class<WText>().SaveTo(&SharedNodes->DisplayText)
                        .Padding({LDropDownMenuCreateInfo::RecommendedPaddedTextPadding, 0.0f, 0.0f, 0.0f})
                        .MinDesiredSize({128_spt, 0})
                        .TextTint(this->IsManual() ? Colors::White : Colors::Gray)
                        .Content("Resolution")
                    + NewNode(Viewport).Class<WInput_Vector2>(*Res).SaveTo(&SharedNodes->VectorInput)
                        .OnVectorChanged([Res](WInput_Vector2& Self)
                        {
                            *Res = Self.Get<u64>();
                        })
                        .Enabled(this->IsManual())
                        .OnDestruct([this, Res](auto&&...)
                        {
                            if (this->IsManual())
                            {
                                if constexpr (IS_COMPILED_LOG(LogWidgets, Verbose))
                                if (Res->x < WWorldViewer::MinViewportExtent.width || Res->y < WWorldViewer::MinViewportExtent.height)
                                {
                                    LOG_VERBOSE(LogWidgets
                                        , "[{}]: Clamping resolution {} to minimum allowed resolution {}."
                                        , this->GetNameAsString(), maths::to_string(*Res)
                                        , maths::to_string(maths::max(*Res, WWorldViewer::MinViewportExtent.ToVec<u64>()))
                                        )
                                }
                                *Res = maths::max(*Res, WWorldViewer::MinViewportExtent.ToVec<u64>());
                                this->DesiredViewportExtent = rhi::extent2::from_vec(*Res);
                            }
                        });
            },
            .IsEnabled = this->IsManual(),
            .OnAction=[](auto&&...){ return LDropDownNodeCustom::reply::unhandled(); }},
        // TODO: Make an option for commonly used resolutions
        LDropDownNodeSeparator{.DisplayName = "WORLD"},
        LDropDownNodeSubmenu{
            .Selector = {
                .DisplayName = "Connect to",
                .Icon = "Icons/Jafg.Sphere",
                },
            .Children = {
                LDropDownNodeOption{
                    .Selector = {
                        .DisplayName = "Localhost",
                        .Icon = "Icons/Jafg.Sphere",
                        },
                    },
                LDropDownNodeOption{
                    .Selector = {
                        .DisplayName = "Localhost",
                        .Icon = "Icons/Jafg.Sphere",
                        },
                    },
                },
            },
        LDropDownNodeSubmenu{
            .Selector = {
                .DisplayName = "Connect to",
                .Icon = "Icons/Jafg.Sphere",
                },
            },
        LDropDownNodeSubmenu{
            .Selector = {
                .DisplayName = "Connect to",
                .Icon = "Icons/Jafg.Sphere",
                },
            },
        },}
        );

    return;
}

void Jafg::WWorldViewer::_ctor_SetBackgroundTint()
{
    this->BorderTint = *GetSingleton<JUserPreferences>().ViewportBackgroundTint;
}
