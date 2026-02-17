// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "ParentBase.generated.h"

namespace Jafg
{

struct LFactoryParentBase;

//#
//# Pure virtual abstraction of a widget parent.
//# To let clients implement their own data structure for children.
//# TODO: Please think of a better name for this class.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryParentBase, ECxxClassFlags::Abstract)
class ENGINE_API WParentBase : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WParentBase)

public:

    virtual TArray<LWidgetSlot*> const& GetChildren() const PURE_VIRTUAL()

    virtual void RemoveChild(WNode* Child) PURE_VIRTUAL()
    virtual void RemoveChild(LWidgetSlot* Slot) PURE_VIRTUAL()
    virtual void RemoveChildAt(const i32 Index) PURE_VIRTUAL()
    virtual void RemoveChildren() PURE_VIRTUAL()
    virtual LWidgetSlot* AddChild(WNode* Child) PURE_VIRTUAL()
    virtual LWidgetSlot* AddChildAt(const i32 Index, WNode* Child) PURE_VIRTUAL()
};

struct LFactoryParentBase : NODE_FACTORY_PARENT(WParentBase)
{
    NODE_FACTORY_BODY(WParentBase)

    FORCEINLINE decltype(auto) operator[](this auto&& Self, LNodeFactoryBase&& F) noexcept
    {
        auto& Node{NODE_FACTORY_SELF()};
        Node.AddChild(&F.GetRawNode());
        for (auto* Sibling : F.GetSiblings())
        {
            Node.AddChild(Sibling);
        }
        algo::orphan(&F.GetMutableSiblings());
        return NODE_FACTORY_RESULT();
    }
};

// template<typename TNode>
// FORCEINLINE typename TWidgetFactoryParentBase<TNode>::TFactoryRetTy& TWidgetFactoryParentBase<TNode>::AddChild(LWidgetFactory* InChild)
// {
//     check( InChild->GetNodeRaw() )
//     this->This()->AddChild(InChild->GetNodeRaw());
//
//     for (LWidgetFactory* const& Sibling : InChild->GetSiblings())
//     {
//         check( Sibling->GetNodeRaw() )
//         this->AddChild(Sibling);
//         continue;
//     }
//     algo::orphan(&InChild->GetMutableSiblingsDangerous());
//
//     return this->Self();
// }

inline Detail::LBeginStylingFnResult::~LBeginStylingFnResult()
{
    this->Parent.AddChild(&this->Parent);
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
inline typename TNode::LFactory Detail::LBeginStylingFnResult::Root(LCxxClass const& Class)
{
    return typename TNode::LFactory{*ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Parent.GetViewport(),.Class=Class}).release()};
}

} /* ~Namespace Jafg */
