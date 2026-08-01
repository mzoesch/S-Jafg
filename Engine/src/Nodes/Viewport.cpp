// Copyright mzoesch. All rights reserved.

#include "Nodes/Viewport.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Nodes/UserWidget.h"
#include "Stats/Stats.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"

namespace
{

Jafg::LBufferObjectRegistrator<Jafg::UBO::VisualShared> _0;
Jafg::LBufferObjectRegistrator<Jafg::SSBO::VisualInstance> _1;

} /* ~Namespace <Anonymous> */

void Jafg::LViewport::Vk_OnLateInit()
{
    LOG_VERBOSE(LogVulkan, "Allocating visual batch buffers.")

    auto& Frontend{this->Surface.GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    auto SetInstance{[this](JMaterialSubsystem* Subsystem)
    {
        if (!Subsystem)
        {
            Subsystem = this->GetSurface().GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>();
        }
        this->VisualBatchMaterialInstance = Subsystem->GetInstanceFromMaterialName("Jafg.VisualBatch"sv);
    }};
    if (auto* MaterialSubsystem{this->GetSurface().GetMutableFrontend().GetSubsystem<JMaterialSubsystem>()})
    {
        SetInstance(MaterialSubsystem);
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, [SetInstance]{ SetInstance(nullptr); });
    }
}

void Jafg::LViewport::DispatchInputs()
{
    STAT_CYCLE_FUNCTION()

    if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Verbose))
    if (auto Key{this->Surface.GetFrontend().GetPhysicalKey(ELogicalKey::P)})
    {
        if (this->Surface.HasConsumableKeyState(*Key, ERawInputStateBits::Press))
        {
            this->Surface.ConsumeKey(*Key);
            std::stringstream ss;
            for (auto* Widget : this->TopLevelWidgets)
            {
                auto AppendTree{[](this auto&& Self, std::stringstream& ss, WNode& Node, std::size_t Indent)
                {
                    if (!Node.ShouldNowDraw())
                    {
                        return;
                    }

                    // TODO: Again, translation is wrong...
                    auto Offset{Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)};
                    auto Size{Node.GetAnchoredSize_v2()};
                    auto Desired{Node.GetDesiredSize_v2()};
                    auto Min{Node.MinDesiredSize.InStaticPoints(Node.GetViewport())};
                    auto Max{Node.MaxDesiredSize.InStaticPoints(Node.GetViewport())};

                    ss << std::string(Indent * 2, ' ') << Node.GetNameAsString()
                        << " ("
                            << LexToString(Node.GetVisibility()) << " bTick{" << Node.GetRawShouldTick() << "} anchor-" << Node.Anchor.ToString() << " "
                            << "aabb{" << static_cast<i64>(Offset.x) << "," << static_cast<i64>(Offset.y)
                                << ", " << static_cast<i64>(Size.x) << "," << static_cast<i64>(Size.y) << "}"
                            << " desired{" << static_cast<i64>(Desired.x) << "," << static_cast<i64>(Desired.y) << "}"
                            << " min{" << static_cast<i64>(Min.x) << "," << static_cast<i64>(Min.y) << "}"
                            << " max{" << static_cast<i64>(Max.x) << "," << static_cast<i64>(Max.y) << "}"
                        << ")\n"
                        ;

                    if (auto* Parent{Node.As<WParent>()})
                    {
                        for (auto& Child : Parent->GetChildren())
                        {
                            Self(ss, *Child, Indent + 1);
                        }
                    }

                    return;
                }};
                check(Widget)
                AppendTree(ss, *Widget, 0);
            }
            LOG_VERBOSE(LogWidgetFramework, "Total number of top level widgets: {}. Tree:\n{}", this->TopLevelWidgets.size(), ss.str())
            checkCode(this->_check_PrintTickTrace = true)
        }
    }

    std::optional<LVec2F> CursorLocation;
    if (this->Surface.HasMouseLocationForOrtho())
    {
        CursorLocation = this->Surface.GetMouseLocationValue();
    }

    for (auto It{this->KeyDelegates.begin()}; It != this->KeyDelegates.end();)
    {
        auto Key{this->Surface.GetConsumableKey(It->Key)};
        if (!Key || !!(Key->State & It->Flags))
        {
            if (It->G)
            {
                It->G();
            }
            this->Surface.ConsumeWeakKey(It->Key);
            It = this->KeyDelegates.erase(It);
            continue;
        }

        if (It->F && It->F(*Key))
        {
            if (It->G)
            {
                It->G();
            }
            this->Surface.ConsumeWeakKey(It->Key);
            It = this->KeyDelegates.erase(It);
            continue;
        }

        this->Surface.ConsumeWeakKey(It->Key);
        ++It;
        continue;
    }

    if (CursorLocation.has_value())
    {
        if (this->Surface.HasConsumableKeyState(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton), ERawInputStateBits::Press)
            || this->Surface.HasConsumableKeyState(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton), ERawInputStateBits::Press))
        {
            bool bHandled{};
            for (auto It{this->TopLevelWidgets.rbegin()}; It != this->TopLevelWidgets.rend(); ++It)
            {
                if (auto Reply{(*It)->SweepFocus({}, *CursorLocation)}; Reply.IsHandled())
                {
                    if (Reply.DoesConsume())
                    {
                        this->Surface.ConsumeWeakKey(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton));
                        this->Surface.ConsumeWeakKey(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton));
                    }
                    this->HandleReply(std::move(Reply));
                    bHandled = true;
                    break;
                }
            }
            if (!bHandled)
            {
                this->HandleReply({TClassStorage<WNode>{}});
            }
        }
    }

    if (this->FocusedWidget)
    {
        check(this->FocusedWidget.IsValidSlow())
        if (!algo::any_of(this->TopLevelWidgets.rbegin(), this->TopLevelWidgets.rend(), [this](WUserWidget const* Widget)
        {
            return Widget->IsNodeInVisiblePath(*this->FocusedWidget.get());
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
        if (auto Reply{(*It)->Sweep({}, CursorLocation)}; Reply.IsHandled())
        {
            check(CursorLocation.has_value())
            this->HandleReply(std::move(Reply));
            break;
        }
    }

    auto HandleEventType{[this, CursorLocation](ERawInputStateFlags Flags)
    {
        for (auto It{this->Surface.GetMutableUnconsumedInputsDangerous().begin()}; It != this->Surface.GetMutableUnconsumedInputsDangerous().end();)
        {
            if (!(It->State & Flags))
            {
                ++It;
                continue;
            }

            if (this->FocusedWidget)
            {
                if (auto Reply{this->FocusedWidget->OnKeyEventFocused({
                    .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                    .FocusedNode = *this->FocusedWidget,
                    .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
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
                    if ((*It2)->ShouldCheckForInputs() && (*It2)->AabbTest({}, *CursorLocation))
                    {
                        if (auto Reply{(*It2)->OnKeyEventUnfocused({
                            .Frontend = this->Surface.GetFrontend(), .Surface = this->Surface, .Viewport = *this,
                            .FocusedNode = this->FocusedWidget ? *this->FocusedWidget : std::optional<std::reference_wrapper<WNode>>{},
                            .CursorLocation = CursorLocation, .Translation = maths::zero_vector<LVec2F>,
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
    }};

    HandleEventType(ERawInputStateBits::Release);
    HandleEventType(ERawInputStateBits::Press|ERawInputStateBits::Repeat);
}

void Jafg::LViewport::Tick()
{
    STAT_CYCLE_FUNCTION()

    this->OnEarlyTick.Broadcast();

    this->DispatchInputs();

    for (WUserWidget* Widget: this->TopLevelWidgets)
    {
        if (Widget->ShouldNowTick())
        {
            Widget->Tick();
        }
    }

    this->OnLateTick.Broadcast();

    checkCode(this->_check_PrintTickTrace = false)
}

void Jafg::LViewport::Draw(LRenderInfo const& Info)
{
    STAT_CYCLE_FUNCTION()

    check(GEngine)
    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};

    this->RecalculateScaleFactor();

    TArray<std::pair<vk::Rect2D, u64>> _Dummy1;
    rhi::object_range<SSBO::VisualInstance> _Dummy2{SSBO::VisualInstance::default_count,SSBO::VisualInstance::default_count};
    LNodeRenderInfo NodeInfo{Info, *this,
        *Frontend.GetSubsystemChecked<JTextureSubsystem>(),
        *Frontend.GetSubsystemChecked<JMaterialSubsystem>(),
        *Frontend.GetSubsystemChecked<JFontSubsystem>(),
        maths::zero_vector<LVec2F>, {},
        _Dummy1, _Dummy2,
        };
    NodeInfo.Batches.reserve(LNodeRenderInfo::MaxBatchCount);
    NodeInfo.BeginNewBatch(vk::Rect2D{.offset = vk::Offset2D{0, 0}, .extent = this->GetExtent(),});

    for (auto const* Widget: this->TopLevelWidgets)
    {
        check(Widget)
        check(IsValidFast(Widget->GetOuter(), Widget))
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
            check(!Widget->ShouldNowDraw())
        }
    }

    if (!NodeInfo.VisualInstances->empty())
    {
        constexpr auto VisualSharedBinding{0uz};
        constexpr auto InstanceBufferBinding{1uz};

        check(this->VisualBatchMaterialInstance.get())
        auto& DsInstance{this->VisualBatchMaterialInstance->Vk_GetUniqueDescriptorSet(0, NodeInfo.Frame)};
        check(DsInstance.Resources.size() == 2)

        auto Dimensions{this->GetExtent().ToVec<f32>()};
        auto VisualSharedWriteInfo{UBO::VisualShared{
                .Proj = glm::orthoRH_ZO(0.0f, Dimensions.x, 0.0f, Dimensions.y, 0.0f, 1.0f),
                .Gamma = *GetSingleton<JUserPreferences>().InterfaceGamma,
                }
            .upload(DsInstance.Resources[VisualSharedBinding].AsBuffer())
            .write_info(*DsInstance.Resources[VisualSharedBinding].AsBuffer())
            };

        auto InstanceBufferWriteInfo{NodeInfo.VisualInstances.upload(DsInstance.Resources[InstanceBufferBinding].AsBuffer())};

        std::array Writes{
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = VisualSharedBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = UBO::VisualShared::descriptor_type(),
                .pBufferInfo = &VisualSharedWriteInfo,
                },
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = InstanceBufferBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = SSBO::VisualInstance::descriptor_type(),
                .pBufferInfo = &InstanceBufferWriteInfo
                },
            };
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

        std::array<vk::DescriptorSet, 2> DescriptorSetsToBind;
        DescriptorSetsToBind[0] = *DsInstance;
        DescriptorSetsToBind[1] = *Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetBindlessTextureArrayDescriptorSet();
        NodeInfo.CommandBuffer.bindDescriptorSets2({
            .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            .layout = *this->VisualBatchMaterialInstance->Material->Pipeline.pipeline_layout,
            .firstSet = 0,
            .descriptorSetCount = DescriptorSetsToBind.size(),
            .pDescriptorSets = DescriptorSetsToBind.data(),
            .dynamicOffsetCount = 0,
            .pDynamicOffsets = nullptr
            });

        NodeInfo.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
        NodeInfo.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->VisualBatchMaterialInstance->Material->Pipeline);

        for (auto Idx{0uz}; Idx < NodeInfo.Batches.size(); ++Idx)
        {
            u64 NextBegin{NodeInfo.VisualInstances->size()};
            if (Idx + 1 < NodeInfo.Batches.size())
            {
                NextBegin = NodeInfo.Batches[Idx + 1].second;
            }
            auto& Batch{NodeInfo.Batches[Idx]};
            NodeInfo.CommandBuffer.setScissor(0, Batch.first);
            NodeInfo.CommandBuffer.draw(
                4, static_cast<u32>(NextBegin - Batch.second),
                0, static_cast<u32>(Batch.second)
                );
        }
    }
}

void Jafg::LViewport::TearDown()
{
    while (!this->TopLevelWidgets.empty())
    {
        this->TopLevelWidgets.back()->MarkAsGarbage_v2();
    }
    check(this->TopLevelWidgets.size() == 0)
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
}

void Jafg::LViewport::_RemoveWidget(WUserWidget* Widget)
{
    check(Widget)
    check(Widget->IsTopLevel())
    algo::erase_once_checked(&this->TopLevelWidgets, Widget);
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
    if (!this->FocusedWidget.IsValidFast())
    {
        this->FocusedWidget.Reset();
    }
    else if (this->FocusedWidget)
    {
        LOG_TRACE(LogWidgetFramework, "Gained focus on [{}].", this->FocusedWidget->GetNameAsString())
        this->FocusedWidget->OnFocusReceived();
    }
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
}
