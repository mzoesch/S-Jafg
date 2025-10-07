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
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryParentBase, EClassFlags::Abstract)
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

template <typename TNode>
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

} /* ~Namespace Jafg */
