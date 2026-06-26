// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Parent.generated.h"

namespace Jafg
{

struct LFactoryParent;

//#
//# The base class for all nodes that can possess children.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryParent, EJxxClassBits::Abstract)
class ENGINE_API WParent : public WNode
{
    GENERATED_CLASS_BODY()

    friend class WNode;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WParent) noexcept
    {
        this->SetVisibility(ENodeVisibility::IntransitiveHitTestInvisible);
    }

public:

#if JAFG_DO_CHECKS
    virtual ~WParent() override
    {
        check(Tasks::IsOnMasterThread())
        check(this->Children.empty())
    }
#endif /* JAFG_DO_CHECKS */

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override { Super::Destruct(); this->RemoveChildren(); }
    virtual void Draw(LNodeRenderInfo const& Info) const override;

    virtual LNodeReply SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location) override;
    virtual LNodeReply Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location) override;

    virtual LNodeReply OnKeyEventUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    NODISCARD FORCEINLINE virtual bool IsFocusWidgetTransitive() const noexcept override
    {
        if (Super::IsFocusWidgetTransitive()) { return true; }
        return algo::any_of(this->Children, [](auto& Child){ return Child->IsFocusWidgetTransitive(); });
    }
    virtual void OnSurfaceResize() override
    {
        Super::OnSurfaceResize();
        for (auto& Child : this->Children)
        {
            check(Child.get())
            Child->OnSurfaceResize();
            continue;
        }
    }
    virtual bool IsNodeInVisiblePath(WNode const& Node) const override;
    virtual WNode const* FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) const noexcept override;
    virtual WNode* FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) noexcept override;

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize() const override;
    //# Virtual update method for the anchored size of a child. Automatically called. Do not call manually.
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const PURE_VIRTUAL()
    //# @return The anchored top-left corner of the direct child relative to the given context's top-left corner.
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const PURE_VIRTUAL()

    FORCEINLINE
    virtual TArray<TJxxUnique<WNode>> const& GetChildren() const noexcept { return this->Children; }
    virtual void RemoveChildren() { algo::orphan(&this->Children); }

    //# Called before a child is removed. This methods must not remove the child itself.
    virtual void OnRemoveChildPrepare(WNode& Child) {}
    //# Called after a child is removed.
    virtual void OnRemoveChildPost(WNode& Child) { check(Child.IsParentValid()) Child._SetParentDangerous(nullptr); }

    //#
    //# All nodes except #WUserWidget cannot be removed from their parent without being destroyed.
    //# If you want to change a #WUserWidget's parent, you have to use #WUserWidget::RemoveFromTree and then
    //# add it to your desired parent with #AddConstructedChild or #AddConstructedChildAt.
    //#
    void RemoveChild(WNode& Child) { this->RemoveChildImpl(Child); }
    WNode& AddChild(TJxxUnique<WNode> Child) { return this->AddChildAt(this->GetChildren().size(), std::move(Child)); }
    WNode& AddChildAt(std::size_t Index, TJxxUnique<WNode> Child);
    WNode& AddConstructedChild(TJxxUnique<WNode> Child) { return this->AddConstructedChildAt(this->GetChildren().size(), std::move(Child)); }
    WNode& AddConstructedChildAt(std::size_t Index, TJxxUnique<WNode> Child);
    virtual WNode& OnAddChild(std::size_t Index, TJxxUnique<WNode> Child, bool bConstructed);

    std::size_t ReorderChild(WNode& Who, std::size_t Desired);

    //# The padding area between the slot and the content it contains.
    LPadding Padding;

private:

    TJxxUnique<WNode> RemoveChildImpl(WNode& Child);

    TArray<TJxxUnique<WNode>> Children;
};

struct LFactoryParent : NODE_FACTORY_PARENT(WParent)
{
    NODE_FACTORY_BODY(WParent)

    decltype(auto) operator[](this auto&& Self, Detail::LNodeFactoryBase&& F) noexcept
    {
        check(F._IsReleased() == false)
        auto& Node{DETAIL_JAFG_NODE_FACTORY_SELF()};
        Node.AddChild(TJxxUnique<WNode>{&F.GetRawNode()});
        for (auto* Sibling : F.GetSiblings())
        {
            Node.AddChild(TJxxUnique<WNode>{Sibling});
        }
        algo::orphan(&F.GetMutableSiblings());
        checkCode(F._Release())
        checkCode(F._Decommission())
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Padding(this auto&& Self, LPadding const& Padding) noexcept
    {
        NODE_FACTORY_SELF().Padding = Padding;
        return NODE_FACTORY_RESULT();
    }
};

inline Detail::LBeginStylingFnResult::~LBeginStylingFnResult()
{
    check(this->Factory.get())
    check(this->Factory->_bReleased == false)

    u64 Idx{this->Where.has_value() ? static_cast<u64>(*this->Where) : this->Parent.GetChildren().size()};
    this->Parent.AddChildAt(Idx++, TJxxUnique<WNode>{&this->Factory->Node});
    for (auto* Sibling : this->Factory->Siblings)
    {
        this->Parent.AddChildAt(Idx++, TJxxUnique<WNode>{Sibling});
    }
    algo::orphan(&this->Factory->Siblings);

#if JAFG_DO_CHECKS
    this->Factory->_Release();
#endif /* JAFG_DO_CHECKS */
}

template<typename TNode /* = WNode */> requires std::is_base_of_v<WNode, TNode>
inline typename TNode::LFactory& Detail::LBeginStylingFnResult::Root(LJxxClass const& Class)
{
    check(this->Factory.get() == nullptr)
    this->Factory = TUnique<typename TNode::LFactory>{
        new typename TNode::LFactory{
            *ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Parent.GetViewport(),.Class=Class}).release()
            }
        };
    return *static_cast<typename TNode::LFactory*>(&*this->Factory);
}

template<typename TNode, typename... TArgs>
    requires std::is_base_of_v<WNode, TNode>
          // && std::is_constructible_v<TNode, TNodeStaticInit<TNode> const&, TArgs&&...>
inline typename TNode::LFactory& Detail::LBeginStylingFnResult::StaticRoot(TArgs&&... Args) noexcept
{
    check(this->Factory.get() == nullptr)
    this->Factory = TUnique<typename TNode::LFactory>{
        new typename TNode::LFactory{
            *ConstructNodeImpl(TNodeStaticInit<TNode>{.Outer=this->Parent.GetViewport()}, std::forward<TArgs>(Args)...).release()
            }
        };
    return *static_cast<typename TNode::LFactory*>(&*this->Factory);
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode* WNode::GetParentUntil() noexcept
{
    WParent* Cursor{this->GetParent()};
    while (Cursor)
    {
        if (auto* Casted{DynamicCast<TNode>(Cursor)})
        {
            return Casted;
        }
        Cursor = Cursor->GetParent();
    }
    return nullptr;
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode const* WNode::GetParentUntil() const noexcept
{
    WParent const* Cursor{this->GetParent()};
    while (Cursor)
    {
        if (auto* Casted{DynamicCast<TNode>(Cursor)})
        {
            return Casted;
        }
        Cursor = Cursor->GetParent();
    }
    return nullptr;
}

} /* ~Namespace Jafg */
