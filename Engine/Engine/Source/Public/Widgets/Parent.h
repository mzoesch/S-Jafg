// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/ParentBase.h"
#include "Parent.generated.h"

namespace Jafg
{

//#
//# The base class for all nodes that can possess children.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WParent : public WParentBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WParent(const LObjectInitializer& ObjectInitializer);

public:

    virtual void OnGarbage() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2& InLocation) override;
    virtual LReply       SweepFocusTest(LViewport& Context, const LVector2& InLocation) override;

    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const override;
    virtual bool FindNodeInVisiblePath(const WNode* InNode) const override;

    FORCEINLINE
    virtual auto GetChildren() const -> const TArray<LWidgetSlot*>& override { return this->Children; }
    virtual void RemoveChild(WNode* Child) override;
    virtual void RemoveChild(LWidgetSlot* Child) override;
    virtual auto AddChild(WNode* InChild) -> LWidgetSlot* override;
    virtual auto AddChildAt(const i32 InIndex, WNode* InChild) -> LWidgetSlot* override;

    FORCEINLINE virtual void SetPadding(const LPadding& InPadding) override { this->Padding = InPadding; }
    FORCEINLINE         auto GetPadding()    const -> const LPadding& { return this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr() const -> const LPadding* override { return &this->Padding; }
    FORCEINLINE virtual auto GetPaddingPtr()       ->       LPadding* override { return &this->Padding; }

private:

    TArray<LWidgetSlot*> Children;

    //# The padding area between the slot and the content it contains.
    LPadding Padding;
};

} /* ~Namespace Jafg */
