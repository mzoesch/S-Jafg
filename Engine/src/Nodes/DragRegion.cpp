// Copyright mzoesch. All rights reserved.

#include "Nodes/DragRegion.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "User/UserPreferences.h"

void Jafg::WDragRegion::OnSurfaceResize()
{
    Super::OnSurfaceResize();

    this->CacheDists();
    this->bUpdateSizes = true;
    this->bSkipChildDsUpdate = true;

    return;
}

void Jafg::WDragRegion::UpdateDesiredSize() const
{
    check(this->Cf == ENodePrimitiveControlflow::Horizontal || this->Cf == ENodePrimitiveControlflow::Vertical)
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))
    check(algo::all_of(this->GetChildren(), [](auto& E){ check(E.get()) return E->TransformsWidgetLayout(); }))

    if (this->LastAnchoredSize)
    {
        if (this->LastAnchoredSize != this->GetAnchoredSize_v2())
        {
            this->bSkipChildDsUpdate = true;
        }
    }
    this->LastAnchoredSize = this->GetAnchoredSize_v2();

    if (this->bSkipChildDsUpdate)
    {
        this->bSkipChildDsUpdate = false;
        this->SetDesiredSizeInSpt(this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport()));
    }
    else
    {
        for (auto const& [Child, Slot]: this->DragChildSlots)
        {
            check(Child)
            check(Child->MinDesiredSize.Type == ENodeSize::StaticPoints && Child->MaxDesiredSize.Type == ENodeSize::StaticPoints
                && Child->MinDesiredSize.Size == Child->MaxDesiredSize.Size)

            if (Slot.MinDesiredSize)
            {
                Child->MinDesiredSize = *Slot.MinDesiredSize;
            }
            else
            {
                Child->MinDesiredSize = {};
            }

            if (Slot.MaxDesiredSize)
            {
                Child->MaxDesiredSize = *Slot.MaxDesiredSize;
            }
            else
            {
                Child->MaxDesiredSize = {};
            }

            continue;
        }

        Super::UpdateDesiredSize();

        for (auto const& [Child, Slot]: this->DragChildSlots)
        {
            check(Child)
            this->SetSizeForChildNoMinMax(*Child, this->GetSizeForDist(Slot.Dist));
            Child->SetDesiredSizeInSpt(Child->GetDesiredSize_v2());

            continue;
        }
    }

    return;
}

void Jafg::WDragRegion::UpdateAnchoredSize() const
{
    /* Not parent */
    WNode::UpdateAnchoredSize();

    if (this->bUpdateSizes)
    {
        this->bUpdateSizes = false;
        this->NormalizeDists();
    }

    this->ApplyDists(this->GetDistribution());

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateAnchoredSize();
        }
        else
        {
            Child->SetAnchoredSize(maths::zero_vector<LVec2F>);
        }
    }

    return;
}

void Jafg::WDragRegion::OnRemoveChildPost(WNode& Child)
{
    Super::OnRemoveChildPost(Child);

    check(this->DragChildSlots.contains(&Child))
    auto const& Slot{this->DragChildSlots.at(&Child)};
    if (Slot.MinDesiredSize)
    {
        Child.MinDesiredSize = *Slot.MinDesiredSize;
    }
    else
    {
        Child.MinDesiredSize = {};
    }
    if (Slot.MaxDesiredSize)
    {
        Child.MaxDesiredSize = *Slot.MaxDesiredSize;
    }
    else
    {
        Child.MaxDesiredSize = {};
    }

    Child.Anchor = EAnchor::TopLeft;

    this->DragChildSlots.erase(&Child);

    this->CacheDists();
    this->bUpdateSizes = true;

    return;
}

Jafg::WNode& Jafg::WDragRegion::OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed)
{
    check(Child.get())
    check(Child->Anchor == EAnchor::TopLeft && "Expected default anchor as this parent does not allow messing with the anchors.")

    check(Child->MinDesiredSize.Size.y == 0.0f)
    check(Child->MaxDesiredSize.Size.y == 0.0f)

    check(this->Cf == ENodePrimitiveControlflow::Horizontal || this->Cf == ENodePrimitiveControlflow::Vertical)
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    if (this->TransformsWidgetLayout() && this->ProjVec(this->GetAnchoredSize_v2()) > 0.0f && !this->GetChildren().empty())
    {
        this->CacheDists();
    }

    Child->Anchor = this->GetChildDesiredAnchor();

    std::optional<LNodeSize2> MinDesiredSize;
    if (Child->MinDesiredSize.Size.x > 0.0f || Child->MinDesiredSize.Size.y > 0.0f)
    {
        MinDesiredSize = Child->MinDesiredSize;
    }
    std::optional<LNodeSize2> MaxDesiredSize;
    if (Child->MaxDesiredSize.Size.x > 0.0f || Child->MaxDesiredSize.Size.y > 0.0f)
    {
        MaxDesiredSize = Child->MaxDesiredSize;
    }

    this->DragChildSlots[Child.get()] = LChildSlot{1.0f / (this->GetChildren().size() + 1), MinDesiredSize, MaxDesiredSize};
    Child->MinDesiredSize = {};
    Child->MaxDesiredSize = {};

    auto& Result{Super::OnAddChild(Index, std::move(Child), bConstructed)};
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    check(this->DragChildSlots[&Result].Dist != WDragRegion::DistAuto)
    this->bUpdateSizes = true;

    return Result;
}

namespace glm
{
template<>
GLM_FUNC_QUALIFIER std::string to_string<LRect2F>(LRect2F const& x)
{
    return "Offset: " + to_string(x.Offset) + ", Extent: " + to_string(x.Extent);
}

}

Jafg::LNodeReply Jafg::WDragRegion::SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location)
{
    if (Info.bSweepChildren && this->CanChildrenBeHitTestable() && this->IsHitTestable() && this->AabbTest(Info, Location))
    {
        if (this->IsLocationOverDragRect(Info.Translation, Location))
        {
            return Super::SweepFocus({
                .Translation = Info.Translation,
                .ChildTranslationHint = Info.ChildTranslationHint,
                .bSweepChildren = false,
                }, Location);
        }
    }
    return Super::SweepFocus(Info, Location);
}

Jafg::LNodeReply Jafg::WDragRegion::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (Info.bSweepChildren && Location && this->IsHitTestable() && this->AabbTest(Info, *Location))
    {
        if (this->IsLocationOverDragRect(Info.Translation, *Location))
        {
            return Super::Sweep({
                .Translation = Info.Translation,
                .ChildTranslationHint = Info.ChildTranslationHint,
                .bSweepChildren = false,
                }, Location);
        }
    }
    return Super::Sweep(Info, Location);
}

Jafg::LNodeReply Jafg::WDragRegion::OnCursorEnter()
{
    if (this->DragHint)
    {
        this->bEntered = true;
        if (!this->UiTickMoveHandle.IsValid() && this->GetChildren().size() > 1)
        {
            this->GetViewport().GetSurface()._SetMouseCursor(this->Cf == ENodePrimitiveControlflow::Horizontal ? ECursor::ResizeEW : ECursor::ResizeNS);
        }
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
        this->InitialDragLocation = Info.Surface.GetMouseLocation();
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        this->DragHint = nullptr;
        this->InitialDragLocation.reset();
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

void Jafg::WDragRegion::SetCfWithSideEffects(ENodePrimitiveControlflow Controlflow) noexcept
{
    check(Controlflow == ENodePrimitiveControlflow::Horizontal || Controlflow == ENodePrimitiveControlflow::Vertical)
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    this->Cf = Controlflow;
    algo::for_each(this->GetChildren(), [this](auto& Child)
    {
        check(Child.get())
        Child->Anchor = this->GetChildDesiredAnchor();
    });

    check(this->Cf == ENodePrimitiveControlflow::Horizontal || this->Cf == ENodePrimitiveControlflow::Vertical)
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    return;
}

void Jafg::WDragRegion::_ctor_SetSpace()
{
    this->Space = {ENodeSize::StaticPoints, static_cast<f32>(*GetSingleton<JUserPreferences>().PreferredDragPadding)};
}

bool Jafg::WDragRegion::IsLocationOverDragRect(LVec2F Translation, LVec2F Location) const noexcept
{
    auto SpaceSpt{this->Space.InStaticPoints(this->GetViewport())};
    if (!this->GetChildren().empty())
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};

        for (auto It{++this->GetChildren().begin()}; It != this->GetChildren().end(); ++It)
        {
            if (maths::aabb_point({
                .Offset = this->GetAnchoredTopLeftFromMostOuterForChild(**It) - this->ProjFlt(static_cast<f32>(*Prefs.PreferredDragOverlap)) - this->ProjFlt(SpaceSpt),
                .Extent = this->ProjFlt(SpaceSpt) + this->ProjFlt(2.0f * static_cast<f32>(*Prefs.PreferredDragOverlap))
                    + this->ProjFltInv(this->ProjVecInv(this->GetAnchoredSize_v2() - this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport()))),
                }, Location))
            {
                if (!this->DragHint)
                {
                    this->DragHint = &**(It - 1);
                }
                return true;
            }
        }
    }

    if (!this->UiTickMoveHandle.IsValid())
    {
        this->DragHint = nullptr;
    }

    return false;
}

f32 Jafg::WDragRegion::GetDistribution() const
{
    f32 Distribution;
    if (this->Cf == ENodePrimitiveControlflow::Horizontal)
    {
        Distribution = this->GetAnchoredSize_v2().x
            - this->Space.InStaticPoints(this->GetViewport()) * static_cast<f32>((this->GetChildren().size() - 1))
            - this->Padding.GetDesiredSizeX().InStaticPoints(this->GetViewport());
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        Distribution = this->GetAnchoredSize_v2().y
            - this->Space.InStaticPoints(this->GetViewport()) * static_cast<f32>((this->GetChildren().size() - 1))
            - this->Padding.GetDesiredSizeY().InStaticPoints(this->GetViewport());
    }

    return Distribution;
}

void Jafg::WDragRegion::CacheDists() const
{
    check(this->GetChildren().size() == this->DragChildSlots.size())
    if (this->GetChildren().empty())
    {
        return;
    }
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    for (auto& Slot: this->DragChildSlots | std::views::values)
    {
        if (Slot.Dist == WDragRegion::DistAuto)
        {
            Slot.Dist = 1.0f / static_cast<f32>(this->GetChildren().size());
        }
        check(Slot.Dist != WDragRegion::DistAuto)
    }

    if (f32 Distributions{this->GetDistribution()}; Distributions > 0.0f)
    {
        for (auto& [Child, Slot]: this->DragChildSlots)
        {
            check(Child)
            if (auto Proj{this->ProjVec(Child->GetAnchoredSize_v2())}; Proj > 0.0f && Child->TransformsWidgetLayout())
            {
                Slot.Dist = Proj / Distributions;
            }
            check(Slot.Dist != WDragRegion::DistAuto)
            continue;
        }
    }

    return;
}

void Jafg::WDragRegion::NormalizeDists() const
{
    check(this->GetChildren().size() == this->DragChildSlots.size())
    if (this->GetChildren().empty())
    {
        return;
    }
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))
    checkCode
    (
        for (auto& Slot: this->DragChildSlots | std::views::values)
        {
            check(Slot.Dist != WDragRegion::DistAuto)
        }
    )

    constexpr auto MaxTries{3uz};
    auto Tries{0uz};
    do
    {
        if (Tries++ > MaxTries)
        {
            LOG_FATAL(LogWidgetFramework, "[{}]: Clamping dists of all children failed after [{}] tries."
                , this->GetNameAsString(), MaxTries)
        }

        f32 Dist{0.0f};
        check(this->GetChildren().size() == this->DragChildSlots.size())
        for (auto& Slot: this->DragChildSlots | std::views::values)
        {
            check(Slot.Dist >= 0.0f && Slot.Dist <= 1.0f)
            check(Slot.Dist != WDragRegion::DistAuto)
            Dist += Slot.Dist;
        }

        if (maths::eq_e(Dist, 1.0f))
        {
            break;
        }

        if (Dist < 1.0f)
        {
            f32 Missing{1.0f - Dist};
            f32 Factor{Missing / static_cast<f32>(this->GetChildren().size())};
            for (auto& Slot: this->DragChildSlots | std::views::values)
            {
                Slot.Dist += Factor;
            }
            continue;
        }
        if (Dist > 1.0f)
        {
            f32 Extra{Dist - 1.0f};
            f32 Factor{Extra / static_cast<f32>(this->GetChildren().size())};
            for (auto& Slot: this->DragChildSlots | std::views::values)
            {
                Slot.Dist -= Factor;
            }
            continue;
        }

        std::unreachable();
    } while (true);

    checkCode
    (
        f32 _dist{};
        for (auto& Child : this->GetChildren())
        {
            check(Child.get())
            auto& Slot{this->DragChildSlots.at(Child.get())};
            _dist += Slot.Dist;
        }

        if (_dist > 1.0f)
        {
            LOG_WARNING(LogWidgetFramework, "[{}]: Accumulated dist is [{}>1.0f]. This is not allowed.",
                this->GetNameAsString(), _dist)
        }
    )

    return;
}

void Jafg::WDragRegion::ApplyDists(f32 Distribution) const
{
    constexpr f32 MinDist{0.05f};

    check(this->GetChildren().size() == this->DragChildSlots.size())
    for (auto const& [Child, Slot]: this->DragChildSlots)
    {
        check(Child)
        check(Slot.Dist != WDragRegion::DistAuto)

        f32 ChildDistInSpt{Distribution * Slot.Dist};
        if (Slot.MinDesiredSize)
        {
            if (auto Min{this->ProjVec(Slot.MinDesiredSize->InStaticPoints(this->GetViewport()))}; ChildDistInSpt < Min)
            {
                Slot.Dist = Min / Distribution;
                f32 ExtraSpt{Min - ChildDistInSpt};
                f32 ExtraDist{ExtraSpt / Distribution};

                bool bFound{};
                for (auto const& [OtherChild, OtherSlot] : this->DragChildSlots)
                {
                    if (!bFound)
                    {
                        bFound = &OtherChild == &Child;
                        continue;
                    }
                    check(OtherSlot.Dist != WDragRegion::DistAuto)

                    if (OtherSlot.Dist - MinDist > ExtraDist)
                    {
                        OtherSlot.Dist -= ExtraDist;
                        break;
                    }

                    ExtraDist -= OtherSlot.Dist - MinDist;
                    OtherSlot.Dist = MinDist;
                    continue;
                }
                check(bFound)

                if (ExtraDist > 0.0f)
                {
                    LOG_WARNING(LogWidgetFramework, "[{}]: Could not fully apply min desired size of child [{}]. Remaining extra dist: [{}]",
                        this->GetNameAsString(), Child->GetNameAsString(), ExtraDist)
                }
            }
        }
        if (Slot.MaxDesiredSize)
        {
            std::unreachable();
        }

        continue;
    }

    for (auto const& [Child, Slot]: this->DragChildSlots)
    {
        if (this->bUseGrid)
        {
            this->SetSizeForChildNoMinMax(*Child, maths::round(Distribution * Slot.Dist));
        }
        else
        {
            this->SetSizeForChildNoMinMax(*Child, Distribution * Slot.Dist);
        }
    }

    return;
}

bool Jafg::WDragRegion::UiTickMove()
{
    if (!this->InitialDragLocation)
    {
        this->InitialDragLocation = this->GetViewport().GetSurface().GetMouseLocation();
        return {};
    }

    // TODO: This is wrong. How do we get the translation here?
    if (!this->DragHint)
    {
        (void)this->IsLocationOverDragRect(maths::zero_vector<LVec2F>, *this->InitialDragLocation);
    }
    if (!this->DragHint)
    {
        if (!this->GetViewport().GetSurface().HasMouseLocationForOrtho())
        {
            return {};
        }
        (void)this->IsLocationOverDragRect(maths::zero_vector<LVec2F>, this->GetViewport().GetSurface().GetMouseLocationValue());
    }
    if (!this->DragHint)
    {
        return {};
    }

    auto& Surface{this->GetViewport().GetSurface()};
    if (!Surface.HasMouseLocationForOrtho())
    {
        return {};
    }

    check(!this->GetChildren().empty())

    auto Distribution{this->GetDistribution()};
    auto SetDistForChild{[Distribution](LChildSlot const& Slot, f32 SizeInSpt)
    {
        if (Distribution > 0.0f)
        {
            Slot.Dist = SizeInSpt / Distribution;
        }
    }};

    auto& DragChild{*this->DragHint};
    check(DragChild.Anchor == this->GetChildDesiredAnchor())
    check(this->DragChildSlots.contains(&DragChild))
    auto& Slot{this->DragChildSlots.at(&DragChild)};
    check(algo::all_of(this->GetChildren(), [this](auto& E){ check(E.get()) return E->Anchor == this->GetChildDesiredAnchor(); }))

    //# Space between tabs.
    f32 SpaceSpt{this->Space.InStaticPoints(this->GetViewport())};
    //# Space to move to the left or up.
    f32 Offset{};
    for (auto It{this->GetChildren().begin()}; &**It != this->DragHint; ++It)
    {
        check(It != this->GetChildren().end())
        Offset += this->ProjVec((*It)->GetAnchoredSize_v2()) + SpaceSpt;
    }
    //# Space to move to the right or down.
    // f32 Remaining{};
    // for (auto It{this->GetChildren().begin() + algo::distance_to(this->GetChildren(), this->DragHint, algo::unique_raw)}; It != this->GetChildren().end(); ++It)
    // {
    //     Remaining += SpaceSpt + this->ProjVec((*It)->GetDesiredSize_v2());
    // }

    // auto Delta = *this->InitialDragLocation - Surface.GetMouseLocationValue(); // TODO: How??
    f32 DesiredSize = this->ProjVec(Surface.GetMouseLocationValue() - this->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)) // TODO: fix trans.
        - Offset - (SpaceSpt * 0.5f);

    if (DesiredSize < this->ProjVec(DragChild.GetAnchoredSize_v2()))
    {
        f32 PrevSize = this->ProjVec(DragChild.GetAnchoredSize_v2());
        f32 NewSize = this->SetSizeForChild(Slot, DragChild, DesiredSize);
        check(NewSize <= PrevSize)
        auto It{this->GetChildren().end()};
        if (NewSize < PrevSize)
        {
            f32 Delta = PrevSize - NewSize;
            It = this->GetChildren().begin();
            for (;It != this->GetChildren().end(); ++It)
            {
                if (&**It == &DragChild)
                {
                    ++It;
                    break;
                }
            }
            check(It != this->GetChildren().end())
            auto& ItSlot{this->DragChildSlots.at(It->get())};
            f32 DesiredForNeighbor = this->ProjVec((*It)->GetAnchoredSize_v2()) + Delta;
            f32 SetForNeighbor = this->SetSizeForChild(ItSlot, **It, DesiredForNeighbor);
            if (DesiredForNeighbor != SetForNeighbor)
            {
                panic("TODO: This ctrl path is not implemented.")
            }
        }

        auto MainDist{Slot.Dist};
        if (NewSize < PrevSize)
        {
            SetDistForChild(Slot, NewSize);
        }
        if (It != this->GetChildren().end())
        {
            auto MainDistDelta{Slot.Dist - MainDist};
            if (MainDistDelta > 0.0f)
            {
                auto& ItSlot{this->DragChildSlots.at(It->get())};
                ItSlot.Dist -= MainDistDelta;
            }
            if (MainDistDelta < 0.0f)
            {
                auto& ItSlot{this->DragChildSlots.at(It->get())};
                ItSlot.Dist -= MainDistDelta;
            }
        }
    }
    else if (DesiredSize > this->ProjVec(DragChild.GetDesiredSize_v2()))
    {
        f32 PrevSize = this->ProjVec(DragChild.GetAnchoredSize_v2());
        f32 NewSize = this->SetSizeForChild(Slot, DragChild, DesiredSize);
        check(NewSize >= PrevSize)
        auto It{this->GetChildren().end()};
        if (NewSize > PrevSize)
        {
            f32 Delta = NewSize - PrevSize;
            It = this->GetChildren().begin();
            for (;It != this->GetChildren().end(); ++It)
            {
                if (&**It == &DragChild)
                {
                    ++It;
                    break;
                }
            }
            check(It != this->GetChildren().end())
            auto& ItSlot{this->DragChildSlots.at(It->get())};
            f32 DesiredForNeighbor = this->ProjVec((*It)->GetAnchoredSize_v2()) - Delta;
            f32 SetForNeighbor = this->SetSizeForChild(ItSlot, **It, DesiredForNeighbor);
            if (DesiredForNeighbor != SetForNeighbor)
            {
                NewSize = this->SetSizeForChild(Slot, DragChild, this->ProjVec(DragChild.GetAnchoredSize_v2()) - (SetForNeighbor - DesiredForNeighbor));
            }
        }
        auto MainDist{Slot.Dist};
        if (NewSize > PrevSize)
        {
            SetDistForChild(Slot, NewSize);
        }
        if (It != this->GetChildren().end())
        {
            auto MainDistDelta{Slot.Dist - MainDist};
            if (MainDistDelta < 0.0f)
            {
                std::unreachable();
            }
            if (MainDistDelta > 0.0f)
            {
                auto& ItSlot{this->DragChildSlots.at(It->get())};
                ItSlot.Dist -= MainDistDelta;
            }
        }
    }

    auto PostDist{0.0f};
    for (auto const& Slot: this->DragChildSlots | std::views::values)
    {
        check(Slot.Dist != WDragRegion::DistAuto)
        PostDist += Slot.Dist;
    }
    if (!maths::eq_e(PostDist, 1.0f, maths::not_so_small_number_f))
    {
        LOG_WARNING(LogWidgetFramework, "[{}]: Distribution [{}] after drag is not normalized anymore. Renormalizing in next tick."
            , this->GetNameAsString(), PostDist)
        this->CacheDists();
        this->bUpdateSizes = true;
    }

    return {};
}

f32 Jafg::WDragRegion::SetSizeForChild(LChildSlot const& Slot, WNode& Child, f32 Size) const
{
    check(algo::contains(this->GetChildren(), &Child, algo::unique_raw))
    check(!(Slot.MinDesiredSize && Slot.MaxDesiredSize) ||
           ((Slot.MinDesiredSize->InStaticPoints(this->GetViewport()).x <= Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()).x)
         && (Slot.MinDesiredSize->InStaticPoints(this->GetViewport()).y <= Slot.MaxDesiredSize->InStaticPoints(this->GetViewport()).y))
         )

    if (this->Cf == ENodePrimitiveControlflow::Horizontal)
    {
        if (Slot.MinDesiredSize)
        {
            if (auto Min{Slot.MinDesiredSize->InStaticPoints(this->GetViewport())}; Size < Min.x)
            {
                Size = Min.x;
            }
        }
        if (Slot.MaxDesiredSize)
        {
            if (auto Max{Slot.MaxDesiredSize->InStaticPoints(this->GetViewport())}; Size > Max.x)
            {
                Size = Max.x;
            }
        }
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        if (Slot.MinDesiredSize)
        {
            if (auto Min{Slot.MinDesiredSize->InStaticPoints(this->GetViewport())}; Size < Min.y)
            {
                Size = Min.y;
            }
        }
        if (Slot.MaxDesiredSize)
        {
            if (auto Max{Slot.MaxDesiredSize->InStaticPoints(this->GetViewport())}; Size > Max.y)
            {
                Size = Max.y;
            }
        }
    }

    return this->SetSizeForChildNoMinMax(Child, Size);
}

f32 Jafg::WDragRegion::SetSizeForChildNoMinMax(WNode& Child, f32 Size) const
{
    if (this->Cf == ENodePrimitiveControlflow::Horizontal)
    {
        Child.MinDesiredSize = {ENodeSize::StaticPoints, Size, 0.0f};
        Child.MaxDesiredSize = {ENodeSize::StaticPoints, Size, 0.0f};
    }
    else
    {
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        Child.MinDesiredSize = {ENodeSize::StaticPoints, 0.0f, Size};
        Child.MaxDesiredSize = {ENodeSize::StaticPoints, 0.0f, Size};
    }

    return Size;
}
