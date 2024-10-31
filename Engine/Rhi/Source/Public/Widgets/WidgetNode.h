// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "WidgetNode.generated.h"

namespace Jafg
{

class WWidgetParent;
class WUserWidget;
class LViewport;
class LSurface;
class WWidgetParentBase;
struct LWidgetConstructor;

namespace EWidgetVisibility
{

enum Type : uint8
{
    /** Visible, takes up space in the widget layout and hit-testable. */
    Visible,

    /** Not visible, takes up space in the widget layout and is not hit-testable. */
    Hidden,

    /** Not visible, does not take up space in the widget layout and is not hit-testable. */
    Collapsed,

    /** Visible, takes up space in the widget layout and is not hit-testable. */
    TransitiveHitTestInvisible,

    /** Visible, takes up space in the widget layout and is itself not hit-testable, but all children are. */
    IntransitiveHitTestInvisible,
};

} /* ~Namespace EWidgetVisibility */

DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class RHI_API WWidgetNode : public ::Jafg::Private::JObjectBase
{
    GENERATED_CLASS_BODY()

    friend LWidgetConstructor;
    friend WWidgetParent;
    friend WWidgetParentBase;
    friend WUserWidget;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetNode)

public:

    // JObjectBase implementation
    virtual void BeginLife() override final { Super::BeginLife(); this->Construct(); return; }
    virtual void EndLife() override final   { this->Destruct();   Super::EndLife();  return; }
    virtual void MarkAsGarbage() override;
    // ~JObjectBase implementation

    virtual void Construct();
    virtual void Tick();
    virtual void Destruct();

    virtual void Draw(LViewport* Context) const { }

    /** Weather this widget is allowed to tick this frame. */
    FORCEINLINE auto ShouldNowTick() const -> bool
    {
        return
               ( this->bDisableTick == false )
            && (
                   this->Visibility == EWidgetVisibility::Visible
                || this->Visibility == EWidgetVisibility::TransitiveHitTestInvisible
                || this->Visibility == EWidgetVisibility::IntransitiveHitTestInvisible
            );
    }

    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> void { this->Visibility = InVisibility; }

    /**
     * Removes this widget from its parent widget.
     * @param bDestroy If true, the widget will be destroyed automatically.
     */
    virtual void RemoveFromParent(const bool bDestroy = true);
    FORCEINLINE auto GetParent()   const -> WWidgetParentBase* { return this->Parent; }

    /** @return The size of the current viewport in pixels. */
    virtual auto GetViewportSize() const -> LIntVector2;

private:

    bool                    bDisableTick = false;
    EWidgetVisibility::Type Visibility   = EWidgetVisibility::Visible;

    WWidgetParentBase*       Parent       = nullptr;
};

template <typename TNode>
FORCEINLINE auto ConstructWidgetNode(Private::LObjectContext* InContext) -> TNode*
{
    return NewObject<TNode>(InContext);
}

template <typename TNode>
FORCEINLINE auto ConstructDeferredWidgetNode(Private::LObjectContext* InContext) -> TNode*
{
    return NewDeferredObject<TNode>(InContext);
}

/** Before constructing empty context widget, update the global specific widget context. */
RHI_API extern Private::LObjectContext* GCurrentWidgetContextState;
/** Constructs a new widget node in the current context of the current program widget state context. */
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

FORCEINLINE void ConstructDeferredWidgetNode(WWidgetNode* InNode)
{
    checkSlow( InNode != nullptr )
    InNode->BeginLife();
    return;
}

///////////////////////////////////////////////////////////////////////////////
// Widget style domain-specific-macro language (Wsdsml)
//
// Use these macros to create complex widget structures.
// Example use cases can be found here:
// @see Player/Frontend/DebugScreen.cpp
///////////////////////////////////////////////////////////////////////////////

#define MakeRootNode(TRoot)   (*this->ReplaceRoot(NewNode(TRoot)))
#define FinishWidgetStyling() ;ConstructDeferredWidgetNode(this->GetRoot());

#define NewNode(TNode) (*ConstructDeferredWidgetNode<TNode>())

} /* ~Namespace Jafg */
