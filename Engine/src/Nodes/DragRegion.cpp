// Copyright mzoesch. All rights reserved.

#include "Nodes/DragRegion.h"
#include "Platform/Surface.h"
#include "User/UserPreferences.h"

void Jafg::WDragRegion::OnSurfaceResize()
{
    Super::OnSurfaceResize();

    this->bUpdateSizes = false;
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        check(this->DragChildSlots.contains(Child.get()))
        Child->MinDesiredSize = {};
        Child->MaxDesiredSize = {};
        continue;
    }

    return;
}

void Jafg::WDragRegion::UpdateDesiredSize() const
{
    check(this->Cf == ENodePrimitiveControlFlow::Horizontal || this->Cf == ENodePrimitiveControlFlow::Vertical)
    checkCode
    (
        if (!this->GetChildren().empty())
        {
            for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
            {
                check(this->GetChildren()[Idx]->Anchor == this->GetChildDesiredAnchor())
            }
            check(this->GetChildren().back()->Anchor == EAnchor::Fill)
        }
    )

    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        for (auto& Child : this->GetChildren())
        {
            check(Child.get())
            check(this->DragChildSlots.contains(Child.get()))
            auto& Slot{this->DragChildSlots.at(Child.get()) };
            check(Slot.Anchor == Child->Anchor)
            if (Slot.MinDesiredSize.has_value() && Slot.MinDesiredSize->InStaticPoints(this->GetViewport()) > Child->MinDesiredSize.InStaticPoints(this->GetViewport()).x)
            {
                Child->MinDesiredSize = {*Slot.MinDesiredSize, 0.0f};
            }
            /* Querying here for min desired size is correct as the UITick only updates the min desired size. */
            if (Slot.MaxDesiredSize.has_value() && Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()) < Child->MinDesiredSize.InStaticPoints(this->GetViewport()).x)
            {
                Child->MaxDesiredSize = {*Slot.MaxDesiredSize, 0.0f};
            }
            else
            {
                Child->MaxDesiredSize= {};
            }
            continue;
        }
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        for (auto& Child : this->GetChildren())
        {
            check(Child.get())
            check(this->DragChildSlots.contains(Child.get()))
            auto& Slot{this->DragChildSlots.at(Child.get()) };
            check(Slot.Anchor == Child->Anchor)
            if (Slot.MinDesiredSize.has_value() && Slot.MinDesiredSize->InStaticPoints(this->GetViewport()) > Child->MinDesiredSize.InStaticPoints(this->GetViewport()).y)
            {
                Child->MinDesiredSize = {0.0f, *Slot.MinDesiredSize};
            }
            /* Querying here for min desired size is correct as the UITick only updates the min desired size. */
            if (Slot.MaxDesiredSize.has_value() && Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()) < Child->MinDesiredSize.InStaticPoints(this->GetViewport()).y)
            {
                Child->MaxDesiredSize = {0.0f, *Slot.MaxDesiredSize};
            }
            else
            {
                Child->MaxDesiredSize= {};
            }
            continue;
        }
    }

    Super::UpdateDesiredSize();
    return;
}


void Jafg::WDragRegion::UpdateAnchoredSize() const
{
    Super::UpdateAnchoredSize();

    if (!this->bUpdateSizes)
    {
        this->bUpdateSizes = true;
        if (!this->GetChildren().empty())
        {
            LInitialDragRegionState InitialState; InitialState.reserve(this->GetChildren().size());
            for (auto& _ : this->GetChildren()) { InitialState.emplace_back(); }
            check(this->GetChildren().size() == InitialState.size())

            f32 InitialDistribution;
            if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
            {
                InitialDistribution = this->GetAnchoredSize_v2().x
                    - this->Space.InStaticPoints(this->GetViewport()) * (this->GetChildren().size() - 1)
                    - this->Padding.GetDesiredSizeX().InStaticPoints(this->GetViewport());
            }
            else
            {
                check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                InitialDistribution = this->GetAnchoredSize_v2().y
                    - this->Space.InStaticPoints(this->GetViewport()) * (this->GetChildren().size() - 1)
                    - this->Padding.GetDesiredSizeY().InStaticPoints(this->GetViewport());
            }

            f32 Distribution{InitialDistribution};
            u32 Clients{};
            std::vector Skips(this->GetChildren().size(), false);

            for (auto Idx{0uz}; Idx < this->GetChildren().size(); ++Idx)
            {
                auto& Child{this->GetChildren()[Idx]};
                check(Child.get())
                check(this->DragChildSlots.contains(Child.get()))
                auto State{InitialState[Idx]};
                auto& Slot{this->DragChildSlots.at(Child.get())};
                if (State.has_value())
                {
                    auto StateSpt{State->InStaticPoints(this->GetViewport())};
                    if (Slot.MinDesiredSize.has_value())
                    {
                        if (auto Min{Slot.MinDesiredSize->InStaticPoints(this->GetViewport())}; StateSpt < Min)
                        {
                            StateSpt = Min;
                        }
                    }
                    if (Slot.MaxDesiredSize.has_value())
                    {
                        if (auto Max{Slot.MaxDesiredSize->InStaticPoints(this->GetViewport())}; StateSpt > Max)
                        {
                            StateSpt = Max;
                        }
                    }
                    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
                    {
                        Child->MinDesiredSize = {ENodeSize::StaticPoints, StateSpt, 0.0f};
                    }
                    else
                    {
                        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                        Child->MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, StateSpt};
                    }
                    Distribution -= StateSpt;
                    Skips[Idx] = true;
                }
                else
                {
                    if (   Slot.MaxDesiredSize.has_value()
                        && Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()) < (InitialDistribution / static_cast<f32>(this->GetChildren().size())))
                    {
                        if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
                        {
                            Child->MinDesiredSize = {Slot.MaxDesiredSize->Type, Slot.MaxDesiredSize->Size, 0.0f};
                        }
                        else
                        {
                            check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                            Child->MinDesiredSize = {Slot.MaxDesiredSize->Type, 0.0f, Slot.MaxDesiredSize->Size};
                        }
                        Distribution -= Slot.MaxDesiredSize->InStaticPoints(this->GetViewport());
                        Skips[Idx] = true;
                    }
                    else
                    {
                        ++Clients;
                    }
                }
                continue;
            }

            for (auto Idx{0uz}; Idx < this->GetChildren().size(); ++Idx)
            {
                auto& Child{this->GetChildren()[Idx]};
                check(Child.get())
                check(this->DragChildSlots.contains(Child.get()))
                auto& Slot{this->DragChildSlots.at(Child.get())};
                if (!Skips[Idx])
                {
                    if (Slot.MinDesiredSize.has_value())
                    {
                        if (auto Min{Slot.MinDesiredSize->InStaticPoints(this->GetViewport())}; (Distribution / static_cast<f32>(Clients)) < Min)
                        {
                            if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
                            {
                                Child->MinDesiredSize = {Slot.MinDesiredSize->Type, Slot.MinDesiredSize->Size, 0.0f};
                            }
                            else
                            {
                                check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                                Child->MinDesiredSize = {Slot.MinDesiredSize->Type, 0.0f, Slot.MinDesiredSize->Size};
                            }
                            continue;
                        }
                    }
                    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
                    {
                        Child->MinDesiredSize = {ENodeSize::StaticPoints, maths::max(Distribution / static_cast<f32>(Clients), 0.0f), 0.0f};
                    }
                    else
                    {
                        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                        Child->MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, maths::max(Distribution / static_cast<f32>(Clients), 0.0f)};
                    }
                }
                continue;
            }
        }
    }

    return;
}

void Jafg::WDragRegion::OnRemoveChildPost(WNode& Child)
{
    Super::OnRemoveChildPost(Child);

    check(this->DragChildSlots.contains(&Child))
    auto const& Slot{this->DragChildSlots.at(&Child)};
    if (Slot.MinDesiredSize.has_value())
    {
        Child.MinDesiredSize = {*Slot.MinDesiredSize, 0.0f};
    }
    else
    {
        Child.MinDesiredSize = {};
    }
    if (Slot.MaxDesiredSize.has_value())
    {
        Child.MaxDesiredSize = {*Slot.MaxDesiredSize, 0.0f};
    }
    else
    {
        Child.MaxDesiredSize = {};
    }

    Child.Anchor = EAnchor::TopLeft;

    this->DragChildSlots.erase(&Child);

    if (!this->GetChildren().empty())
    {
        this->GetChildren().back()->Anchor = EAnchor::Fill;
        check(this->DragChildSlots.contains(this->GetChildren().back().get()))
        checkCode(this->DragChildSlots.at(this->GetChildren().back().get()).Anchor = EAnchor::Fill)
    }

    return;
}

Jafg::WNode& Jafg::WDragRegion::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    check(Child.get())
    check(Child->Anchor == EAnchor::TopLeft && "Expected default anchor as this parent does not allow messing with the anchors.")

    check(Child->MinDesiredSize.Size.y == 0.0f)
    check(Child->MaxDesiredSize.Size.y == 0.0f)

    Child->Anchor = this->GetChildDesiredAnchor();
    this->DragChildSlots[Child.get()] = LChildSlot{Child->Anchor
        , (Child->MinDesiredSize.Size.x > 0.0f) ? LNodeSize1{Child->MinDesiredSize.Type, Child->MinDesiredSize.Size.x} : std::optional<LNodeSize1>{}
        , (Child->MaxDesiredSize.Size.x > 0.0f) ? LNodeSize1{Child->MaxDesiredSize.Type, Child->MaxDesiredSize.Size.x} : std::optional<LNodeSize1>{}
        };
    Child->MinDesiredSize = {};
    Child->MaxDesiredSize = {};

    auto& Result{Super::OnAddChild(Index, std::move(Child), bConstructed)};

    for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
    {
        auto& Child{this->GetChildren()[Idx]};
        check(Child.get())
        check(this->DragChildSlots.contains(Child.get()))
        check(this->DragChildSlots.at(Child.get()).Anchor == Child->Anchor)
        check(Child->Anchor == this->GetChildDesiredAnchor() || Child->Anchor == EAnchor::Fill)
        Child->Anchor = this->GetChildDesiredAnchor();
        checkCode(this->DragChildSlots.at(Child.get()).Anchor = Child->Anchor)
    }

    check(this->GetChildren().empty() == false)
    auto& Back{this->GetChildren().back()};
    check(Back.get())
    check(this->DragChildSlots.contains(Back.get()))
    check(this->DragChildSlots.at(Back.get()).Anchor == Back->Anchor)
    check(Back->Anchor == this->GetChildDesiredAnchor() || Back->Anchor == EAnchor::Fill)
    Back->Anchor = EAnchor::Fill;
    checkCode(this->DragChildSlots.at(Back.get()).Anchor = Back->Anchor)

    this->bUpdateSizes = false;

    return Result;
}

Jafg::LNodeReply Jafg::WDragRegion::OnCursorEnter()
{
    this->bEntered = true;
    if (!this->UiTickMoveHandle.IsValid() && this->GetChildren().size() > 1)
    {
        this->GetViewport().GetSurface()._SetMouseCursor(this->Cf == ENodePrimitiveControlFlow::Horizontal ? ECursor::ResizeEW : ECursor::ResizeNS);
    }
    return Super::OnCursorEnter();
}

void Jafg::WDragRegion::OnCursorLeave()
{
    Super::OnCursorLeave();
    this->bEntered = false;
    if (!this->UiTickMoveHandle.IsValid())
    {
        this->GetViewport().GetSurface()._SetMouseCursor(ECursor::Default);
    }
    return;
}

Jafg::LNodeReply Jafg::WDragRegion::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (auto Reply{Super::OnKeyEventFocused(Info, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (!this->GetChildren().empty() && Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        check(!this->UiTickMoveHandle.IsValid())
        this->UiTickMoveHandle = Info.Viewport.OnLateTick.Emplace(this, &WDragRegion::UiTickMove);
        this->InitialMouseLocation = Info.Surface.GetMouseLocation();
        if (this->InitialMouseLocation.has_value())
        {
            this->DragChildOffset = this->CalculateDragChildOffset(Info.Translation);
        }
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        this->InitialMouseLocation.reset();
        if (this->UiTickMoveHandle.IsValid())
        {
            Info.Viewport.OnLateTick.Remove(&this->UiTickMoveHandle);
            if (!this->bEntered)
            {
                Info.Surface._SetMouseCursor(ECursor::Default);
            }
        }
        return LNodeReply::Handled();
    }

    return LNodeReply::Unhandled();
}

void Jafg::WDragRegion::_ctor_SetSpace()
{
    this->Space = {ENodeSize::StaticPoints, static_cast<f32>(*GetSingleton<JUserPreferences>().PreferredDragPadding)};
}

std::optional<Jafg::WDragRegion::LDragChildOffset> Jafg::WDragRegion::CalculateDragChildOffset(LVec2F const& Translation) const
{
    check(!this->GetChildren().empty())

    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
        {
            auto& Child{this->GetChildren()[Idx]};
            if (Child->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation).x >= this->InitialMouseLocation->x)
            {
                if (Idx == 0)
                {
                    return LDragChildOffset{.Idx = 0};
                }
                return LDragChildOffset{.Idx = static_cast<u32>(Idx - 1),
                    // TODO: Fix translation.
                    .Offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x
                    };
            }
        }
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
        {
            auto& Child{this->GetChildren()[Idx]};
            if (Child->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation).y >= this->InitialMouseLocation->y)
            {
                if (Idx == 0)
                {
                    return LDragChildOffset{.Idx = 0 };
                }
                return LDragChildOffset{.Idx = static_cast<u32>(Idx - 1),
                    .Offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x
                    };
            }
        }
    }

    if (this->GetChildren().size() > 1)
    {
        return LDragChildOffset{
            .Idx = static_cast<u32>(this->GetChildren().size() - 1 - 1),
            .Offset =
                this->Cf == ENodePrimitiveControlFlow::Horizontal ? // TODO: Fix translation.
                     this->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x
                   : this->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y
            };
    }
    return {};
}

bool Jafg::WDragRegion::UiTickMove()
{
    if (!this->InitialMouseLocation.has_value())
    {
        this->InitialMouseLocation = this->GetViewport().GetSurface().GetMouseLocation();
        return {};
    }

    if (!this->DragChildOffset.has_value())
    {
        // TODO: This is wrong. How do we get the translation here?
        this->DragChildOffset = this->CalculateDragChildOffset(maths::zero_vector<LVec2F>);
        if (!this->DragChildOffset.has_value())
        {
            return {};
        }
    }

    if (!this->GetViewport().GetSurface().HasMouseLocation())
    {
        return {};
    }

    check(this->DragChildOffset->Idx < this->GetChildren().size())
    /* The last child always fills the gap. Therefore, we cannot change its size. */
    if (this->GetChildren().size() - 1 == this->DragChildOffset->Idx)
    {
        return {};
    }

    auto& DragChild{this->GetChildren()[this->DragChildOffset->Idx]};
    check(DragChild->Anchor == this->GetChildDesiredAnchor())
    check(this->DragChildSlots.contains(DragChild.get()))
    auto& Slot{this->DragChildSlots.at(DragChild.get())};
    LVec2F AnchoredSize{this->GetAnchoredSize_v2() - this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport())};
    f32 SpaceSpt{this->Space.InStaticPoints(this->GetViewport())};

    f32 Offset{};
    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        for (auto Idx{0uz}; Idx < this->DragChildOffset->Idx; ++Idx)
        {
            Offset += this->GetChildren()[Idx]->GetDesiredSize_v2().x + SpaceSpt;
        }
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        for (auto Idx{0uz}; Idx < this->DragChildOffset->Idx; ++Idx)
        {
            Offset += this->GetChildren()[Idx]->GetDesiredSize_v2().y + SpaceSpt;
        }
    }

    f32 Remaining{};
    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        for (auto Idx{this->DragChildOffset->Idx + 1}; Idx < this->GetChildren().size(); ++Idx)
        {
            Remaining += SpaceSpt;
            if (Idx == this->GetChildren().size() - 1)
            {
                Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().x;
            }
            else
            {
                Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().x;
            }
        }
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        for (auto Idx{this->DragChildOffset->Idx + 1}; Idx < this->GetChildren().size(); ++Idx)
        {
            Remaining += SpaceSpt;
            if (Idx == this->GetChildren().size() - 1)
            {
                Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().y;
            }
            else
            {
                Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().y;
            }
        }
    }

    f32 MaxSize;
    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        MaxSize = {AnchoredSize.x - Offset - Remaining};
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        MaxSize = {AnchoredSize.y - Offset - Remaining};
    }

    // TODO: Not optimal. We should calculate the offset of the initial cursor. Now in the first tick the
    //       node always flicks to the middle.
    f32 DesiredSize;
    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        DesiredSize = {this->GetViewport().GetSurface().GetMouseLocationValue().x - this->DragChildOffset->Offset - Offset - (SpaceSpt * 0.5f)};
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        DesiredSize = {this->GetViewport().GetSurface().GetMouseLocationValue().y - this->DragChildOffset->Offset - Offset - (SpaceSpt * 0.5f)};
    }

    if (Slot.MaxDesiredSize.has_value())
    {
        f32 MaxAllowedDesiredSize{Slot.MaxDesiredSize->InStaticPoints(this->GetViewport())};
        if (DesiredSize > MaxAllowedDesiredSize)
        {
            DesiredSize = MaxAllowedDesiredSize;
        }
    }
    f32 NewSize{maths::min(MaxSize, DesiredSize)};
    f32 Delta{DesiredSize - NewSize};
    if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
    {
        DragChild->MinDesiredSize = {ENodeSize::StaticPoints, NewSize, 0.0f};
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        DragChild->MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, NewSize};
    }

    if (Delta > 0.0f && maths::eq_zero_e(Delta) == false)
    {
        for (auto Idx{this->DragChildOffset->Idx + 1}; Idx < this->GetChildren().size(); ++Idx)
        {
            if (Delta < 0.0f || maths::eq_zero_e(Delta))
            {
                Delta = 0.0f;
                break;
            }
            auto& Child{this->GetChildren()[Idx]};
            check(this->DragChildSlots.contains(Child.get()))
            auto& Slot{this->DragChildSlots.at(Child.get())};
            f32 ChildMinDesiredSize;
            if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
            {
                ChildMinDesiredSize = {Child->MinDesiredSize.InStaticPoints(this->GetViewport()).x};
            }
            else
            {
                check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                ChildMinDesiredSize = {Child->MinDesiredSize.InStaticPoints(this->GetViewport()).y};
            }
            f32 TrueMinDesiredSize{Slot.MinDesiredSize.has_value() ? Slot.MinDesiredSize->InStaticPoints(this->GetViewport()) : 0.0f};

            f32 RemovableSpace{ChildMinDesiredSize - TrueMinDesiredSize};
            if (RemovableSpace < 0.0f || maths::eq_zero_e(RemovableSpace))
            {
                continue;
            }
            if (RemovableSpace > Delta)
            {
                if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
                {
                    Child->MinDesiredSize = {ENodeSize::StaticPoints, TrueMinDesiredSize + RemovableSpace - Delta, 0.0f};
                }
                else
                {
                    check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                    Child->MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, TrueMinDesiredSize + RemovableSpace - Delta};
                }
                Delta = 0.0f;
                continue;
            }
            Delta -= RemovableSpace;
            if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
            {
                Child->MinDesiredSize = {ENodeSize::StaticPoints, TrueMinDesiredSize, 0.0f};
            }
            else
            {
                check(this->Cf == ENodePrimitiveControlFlow::Vertical)
                Child->MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, TrueMinDesiredSize};
            }
            continue;
        }
    }

    return {};
}
