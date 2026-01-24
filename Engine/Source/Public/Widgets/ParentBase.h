// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Widgets/ParentBaseFactory.h"
#include "ParentBase.generated.h"

namespace Jafg
{

//#
//# Pure virtual abstraction of a widget parent.
//# To let clients implement their own data structure for children.
//# TODO: Please think of a better name for this class.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryParentBase, ECxxClassFlags::Abstract)
class ENGINE_API WParentBase : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WParentBase)

public:

    virtual auto GetChildren() const -> const TArray<LWidgetSlot*>& PURE_VIRTUAL(return WParentBase::NothingArrayReference)

    virtual void RemoveChild(WNode* InChild) PURE_VIRTUAL()
    virtual void RemoveChild(LWidgetSlot* InSlot) PURE_VIRTUAL()
    virtual void RemoveChildAt(const i32 InIndex) PURE_VIRTUAL()
    virtual void RemoveChildren() PURE_VIRTUAL()
    virtual LWidgetSlot* AddChild(WNode* InChild) PURE_VIRTUAL(return nullptr)
    virtual LWidgetSlot* AddChildAt(const i32 InIndex, WNode* InChild) PURE_VIRTUAL(return nullptr)

private:

    //# Empty array for unreachable code paths - just to satisfy the compiler. Has no use-case.
    static inline TArray<LWidgetSlot*> NothingArrayReference;
};

template<typename TNode>
FORCEINLINE typename TWidgetFactoryParentBase<TNode>::TFactoryRetTy& TWidgetFactoryParentBase<TNode>::AddChild(LWidgetFactory* InChild)
{
    check( InChild->GetNodeRaw() )
    this->This()->AddChild(InChild->GetNodeRaw());

    for (LWidgetFactory* const& Sibling : InChild->GetSiblings())
    {
        check( Sibling->GetNodeRaw() )
        this->AddChild(Sibling);
        continue;
    }
    algo::orphan(&InChild->GetMutableSiblingsDangerous());

    return this->Self();
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* ConstructWidgetNode(WParentBase* Parent)
{
    return ConstructWidgetNode<TNode>(Parent, TNode::StaticClass());
}
template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* ConstructWidgetNode(WParentBase* Parent, TSubclassOf<TNode> const& Class)
{
    check( Parent )

    auto* Node{ ConstructDeferredWidgetNode(Parent->GetOuter(), Class) };
    Parent->AddChild(Node);
    MakeDeferredWidgetNodeFinal(Node);

    return Node;
}

FORCEINLINE void LWidgetFactory::TrailingParent(WParentBase* InParent) noexcept
{
    check( InParent )

    InParent->AddChild(this->GetNodeRaw());

    for (LWidgetFactory* Sibling : this->GetSiblings())
    {
        Sibling->TrailingParent(InParent);
    }
    algo::orphan(&this->GetMutableSiblingsDangerous());

    return;
}

FORCEINLINE void LWidgetFactory::TrailingParentAt(u64 Where, WParentBase* InParent) noexcept
{
    check( InParent )

    InParent->AddChildAt(static_cast<i32>(Where), this->GetNodeRaw());

    for (LWidgetFactory* Sibling : this->GetSiblings())
    {
        Sibling->TrailingParent(InParent);
    }
    algo::orphan(&this->GetMutableSiblingsDangerous());

    return;
}

FORCEINLINE void LWidgetFactory::FinishWithParent(WParentBase* InParent) noexcept
{
    check( InParent )

    InParent->AddChild(this->GetNodeRaw());

    for (LWidgetFactory* Sibling : this->GetSiblings())
    {
        Sibling->TrailingParent(InParent);
    }
    algo::orphan(&this->GetMutableSiblingsDangerous());

    this->Finish();

    return;
}

FORCEINLINE void LWidgetFactory::FinishWithParentAt(u64 Where, WParentBase* InParent) noexcept
{
    check( InParent )

    InParent->AddChildAt(static_cast<i32>(Where), this->GetNodeRaw());

    for (LWidgetFactory* Sibling : this->GetSiblings())
    {
        Sibling->TrailingParent(InParent);
    }
    algo::orphan(&this->GetMutableSiblingsDangerous());

    this->Finish();

    return;
}

} /* ~Namespace Jafg */
