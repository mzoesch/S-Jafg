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
        Child->SetMinDesiredSize({});
        Child->SetMaxDesiredSize({});
        continue;
    }

    return;
}

void Jafg::WHDragRegion::UpdateDesiredSize() const
{
    checkCode
    (
        for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
        {
            check(this->GetChildren()[Idx]->GetAnchor() == EAnchor::VFill)
        }
        if (this->GetChildren().empty() == false)
        {
            check(this->GetChildren().back()->GetAnchor() == EAnchor::Fill)
        }
    )

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        check(this->DragChildSlots.contains(Child.get()))
        auto& Slot{this->DragChildSlots.at(Child.get()) };
        check(Slot.Anchor == Child->GetAnchor())
        if (Slot.MinDesiredSize.has_value() && InSpt(this->GetViewport(), *Slot.MinDesiredSize) > InSpt(this->GetViewport(), Child->GetMinDesiredSize()).x)
        {
            Child->SetMinDesiredSize({*Slot.MinDesiredSize, 0.0f});
        }
        /* Querying here for min desired size is correct as the UITick only updates the min desired size. */
        if (Slot.MaxDesiredSize.has_value() && InSpt(this->GetViewport(), *Slot.MaxDesiredSize) < InSpt(this->GetViewport(), Child->GetMinDesiredSize()).x)
        {
            Child->SetMaxDesiredSize({*Slot.MaxDesiredSize, 0.0f});
        }
        else
        {
            Child->SetMaxDesiredSize({});
        }
        continue;
    }

    Super::UpdateDesiredSize();
    return;
}

void Jafg::WHDragRegion::UpdateAnchoredSize(LViewport const& Viewport) const
{
    Super::UpdateAnchoredSize(Viewport);

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
                - InSpt(this->GetViewport(), this->GetHSpace()) * (this->GetChildren().size() - 1)
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
                    auto StateSpt{InSpt(this->GetViewport(), *State)};
                    if (Slot.MinDesiredSize.has_value())
                    {
                        if (auto Min{InSpt(this->GetViewport(), *Slot.MinDesiredSize)}; StateSpt < Min)
                        {
                            StateSpt = Min;
                        }
                    }
                    if (Slot.MaxDesiredSize.has_value())
                    {
                        if (auto Max{InSpt(this->GetViewport(), *Slot.MaxDesiredSize)}; StateSpt > Max)
                        {
                            StateSpt = Max;
                        }
                    }
                    Child->SetMinDesiredSize({EWidgetSize::StaticPoints, StateSpt, 0.0f});
                    Distribution -= StateSpt;
                    Skips[Idx] = true;
                }
                else
                {
                    if (   Slot.MaxDesiredSize.has_value()
                        && InSpt(this->GetViewport(), *Slot.MaxDesiredSize) < (InitialDistribution / static_cast<f32>(this->GetChildren().size())))
                    {
                        Child->SetMinDesiredSize({Slot.MaxDesiredSize->Type, Slot.MaxDesiredSize->Size, 0.0f});
                        Distribution -= InSpt(this->GetViewport(), *Slot.MaxDesiredSize);
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
                        if (auto Min{InSpt(this->GetViewport(), *Slot.MinDesiredSize)}; (Distribution / static_cast<f32>(Clients)) < Min)
                        {
                            Child->SetMinDesiredSize({Slot.MinDesiredSize->Type
                                , Slot.MinDesiredSize->Size
                                , 0.0f
                                });
                            continue;
                        }
                    }
                    Child->SetMinDesiredSize({EWidgetSize::StaticPoints
                        , maths::max(Distribution / static_cast<f32>(Clients), 0.0f)
                        , 0.0f
                        });
                }
                continue;
            }
        }
    }

    return;
}

void Jafg::WHDragRegion::AddChildAt(u64 Index, TJxxUnique<WNode> Child)
{
    check(this->bInitialStateExecuted == false && "It is currently only allowed to add children during construction of this node.")

    check(Child.get())
    check(Child->GetAnchor() == EAnchor::TopLeft && "Expected default anchor as this parent does not allow messing with the anchors.")

    check(Child->GetMinDesiredSize().Y == 0.0f)
    check(Child->GetMaxDesiredSize().Y == 0.0f)

    Child->SetAnchor(EAnchor::VFill);
    this->DragChildSlots[Child.get()] = LChildSlot{Child->GetAnchor()
        , (Child->GetMinDesiredSize().X > 0.0f) ? LWidgetSize1{Child->GetMinDesiredSize().Type, Child->GetMinDesiredSize().X} : TOptional<LWidgetSize1>{}
        , (Child->GetMaxDesiredSize().X > 0.0f) ? LWidgetSize1{Child->GetMaxDesiredSize().Type, Child->GetMaxDesiredSize().X} : TOptional<LWidgetSize1>{}
        };
    Child->SetMinDesiredSize({});
    Child->SetMaxDesiredSize({});

    Super::AddChildAt(Index, std::move(Child));

    for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
    {
        auto& Child{this->GetChildren()[Idx]};
        check(Child.get())
        check(this->DragChildSlots.contains(Child.get()))
        check(this->DragChildSlots.at(Child.get()).Anchor == Child->GetAnchor())
        check(Child->GetAnchor() == EAnchor::VFill || Child->GetAnchor() == EAnchor::Fill)
        Child->SetAnchor(EAnchor::VFill);
        checkCode(this->DragChildSlots.at(Child.get()).Anchor = Child->GetAnchor())
    }

    check(this->GetChildren().empty() == false)
    auto& Back{this->GetChildren().back()};
    check(Back.get())
    check(this->DragChildSlots.contains(Back.get()))
    check(this->DragChildSlots.at(Back.get()).Anchor == Back->GetAnchor())
    check(Back->GetAnchor() == EAnchor::VFill || Back->GetAnchor() == EAnchor::Fill)
    Back->SetAnchor(EAnchor::Fill);
    checkCode(this->DragChildSlots.at(Back.get()).Anchor = Back->GetAnchor())

    return;
}

Jafg::LCursorReply Jafg::WHDragRegion::OnCursorEnter()
{
    this->bEntered = true;
    if (this->UiTickMoveHandle.IsValid())
    {
        return LCursorReply::Handled();
    }
    if (this->OnCursorEnterEvent.IsValid())
    {
        return this->OnCursorEnterEvent.Invoke(*this);
    }
    return {EMouseCursor::ResizeEW};
}

Jafg::LCursorReply Jafg::WHDragRegion::OnCursorLeave()
{
    this->bEntered = false;
    if (this->UiTickMoveHandle.IsValid())
    {
        return LCursorReply::Handled();
    }
    if (this->OnCursorEnterEvent.IsValid())
    {
        return this->OnCursorEnterEvent.Invoke(*this);
    }
    return {EMouseCursor::Default};
}

Jafg::LReply Jafg::WHDragRegion::OnKeyDown(LNodeKeyDownData const& Data, LKeyEvent const& Event)
{
    if (this == &Data.Node)
    {
        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
        {
            check(this->UiTickMoveHandle.IsValid() == false)
            this->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(this, &WHDragRegion::UiTickMove);
            this->InitialMouseLocation = Data.Surface.GetMouseLocation();
            if (this->InitialMouseLocation.has_value())
            {
                this->DragChildOffset = this->CalculateDragChildOffset();
            }
            return LReply::Handled();
        }
    }
    return Super::OnKeyDown(Data, Event);
}

Jafg::LReply Jafg::WHDragRegion::OnKeyUp(LNodeKeyDownData const& Data, LKeyEvent const& Event)
{
    if (this == &Data.Node)
    {
        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
        {
            if (this->UiTickMoveHandle.IsValid())
            {
                Data.Viewport.OnLateTick.Remove(&this->UiTickMoveHandle);
            }
            this->InitialMouseLocation.reset();
            if (this->bEntered == false)
            {
                Data.Surface._SetMouseCursor(EMouseCursor::Default);
            }
            return LReply::Handled();
        }
    }
    return Super::OnKeyUp(Data, Event);
}

TOptional<Jafg::WHDragRegion::LDragChildOffset> Jafg::WHDragRegion::CalculateDragChildOffset()
{
    for (auto Idx{0uz}; Idx < this->GetChildren().size() - 1; ++Idx)
    {
        auto& Child{this->GetChildren()[Idx]};
        if (Child->GetAnchoredAndTranslatedTopLeftFromMostOuter(this->GetViewport()).x >= this->InitialMouseLocation->x)
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

void Jafg::WHDragRegion::UiTickMove()
{
    if (this->InitialMouseLocation.has_value() == false)
    {
        this->InitialMouseLocation = this->GetViewport().GetSurface().GetMouseLocation();
        return;
    }

    if (this->DragChildOffset.has_value() == false)
    {
        this->DragChildOffset = this->CalculateDragChildOffset();
        if (this->DragChildOffset.has_value() == false)
        {
            return;
        }
    }

    if (this->GetViewport().GetSurface().HasMouseLocation() == false)
    {
        return;
    }

    check(this->DragChildOffset->Idx < this->GetChildren().size())
    /* The last child always fills the gap. Therefore, we cannot change its size. */
    if (this->GetChildren().size() - 1 == this->DragChildOffset->Idx)
    {
        return;
    }

    auto& DragChild{this->GetChildren()[this->DragChildOffset->Idx]};
    check(DragChild->GetAnchor() == EAnchor::VFill)
    check(this->DragChildSlots.contains(DragChild.get()))
    auto& Slot{this->DragChildSlots.at(DragChild.get())};
    LVec2F AnchoredSize{this->GetAnchoredSize_v2()};
    f32 HSpaceSpt{InSpt(this->GetViewport(), this->GetHSpace())};

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
        f32 MaxAllowedDesiredSize{InSpt(this->GetViewport(), *Slot.MaxDesiredSize)};
        if (DesiredSize > MaxAllowedDesiredSize)
        {
            DesiredSize = MaxAllowedDesiredSize;
        }
    }
    f32 NewSize{maths::min(MaxSize, DesiredSize)};
    f32 Delta{DesiredSize - NewSize};
    DragChild->SetMinDesiredSize({EWidgetSize::StaticPoints, NewSize, 0.0f});

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
            f32 ChildMinDesiredSize{InSpt(this->GetViewport(), Child->GetMinDesiredSize()).x};
            f32 TrueMinDesiredSize{Slot.MinDesiredSize.has_value() ? InSpt(this->GetViewport(), *Slot.MinDesiredSize) : 0.0f};

            f32 RemovableSpace{ChildMinDesiredSize - TrueMinDesiredSize};
            if (RemovableSpace < 0.0f || maths::eq_zero_e(RemovableSpace))
            {
                continue;
            }
            if (RemovableSpace > Delta)
            {
                Child->SetMinDesiredSize({EWidgetSize::StaticPoints, TrueMinDesiredSize + RemovableSpace - Delta, 0.0f});
                Delta = 0.0f;
                continue;
            }
            Delta -= RemovableSpace;
            Child->SetMinDesiredSize({EWidgetSize::StaticPoints, TrueMinDesiredSize, 0.0f});
            continue;
        }
    }

    return;
}
