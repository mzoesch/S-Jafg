// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParentBase.h"
#include "WidgetParent.generated.h"

namespace Jafg
{

//#
//# The base class for all nodes that can possess children.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WWidgetParent : public WWidgetParentBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WWidgetParent(const LObjectInitializer& ObjectInitializer);

public:

    virtual void OnGarbage() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2& InLocation) override;
    virtual LReply       SweepFocusTest(LViewport& Context, const LVector2& InLocation) override;

    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const override;

    virtual bool FindNodeInVisiblePath(const WWidgetNode* InNode) const override;

    virtual void UpdateDesiredSize() const override;
    virtual auto GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const -> LVector2 override;
    virtual void UpdateAnchoredSize(const LViewport& Context) const override;
    virtual auto GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const -> LVector2 override;

    FORCEINLINE
    virtual auto         GetChildren() const -> const TdhArray<LWidgetSlot*>& override { return this->Children; }
    virtual void         RemoveChild(WWidgetNode* Child) override;
    virtual void         RemoveChild(LWidgetSlot* Child) override;
    virtual LWidgetSlot* AddChild(WWidgetNode* InChild) override;
    virtual LWidgetSlot* AddChildAt(const int32 InIndex, WWidgetNode* InChild) override;

    FORCEINLINE virtual void SetPadding(const LPadding& InPadding) override { this->Padding = InPadding; }
    FORCEINLINE         auto GetPadding()    const -> const LPadding& { return this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr() const -> const LPadding* override { return &this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr()       ->       LPadding* override { return &this->Padding; }

private:

    TdhArray<LWidgetSlot*> Children;

    //# The padding area between the slot and the content it contains.
    LPadding               Padding;
};

} /* ~Namespace Jafg */
