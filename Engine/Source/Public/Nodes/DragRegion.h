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
typedef TArray<std::optional<LNodeSize1>> LInitialDragRegionState;

//#
//# A base building block that allows to change their children size with the mouse by dragging.
//# @note You are not allowed to change the anchor of the direct children of this node. This node will take care of them.
//#       You are only allowed to change the min/max desired size of direct children while constructing them.
//#       But don't worry, Jafg have assertions for that :)
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
        this->_ctor_SetSpace();
    }

public:

    virtual void OnSurfaceResize() override;

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize() const override;
    virtual void OnRemoveChildPost(WNode& Child) override;
    virtual WNode& OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed) override;

    virtual LNodeReply OnCursorEnter() override;
    virtual void OnCursorLeave() override;

    virtual LNodeReply OnKeyDownFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;
    virtual LNodeReply OnKeyUpFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;

    NODISCARD FORCEINLINE constexpr auto const& GetDragChildSlots() const noexcept { return this->DragChildSlots; }

private:

    void _ctor_SetSpace();

    FORCEINLINE EAnchor::Type GetChildDesiredAnchor() const noexcept
    {
        if (this->Cf == ENodePrimitiveControlFlow::Horizontal)
        {
            return EAnchor::VFill;
        }
        check(this->Cf == ENodePrimitiveControlFlow::Vertical)
        return EAnchor::HFill;
    }

    bool bEntered{};
    mutable bool bUpdateSizes{};

    struct LDragChildOffset
    {
        u32 Idx{};
        f32 Offset{};
    };
    std::optional<LDragChildOffset> CalculateDragChildOffset(LVec2F const& Translation) const;
    std::optional<LDragChildOffset> DragChildOffset;

    bool UiTickMove();
    LDelegateHandle UiTickMoveHandle{ nullptr };
    std::optional<LVec2F> InitialMouseLocation;

    struct LChildSlot
    {
#if JAFG_DO_CHECKS
        LAnchor Anchor;
#endif /* JAFG_DO_CHECKS */
        std::optional<LNodeSize1> MinDesiredSize;
        std::optional<LNodeSize1> MaxDesiredSize;
    };
    std::unordered_map<WNode*, LChildSlot> DragChildSlots;
};

} /* ~Namespace Jafg */
