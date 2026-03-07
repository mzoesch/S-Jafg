// Copyright mzoesch. All rights reserved.

#include "Widgets/Viewport.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Platform/Surface.h"
#include "User/Input/Replies.h"
#include "Widgets/UserWidget.h"
#include "Stats/Stats.h"
#include "Framework/MaterialSubsystem.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::LViewport::Vk_OnLateInit()
{
    LOG_VERBOSE(LogVulkan, "Allocating visual batch buffers.")

    auto& Frontend{this->Surface.GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    LOG_TRACE(LogRhi, "Allocating visual batch buffers.")
    for (auto Idx{0uz}; Idx < Frontend.Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        this->VisualBatches[Idx] = Frontend.Vk_CreateMappedBuffer({
            .size = sizeof(LVisualInstance) * this->MaxInstanceCount,
            .usage = vk::BufferUsageFlagBits::eStorageBuffer,
            .sharingMode = vk::SharingMode::eExclusive
            });
    }

    TArray<vk::DescriptorSetLayout> LayoutsToAllocate; LayoutsToAllocate.reserve(Frontend.Vk_GetNumberOfFramesInFlight());
    for (auto Idx{0uz}; Idx < Frontend.Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        LayoutsToAllocate.push_back(*Frontend.Vk_GetDescriptorSetLayouts().at("Jafg.VisualShared"));
    }
    auto Sets = Frontend.Vk_GetDevice().allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        .descriptorPool = Frontend.Vk_GetDescriptorPool(),
        .descriptorSetCount = static_cast<u32>(Frontend.Vk_GetNumberOfFramesInFlight()),
        .pSetLayouts = LayoutsToAllocate.data(),
        });
    for (auto Idx{0uz}; Idx < Sets.size(); ++Idx)
    {
        this->Vk_VisualSharedDescriptorSets[Idx] = std::move(Sets[Idx]);
        this->Vk_VisualSharedBuffers[Idx] = Frontend.Vk_CreateMappedBuffer(UBO::VisualShared::CreateInfo());
    }

    Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, [this]
    {
        check(GEngine)
        auto& MaterialSubsystem{*this->GetSurface().GetFrontend().GetSubsystemChecked<JMaterialSubsystem>()};
        if (auto It{MaterialSubsystem.GetSharedMaterialInstances().find("Jafg.VisualBatch")}; It != MaterialSubsystem.GetSharedMaterialInstances().end())
        {
            this->VisualBatchMaterial = It->second;
        }
        else
        {
            this->VisualBatchMaterial = MaterialSubsystem.GetInstanceFromMaterialName("Jafg.VisualBatch");
            MaterialSubsystem.RegisterSharedMaterialInstance("Jafg.VisualBatch", this->VisualBatchMaterial);
        }
    });

    return;
}

void Jafg::LViewport::ClearInvalidWidgets()
{
    if (this->FocusedWidget.IsValidDeep() == false)
    {
        if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
        {
            if (this->FocusedWidget.IsNotNull())
            {
                LOG_VERBOSE(LogWidgetFramework, "Current focused widget is invalid.")
            }
        }

        this->FocusedWidget.Reset();
    }

    auto ClearOnContainer{[](TArray<TClassStorage<WNode>>* InContainer) -> void
    {
        auto const Removed{ algo::erase_if(InContainer, [](auto const& E)
        {
            return E.IsValidDeep() == false;
        }) };
        if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
        {
            if (Removed > 0)
            {
                LOG_VERBOSE(LogWidgetFramework, "Found [{}] hovered widgets from last frame that are now invalid.", Removed)
            }
        }

        return;
    }};

    ClearOnContainer(&this->LastFrameHoveredWidgets);
    ClearOnContainer(&this->HoveredWidgets);

    return;
}

void Jafg::LViewport::DispatchInputs(LSurface& Surface, TOptional<LVec2F> const& CursorLocation)
{
    check( &this->Surface == &Surface )
    this->SweepTranslation = maths::zero_vector<LVec2F>;

    if (CursorLocation.has_value())
    {
        this->CachedCursorLocation = CursorLocation.value();
    }
    else
    {
        this->CachedCursorLocation.reset();
    }

    this->LastFrameHoveredWidgets = this->HoveredWidgets;
    if (CursorLocation.has_value())
    {
        this->HoveredWidgets.clear();
    }

    LCursorReply SweepReply;

    /* Sweep cursor input over widgets. */
    if (CursorLocation.has_value())
    {
        for (auto It{this->TopLevelWidgets.rbegin()}; It != this->TopLevelWidgets.rend(); ++It)
        {
            if ((*It)->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (LCursorReply Reply{ (*It)->SweepMouse(*this, CursorLocation.value()) }; Reply.IsHandled())
            {
                SweepReply = std::move(Reply);
                break;
            }

            continue;
        }
    }

    /* Check for cursor leave events. */
    if (CursorLocation.has_value())
    {
        LCursorReply MostRecentReply = LCursorReply::Unhandled();
        for (auto& Node : this->LastFrameHoveredWidgets)
        {
            if (algo::contains(this->HoveredWidgets, Node) == false)
            {
                if (const LCursorReply Reply { Node->OnCursorLeave() }; MostRecentReply.IsHandled() == false && Reply.IsHandled())
                {
                    MostRecentReply = Reply;
                }
            }

            continue;
        }
        if (MostRecentReply.IsHandled())
        {
            this->HandleReply(Surface, MostRecentReply);
        }
    }

    /*
     * Handle the sweep reply after the cursor leave events, so that in the case of mutual changes to (e.g., the
     * cursor) are still reflected in the importance. Obviously, the sweep reply is more important than some random
     * fuck widget that was hovered last frame.
     */
    if (SweepReply.IsHandled())
    {
        this->HandleReply(Surface, SweepReply);
    }

    /* Check for left-mouse-button down events to focus on another widget. */
    if (CursorLocation.has_value() && Surface.IsNewKeyDown(EKeys::LeftMouseButton))
    {
        bool bIsHandled { false };
        for (auto& Widget : this->HoveredWidgets)
        {
            if (Widget->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (const LReply Reply{ Widget->SweepFocusTest(*this, CursorLocation.value()) }; Reply.IsHandled())
            {
                this->HandleReply(Surface, Reply);
                bIsHandled = true;
                break;
            }

            continue;
        }
        if (bIsHandled == false)
        {
            this->HandleReply(Surface, LReply::HandledWithFocusLost());
        }
    }

    /* Check if the focused widget is valid to be focused. */
    if (this->FocusedWidget.IsValid())
    {
        bool bIsDrawn { false };
        for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
        {
            if ((*It)->FindNodeInVisiblePath(this->FocusedWidget.Get()))
            {
                bIsDrawn = true;
                break;
            }

            continue;
        }

        if (bIsDrawn == false)
        {
            LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
            this->FocusedWidget->OnFocusLost();
            this->FocusedWidget = nullptr;
        }
    }

    /* Check for key down events. */
    for (const LRawInput& Input : Surface.GetCurrentlyPressedKeys())
    {
        if (Input.bRepeated == false && Surface.IsNewKeyDown(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget.IsNotNull())
        {
            if (const LReply Reply { this->FocusedWidget->OnKeyDown(*this, Input) }; Reply.IsHandled())
            {
                this->HandleReply(Surface, Reply);
                continue;
            }
        }

        if (CursorLocation.has_value())
        {
            for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
            {
                if ((*It) == this->FocusedWidget || (*It)->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if ((*It)->IsInBounds(*this, CursorLocation.value()) == false)
                {
                    continue;
                }

                if (const LReply Reply { (*It)->OnKeyDownNoFocus(*this, Input) }; Reply.IsHandled())
                {
                    this->HandleReply(Surface, Reply);
                    break;
                }

                continue;
            }
        }

        continue;
    }

    /* Check for key up events. */
    for (const LRawInput& Input : Surface.GetLastFramePressedKeys())
    {
        if (Surface.IsKeyUp(Input) == false)
        {
            continue;
        }

        if (this->FocusedWidget.IsNotNull())
        {
            if (const LReply Reply { this->FocusedWidget->OnKeyUp(*this, Input) }; Reply.IsHandled())
            {
                this->HandleReply(Surface, Reply);
                continue;
            }
        }

        if (CursorLocation.has_value())
        {
            for (auto It{ this->TopLevelWidgets.rbegin() }; It != this->TopLevelWidgets.rend(); ++It)
            {
                if ((*It) == this->FocusedWidget || (*It)->ShouldCheckForInputs() == false)
                {
                    continue;
                }

                if ((*It)->IsInBounds(*this, CursorLocation.value()) == false)
                {
                    continue;
                }

                if (const LReply Reply { (*It)->OnKeyUpNoFocus(*this, Input) }; Reply.IsHandled())
                {
                    this->HandleReply(Surface, Reply);
                    break;
                }

                continue;
            }
        }

        continue;
    }

    return;
}

void Jafg::LViewport::OnMouseLeftViewport(LSurface& Context, const bool bInvalidateAllInputs)
{
    check( &this->Surface == &Context )

    if (this->HoveredWidgets.empty() == false || this->LastFrameHoveredWidgets.empty() == false)
    {
        LCursorReply MostRecentReply;
        for (auto& Node : this->HoveredWidgets)
        {
            if (LCursorReply Reply { Node->OnCursorLeave() }; Reply.IsHandled())
            {
                MostRecentReply = std::move(Reply);
            }
        }
        if (MostRecentReply.IsHandled())
        {
            if (MostRecentReply.GetCursorType() != EMouseCursor::None)
            {
                Context._SetMouseCursor(MostRecentReply.GetCursorType());
            }
        }

        algo::orphan(&this->HoveredWidgets);
        algo::orphan(&this->LastFrameHoveredWidgets);
    }

    if (bInvalidateAllInputs && this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusLost();
        this->FocusedWidget = nullptr;
    }

    return;
}

void Jafg::LViewport::Tick()
{
    STAT_CYCLE_FUNCTION()

    for (WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldNowTick())
        {
            Widget->Tick();
        }

        continue;
    }

    this->OnLateTick.Broadcast(*this);

    return;
}

void Jafg::LViewport::Draw(LRenderInfo const& Info)
{
    STAT_CYCLE_FUNCTION()

    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};
    auto& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};

    this->ClearInvalidWidgets();

    this->FrameZLayerDepth = 0.0f;
    this->FrameTranslation = maths::zero_vector<LVec2F>;
    this->RecalculateScaleFactor();

    // this->IntermediateBuffer.MakeDrawTarget();

    const auto Dimensions{ this->GetDimensions() };
    // this->CachedOrthographicProjectionMatrix = Maths::MakeOrthographicProjectionMatrix
    // (
    //     LVector2(static_cast<f32>(Dimensions.X), static_cast<f32>(Dimensions.Y))
    // );

    // RendererStateMachine::PrepareForPerspectivePainting();
    // for (auto const& [Eye, World] : this->PerspectiveViews)
    // {
    //     Eye->UpdateViewMatrix();
    //
    //     // for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
    //     // {
    //     //     checkSlow( Shader && Shader->IsValid() )
    //     //     Shader->UpdateWorldUniforms(*this, *World, *Eye);
    //     //     continue;
    //     // }
    //
    //     // Info.Surface.GetDimensions()
    //
    //     World->Draw(Info, *Eye);
    //
    //     continue;
    // }

    // RendererStateMachine::PrepareForOrthographicPainting();
    // for (LEngineShader* Shader : GEngine->GetShaders() | std::views::values)
    // {
    //     checkSlow( Shader && Shader->IsValid() )
    //     Shader->UpdateViewportUniforms(*this);
    //     continue;
    // }

    LNodeRenderInfo NodeInfo{Info, *this};

    {
        auto Dims{this->GetDimensionsF()};
        UBO::VisualShared Shared{.Proj = glm::orthoRH_ZO(
            0.0f, Dims.x,
            0.0f,Dims.y,
            0.0f, 1.0f
            )};
        Shared.Upload(this->Vk_VisualSharedBuffers[NodeInfo.Frame]);
        auto WorldDataWriteInfo{Shared.WriteInfo(*this->Vk_VisualSharedBuffers[NodeInfo.Frame])};
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = this->Vk_VisualSharedDescriptorSets[NodeInfo.Frame],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &WorldDataWriteInfo,
            }};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }

    for (auto const* Widget : this->TopLevelWidgets)
    {
        if (Widget->TransformsWidgetLayout())
        {
            Widget->UpdateDesiredSize();
            Widget->UpdateAnchoredSize(*this);
            if (Widget->ShouldNowDraw())
            {
                STAT_QUICK_CYCLE_START(Widget->GetNameAsString())
                Widget->Draw(NodeInfo);
            }
        }

        continue;
    }

    check(this->VisualBatches[NodeInfo.Frame].GetData())
    std::memcpy(
          this->VisualBatches[NodeInfo.Frame].GetData()
        , NodeInfo.VisualInstances.data()
        , sizeof(decltype(NodeInfo.VisualInstances)::value_type) * NodeInfo.VisualInstances.size()
        );

    LMaterialInstance& Instance{*MaterialSubsystem.GetSharedMaterialInstances().at("Jafg.VisualBatch")};
    auto& FetchedMaterial{Instance.Material->FetchedMaterial};
    auto& FetchedShader{FetchedMaterial.FetchedShader};
    vk::DescriptorBufferInfo BufferInfo{
        .buffer = *this->VisualBatches[NodeInfo.Frame],
        .offset = 0,
        .range = sizeof(decltype(NodeInfo.VisualInstances)::value_type) * NodeInfo.VisualInstances.size()
        };
    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = *Instance.FrequentDescriptorSets[NodeInfo.Frame].front().second,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eStorageBuffer,
            .pBufferInfo = &BufferInfo
            },
        };
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    std::array<vk::DescriptorSet, 3> DescriptorSetsToBind;
    DescriptorSetsToBind[0] = *this->Vk_VisualSharedDescriptorSets[NodeInfo.Frame];
    DescriptorSetsToBind[1] = *Frontend.Vk_GetBindlessTextureArrayDescriptorSet();
    DescriptorSetsToBind[2] = *Instance.FrequentDescriptorSets[NodeInfo.Frame].front().second;
    NodeInfo.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        .layout = *Instance.Material->Pipeline.Layout,
        .firstSet = 0,
        .descriptorSetCount = DescriptorSetsToBind.size(),
        .pDescriptorSets = DescriptorSetsToBind.data(),
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    NodeInfo.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Instance.Material->Pipeline);
    NodeInfo.CommandBuffer.draw(4, NodeInfo.VisualInstances.size(), 0, 0);

    checkCode
    (
        if (maths::eq_zero_e(this->FrameTranslation) == false)
        {
            LOG_WARNING(LogWidgetFramework, "Viewport translation state is not zero: [{}].", maths::to_string(this->FrameTranslation))
        }
    )

    // LFrameBuffer::MakeDefaultDrawTarget();
    // this->IntermediateBuffer.PaintToViewport(*this);

    return;
}

void Jafg::LViewport::TearDown()
{
    while (this->TopLevelWidgets.empty() == false)
    {
        this->TopLevelWidgets.back()->MarkAsGarbage_v2();
    }
    algo::orphan(&this->TopLevelWidgets);

    return;
}

void Jafg::LViewport::_AddWidget(WUserWidget* Widget)
{
    check(Widget)
    check(Widget->_HasBegunLife() == false)

    // TODO: Check that #Widget is also not in the viewport as a child of some other user widget.
    check(algo::contains(this->TopLevelWidgets, Widget) == false)

    this->TopLevelWidgets.emplace_back(Widget);
    Widget->bIsTopLevel = true;
    MakeCxxObjectFinal(*Widget);

    check(&Widget->GetViewport() == this)

    return;
}

void Jafg::LViewport::_RemoveWidget(WUserWidget* Widget)
{
    check(Widget)
    check(Widget->IsTopLevel())
    algo::erase_once_checked(&this->TopLevelWidgets, Widget);
}

LVec2u32 Jafg::LViewport::GetDimensions() const noexcept
{
    return this->Surface.GetDimensions();
}

// void Jafg::LViewport::ChangeDimensions(const LIntVector2& InDimensions)
// {
//     check( GEngine )
//     check( InDimensions.X > 0 && InDimensions.Y > 0 )
//
//     this->Dimensions = InDimensions;
//
//     if (this->IntermediateBuffer.IsValid())
//     {
//         this->IntermediateBuffer = { };
//     }
//
//     this->IntermediateBuffer.Build(this->GetDimensions());
//
//     return;
// }

Jafg::WNode* Jafg::LViewport::GetTopLevelWidgetByClass(TSubclassOf<WNode> Class) const noexcept
{
    for (auto* Widget : this->TopLevelWidgets)
    {
        if (Widget->IsA(Class))
        {
            return Widget;
        }
        continue;
    }
    return nullptr;
}

bool Jafg::LViewport::FocusWidgetNode(WNode* InNode)
{
    if (InNode == nullptr)
    {
        return false;
    }

    for (const WUserWidget* Widget : this->TopLevelWidgets)
    {
        if (Widget->FindNodeInVisiblePath(InNode))
        {
            this->ChangeFocusUnsafe(InNode);
            return true;
        }

        continue;
    }

    /*
     * The node cannot be focused because it is not visible.
     */
    return false;
}

bool Jafg::LViewport::AddHoveredWidgetForFrame(WNode* Node)
{
    check( algo::contains(this->HoveredWidgets, Node) == false )
    this->HoveredWidgets.emplace_back(Node);
    return algo::contains(this->LastFrameHoveredWidgets, Node) == false;
}

void Jafg::LViewport::ChangeFocusUnsafe(WNode* InNode)
{
    if (this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusLost();
    }

    this->FocusedWidget = InNode;

    if (this->FocusedWidget.IsNotNull())
    {
        LOG_VERBOSE(LogWidgetFramework, "Gained focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusReceived();
    }

    return;
}

void Jafg::LViewport::RecalculateScaleFactor()
{
    this->ScaleFactor = this->PlatformDpi / this->BaseDpi;
}

void Jafg::LViewport::HandleReply(LSurface& Context, const LCursorReply& Reply)
{
    check( Reply.IsHandled() )

    if (Reply.GetCursorType() != EMouseCursor::None)
    {
        Context._SetMouseCursor(Reply.GetCursorType());
    }

    if (Reply.ShouldLooseFocus())
    {
        this->ChangeFocusUnsafe(nullptr);
    }

    if (Reply.IsFocusedWidgetValid())
    {
        if (this->FocusedWidget != Reply.GetFocusedWidget())
        {
            this->ChangeFocusUnsafe(Reply.GetFocusedWidget());
        }
    }

    return;
}

void Jafg::LViewport::HandleReply(LSurface& Context, const LReply& Reply)
{
    check( Reply.IsHandled() )

    if (Reply.ShouldLooseFocus())
    {
        this->ChangeFocusUnsafe(nullptr);
    }

    if (Reply.IsFocusedWidgetValid())
    {
        if (this->FocusedWidget != Reply.GetFocusedWidget())
        {
            this->ChangeFocusUnsafe(Reply.GetFocusedWidget());
        }
    }

    return;
}
