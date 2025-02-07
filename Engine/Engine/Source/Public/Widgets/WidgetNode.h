// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Widgets/Whitespace.h"
#include "Widgets/InterfaceTypes.h"
#include "User/Input/Replies.h"
#include "User/Input/Events.h"
#include "WidgetNode.generated.h"

namespace Jafg
{

class LLocalEgo;
class WWidgetNode;
class WWidgetParent;
class WUserWidget;
class LViewport;
class LSurface;
class LWidgetFactory;
class LWidgetFactory;
class WWidgetParentBase;
class LApplicationInstance;
template <typename TNode>
class TWidgetFactory;
template <typename TNode>
class TWidgetFactoryParentBase;
struct LWidgetSlot;
struct LWidgetConstructor;

namespace Private
{

ENGINE_API void            AddWidgetFactory(LWidgetFactory* InFactory);
ENGINE_API LWidgetFactory* FindOrNullWidgetFactory(const void* InNode);
ENGINE_API LWidgetFactory& GetWidgetFactory(const void* InNode);
ENGINE_API int32           PurgeWidgetFactories();

//#
//# Private struct that is the single factory friend.
//#
struct LWidgetFactoryUtility final
{
    LWidgetFactoryUtility() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWidgetFactoryUtility)
    ~LWidgetFactoryUtility() = delete;

    //#
    //# Creates a Wsdsml factory for the given node. Only valid in the engine
    //# tick where the factory was requested for.
    //#
    //# @tparam TNode The node-type to create a factory for.
    //# @param  InNode The node to create a factory for.
    //# @return The factory for the given node.
    //#
    template <typename TNode>
    NODISCARD static auto MakeWidgetFactory(const WWidgetNode* InNode) -> typename TNode::TWidgetFactory&;
};

} /* ~Namespace Private */

//#
//# How to anchor a child to its parent if the parent can have children.
//#
namespace EAnchor
{

enum Type : uint8
{
    Default    = 0x0 << 0,
    VTop       = 0x1 << 0,
    VCenter    = 0x1 << 1,
    VBottom    = 0x1 << 2,
    HLeft      = 0x1 << 3,
    HCenter    = 0x1 << 4,
    HRight     = 0x1 << 5,

    VFill      = 0x1 << 6,
    HFill      = 0x1 << 7,

    TopLeft         = VTop    | HLeft,
    TopCenter       = VTop    | HCenter,
    TopRight        = VTop    | HRight,
    CenterLeft      = VCenter | HLeft,
    CenterCenter    = VCenter | HCenter,
    CenterRight     = VCenter | HRight,
    BottomLeft      = VBottom | HLeft,
    BottomCenter    = VBottom | HCenter,
    BottomRight     = VBottom | HRight,

    Fill            = VFill | HFill,
};

} /* ~Namespace EAnchor */
ENUM_CLASS_FLAGS(EAnchor::Type)

struct LAnchor final
{
    union
    {
        struct
        {
            float MinX;
            float MinY;
            float MaxX;
            float MaxY;
        };

        //#
        //# Minimum to maximum.
        //# Left, Top, Right, Bottom.
        //#
        LVector4 Anchors;
    };

    LAnchor() : Anchors(0.0f, 0.0f, 0.0f, 0.0f) { }
    LAnchor(const float InUniformAnchors)
        : Anchors(InUniformAnchors, InUniformAnchors, InUniformAnchors, InUniformAnchors)
    {
        check( this->IsNormalized() )
    }
    LAnchor(const float InHorizontalUniform, const float InVerticalUniform)
        : Anchors(InHorizontalUniform, InVerticalUniform, InHorizontalUniform, InVerticalUniform)
    {
        check( this->IsNormalized() )
    }
    LAnchor(const float InMinX, const float InMinY, const float InMaxX, const float InMaxY)
        : Anchors(InMinX, InMinY, InMaxX, InMaxY)
    {
        check( this->IsNormalized() )
    }
    LAnchor(const EAnchor::Type InAnchors)
    {
        this->Anchors = LVector4::Zero();
        this->ApplyConstraints(InAnchors);
        return;
    }
    LAnchor(const LAnchor& InOther, const EAnchor::Type InConstraints)
    {
        this->Anchors = InOther.Anchors;
        this->ApplyConstraints(InConstraints);
        return;
    }
    LAnchor(const LVector4& InOther, const EAnchor::Type InConstraints)
    {
        this->Anchors = InOther;
        this->ApplyConstraints(InConstraints);
        return;
    }
    LAnchor(const LVector4& InAnchors) : Anchors(InAnchors) { }
    LAnchor(const LAnchor& InOther) noexcept : Anchors(InOther.Anchors) { }
    LAnchor(LAnchor&& InOther) noexcept : Anchors(std::move(InOther.Anchors)) { }
    LAnchor& operator=(const LAnchor& InOther)  noexcept { this->Anchors = InOther.Anchors; return *this; }
    LAnchor& operator=(LAnchor&& InOther) noexcept { this->Anchors = std::move(InOther.Anchors); return *this; }

    ENGINE_API bool IsNormalized() const;
    ENGINE_API void Normalize();

    FORCEINLINE bool IsStretchedHorizontal() const { return this->MinX != this->MaxX; }
    FORCEINLINE bool IsStretchedVertical() const { return this->MinY != this->MaxY; }
    FORCEINLINE bool IsStretched() const { return this->IsStretchedHorizontal() || this->IsStretchedVertical(); }

    void ApplyConstraints(const EAnchor::Type InConstraints)
    {
        if (InConstraints & EAnchor::VTop)    { this->Anchors += LAnchor::VTop.Anchors; }
        if (InConstraints & EAnchor::VCenter) { this->Anchors += LAnchor::VCenter.Anchors; }
        if (InConstraints & EAnchor::VBottom) { this->Anchors += LAnchor::VBottom.Anchors; }
        if (InConstraints & EAnchor::HLeft)   { this->Anchors += LAnchor::HLeft.Anchors; }
        if (InConstraints & EAnchor::HCenter) { this->Anchors += LAnchor::HCenter.Anchors; }
        if (InConstraints & EAnchor::HRight)  { this->Anchors += LAnchor::HRight.Anchors; }
        if (InConstraints & EAnchor::VFill)   { this->Anchors += LAnchor::VFill.Anchors; }
        if (InConstraints & EAnchor::HFill)   { this->Anchors += LAnchor::HFill.Anchors; }

        check( this->IsNormalized() )

        return;
    }

    bool operator==(const LAnchor& InOther) const { return this->Anchors == InOther.Anchors; }
    bool operator!=(const LAnchor& InOther) const { return this->Anchors != InOther.Anchors; }

    ENGINE_API static const LAnchor VTop;
    ENGINE_API static const LAnchor VCenter;
    ENGINE_API static const LAnchor VBottom;
    ENGINE_API static const LAnchor HLeft;
    ENGINE_API static const LAnchor HCenter;
    ENGINE_API static const LAnchor HRight;

    ENGINE_API static const LAnchor VFill;
    ENGINE_API static const LAnchor HFill;
};

namespace EWidgetVisibility
{

enum Type : uint8
{
    //#
    //# Visible, takes up space in the widget layout and is hit-testable.
    //# Widgets in this state will be ticked.
    //#
    Visible,

    //#
    //# Not visible, takes up space in the widget layout and is not hit-testable.
    //# Widgets in this state will not be ticked.
    //#
    Hidden,

    //#
    //# Not visible, does not take up space in the widget layout and is not hit-testable.
    //# Widgets in this state will not be ticked.
    //#
    Collapsed,

    //#
    //# Visible, takes up space in the widget layout and is hit-testable, but all children are not.
    //# Widgets in this state will be ticked.
    //#
    DerivedHitTestInvisible,

    //#
    //# Visible, takes up space in the widget layout and is not hit-testable.
    //# Widgets in this state will be ticked.
    //#
    TransitiveHitTestInvisible,

    //#
    //# Visible, takes up space in the widget layout and is itself not hit-testable, but all children are.
    //# Widgets in this state will be ticked.
    //#
    IntransitiveHitTestInvisible,
};

FORCEINLINE bool IsDrawn(const EWidgetVisibility::Type InVisibility)
{
    return InVisibility == EWidgetVisibility::Visible
        || InVisibility == EWidgetVisibility::DerivedHitTestInvisible
        || InVisibility == EWidgetVisibility::TransitiveHitTestInvisible
        || InVisibility == EWidgetVisibility::IntransitiveHitTestInvisible;
}

FORCEINLINE bool IsTicked(const EWidgetVisibility::Type InVisibility)
{
    return EWidgetVisibility::IsDrawn(InVisibility);
}

FORCEINLINE bool IsHitTestable(const EWidgetVisibility::Type InVisibility)
{
    return InVisibility == EWidgetVisibility::Visible
        || InVisibility == EWidgetVisibility::DerivedHitTestInvisible;
}

FORCEINLINE bool IsDerivedHitTestable(const EWidgetVisibility::Type InVisibility)
{
    return InVisibility == EWidgetVisibility::Visible
        || InVisibility == EWidgetVisibility::IntransitiveHitTestInvisible;
}

} /* ~Namespace EWidgetVisibility */
ENGINE_API LSimpleString LexToString(const EWidgetVisibility::Type InVisibility);

//#
//# The base struct for every widget slot.
//#
struct LWidgetSlot
{
    //#
    //# The parent of this slot and the owner of the memory.
    //#
    WWidgetParentBase* Parent;

    //#
    //# The content of this slot. We interpret all names in this and derived structs as of the view of the content.
    //#
    WWidgetNode* Content;

    //#
    //# The padding of the parent widget aka the margin of the child widget.
    //#
    LMargin* Margin;
};

//#
//# Base class of all widget factories.
//# @see TWidgetFactory<TNode> below.
//#
class LWidgetFactory
{
public:

    friend WWidgetNode;
    friend Private::LWidgetFactoryUtility;
    template <typename TNode>
    friend class TWidgetFactory;
    template <typename TNode>
    friend class TWidgetFactoryParentBase;

    FORCEINLINE WWidgetNode* GetNodeRaw() const { check( this->Node ) return this->Node; }

    FORCEINLINE bool HasAnySibling() const { return this->Siblings.IsEmpty() == false; }
    FORCEINLINE auto GetSiblings() const -> const TdhArray<LWidgetFactory*>& { return this->Siblings; }

private:

    FORCEINLINE auto GetMutableSiblingsDangerous() -> TdhArray<LWidgetFactory*>& { return this->Siblings; }

    WWidgetNode* Node = nullptr;
    TdhArray<LWidgetFactory*> Siblings = { };
};

//#
//# Base class of all widget factories that can be used with the declarative syntax defined by Wsdsml.
//#
template <typename TNode>
class TWidgetFactory : public LWidgetFactory
{
public:

    friend WWidgetNode;
    friend Private::LWidgetFactoryUtility;

    using Super         = LWidgetFactory;

    //# The node to target. Always valid.
    using TNodeTy       = TNode;
    //# The return type of the factory. Always valid.
    using TFactoryRetTy = typename TNodeTy::TWidgetFactory;

#if DO_SLOW_CHECKS /* This just costs too much runtime performance. So just check the cast with slow checks. */
    FORCEINLINE TNodeTy* GetNode() const { return CheckedStaticCast<TNodeTy>(this->GetNodeRaw()); }
#else /* DO_SLOW_CHECKS */
    FORCEINLINE TNode* GetNode() const { return reinterpret_cast<TNode*>(this->GetNodeRaw()); }
#endif /* !DO_SLOW_CHECKS */

    FORCEINLINE TFactoryRetTy& Self() { return *static_cast<TFactoryRetTy*>(this); }
    FORCEINLINE TNodeTy*       This() { return this->GetNode(); }

    FORCEINLINE TFactoryRetTy& Anchor(const LAnchor&      InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Anchor(const EAnchor::Type InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }

    FORCEINLINE TFactoryRetTy& MinDesiredSize(const LVector2& InSize) { this->This()->SetMinDesiredSize(InSize); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Visibility(const EWidgetVisibility::Type InVisibility) { this->This()->SetVisibility(InVisibility); return this->Self(); }

    template <typename T> FORCEINLINE auto SaveTo(T*& Out) -> TFactoryRetTy&;
    template <typename T> FORCEINLINE auto operator>>(T*& Out) -> TFactoryRetTy& { return this->SaveTo(std::forward<T*&>(Out)); }

    FORCEINLINE TFactoryRetTy& AddSibling(LWidgetFactory* InSibling);
    FORCEINLINE TFactoryRetTy& operator+(LWidgetFactory& InSibling) { return this->AddSibling(&InSibling); }
};

//#
//# Constructs a new widget node in the given context
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template <typename TNode>
FORCEINLINE auto ConstructWidgetNode(Private::LObjectContext* InContext) -> TNode*;
FORCEINLINE auto ConstructWidgetNode(Private::LObjectContext* InContext, const TSubclassOf<WWidgetNode>& InClass) -> WWidgetNode*;
//#
//# Constructs a new deferred widget node in the given context.
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode(Private::LObjectContext* InContext) -> TNode*;
FORCEINLINE auto ConstructDeferredWidgetNode(Private::LObjectContext* InContext, const TSubclassOf<WWidgetNode>& InClass) -> WWidgetNode*;

//#
//# Before constructing empty context widget, update the global specific widget context.
//# This behaves like a state machine.
//#
ENGINE_API extern Private::LObjectContext* GCurrentWidgetContextState;

//# Constructs a new widget node in the current context of the current program widget state context.
template <typename TNode>
FORCEINLINE auto ConstructWidgetNode() -> TNode*;
template <typename TNode>
FORCEINLINE auto ConstructWidgetNode(const TSubclassOf<TNode>& InClass) -> TNode*;
FORCEINLINE auto ConstructWidgetNode(const TSubclassOf<WWidgetNode>& InClass) -> WWidgetNode*;
//# Constructs a new deferred widget node in the current context of the current program widget state context.
template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode() -> TNode*;
template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode(const TSubclassOf<TNode>& InClass) -> TNode*;
FORCEINLINE auto ConstructDeferredWidgetNode(const TSubclassOf<WWidgetNode>& InClass) -> WWidgetNode*;
FORCEINLINE auto ConstructDeferredWidgetNodeImpl(const TSubclassOf<WWidgetNode>& InClass) -> WWidgetNode*;

//# Call this method to finalize a widget that was deferred.
FORCEINLINE void MakeDeferredWidgetNodeFinal(WWidgetNode* InNode);

struct LWidgetNodeData
{
    LName DerivedClass;
};

//#
//# The base class for everything that can be interpreted as a visual element.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactory, EClassFlags::Abstract)
class ENGINE_API WWidgetNode : public ::Jafg::Private::JObjectBase
{
    GENERATED_CLASS_BODY()

    friend WUserWidget;
    friend WWidgetParent;
    friend WWidgetParentBase;
    friend LWidgetConstructor;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetNode)

public:

    // JObjectBase implementation
    virtual void BeginLife() override final { Super::BeginLife(); this->Construct(); return; }
    virtual void EndLife() override final   { this->Destruct();   Super::EndLife();  return; }
    virtual void OnGarbage() override;
    // ~JObjectBase implementation

    //#
    //# Called when this widget is constructed. This does not mean being drawn to a canvas. A widget might be
    //# constructed but never dawned on a canvas in their entire lifespan. This method replaces the #BeginLife super
    //# method.
    //#
    virtual void Construct() { }

    //#
    //# Called when this widget is being ticked.
    //# See EWidgetVisibility for more information about when to tick a widget.
    //#
    virtual void Tick() { }

    //#
    //# Called when this widget is being destructed. This does not mean being removed from its parent. This method
    //# replaces the #EndLife super method.
    //#
    virtual void Destruct() { }

    //# Use this method to pass arbitrary typesafe data to the widget.
    virtual bool AddData(LWidgetNodeData* InData) { return false; }

            bool         IsInBounds(const LViewport& Context, const LVector2& InLocation) const;
    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2& InLocation);
    virtual LCursorReply OnCursorEnter() { return LCursorReply::Unhandled(); }
    virtual LCursorReply OnCursorMoved(const LVector2& InLocation) { return LCursorReply::Unhandled(); }
    virtual LCursorReply OnCursorLeave() { return LCursorReply::Unhandled(); }
    virtual LReply       SweepFocusTest(LViewport& Context, const LVector2& InLocation);
    virtual void         OnFocusReceived() { }
    virtual void         OnFocusLost() { }
    virtual LReply       OnKeyDown(LKeyEvent& InKeyEvent) { return LReply::Unhandled(); }
    virtual LReply       OnKeyUp(LKeyEvent& InKeyEvent) { return LReply::Unhandled(); }

            bool IsFocusWidget() const;
    virtual bool IsFocusWidgetTransitive() const; /* Slow. */
    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const;

    virtual auto Draw(LViewport& Context) const -> void { }

    FORCEINLINE auto ShouldNowTick() const -> bool { return ( this->bDisableTick == false ) && ( this->ShouldNowDraw() ); }
    FORCEINLINE auto GetRawShouldTick() const -> bool { return this->bDisableTick == false; }
    FORCEINLINE auto SetShouldTick(const bool bInShouldTick) -> void { this->bDisableTick = (bInShouldTick == false); }
    FORCEINLINE auto ShouldNowDraw() const -> bool { return EWidgetVisibility::IsDrawn(this->Visibility); }
    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE auto IsWidgetVisible() const -> bool { return this->ShouldNowDraw(); }
    FORCEINLINE auto IsHitTestable() const -> bool { return EWidgetVisibility::IsHitTestable(this->Visibility); }
    FORCEINLINE auto CanChildrenBeHitTestable() const -> bool { return EWidgetVisibility::IsDerivedHitTestable(this->Visibility); }
    FORCEINLINE auto ShouldCheckForInputs() const -> bool { return this->IsHitTestable() || this->CanChildrenBeHitTestable(); }
    FORCEINLINE auto IsVisible() const -> bool { return this->Visibility == EWidgetVisibility::Visible; }
    FORCEINLINE auto IsHidden() const -> bool { return this->Visibility == EWidgetVisibility::Hidden; }
    FORCEINLINE auto IsCollapsed() const -> bool { return this->Visibility == EWidgetVisibility::Collapsed; }
    FORCEINLINE auto IsDerivedHitTestInvisible() const -> bool { return this->Visibility == EWidgetVisibility::DerivedHitTestInvisible; }
    FORCEINLINE auto IsTransitiveHitTestInvisible() const -> bool { return this->Visibility == EWidgetVisibility::TransitiveHitTestInvisible; }
    FORCEINLINE auto IsIntransitiveHitTestInvisible() const -> bool { return this->Visibility == EWidgetVisibility::IntransitiveHitTestInvisible; }
                auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> void;

    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) { }

    //#
    //# Orphans the child from its parent widget.
    //# @param bDestroy If true, this child will be killed automatically by the butcher at his next sweep.
    //#
    virtual auto RemoveFromParent(const bool bDestroy = true) -> void;
            auto GetParent() const -> WWidgetParentBase*;

    //#
    //# Searches for a node in this widget tree. Only searches leafs that are drawn.
    //# @return True, if the target node exists in this widget tree and is visible.
    //#
    virtual bool FindNodeInVisiblePath(const WWidgetNode* InNode) const;

    //# @return The size of the current viewport in pixels.
    virtual auto GetViewportSize() const -> LIntVector2;
    virtual auto GetViewport() const -> LViewport*;
    virtual auto GetCheckedViewport() const -> LViewport*;
    virtual auto GetPanickedViewport() const -> LViewport*;

    //# @return The top left corner of this widget relative to its parent. If no parent, relative to the viewport.
    virtual auto GetRelativeTopLeftFromOuter(const WWidgetNode* WhoAsked) const -> LVector2;
    //#
    //# @param  WhoAsked The widget that asked for the relative top left. Must be a direct child.
    //# @return The top left corner relative to the most outer parent. In the best case, this should be the viewport,
    //#         although this is not guaranteed.
    //#
    virtual auto GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const -> LVector2;
    //# Virtual update method for the desired size. Automatically called.
    virtual void UpdateDesiredSize() const { }
            void SetDesiredSize(const LVector2& InSize) const;
    FORCEINLINE auto GetDesiredSize() const -> const LVector2& { return this->DesiredSize; }
    FORCEINLINE auto GetMinDesiredSize() const -> const LVector2& { return this->MinDesiredSize; }
    FORCEINLINE auto SetMinDesiredSize(const LVector2& InSize) -> void { this->MinDesiredSize = InSize; }

    //#
    //# @param WhoAsked The widget that asked for the anchored top left. Must be a direct child.
    //# @return The anchored top left corner of this widget relative to the most outer parent. In the best case,
    //#         this should be the viewport, although this is not guaranteed.
    //#
    virtual auto GetAnchoredTopLeftFromMostOuter(const LViewport& Context, const WWidgetNode* WhoAsked) const -> LVector2;
    //# Virtual update method for the anchored size. Automatically called.
    virtual void UpdateAnchoredSize(const LViewport& Context) const;
    virtual void UpdateAnchoredSizeOfChildren(const LViewport& Context) const { }
    FORCEINLINE auto SetAnchoredSize(const LVector2& InSize) const -> void { this->AnchoredSize = InSize; }
    FORCEINLINE auto GetAnchoredSize() const -> LVector2 { return this->AnchoredSize; }

    FORCEINLINE auto GetSlot() const -> LWidgetSlot* { return this->Slot; }

    //#
    //# Prepare and use the factory for the given node. Only valid in the engine tick where the factory was requested
    //# for. A new factory has to be requested for every new widget node and if the Wdsmml syntax is used for a given
    //# node that is already living for an x amount of time.
    //#
    //# @tparam TNode The node to get the factory for.
    //# @return The factory for that node.
    //# @remark !!! Master thread only !!!
    //# @see    #TWidgetFactoryTy<TNode>
    //# @see    #Private::LWidgetFactoryUtility::MakeWidgetFactory<TNode>
    //#
    template <typename TNode>
    NODISCARD FORCEINLINE typename TNode::TWidgetFactory& GetFactory()
    {
        return ::Jafg::Private::LWidgetFactoryUtility::MakeWidgetFactory<TNode>(this);
    }

    FORCEINLINE auto GetAnchor()           ->       LAnchor& { return this->Anchor; }
    FORCEINLINE auto GetAnchor()     const -> const LAnchor& { return this->Anchor; }
    FORCEINLINE void SetAnchor(const LAnchor&      InAnchor) { this->Anchor = InAnchor; }
    FORCEINLINE void SetAnchor(const EAnchor::Type InAnchor) { this->Anchor = InAnchor; }

    auto GetApplicationInstance() const -> LApplicationInstance*;
    auto GetEngine() const -> LEngine*;
    auto GetLocalEgo() const -> LLocalEgo*;

private:

    bool bDisableTick = false;
    EWidgetVisibility::Type Visibility = EWidgetVisibility::TransitiveHitTestInvisible;

    //#
    //# The slot that this widget is currently in. Might be null if the widget is a standalone.
    //# This class is not the owner of this slot. But the parent holding the child is.
    //#
    LWidgetSlot* Slot = nullptr;

    //#
    //# The desired size of this widget.
    //#
    mutable LVector2 DesiredSize = LVector2::Zero();

    //#
    //# The minimum content area.
    //#
    LVector2 MinDesiredSize = LVector2::Zero();

    //#
    //# The anchored size of this widget.
    //#
    mutable LVector2 AnchoredSize = LVector2::Zero();

    LAnchor Anchor = EAnchor::TopLeft;
};

template <typename TInNode>
typename TInNode::TWidgetFactory& Private::LWidgetFactoryUtility::MakeWidgetFactory(const WWidgetNode* InNode)
{
    using TNode    = TInNode;
    using TFactory = typename TNode::TWidgetFactory;
    using TFacNode = typename TNode::TWidgetFactory::TNodeTy;

    static_assert(std::is_base_of_v<WWidgetNode, TNode>, "The node must be a widget node.");
    static_assert(std::is_base_of_v<WWidgetNode, TFacNode>, "The factory must be a widget factory.");

    check( DynamicCast<TNode>(InNode) )

    if (LWidgetFactory* Factory = FindOrNullWidgetFactory(InNode); Factory)
    {
        return *reinterpret_cast<TFactory*>(Factory);
    }

    TFactory* Factory = new TFactory();
    Factory->Node = const_cast<WWidgetNode*>(InNode);

    Private::AddWidgetFactory(Factory);

    return *Factory;
}

template <typename TNode>
template <typename T>
typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::SaveTo(T*& Out)
{
    static_assert(std::is_base_of_v<WWidgetNode, T>);
    static_assert(std::is_base_of_v<T, TNodeTy>);
    Out = this->GetNode();
    return this->Self();
}

template <typename TNode>
typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::AddSibling(LWidgetFactory* InSibling)
{
    check( this->Siblings.Contains(InSibling) == false )
    this->Siblings.Add(InSibling);
    return this->Self();
}

WWidgetNode* ConstructWidgetNode(Private::LObjectContext* InContext, const TSubclassOf<WWidgetNode>& InClass)
{
    WWidgetNode* Node = ConstructDeferredWidgetNode(InContext, InClass);
    ::Jafg::MakeDeferredWidgetNodeFinal(Node);
    return Node;
}

template <typename TNode>
TNode* ConstructWidgetNode(Private::LObjectContext* InContext)
{
    return CheckedStaticCast<TNode>(InContext, TNode::StaticClass());
}

template <typename TNode>
TNode* ConstructDeferredWidgetNode(Private::LObjectContext* InContext)
{
    return CheckedStaticCast<TNode>(ConstructDeferredWidgetNode(InContext, TNode::StaticClass()));
}

WWidgetNode* Jafg::ConstructDeferredWidgetNode(Private::LObjectContext* InContext, const TSubclassOf<WWidgetNode>& InClass)
{
    return NewDeferredObject<WWidgetNode, false, true>(InContext, InClass);
}

template <typename TNode>
TNode* ConstructWidgetNode()
{
    return ConstructWidgetNode(TNode::StaticClass());
}

template <typename TNode>
TNode* ConstructWidgetNode(const TSubclassOf<TNode>& InClass)
{
    return CheckedStaticCast<TNode>(ConstructWidgetNode(InClass));
}

WWidgetNode* ConstructWidgetNode(const TSubclassOf<WWidgetNode>& InClass)
{
    checkSlow( GCurrentWidgetContextState )
    return ConstructWidgetNode(GCurrentWidgetContextState, InClass);
}

template <typename TNode>
TNode* ConstructDeferredWidgetNode()
{
    return CheckedStaticCast<TNode>(ConstructDeferredWidgetNode(TNode::StaticClass()));
}

template <typename TNode>
TNode* ConstructDeferredWidgetNode(const TSubclassOf<TNode>& InClass)
{
    return CheckedStaticCast<TNode>(ConstructDeferredWidgetNodeImpl(InClass));
}

WWidgetNode* ConstructDeferredWidgetNode(const TSubclassOf<WWidgetNode>& InClass)
{
    return ConstructDeferredWidgetNodeImpl(InClass);
}

WWidgetNode* ConstructDeferredWidgetNodeImpl(const TSubclassOf<WWidgetNode>& InClass)
{
    checkSlow( GCurrentWidgetContextState )
    return ConstructDeferredWidgetNode(GCurrentWidgetContextState, InClass);
}

FORCEINLINE void MakeDeferredWidgetNodeFinal(WWidgetNode* InNode)
{
    checkSlow( InNode )
    MakeDeferredObjectFinal(InNode);
    return;
}

} /* ~Namespace Jafg */

///////////////////////////////////////////////////////////////////////////////
// Widget style domain-specific-macro language (Wsdsml)
//
// Use these macros to create complex widget structures.
// Example use cases can be found here:
// @see User/Frontend/Osd/DebugScreen.cpp
///////////////////////////////////////////////////////////////////////////////

#define MakeRootNode(TRoot)   (*this->ReplaceRoot(NewNodeNoFactory(TRoot))).GetFactory<TRoot>()
#define FinishWidgetStyling() ;MakeDeferredWidgetNodeFinal(this->GetRoot());

#define NewNodeNoFactory(TNode) (*ConstructDeferredWidgetNode<TNode>())
#define NewNode(TNode)          (*ConstructDeferredWidgetNode<TNode>()).GetFactory<TNode>()
