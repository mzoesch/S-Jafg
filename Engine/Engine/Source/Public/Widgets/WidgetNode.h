// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "Widgets/Whitespace.h"
#include "Widgets/InterfaceTypes.h"
#include "WidgetNode.generated.h"

namespace Jafg
{

class WWidgetNode;
class WWidgetParent;
class WUserWidget;
class LViewport;
class LSurface;
class WWidgetParentBase;
class LApplicationInstance;
struct LWidgetConstructor;

/**
 * How to anchor a child to its parent if the parent can have children.
 */
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

        /**
         * Minimum to maximum.
         * Left, Top, Right, Bottom.
         */
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
    /**
     * Visible, takes up space in the widget layout and is hit-testable.
     * Widgets in this state will be ticked.
     */
    Visible,

    /**
     * Not visible, takes up space in the widget layout and is not hit-testable.
     * Widgets in this state will not be ticked.
     */
    Hidden,

    /**
     * Not visible, does not take up space in the widget layout and is not hit-testable.
     * Widgets in this state will not be ticked.
     */
    Collapsed,

    /**
     * Visible, takes up space in the widget layout and is not hit-testable.
     * Widgets in this state will be ticked.
     */
    TransitiveHitTestInvisible,

    /**
     * Visible, takes up space in the widget layout and is itself not hit-testable, but all children are.
     * Widgets in this state will be ticked.
     */
    IntransitiveHitTestInvisible,
};

} /* ~Namespace EWidgetVisibility */
ENGINE_API LSimpleString LexToString(const EWidgetVisibility::Type InVisibility);

/** The base struct for every widget slot. */
struct LWidgetSlot
{
    /**
     * The parent of this slot and the owner of the memory.
     */
    WWidgetParentBase* Parent;

    /**
     * The content of this slot. We interpret all names in this and derived structs as of the view of the content.
     */
    WWidgetNode* Content;

    /**
     * The padding of the parent widget aka the margin of the child widget.
     */
    LMargin* Margin;
};

/**
 * Constructs a new widget node in the given context
 * @see NewNode(TNode) (Wsdsml)
 * @see User/Frontend/DebugScreen.cpp (for usage example)
 */
template <typename TNode>
FORCEINLINE auto ConstructWidgetNode(Private::LObjectContext* InContext) -> TNode*;
/**
 * Constructs a new deferred widget node in the given context.
 * @see NewNode(TNode) (Wsdsml)
 * @see User/Frontend/DebugScreen.cpp (for usage example)
 */
template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode(Private::LObjectContext* InContext) -> TNode*;

/**
 * Before constructing empty context widget, update the global specific widget context.
 * This behaves like a state machine.
 */
ENGINE_API extern Private::LObjectContext* GCurrentWidgetContextState;

/** Constructs a new widget node in the current context of the current program widget state context. */
template <typename TNode>
FORCEINLINE auto ConstructWidgetNode() -> TNode*;
/** Constructs a new deferred widget node in the current context of the current program widget state context. */
template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode() -> TNode*;

/** Call this method to finalize a widget that was deferred. */
FORCEINLINE void MakeDeferredWidgetNodeFinal(WWidgetNode* InNode);

/**
 * The base class for everything that can be interpreted as a visual element.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
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

    virtual void Construct() { }
    virtual void Tick()      { }
    virtual void Destruct()  { }

    virtual void Draw(LViewport& Context) const { }

    /** Weather this widget is allowed to tick this frame. */
    FORCEINLINE auto ShouldNowTick() const -> bool;
    FORCEINLINE auto ShouldNowDraw() const -> bool { return this->ShouldNowTick(); }
    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> void { this->Visibility = InVisibility; }

    /**
     * Orphans the child from its parent widget.
     * @param bDestroy If true, this child will be killed automatically by the butcher at his next sweep.
     */
    virtual auto RemoveFromParent(const bool bDestroy = true) -> void;
            auto GetParent() const -> WWidgetParentBase*;

    /** @return The size of the current viewport in pixels. */
    virtual auto GetViewportSize() const -> LIntVector2;

    /** @return The top left corner of this widget relative to its parent. If no parent, relative to the viewport. */
    auto GetRelativeTopLeftFromOuter() const -> LVector2;

    /**
     * @param  WhoAsked The widget that asked for the relative top left. Must be a direct child.
     * @return The top left corner relative to the most outer parent. In the best case, this should be the viewport,
     *         although this is not guaranteed.
     */
    virtual auto GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const -> LVector2;

    virtual auto GetAnchoredTopLeftFromMostOuter(const LViewport& Context, const WWidgetNode* WhoAsked) const -> LVector2;

    FORCEINLINE auto GetSlot() const -> LWidgetSlot* { return this->Slot; }

    /** Virtual update method for the desired size. Automatically called. */
    virtual void UpdateDesiredSize() const { }
    FORCEINLINE auto SetDesiredSize(const LVector2& InSize) const -> void { this->DesiredSize = InSize; }
    FORCEINLINE auto GetDesiredSize() const -> const LVector2& { return this->DesiredSize; }
                auto GetAnchoredSize(const LViewport& Context) const -> LVector2;

    FORCEINLINE auto GetAnchor()           ->       LAnchor& { return this->Anchor; }
    FORCEINLINE auto GetAnchor()     const -> const LAnchor& { return this->Anchor; }
    FORCEINLINE auto SetAnchor(const LAnchor& InAnchor) -> WWidgetNode& { this->Anchor = InAnchor; return *this; }
    FORCEINLINE auto SetAnchor(const EAnchor::Type InAnchor) -> WWidgetNode& { this->Anchor = InAnchor; return *this; }

    LApplicationInstance* GetApplicationInstance() const;

private:

    bool bDisableTick = false;
    EWidgetVisibility::Type Visibility = EWidgetVisibility::Visible;

    /**
     * The slot that this widget is currently in. Might be null if the widget is a standalone.
     * This class is not the owner of this slot. But the parent holding the child is.
     */
    LWidgetSlot* Slot = nullptr;

    /**
     * The desired size of this widget.
     */
    mutable LVector2 DesiredSize = LVector2::Zero();
    LAnchor Anchor = EAnchor::TopLeft;
};

template <typename TNode>
FORCEINLINE auto ConstructWidgetNode(Private::LObjectContext* InContext) -> TNode*
{
    TNode* Node = ConstructDeferredWidgetNode<TNode>(InContext);
    ::Jafg::MakeDeferredWidgetNodeFinal(Node);
    return Node;
}

template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode(Private::LObjectContext* InContext) -> TNode*
{
    checkSlow( InContext )
    return NewDeferredObject<TNode, false, true>(InContext, TNode::StaticClass());
}

template <typename TNode>
FORCEINLINE auto ConstructWidgetNode() -> TNode*
{
    checkSlow( GCurrentWidgetContextState != nullptr )
    return ConstructWidgetNode<TNode>(GCurrentWidgetContextState);
}

template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode() -> TNode*
{
    checkSlow( GCurrentWidgetContextState != nullptr )
    return ConstructDeferredWidgetNode<TNode>(GCurrentWidgetContextState);
}

FORCEINLINE void MakeDeferredWidgetNodeFinal(WWidgetNode* InNode)
{
    checkSlow( InNode )
    MakeDeferredObjectFinal(InNode);
    return;
}

///////////////////////////////////////////////////////////////////////////////
// Widget style domain-specific-macro language (Wsdsml)
//
// Use these macros to create complex widget structures.
// Example use cases can be found here:
// @see User/Frontend/Osd/DebugScreen.cpp
///////////////////////////////////////////////////////////////////////////////

#define MakeRootNode(TRoot)   (*this->ReplaceRoot(NewNode(TRoot)))
#define FinishWidgetStyling() ;MakeDeferredWidgetNodeFinal(this->GetRoot());

#define NewNode(TNode) (*ConstructDeferredWidgetNode<TNode>())

} /* ~Namespace Jafg */

bool Jafg::WWidgetNode::ShouldNowTick() const
{
    return
        ( this->bDisableTick == false )
        && (
               this->Visibility == EWidgetVisibility::Visible
            || this->Visibility == EWidgetVisibility::TransitiveHitTestInvisible
            || this->Visibility == EWidgetVisibility::IntransitiveHitTestInvisible
        );
}
