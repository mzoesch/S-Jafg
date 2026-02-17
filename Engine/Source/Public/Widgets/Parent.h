// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/ParentBase.h"
#include "Parent.generated.h"

namespace Jafg
{

struct LFactoryParent;

//#
//# The base class for all nodes that can possess children.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryParent, ECxxClassFlags::Abstract)
class ENGINE_API WParent : public WParentBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WParent) noexcept
    {
        this->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
    }

public:

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual LCursorReply SweepMouse(LViewport& Context, const LVec2F& Location) override;
    virtual LReply       SweepFocusTest(const LViewport& Context, const LVec2F& Location) override;

    virtual LReply OnKeyDownNoFocus(const LViewport& Viewport, const LKeyEvent& KeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& Viewport, const LKeyEvent& KeyEvent) override;

    virtual bool IsFocusWidgetTransitive(const LViewport* Viewport) const override;
    virtual bool FindNodeInVisiblePath(const WNode* Node) const override;

    FORCEINLINE
    virtual auto GetChildren() const -> TArray<LWidgetSlot*> const& override { return this->Children; }
    virtual void RemoveChild(WNode* Child) override;
    virtual void RemoveChild(LWidgetSlot* Child) override;
    virtual void RemoveChildAt(const i32 InIndex) override;
    virtual void RemoveChildren() override;
    virtual auto AddChild(WNode* Child) -> LWidgetSlot* override;
    virtual auto AddChildAt(const i32 Index, WNode* Child) -> LWidgetSlot* override;

    FORCEINLINE void SetPadding(const LPadding& InPadding)    { this->Padding = InPadding; }
    FORCEINLINE auto GetPadding()    const -> const LPadding& { return this->Padding; }
    FORCEINLINE auto GetPaddingPtr() const -> const LPadding* { return &this->Padding; }
    FORCEINLINE auto GetPaddingPtr()       ->       LPadding* { return &this->Padding; }

private:

    TArray<LWidgetSlot*> Children;

    //# The padding area between the slot and the content it contains.
    LPadding Padding;
};

struct LFactoryParent : NODE_FACTORY_PARENT(WParent)
{
    NODE_FACTORY_BODY(WParent)

    FORCEINLINE decltype(auto) Padding(this auto&& Self, LPadding const& P) noexcept
    {
        NODE_FACTORY_SELF().SetPadding(P);
        return std::forward<decltype(Self)>(Self);
    }
};

} /* ~Namespace Jafg */
