// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HRegion.h"
#include "HDragRegion.generated.h"

namespace Jafg
{

struct LFactoryHDragRegion;

//#
//# The initial state of the h-drag region.
//# If not set / empty. Then all children will be evenly sized. Otherwise, the size of the children will
//# be set according to the state. If a state for a child is missing, then the remaining space will be split evenly
//# among all children that do not have an initial state.
//#
typedef TArray<TOptional<LWidgetSize1>> LInitialHDragRegionState;

//#
//# An HRegion that allows to change their children size with the mouse by dragging.
//# @note You are not allowed to change the anchor of the direct children of this node. This node will take care of them.
//#       You are only allowed to change the min/max desired size of direct children while constructing them.
//#
//# TODO: Fix initial state clamping. We can charge a lot of initial space that does not exist => In this case all
//#       all children have to be clamped.
//# TODO: When resizing store percentage and reapply them.
//# TODO: Detect size changes when the parent dictates them?
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHDragRegion)
class ENGINE_API WHDragRegion : public WHRegion
{
    GENERATED_CLASS_BODY()

    //# For initial state access.
    friend LFactoryHDragRegion;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WHDragRegion)
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetAnchor(EAnchor::Fill);
        // this->SetTint(Colors::Black);
        this->SetHSpace(3_spt);
    }

public:

    virtual void OnSurfaceResize() override;

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(LViewport const& Viewport) const override;
    virtual void AddChildAt(u64 Index, TJxxUnique<WNode> Child) override;

    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;

    virtual LReply OnKeyDown(LNodeKeyDownData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyDownData const& Data, LKeyEvent const& Event) override;

private:

    bool bEntered{};

    mutable bool bInitialStateExecuted{};
    mutable LInitialHDragRegionState InitialState;

    struct LDragChildOffset
    {
        u32 Idx{};
        f32 Offset{};
    };
    TOptional<LDragChildOffset> CalculateDragChildOffset();
    TOptional<LDragChildOffset> DragChildOffset;

    void UiTickMove();
    LDelegateHandle UiTickMoveHandle{ nullptr };
    TOptional<LVec2F> InitialMouseLocation;

    struct LChildSlot
    {
#if JAFG_DO_CHECKS
        LAnchor Anchor;
#endif /* JAFG_DO_CHECKS */
        TOptional<LWidgetSize1> MinDesiredSize;
        TOptional<LWidgetSize1> MaxDesiredSize;
    };
    std::unordered_map<WNode*, LChildSlot> DragChildSlots;
};

struct LFactoryHDragRegion : NODE_FACTORY_PARENT(WHDragRegion)
{
    NODE_FACTORY_BODY(WHDragRegion)

    decltype(auto) SetInitialState(this auto&& Self, LInitialHDragRegionState State) noexcept
    {
        NODE_FACTORY_SELF().InitialState = std::move(State);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) PushBackInitialState(this auto&& Self, LInitialHDragRegionState::value_type State) noexcept
    {
        NODE_FACTORY_SELF().InitialState.push_back(std::move(State));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) AppendInitialState(this auto&& Self, LInitialHDragRegionState State) noexcept
    {
        NODE_FACTORY_SELF().InitialState.append_range(std::move(State));
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
