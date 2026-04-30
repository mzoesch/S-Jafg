// Copyright mzoesch. All rights reserved.

#include "Nodes/Viewport.h"
#include "Engine/Engine.h"
#include "Framework/Eye.h"
#include "Platform/Surface.h"
#include "Nodes/UserWidget.h"
#include "Stats/Stats.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"

void Jafg::LViewport::Vk_OnLateInit()
{
    LOG_VERBOSE(LogVulkan, "Allocating visual batch buffers.")

    auto& Frontend{this->Surface.GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    LOG_TRACE(LogRhi, "Allocating visual batch buffers.")
    for (auto Idx{0uz}; Idx < Frontend.Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        this->VisualBatches[Idx] = Frontend.Vk_CreateMappedBuffer({
            .size = sizeof(LVisualInstance) * LNodeRenderInfo::MaxInstanceCount,
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
        auto& MaterialSubsystem{*this->GetSurface().GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>()};
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
void Jafg::LViewport::DispatchInputs()
{
    STAT_CYCLE_FUNCTION()

    auto& CursorLocation{this->Surface.GetMouseLocation()};

    if (CursorLocation.has_value())
    {
        if (this->Surface.HasConsumableKeyState(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton), ERawInputStateBits::Press))
        {
            bool bHandled{};
            for (auto It{this->TopLevelWidgets.rbegin()}; It != this->TopLevelWidgets.rend(); ++It)
            {
                if (auto Reply{(*It)->SweepFocus({.Translation=maths::zero_vector<LVec2F>}, *CursorLocation)}; Reply.IsHandled())
                {
                    if (Reply.DoesConsume())
                    {
                        this->Surface.ConsumeKey(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton));
                    }
                    this->HandleReply(std::move(Reply));
                    bHandled = true;
                    break;
                }
            }
            if (!bHandled)
            {
                this->Surface.ConsumeKey(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton));
                this->HandleReply({TClassStorage<WNode>{}});
            }
        }
    }

    if (this->FocusedWidget)
    {
        check(this->FocusedWidget.IsValidSlow())
        if (!algo::any_of(this->TopLevelWidgets.rbegin(), this->TopLevelWidgets.rend(), [this](WUserWidget const* Widget)
        {
            return Widget->IsNodeInVisiblePath(this->FocusedWidget.get());
        }))
        {
            this->HandleReply({TClassStorage<WNode>{}});
        }
        else
        {
            this->FocusedWidget->OnFocusTick();
        }
    }

    for (auto It{this->TopLevelWidgets.rbegin()}; It != this->TopLevelWidgets.rend(); ++It)
    {
        checkCode((*It)->_check_StateInvariant())
        if (auto Reply{(*It)->Sweep({.Translation=maths::zero_vector<LVec2F>}, CursorLocation)}; Reply.IsHandled())
        {
            check(CursorLocation.has_value())
            this->HandleReply(std::move(Reply));
            break;
        }
    }

    for (auto It{this->Surface.GetMutableUnconsumedInputsDangerous().begin()}; It != this->Surface.GetMutableUnconsumedInputsDangerous().end();)
    {
        if (!(It->State & ERawInputStateBits::Release))
        {
            ++It;
            continue;
        }

        if (this->FocusedWidget)
        {
            if (auto Reply{this->FocusedWidget->OnKeyUpFocused({
                .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                .FocusedNode = *this->FocusedWidget, .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
                }, *It)};
                Reply.IsHandled())
            {
                if (Reply.DoesConsume())
                {
                    It = this->Surface.ConsumeKey(It);
                    this->HandleReply(std::move(Reply));
                    continue;
                }
                this->HandleReply(std::move(Reply));
            }
        }

        if (CursorLocation.has_value())
        {
            bool bConsumed{};
            for (auto It2{this->TopLevelWidgets.rbegin()}; It2 != this->TopLevelWidgets.rend(); ++It2)
            {
                if ((*It2)->ShouldCheckForInputs() && (*It2)->AabbTest({.Translation=maths::zero_vector<LVec2F>}, *CursorLocation))
                {
                    if (auto Reply{(*It2)->OnKeyUpUnfocused({
                        .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                        .FocusedNode = **It2, .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
                        }, *It)};
                        Reply.IsHandled())
                    {
                        bConsumed = Reply.DoesConsume();

                        if (bConsumed)
                        {
                            It = this->Surface.ConsumeKey(It);
                        }
                        this->HandleReply(std::move(Reply));
                        if (bConsumed)
                        {
                            break;
                        }
                    }
                }
                continue;
            }

            if (bConsumed)
            {
                continue;
            }
        }

        ++It;

        continue;
    }

    for (auto It{this->Surface.GetMutableUnconsumedInputsDangerous().begin()}; It != this->Surface.GetMutableUnconsumedInputsDangerous().end();)
    {
        if (!(It->State & ERawInputStateBits::Press))
        {
            ++It;
            continue;
        }

        if (this->FocusedWidget)
        {
            if (auto Reply{this->FocusedWidget->OnKeyDownFocused({
                .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                .FocusedNode = *this->FocusedWidget, .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
                }, *It)};
                Reply.IsHandled())
            {
                if (Reply.DoesConsume())
                {
                    It = this->Surface.ConsumeKey(It);
                    this->HandleReply(std::move(Reply));
                    continue;
                }
                this->HandleReply(std::move(Reply));
            }
        }

        if (CursorLocation.has_value())
        {
            bool bConsumed{};
            for (auto It2{this->TopLevelWidgets.rbegin()}; It2 != this->TopLevelWidgets.rend(); ++It2)
            {
                if ((*It2)->ShouldCheckForInputs() && (*It2)->AabbTest({.Translation=maths::zero_vector<LVec2F>}, *CursorLocation))
                {
                    if (auto Reply{(*It2)->OnKeyDownUnfocused({
                        .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                        .FocusedNode = **It2, .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
                        }, *It)};
                        Reply.IsHandled())
                    {
                        bConsumed = Reply.DoesConsume();
                        if (bConsumed)
                        {
                            It = this->Surface.ConsumeKey(It);
                        }
                        this->HandleReply(std::move(Reply));
                        if (bConsumed)
                        {
                            break;
                        }
                    }
                    continue;
                }
            }

            if (bConsumed)
            {
                continue;
            }
        }

        ++It;

        continue;
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
    }

    this->OnLateTick.Broadcast();

    return;
}

void Jafg::LViewport::Draw(LRenderInfo const& Info)
{
    STAT_CYCLE_FUNCTION()

    check(GEngine)
    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};

    this->RecalculateScaleFactor();

    TArray<std::pair<vk::Rect2D, u64>> _Dummy1;
    TArray<LVisualInstance> _Dummy2;
    LNodeRenderInfo NodeInfo{Info, *this,
        *Frontend.GetSubsystemChecked<JTextureSubsystem>(),
        *Frontend.GetSubsystemChecked<JMaterialSubsystem>(),
        *Frontend.GetSubsystemChecked<JFontSubsystem>(),
        maths::zero_vector<LVec2F>, {},
        _Dummy1, _Dummy2,
        };
    NodeInfo.Batches.reserve(LNodeRenderInfo::MaxBatchCount);
    NodeInfo.VisualInstances.reserve(LNodeRenderInfo::MaxInstanceCount);
    NodeInfo.BeginNewBatch(vk::Rect2D{
        .offset = vk::Offset2D{0, 0},
        .extent = vk::Extent2D{this->GetDimensions().x, this->GetDimensions().y}
        });

    for (auto const* Widget : this->TopLevelWidgets)
    {
        check(Widget)
        check(IsValidFast(&Widget->GetOuter(), Widget))
        if (Widget->TransformsWidgetLayout())
        {
            STAT_QUICK_CYCLE_START(Widget->GetNameAsString())
            Widget->UpdateDesiredSize();
            Widget->UpdateAnchoredSize();
            if (Widget->ShouldNowDraw())
            {
                Widget->Draw(NodeInfo);
            }
        }
        else
        {
            check(Widget->ShouldNowDraw() == false)
        }

        continue;
    }

    check(this->VisualBatches[NodeInfo.Frame].GetData())

    if (NodeInfo.VisualInstances.empty() == false)
    {
        if (JAFG_UNLIKELY(NodeInfo.VisualInstances.size() > LNodeRenderInfo::MaxInstanceCount))
        {
            LOG_FATAL(LogWidgetFramework, "Number of visual instances [{}] exceeds the maximum instance count [{}]."
                , NodeInfo.VisualInstances.size(), LNodeRenderInfo::MaxInstanceCount)
        }

        LMaterialInstance& Instance{*NodeInfo.MaterialSubsystem.GetSharedMaterialInstances().at("Jafg.VisualBatch")};

        auto Dimensions{this->GetDimensionsF()};
        UBO::VisualShared Shared{
            .Proj = glm::orthoRH_ZO(0.0f, Dimensions.x, 0.0f, Dimensions.y, 0.0f, 1.0f),
            .Gamma = *GetSingleton<JUserPreferences>().InterfaceGamma,
            };
        Shared.Upload(this->Vk_VisualSharedBuffers[NodeInfo.Frame]);
        auto WorldDataWriteInfo{Shared.WriteInfo(*this->Vk_VisualSharedBuffers[NodeInfo.Frame])};

        std::memcpy(
              this->VisualBatches[NodeInfo.Frame].GetData()
            , NodeInfo.VisualInstances.data()
            , sizeof(std::remove_cvref_t<decltype(NodeInfo.VisualInstances)>::value_type) * NodeInfo.VisualInstances.size()
            );
        vk::DescriptorBufferInfo BufferInfo{
            .buffer = *this->VisualBatches[NodeInfo.Frame],
            .offset = 0,
            .range = sizeof(std::remove_cvref_t<decltype(NodeInfo.VisualInstances)>::value_type) * NodeInfo.VisualInstances.size()
            };

        std::array Writes{
            vk::WriteDescriptorSet{
                .dstSet = this->Vk_VisualSharedDescriptorSets[NodeInfo.Frame],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .pBufferInfo = &WorldDataWriteInfo,
                },
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

        NodeInfo.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
        NodeInfo.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Instance.Material->Pipeline);

        for (auto Idx{0uz}; Idx < NodeInfo.Batches.size(); ++Idx)
        {
            u64 NextBegin{NodeInfo.VisualInstances.size()};
            if (Idx + 1 < NodeInfo.Batches.size())
            {
                NextBegin = NodeInfo.Batches[Idx + 1].second;
            }
            auto& Batch{NodeInfo.Batches[Idx]};
            NodeInfo.CommandBuffer.setScissor(0, Batch.first);
            NodeInfo.CommandBuffer.draw(
                4, NextBegin - Batch.second,
                0, Batch.second
                );
        }
    }

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

void Jafg::LViewport::ChangeFocusImpl(TClassStorage<WNode> Node)
{
    if (   this->FocusedWidget
        && GEngine->IsClassOuterKnown(this->FocusedWidget.get_outer())
        && this->FocusedWidget.get_outer()->IsHiredHere(this->FocusedWidget.get_unsafe())
        )
    {
        LOG_TRACE(LogWidgetFramework, "Lost focus on [{}].", this->FocusedWidget.get_unsafe()->GetNameAsString())
        this->FocusedWidget.get_unsafe()->OnFocusLost();
    }
    this->FocusedWidget = Node;
    if (this->FocusedWidget)
    {
        LOG_TRACE(LogWidgetFramework, "Gained focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusReceived();
    }

    return;
}

void Jafg::LViewport::HandleReply(LNodeReply&& Reply)
{
    check(Reply.IsHandled())

    if (Reply.GetCursor() != ECursor::None)
    {
        this->Surface._SetMouseCursor(Reply.GetCursor());
    }
    if (Reply.IsFocusValid())
    {
        this->ChangeFocusImpl(Reply.GetFocus());
    }

    return;
}
