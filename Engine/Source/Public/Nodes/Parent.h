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
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryParent, ECxxClassFlags::Abstract)
class ENGINE_API WParent : public WNode
{
    GENERATED_CLASS_BODY()

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

    virtual LCursorReply SweepMouse(LNodeSweepData const& Data, LVec2F const& Location) override;
    virtual LReply       SweepFocusTest(LNodeSweepData const& Data, LVec2F const& Location) override;

    virtual LReply OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;

    virtual bool IsFocusWidgetTransitive(LViewport const* Viewport) const override;
    virtual void OnSurfaceResize() override;
    virtual bool FindNodeInVisiblePath(WNode const* Node) const override;

    FORCEINLINE
    virtual TArray<TJxxUnique<WNode>> const& GetChildren() const noexcept { return this->Children; }
    virtual void RemoveChildren() { algo::orphan(&this->Children); }

    virtual void RemoveChild(WNode* Child)
    {
        check(Child)
        auto It{algo::find(this->Children, Child, [](auto const& E){return &*E;})};
        if (It == this->Children.end())
        {
            LOG_FATAL(LogWidgetFramework, "The child [{}] is not a child of [{}]."
                , Child->GetNameAsString(), this->GetNameAsString()
                )
        }
        this->Children.erase(It);
    }

    void AddChild(TJxxUnique<WNode> Child) { this->AddChildAt(this->GetChildren().size(), std::move(Child)); }
    virtual void AddChildAt(u64 Index, TJxxUnique<WNode> Child);

    FORCEINLINE constexpr void SetPadding(LPadding const& InPadding) noexcept { this->Padding = InPadding; }
    FORCEINLINE constexpr LPadding const& GetPadding() const noexcept { return this->Padding; }

private:

    TArray<TJxxUnique<WNode>> Children;

    //# The padding area between the slot and the content it contains.
    LPadding Padding;
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

    decltype(auto) Padding(this auto&& Self, LPadding const& P) noexcept
    {
        NODE_FACTORY_SELF().SetPadding(P);
        return NODE_FACTORY_RESULT();
    }
};

inline Detail::LBeginStylingFnResult::~LBeginStylingFnResult()
{
    check(this->Factory.get())
    check(this->Factory->_bReleased == false)

    u64 Idx{this->Where.has_value() ? this->Where.value() : this->Parent.GetChildren().size()};
    this->Parent.AddChildAt(Idx++, TJxxUnique<WNode>{&this->Factory->Node});
    for (auto* Sibling : this->Factory->Siblings)
    {
        this->Parent.AddChildAt(Idx++, TJxxUnique<WNode>{Sibling});
    }
    algo::orphan(&this->Factory->Siblings);

#if JAFG_DO_CHECKS
    this->Factory->_Release();
#endif /* JAFG_DO_CHECKS */

    return;
}

template<typename TNode /* = WNode */> requires std::is_base_of_v<WNode, TNode>
inline typename TNode::LFactory& Detail::LBeginStylingFnResult::Root(LCxxClass const& Class)
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
