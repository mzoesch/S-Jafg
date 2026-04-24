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
        if (Slot.MinDesiredSize.has_value() && InSpt(this->GetViewport(), *Slot.MinDesiredSize) > InSpt(this->GetViewport(), Child->MinDesiredSize).x)
        {
            Child->MinDesiredSize = {*Slot.MinDesiredSize, 0.0f};
        }
        /* Querying here for min desired size is correct as the UITick only updates the min desired size. */
        if (Slot.MaxDesiredSize.has_value() && InSpt(this->GetViewport(), *Slot.MaxDesiredSize) < InSpt(this->GetViewport(), Child->MinDesiredSize).x)
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
                - InSpt(this->GetViewport(), this->HSpace) * (this->GetChildren().size() - 1)
                - this->Padding.GetDesiredSizeXInSpt(this->GetViewport())
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
                    Child->MinDesiredSize = {EWidgetSize::StaticPoints, StateSpt, 0.0f};
                    Distribution -= StateSpt;
                    Skips[Idx] = true;
                }
                else
                {
                    if (   Slot.MaxDesiredSize.has_value()
                        && InSpt(this->GetViewport(), *Slot.MaxDesiredSize) < (InitialDistribution / static_cast<f32>(this->GetChildren().size())))
                    {
                        Child->MinDesiredSize = {Slot.MaxDesiredSize->Type, Slot.MaxDesiredSize->Size, 0.0f};
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
                            Child->MinDesiredSize = {Slot.MinDesiredSize->Type
                                , Slot.MinDesiredSize->Size
                                , 0.0f
                                };
                            continue;
                        }
                    }
                    Child->MinDesiredSize = {EWidgetSize::StaticPoints
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

    check(Child->MinDesiredSize.Y == 0.0f)
    check(Child->MaxDesiredSize.Y == 0.0f)

    Child->Anchor = EAnchor::VFill;
    this->DragChildSlots[Child.get()] = LChildSlot{Child->Anchor
        , (Child->MinDesiredSize.X > 0.0f) ? LWidgetSize1{Child->MinDesiredSize.Type, Child->MinDesiredSize.X} : TOptional<LWidgetSize1>{}
        , (Child->MaxDesiredSize.X > 0.0f) ? LWidgetSize1{Child->MaxDesiredSize.Type, Child->MaxDesiredSize.X} : TOptional<LWidgetSize1>{}
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

Jafg::LReply Jafg::WHDragRegion::OnKeyDown(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (this == &Data.Node && !this->GetChildren().empty())
    {
        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            check(this->UiTickMoveHandle.IsValid() == false)
            this->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(this, &WHDragRegion::UiTickMove);
            this->InitialMouseLocation = Data.Surface.GetMouseLocation();
            if (this->InitialMouseLocation.has_value())
            {
                this->DragChildOffset = this->CalculateDragChildOffset(Data.Translation);
            }
            return LReply::Handled();
        }
    }
    return Super::OnKeyDown(Data, Event);
}

Jafg::LReply Jafg::WHDragRegion::OnKeyUp(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (this == &Data.Node)
    {
        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
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

TOptional<Jafg::WHDragRegion::LDragChildOffset> Jafg::WHDragRegion::CalculateDragChildOffset(LVec2F const& Translation)
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
    LVec2F AnchoredSize{this->GetAnchoredSize_v2() - this->Padding.GetDesiredSizeInSpt(this->GetViewport())};
    f32 HSpaceSpt{InSpt(this->GetViewport(), this->HSpace)};

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
    DragChild->MinDesiredSize = {EWidgetSize::StaticPoints, NewSize, 0.0f};

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
            f32 ChildMinDesiredSize{InSpt(this->GetViewport(), Child->MinDesiredSize).x};
            f32 TrueMinDesiredSize{Slot.MinDesiredSize.has_value() ? InSpt(this->GetViewport(), *Slot.MinDesiredSize) : 0.0f};

            f32 RemovableSpace{ChildMinDesiredSize - TrueMinDesiredSize};
            if (RemovableSpace < 0.0f || maths::eq_zero_e(RemovableSpace))
            {
                continue;
            }
            if (RemovableSpace > Delta)
            {
                Child->MinDesiredSize = {EWidgetSize::StaticPoints, TrueMinDesiredSize + RemovableSpace - Delta, 0.0f};
                Delta = 0.0f;
                continue;
            }
            Delta -= RemovableSpace;
            Child->MinDesiredSize = {EWidgetSize::StaticPoints, TrueMinDesiredSize, 0.0f};
            continue;
        }
    }

    return {};
}
