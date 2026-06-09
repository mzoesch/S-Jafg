// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/DynRegion.h"
#include "DragRegion.generated.h"

namespace Jafg
{

//#
//# The initial state of a drag region.
//# If not set / empty. Then all children will be evenly sized. Otherwise, the size of the children will
//# be set according to the state. If a state for a child is missing, then the remaining space will be split evenly
//# among all children that do not have an initial state.
//#
// typedef TArray<std::variant<std::monostate, f32, LNodeSize1>> LInitialDragRegionState;

//#
//# A base building block that allows to change their children size with the mouse by dragging.
//# @note You are not allowed to change the anchor of the direct children of this node. This node will take care of them.
//#       You are only allowed to change the min/max desired size of direct children while constructing them.
//#       But don't worry, Jafg has assertions for that :)
//#
//# TODO: Fix initial state clamping. We can charge a lot of initial space that does not exist => In this case all
//#       all children have to be clamped.
//# TODO: Do not drag if mouse if over the padding.
//# TODO: When resizing store percentage and reapply them.
//# TODO: Detect size changes when the parent dictates them?
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WDragRegion : public WDynRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WDragRegion)
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->Anchor = EAnchor::Fill;
        this->Cf = ENodePrimitiveControlflow::Horizontal;
        this->_ctor_SetSpace();
    }

public:

    virtual void OnSurfaceResize() override;

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize() const override;
    virtual void OnRemoveChildPost(WNode& Child) override;
    virtual WNode& OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed) override;

    virtual LNodeReply SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location) override;
    virtual LNodeReply Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location) override;
    virtual LNodeReply OnCursorEnter() override;
    virtual void OnCursorLeave() override;

    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    NODISCARD FORCEINLINE constexpr auto const& GetDragChildSlots() const noexcept { return this->DragChildSlots; }
    inline void SetDistFor(WNode& Node, f32 Dist) noexcept
    {
        check(Dist > 0.0f)
        this->DragChildSlots.at(&Node).Dist = Dist;
    }

    NODISCARD FORCEINLINE ENodePrimitiveControlflow GetCf() const noexcept { return this->Cf; }
    void SetCfWithSideEffects(ENodePrimitiveControlflow Controlflow) noexcept;

    NODISCARD FORCEINLINE EAnchor GetChildDesiredAnchor() const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return EAnchor::VFill;
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return EAnchor::HFill;
    }

    //#
    //# Whether to use the pixels as a grid.
    //# This is visual only. It has no effect on behavior.
    //#
    bool bUseGrid{true};

private:

    //#
    //# Changing this requires side effects for childs of the drag region.
    //# Use the protected SetCf instead.
    //#
    using Super::Cf;

    void _ctor_SetSpace();

    NODISCARD FORCEINLINE f32 ProjVec(LVec2F const& Vec) const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return Vec.x;
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return Vec.y;
    }
    NODISCARD FORCEINLINE f32 ProjVecInv(LVec2F const& Vec) const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return Vec.y;
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return Vec.x;
    }

    NODISCARD FORCEINLINE LVec2F ProjFlt(f32 Flt) const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return LVec2F{Flt, 0.0f};
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return LVec2F{0.0f, Flt};
    }
    NODISCARD FORCEINLINE LVec2F ProjFltInv(f32 Flt) const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return LVec2F{0.0f, Flt};
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return LVec2F{Flt, 0.0f};
    }

    NODISCARD FORCEINLINE LVec2F ProjTake(LVec2F const& Vec) const noexcept
    {
        return this->ProjFlt(this->ProjVec(Vec));
    }
    NODISCARD FORCEINLINE LVec2F ProjTakeInv(LVec2F const& Vec) const noexcept
    {
        return this->ProjFltInv(this->ProjVecInv(Vec));
    }

    mutable std::optional<LVec2F> LastAnchoredSize;
    mutable WNode* DragHint{};
    std::optional<LVec2F> InitialDragLocation;
    bool bEntered:1{};
    mutable bool bUpdateSizes:1{ true };
    mutable bool bSkipChildDsUpdate:1{ true };

    bool IsLocationOverDragRect(LVec2F Translation, LVec2F Location) const noexcept;
    struct LDragChildOffset
    {
        u32 Idx{};
        f32 Offset{};
    };

    f32 GetDistribution() const;
    void CacheDists() const;
    void NormalizeDists() const;
    void ApplyDists(f32 Distribution) const;

    bool UiTickMove();
    LDelegateHandle UiTickMoveHandle;

    inline static constexpr f32 DistAuto{};
    struct LChildSlot
    {
        mutable f32 Dist{ DistAuto };
        std::optional<LNodeSize2> MinDesiredSize;
        std::optional<LNodeSize2> MaxDesiredSize;
    };
    std::unordered_map<WNode*, LChildSlot> DragChildSlots;
    //# @return Clamped size (because of optional child preferences).
    f32 SetSizeForChild(LChildSlot const& Slot, WNode& Child, f32 Size) const;
    f32 SetSizeForChildNoMinMax(WNode& Child, f32 Size) const;

    inline f32 GetSizeForDist(f32 Dist) const noexcept
    {
        if (this->Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return this->GetAnchoredSize_v2().x * Dist;
        }
        check(this->Cf == ENodePrimitiveControlflow::Vertical)
        return this->GetAnchoredSize_v2().y * Dist;
    }
};

} /* ~Namespace Jafg */

// TODO: Chane LSize from points to sp
