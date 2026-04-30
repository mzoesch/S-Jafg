// Copyright mzoesch. All rights reserved.

#include "Nodes/HDragRegion.h"
#include "Nodes/Viewport.h"
#include "Platform/Surface.h"

void Jafg::WHDragRegion::OnSurfaceResize()
{
    Super::OnSurfaceResize();

    this->bInitialStateExecuted = false;
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

void Jafg::WHDragRegion::UpdateDesiredSize() const
{
    checkCode
    (
        if (this->GetChildren().empty() == false)
        {
            for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
            {
                check(this->GetChildren()[Idx]->Anchor == EAnchor::VFill)
            }
            check(this->GetChildren().back()->Anchor == EAnchor::Fill)
        }
    )

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

    Super::UpdateDesiredSize();
    return;
}

void Jafg::WHDragRegion::UpdateAnchoredSize() const
{
    Super::UpdateAnchoredSize();

    if (this->bInitialStateExecuted == false)
    {
        this->bInitialStateExecuted = true;
        if (this->InitialState.empty() == false && this->GetChildren().size() != this->InitialState.size())
        {
            LOG_FATAL(LogWidgets
                , "The initial state size does not match the number of children [{} != {}]."
                , this->InitialState.size(), this->GetChildren().size()
                )
        }
        if (this->GetChildren().empty() == false)
        {
            if (this->InitialState.empty())
            {
                for (auto& _ : this->GetChildren())
                {
                    this->InitialState.emplace_back();
                }
            }
            check(this->GetChildren().size() == this->InitialState.size())

            f32 InitialDistribution{
                  this->GetAnchoredSize_v2().x
                - this->HSpace.InStaticPoints(this->GetViewport()) * (this->GetChildren().size() - 1)
                - this->Padding.GetDesiredSizeX().InStaticPoints(this->GetViewport())
                };
            f32 Distribution{InitialDistribution};
            u32 Clients{};
            TArray<bool> Skips(this->GetChildren().size(), false);

            for (auto Idx{0uz}; Idx < this->GetChildren().size(); ++Idx)
            {
                auto& Child{this->GetChildren()[Idx]};
                check(Child.get())
                check(this->DragChildSlots.contains(Child.get()))
                auto State{this->InitialState[Idx]};
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
                    Child->MinDesiredSize = {ENodeSize::StaticPoints, StateSpt, 0.0f};
                    Distribution -= StateSpt;
                    Skips[Idx] = true;
                }
                else
                {
                    if (   Slot.MaxDesiredSize.has_value()
                        && Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()) < (InitialDistribution / static_cast<f32>(this->GetChildren().size())))
                    {
                        Child->MinDesiredSize = {Slot.MaxDesiredSize->Type, Slot.MaxDesiredSize->Size, 0.0f};
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
                if (Skips[Idx] == false)
                {
                    if (Slot.MinDesiredSize.has_value())
                    {
                        if (auto Min{Slot.MinDesiredSize->InStaticPoints(this->GetViewport())}; (Distribution / static_cast<f32>(Clients)) < Min)
                        {
                            Child->MinDesiredSize = {Slot.MinDesiredSize->Type
                                , Slot.MinDesiredSize->Size
                                , 0.0f
                                };
                            continue;
                        }
                    }
                    Child->MinDesiredSize = {ENodeSize::StaticPoints
                        , maths::max(Distribution / static_cast<f32>(Clients), 0.0f)
                        , 0.0f
                        };
                }
                continue;
            }
        }
    }

    return;
}

Jafg::WNode& Jafg::WHDragRegion::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    check(Child.get())
    check(Child->Anchor == EAnchor::TopLeft && "Expected default anchor as this parent does not allow messing with the anchors.")

    check(Child->MinDesiredSize.Size.y == 0.0f)
    check(Child->MaxDesiredSize.Size.y == 0.0f)

    Child->Anchor = EAnchor::VFill;
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
        check(Child->Anchor == EAnchor::VFill || Child->Anchor == EAnchor::Fill)
        Child->Anchor = EAnchor::VFill;
        checkCode(this->DragChildSlots.at(Child.get()).Anchor = Child->Anchor)
    }

    check(this->GetChildren().empty() == false)
    auto& Back{this->GetChildren().back()};
    check(Back.get())
    check(this->DragChildSlots.contains(Back.get()))
    check(this->DragChildSlots.at(Back.get()).Anchor == Back->Anchor)
    check(Back->Anchor == EAnchor::VFill || Back->Anchor == EAnchor::Fill)
    Back->Anchor = EAnchor::Fill;
    checkCode(this->DragChildSlots.at(Back.get()).Anchor = Back->Anchor)

    return Result;
}

Jafg::LNodeReply Jafg::WHDragRegion::OnCursorEnter()
{
    this->bEntered = true;
    if (!this->UiTickMoveHandle.IsValid() && this->GetChildren().size() > 1)
    {
        this->GetViewport().GetSurface()._SetMouseCursor(ECursor::ResizeEW);
    }
    return Super::OnCursorEnter();
}

Jafg::LNodeReply Jafg::WHDragRegion::OnCursorMoved(LVec2F const& Location)
{
    return Super::OnCursorMoved(Location);
}

void Jafg::WHDragRegion::OnCursorLeave()
{
    Super::OnCursorLeave();
    this->bEntered = false;
    if (!this->UiTickMoveHandle.IsValid())
    {
        this->GetViewport().GetSurface()._SetMouseCursor(ECursor::Default);
    }
    return;
}

Jafg::LNodeReply Jafg::WHDragRegion::OnKeyDownFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (auto Reply{Super::OnKeyDownFocused(Data, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (!this->GetChildren().empty() && Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
    {
        check(this->UiTickMoveHandle.IsValid() == false)
        this->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(this, &WHDragRegion::UiTickMove);
        this->InitialMouseLocation = Data.Surface.GetMouseLocation();
        if (this->InitialMouseLocation.has_value())
        {
            this->DragChildOffset = this->CalculateDragChildOffset(Data.Translation);
        }
        return LNodeReply::Handled();
    }

    return LNodeReply::Unhandled();
}

Jafg::LNodeReply Jafg::WHDragRegion::OnKeyUpFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (auto Reply{Super::OnKeyUpFocused(Data, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
    {
        this->InitialMouseLocation.reset();
        if (this->UiTickMoveHandle.IsValid())
        {
            Data.Viewport.OnLateTick.Remove(&this->UiTickMoveHandle);
            if (!this->bEntered)
            {
                Data.Surface._SetMouseCursor(ECursor::Default);
            }
        }
        return LNodeReply::Handled();
    }

    return LNodeReply::Unhandled();
}

std::optional<Jafg::WHDragRegion::LDragChildOffset> Jafg::WHDragRegion::CalculateDragChildOffset(LVec2F const& Translation)
{
    check(!this->GetChildren().empty())
    for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
    {
        auto& Child{this->GetChildren()[Idx]};
        if (Child->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation).x >= this->InitialMouseLocation->x)
        {
            if (Idx == 0)
            {
                return LDragChildOffset{.Idx = 0 };
            }
            return LDragChildOffset{.Idx = static_cast<u32>(Idx - 1)};
        }
    }
    if (this->GetChildren().size() > 1)
    {
        return LDragChildOffset{.Idx = static_cast<u32>(this->GetChildren().size() - 1 - 1)};
    }
    return {};
}

bool Jafg::WHDragRegion::UiTickMove()
{
    if (this->InitialMouseLocation.has_value() == false)
    {
        this->InitialMouseLocation = this->GetViewport().GetSurface().GetMouseLocation();
        return {};
    }

    if (this->DragChildOffset.has_value() == false)
    {
        // TODO: This is wrong. How do we get the translation here?
        this->DragChildOffset = this->CalculateDragChildOffset(maths::zero_vector<LVec2F>);
        if (this->DragChildOffset.has_value() == false)
        {
            return {};
        }
    }

    if (this->GetViewport().GetSurface().HasMouseLocation() == false)
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
    check(DragChild->Anchor == EAnchor::VFill)
    check(this->DragChildSlots.contains(DragChild.get()))
    auto& Slot{this->DragChildSlots.at(DragChild.get())};
    LVec2F AnchoredSize{this->GetAnchoredSize_v2() - this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport())};
    f32 HSpaceSpt{this->HSpace.InStaticPoints(this->GetViewport())};

    f32 Offset{};
    for (auto Idx{0uz}; Idx < this->DragChildOffset->Idx; ++Idx)
    {
        Offset += this->GetChildren()[Idx]->GetDesiredSize_v2().x + HSpaceSpt;
    }

    f32 Remaining{};
    for (auto Idx{this->DragChildOffset->Idx + 1}; Idx < this->GetChildren().size(); ++Idx)
    {
        Remaining += HSpaceSpt;
        if (Idx == this->GetChildren().size() - 1)
        {
            Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().x;
        }
        else
        {
            Remaining += this->GetChildren()[Idx]->GetDesiredSize_v2().x;
        }
    }

    f32 MaxSize{AnchoredSize.x - Offset - Remaining};
    // TODO: Not optimal. We should calculate the offset of the initial cursor. Now in the first tick the
    //       node always flicks to the middle.
    f32 DesiredSize{this->GetViewport().GetSurface().GetMouseLocationValue().x - Offset - (HSpaceSpt * 0.5f)};
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
    DragChild->MinDesiredSize = {ENodeSize::StaticPoints, NewSize, 0.0f};

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
            f32 ChildMinDesiredSize{Child->MinDesiredSize.InStaticPoints(this->GetViewport()).x};
            f32 TrueMinDesiredSize{Slot.MinDesiredSize.has_value() ? Slot.MinDesiredSize->InStaticPoints(this->GetViewport()) : 0.0f};

            f32 RemovableSpace{ChildMinDesiredSize - TrueMinDesiredSize};
            if (RemovableSpace < 0.0f || maths::eq_zero_e(RemovableSpace))
            {
                continue;
            }
            if (RemovableSpace > Delta)
            {
                Child->MinDesiredSize = {ENodeSize::StaticPoints, TrueMinDesiredSize + RemovableSpace - Delta, 0.0f};
                Delta = 0.0f;
                continue;
            }
            Delta -= RemovableSpace;
            Child->MinDesiredSize = {ENodeSize::StaticPoints, TrueMinDesiredSize, 0.0f};
            continue;
        }
    }

    return {};
}
