// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "ParentBase.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryParentBase : public TWidgetFactory<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactory)

    FORCEINLINE TFactoryRetTy& Padding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }

    FORCEINLINE TFactoryRetTy& AddChild(LWidgetFactory* InChild);
    FORCEINLINE TFactoryRetTy& operator[](LWidgetFactory& InChild) { return this->AddChild(&InChild); }
};

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
    virtual LWidgetSlot* AddChild(WNode* InChild) PURE_VIRTUAL(return nullptr)
    virtual LWidgetSlot* AddChildAt(const i32 InIndex, WNode* InChild) PURE_VIRTUAL(return nullptr)

    virtual void SetPadding(const LPadding& InPadding) PURE_VIRTUAL()
    virtual auto GetPaddingPtr()       ->       LPadding* PURE_VIRTUAL(return nullptr)
    virtual auto GetPaddingPtr() const -> const LPadding* PURE_VIRTUAL(return nullptr)

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
    InChild->GetMutableSiblingsDangerous().Empty();

    return this->Self();
}

} /* ~Namespace Jafg */
