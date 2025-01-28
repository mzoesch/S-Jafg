// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "WidgetParentBase.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryParentBase : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Padding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }

    FORCEINLINE TFactoryRetTy& AddChild(LWidgetFactory* InChild);
    FORCEINLINE TFactoryRetTy& operator[](LWidgetFactory& InChild) { return this->AddChild(&InChild); }
};

//#
//# Pure virtual abstraction of a widget parent.
//# To let other widgets implement their own data structure for children.
//# TODO: Please think of a better name for this class.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryParentBase, EClassFlags::Abstract)
class ENGINE_API WWidgetParentBase : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParentBase)

public:

    virtual void Tick() override;

    virtual auto SweepMouse(LViewport& Context, const LVector2& InLocation) -> LCursorReply override;
    virtual auto SweepFocusTest(LViewport& Context, const LVector2& InLocation) -> LReply override;

    virtual bool IsFocusWidgetTransitive() const override final;
    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const override;

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(const LViewport& Context) const override;

    virtual auto GetChildren() const               -> const TdhArray<LWidgetSlot*>& PURE_VIRTUAL(return WWidgetParentBase::NothingArrayReference)
    virtual auto RemoveChild(WWidgetNode* InChild) -> void PURE_VIRTUAL()
    virtual auto RemoveChild(LWidgetSlot* InSlot)  -> void PURE_VIRTUAL()
    virtual auto AddChild(WWidgetNode* InChild)    -> LWidgetSlot* PURE_VIRTUAL(return nullptr)
    virtual auto AddChildAt(const int32 InIndex, WWidgetNode* InChild) -> LWidgetSlot* PURE_VIRTUAL(return nullptr)
    virtual auto FindNodeInVisiblePath(const WWidgetNode* InNode) const -> bool override;

    virtual void SetPadding(const LPadding& InPadding)    PURE_VIRTUAL()
    virtual auto GetPaddingPtr() const -> const LPadding* PURE_VIRTUAL(return nullptr)
    virtual auto GetPaddingPtr()       ->       LPadding* PURE_VIRTUAL(return nullptr)

private:

    //# Empty array for unreachable code paths - just to satisfy the compiler. Has no use-case.
    static inline TdhArray<LWidgetSlot*> NothingArrayReference;
};

template <typename TNode>
typename TWidgetFactoryParentBase<TNode>::TFactoryRetTy&
TWidgetFactoryParentBase<TNode>::AddChild(LWidgetFactory* InChild)
{
    check( InChild->GetNodeRaw() )
    this->This()->AddChild(InChild->GetNodeRaw());

    for (LWidgetFactory* const& Sibling : InChild->GetSiblings())
    {
        check( Sibling->GetNodeRaw() )
        this->AddChild(Sibling);
        continue;
    }
    InChild->GetMutableSiblingsDangerous().Empty();

    return this->Self();
}

} /* ~Namespace Jafg */
