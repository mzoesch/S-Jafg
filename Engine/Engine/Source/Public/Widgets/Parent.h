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
DECLARE_JAFG_WIDGET(ECxxClassFlags::Abstract)
class WParent : public WParentBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WParent(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WParent)

public:

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2& InLocation) override;
    virtual LReply       SweepFocusTest(const LViewport& Context, const LVector2& InLocation) override;

    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const override;
    virtual bool FindNodeInVisiblePath(const WNode* InNode) const override;

    FORCEINLINE
    virtual auto GetChildren() const -> const TArray<LWidgetSlot*>& override { return this->Children; }
    virtual void RemoveChild(WNode* Child) override;
    virtual void RemoveChild(LWidgetSlot* Child) override;
    virtual void RemoveChildAt(const i32 InIndex) override;
    virtual void RemoveChildren() override;
    virtual auto AddChild(WNode* InChild) -> LWidgetSlot* override;
    virtual auto AddChildAt(const i32 InIndex, WNode* InChild) -> LWidgetSlot* override;

    void MakeChildrenFinal();

    FORCEINLINE void SetPadding(const LPadding& InPadding)    { this->Padding = InPadding; }
    FORCEINLINE auto GetPadding()    const -> const LPadding& { return this->Padding; }
    FORCEINLINE auto GetPaddingPtr() const -> const LPadding* { return &this->Padding; }
    FORCEINLINE auto GetPaddingPtr()       ->       LPadding* { return &this->Padding; }

    virtual void RecacheViewport() noexcept override;

private:

    TArray<LWidgetSlot*> Children;

    //# The padding area between the slot and the content it contains.
    LPadding Padding;
};

} /* ~Namespace Jafg */
