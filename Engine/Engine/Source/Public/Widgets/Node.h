// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "Widgets/Whitespace.h"
#include "User/Input/Replies.h"
#include "User/Input/Events.h"
#include "Platform/SurfaceForward.h"
#include "Widgets/InterfaceTypes.h"
#include "Node.generated.h"

namespace Jafg
{

class LLocalEgo;
class WNode;
class WParent;
class WUserWidget;
class LViewport;
class LWidgetFactory;
class WParentBase;
template<typename TNode>
class TWidgetFactory;
template<typename TNode>
class TWidgetFactoryParentBase;
struct LWidgetSlot;
struct LWidgetConstructor;
struct LWidgetNodeData;

namespace Private
{

ENGINE_API void            AddWidgetFactory(LWidgetFactory* InFactory);
ENGINE_API LWidgetFactory* FindOrNullWidgetFactory(const void* InNode);
ENGINE_API LWidgetFactory& GetWidgetFactory(const void* InNode);
ENGINE_API i32             PurgeWidgetFactories();

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
    template<typename TNode>
    NODISCARD static auto MakeWidgetFactory(const WNode* InNode) -> typename TNode::TWidgetFactory&;
};

} /* ~Namespace Private */

//#
//# How to anchor a child to its parent if the parent can have children.
//# This enum can be used for simple and complex anchoring.
//#
namespace EAnchor
{

enum Type : u8
{
    //# Default behavior. Usually this is the VTop | HLeft.
    Default    = 0x0 << 0,

    //# The top on the vertical axis.
    VTop       = 0x1 << 0,
    //# The center on the vertical axis.
    VCenter    = 0x1 << 1,
    //# The bottom on the vertical axis.
    VBottom    = 0x1 << 2,
    //# The left on the horizontal axis.
    HLeft      = 0x1 << 3,
    //# The center on the horizontal axis.
    HCenter    = 0x1 << 4,
    //# The right on the horizontal axis.
    HRight     = 0x1 << 5,

    //# The vertical axis is stretched to the maximum.
    VFill      = 0x1 << 6,
    //# The horizontal axis is stretched to the maximum.
    HFill      = 0x1 << 7,

    //#
    //# Compounds.
    //#

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

//#
//# The anchor for complex anchoring only.
//# This struct may not be used when dealing with widgets that only support a simple layout flow.
//#
struct LAnchor final
{
    union
    {
        struct
        {
            f32 MinX;
            f32 MinY;
            f32 MaxX;
            f32 MaxY;
        };

        //#
        //# Minimum to maximum.
        //# Left, Top, Right, Bottom.
        //#
        LVector4 Anchors;
    };

    FORCEINLINE constexpr LAnchor() noexcept : Anchors(0.0f, 0.0f, 0.0f, 0.0f) { }
    FORCEINLINE constexpr LAnchor(const f32 InUniformAnchors) noexcept
        : Anchors(InUniformAnchors, InUniformAnchors, InUniformAnchors, InUniformAnchors)
    {
        check( this->IsNormalized() )
    }
    FORCEINLINE constexpr LAnchor(const f32 InHorizontalUniform, const f32 InVerticalUniform) noexcept
        : Anchors(InHorizontalUniform, InVerticalUniform, InHorizontalUniform, InVerticalUniform)
    {
        check( this->IsNormalized() )
    }
    FORCEINLINE constexpr LAnchor(const f32 InMinX, const f32 InMinY, const f32 InMaxX, const f32 InMaxY) noexcept
        : Anchors(InMinX, InMinY, InMaxX, InMaxY)
    {
        check( this->IsNormalized() )
    }
    FORCEINLINE constexpr LAnchor(const EAnchor::Type InAnchors) noexcept
    {
        this->Anchors = LVector4::ZeroVector;
        this->ApplyConstraints(InAnchors);
        return;
    }
    FORCEINLINE constexpr LAnchor(const LAnchor& InOther, const EAnchor::Type InConstraints) noexcept
    {
        this->Anchors = InOther.Anchors;
        this->ApplyConstraints(InConstraints);
        return;
    }
    FORCEINLINE constexpr LAnchor(const LVector4& InOther, const EAnchor::Type InConstraints) noexcept
    {
        this->Anchors = InOther;
        this->ApplyConstraints(InConstraints);
        return;
    }
    FORCEINLINE constexpr LAnchor(const LVector4& InAnchors) noexcept : Anchors(InAnchors) { }
    FORCEINLINE constexpr LAnchor(const LAnchor& InOther) noexcept : Anchors(InOther.Anchors) { }
    FORCEINLINE constexpr LAnchor(LAnchor&& InOther) noexcept : Anchors(std::move(InOther.Anchors)) { }
    FORCEINLINE constexpr LAnchor& operator=(const LAnchor& InOther)  noexcept { this->Anchors = InOther.Anchors; return *this; }
    FORCEINLINE constexpr LAnchor& operator=(LAnchor&& InOther) noexcept { this->Anchors = std::move(InOther.Anchors); return *this; }

    ENGINE_API bool IsNormalized() const noexcept;
    ENGINE_API void Normalize() noexcept;

    FORCEINLINE constexpr bool IsPushedHorizontal() const noexcept { return this->MinX > 0.0f; }
    FORCEINLINE constexpr bool IsPushedVertical() const noexcept { return this->MinY > 0.0f; }
    FORCEINLINE constexpr bool IsStretchedHorizontal() const noexcept { return this->MaxX > 0.0f; }
    FORCEINLINE constexpr bool IsStretchedVertical() const noexcept { return this->MaxY > 0.0f; }

    FORCEINLINE constexpr bool IsPushed() const noexcept { return this->IsPushedHorizontal() || this->IsPushedVertical(); }
    FORCEINLINE constexpr bool IsStretched() const noexcept { return this->IsStretchedHorizontal() || this->IsStretchedVertical(); }

    constexpr void ApplyConstraints(const EAnchor::Type InConstraints) noexceptcheck
    {
        if (InConstraints & EAnchor::VTop)    { this->Anchors += LAnchor::VTop.Anchors;    }
        if (InConstraints & EAnchor::VCenter) { this->Anchors += LAnchor::VCenter.Anchors; }
        if (InConstraints & EAnchor::VBottom) { this->Anchors += LAnchor::VBottom.Anchors; }
        if (InConstraints & EAnchor::HLeft)   { this->Anchors += LAnchor::HLeft.Anchors;   }
        if (InConstraints & EAnchor::HCenter) { this->Anchors += LAnchor::HCenter.Anchors; }
        if (InConstraints & EAnchor::HRight)  { this->Anchors += LAnchor::HRight.Anchors;  }
        if (InConstraints & EAnchor::VFill)   { this->Anchors += LAnchor::VFill.Anchors;   }
        if (InConstraints & EAnchor::HFill)   { this->Anchors += LAnchor::HFill.Anchors;   }

        check( this->IsNormalized() )

        return;
    }

    FORCEINLINE constexpr bool operator==(const LAnchor& InOther) const noexcept { return this->Anchors == InOther.Anchors; }
    FORCEINLINE constexpr bool operator!=(const LAnchor& InOther) const noexcept { return this->Anchors != InOther.Anchors; }

    FORCEINLINE LString ToString() const
    {
        return this->Anchors.ToString();
    }

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

enum Type : u8
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

FORCEINLINE constexpr bool IsDrawn(const EWidgetVisibility::Type InVisibility) noexcept
{
    return InVisibility != EWidgetVisibility::Hidden
        && InVisibility != EWidgetVisibility::Collapsed;
}

FORCEINLINE constexpr bool IsTicked(const EWidgetVisibility::Type InVisibility) noexcept
{
    return EWidgetVisibility::IsDrawn(InVisibility);
}

FORCEINLINE constexpr bool IsHitTestable(const EWidgetVisibility::Type InVisibility) noexcept
{
    return InVisibility == EWidgetVisibility::Visible
        || InVisibility == EWidgetVisibility::DerivedHitTestInvisible;
}

FORCEINLINE constexpr bool IsDerivedHitTestable(const EWidgetVisibility::Type InVisibility) noexcept
{
    return InVisibility == EWidgetVisibility::Visible
        || InVisibility == EWidgetVisibility::IntransitiveHitTestInvisible;
}

FORCEINLINE constexpr bool TransformsWidgetLayout(const EWidgetVisibility::Type InVisibility) noexcept
{
    return InVisibility != EWidgetVisibility::Collapsed;
}

} /* ~Namespace EWidgetVisibility */
ENGINE_API LString LexToString(const EWidgetVisibility::Type InVisibility);

//#
//# The base struct for every widget slot.
//#
struct LWidgetSlot final
{
    //#
    //# The parent of this slot and the owner of the memory.
    //#
    WParentBase* Parent{ nullptr };

    //#
    //# The content of this slot. We interpret all names in this and derived structs as of the view of the content.
    //#
    WNode* Content{ nullptr };

    //#
    //# The padding of the parent widget aka the margin of the child widget.
    //#
    LMargin* Margin{ nullptr };
};

//#
//# Base class of all widget factories.
//# @see TWidgetFactory<TNode> below.
//#
class LWidgetFactory
{
public:

    friend WNode;
    friend Private::LWidgetFactoryUtility;
    template<typename TNode>
    friend class TWidgetFactory;
    template<typename TNode>
    friend class TWidgetFactoryParentBase;

    FORCEINLINE WNode* GetNodeRaw() const noexcept { check( this->Node ) return this->Node; }

    FORCEINLINE bool HasAnySibling() const noexcept { return this->Siblings.empty() == false; }
    FORCEINLINE auto GetSiblings() const noexcept -> const TArray<LWidgetFactory*>& { return this->Siblings; }

private:

    FORCEINLINE TArray<LWidgetFactory*>& GetMutableSiblingsDangerous() noexcept { return this->Siblings; }

    WNode* Node { nullptr };
    TArray<LWidgetFactory*> Siblings;
};

//#
//# Base class of all widget factories that can be used with the declarative syntax defined by Wsdsml.
//#
template<typename TNode>
class TWidgetFactory : public LWidgetFactory
{
public:

    friend WNode;
    friend Private::LWidgetFactoryUtility;

    using Super         = LWidgetFactory;

    //# The node to target. Always valid.
    using TNodeTy       = TNode;
    //# The return type of the factory. Always valid.
    using TFactoryRetTy = typename TNodeTy::TWidgetFactory;

    FORCEINLINE TNodeTy* GetNode() const { return StaticCastChecked<TNodeTy>(this->GetNodeRaw()); }

    //# @return A pointer to self.
    FORCEINLINE TFactoryRetTy& Self() noexcept { return *static_cast<TFactoryRetTy*>(this); }
    //# @return A pointer to this.
    FORCEINLINE TNodeTy*       This() { return this->GetNode(); }

    FORCEINLINE TFactoryRetTy& Anchor(const LAnchor&      InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Anchor(const EAnchor::Type InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }

    FORCEINLINE TFactoryRetTy& MinDesiredSize(const LVector2& InSize) { this->This()->SetMinDesiredSize(InSize); return this->Self(); }
    FORCEINLINE TFactoryRetTy& MaxDesiredSize(const LVector2& InSize) { this->This()->SetMaxDesiredSize(InSize); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Visibility(const EWidgetVisibility::Type InVisibility) { this->This()->SetVisibility(InVisibility); return this->Self(); }

    template<typename T> FORCEINLINE auto SaveTo(T** Out) -> TFactoryRetTy&;
    template<typename T> FORCEINLINE auto operator>>(T** Out) -> TFactoryRetTy& { return this->SaveTo(std::forward<T*&>(Out)); }

    FORCEINLINE TFactoryRetTy& AddSibling(LWidgetFactory* InSibling);
    FORCEINLINE TFactoryRetTy& operator+(LWidgetFactory& InSibling) { return this->AddSibling(&InSibling); }
    FORCEINLINE TFactoryRetTy& operator+(LWidgetFactory* InSibling) { return this->AddSibling(InSibling); }

    FORCEINLINE TFactoryRetTy& Data(const LWidgetNodeData* InData) { this->This()->AddData(InData); return this->Self(); }
};

//#
//# The body of every factory deriving from TWidgetFactory<TNode>.
//#
#define GENERATED_FACTORY_BODY(TSuper)                   \
    using Super         = TSuper<TNode>;                 \
    using TFactoryRetTy = typename Super::TFactoryRetTy; \

//#
//# Constructs a new widget node in the given outer.
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template<typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LClassOuter* Outer);
template<typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LClassOuter* Outer, TSubclassOf<TNode> const& Class);

//#
//# Constructs a new deferred widget node in the given context.
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template<typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LClassOuter* Outer);
template<typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LClassOuter* Outer, TSubclassOf<TNode> const& Class);

//# Call this method to finalize a widget that was deferred.
FORCEINLINE void MakeDeferredWidgetNodeFinal(WNode* Node);

struct LWidgetNodeData
{
    LName DerivedClass;
};

//#
//# The base class for everything that can be interpreted as a visual element.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactory, ECxxClassFlags::Abstract)
class WNode : public JCxxClass
{
    GENERATED_CLASS_BODY()

    friend WUserWidget;
    friend LWidgetConstructor;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WNode)

public:

#if LAL_DO_CHECKS
    virtual ~WNode() override;
#endif /* LAL_DO_CHECKS */

    // JObjectBase implementation
    virtual void BeginLife() override final { Super::BeginLife(); this->Construct(); return; }
    virtual void EndLife() override final   { this->Destruct();   Super::EndLife();  return; }
    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override;
    // ~JObjectBase implementation

    //#
    //# Called when this widget is constructed. This does not mean being drawn to a canvas. A widget might be
    //# constructed but never drawn on a canvas in their entire lifespan. This method replaces the #BeginLife super
    //# method.
    //#
    virtual void Construct() { }

    //#
    //# Called when this widget is being ticked.
    //# See #EWidgetVisibility for more information about when to tick a widget.
    //#
    virtual void Tick() { }

    //#
    //# Called when this widget is being destructed. This does not mean being removed from its parent. This method
    //# replaces the #EndLife super method.
    //#
    virtual void Destruct() { }

    //#
    //# The paint function for a widget. Only called if the widget is visible and paintable.
    //# Do not update any values of any widgets when inside this method.
    //# Automatically called by the owning viewport. Do not call manually.
    //#
    virtual void Draw(LViewport& Context) const { check( this->ShouldNowDraw() ) }

    //#
    //# Use this method to pass arbitrary typesafe data to the widget.
    //# @return True, if the data was used successfully handled.
    //#
    virtual bool AddData(const LWidgetNodeData* InData) { return false; }

    bool IsInBounds(const LViewport& Context, const LVector2& InLocation) const;
    virtual LCursorReply SweepMouse(LViewport& Context, const LVector2& InLocation);

    virtual LCursorReply OnCursorEnter() { return LCursorReply::Unhandled(); }
    virtual LCursorReply OnCursorMoved(const LVector2& InLocation) { return LCursorReply::Unhandled(); }
    virtual LCursorReply OnCursorLeave() { return LCursorReply::Unhandled(); }

    virtual LReply SweepFocusTest(const LViewport& Context, const LVector2& InLocation);

    //#
    //# Called if this widget is being focused / unfocused. Extra care is given by the owing viewport for special
    //# events, only receivable by a focused widget. This includes e.g., platform buffered input.
    //# You may decide to bubble this event up or down.
    //# Note that the #WParent node will not bubble this event in any direction as it is usually meant for the
    //# most inner node only.
    //#
    virtual void OnFocusReceived() { }
    virtual void OnFocusLost() { }

    //#
    //# These events are for the focused widget only. You may want to bubble these events down to children if you want.
    //# If these events are unhandled by the currently focused derived class, they will be bubbled up to the most
    //# outer parent.
    //#
    //# Therefore, you should first handle the reply if possible, and if not, you should call the super method.
    //#
    //# If a key event is unhandled here, other widgets will be able to receive it through the #OnKeyDownNoFocus and
    //# #OnKeyUpNoFocus methods.
    //#
    //# @remark This key event includes repeated key events. Make sure to filter them accordingly.
    //#
    virtual LReply OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent);
    virtual LReply OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent);

    //#
    //# These events are meant to be bubbled from the parent down to the most outer children. If a child does handle
    //# the event call, the reply should be returned; if not, the direct parent is eligible to handle it.
    //#
    //# Therefore, you should only handle the reply in the derived class if it was not handled in the
    //# super method call expression.
    //# Be aware: This is the exact opposite behavior to the #OnKeyDown and #OnKeyUp methods.
    //#
    //# @remark This key event includes repeated key events. Make sure to filter them accordingly.
    //#
    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent);
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent);

    //#
    //# @return Whether this is the focused widget.
    //# @remark This method is fairly slow. Consider young the #IsFocusWidget with a viewport parameter if cached.
    //#
    bool IsFocusWidget() const;
    //#
    //# @param InViewport The viewport to check for or null.
    //# @return Whether this is the focused widget.
    //#
    bool IsFocusWidget(const LViewport* InViewport) const;

    //#
    //# @return Whether this is the focused widget or any of its children.
    //# @remark This method is fairly slow. Consider young the #IsFocusWidgetTransitive with a viewport
    //#         parameter if cached.
    //#
    bool IsFocusWidgetTransitive() const; /* Warning: Slow. */
    //#
    //# @param InViewport The viewport to check for or null.
    //# @return Whether this is the focused widget or any of its children.
    //#
    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const;

    FORCEINLINE bool ShouldNowTick() const { return this->bAllowTick && EWidgetVisibility::IsTicked(this->Visibility); }
    FORCEINLINE bool GetRawShouldTick() const { return this->bAllowTick; }
    FORCEINLINE void SetShouldTick(const bool bInShouldTick) { this->bAllowTick = bInShouldTick; }
    FORCEINLINE bool ShouldNowDraw() const { return EWidgetVisibility::IsDrawn(this->Visibility); }
    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE bool IsPainted() const { return this->ShouldNowDraw(); }
    FORCEINLINE bool IsHitTestable() const { return EWidgetVisibility::IsHitTestable(this->Visibility); }
    FORCEINLINE bool CanChildrenBeHitTestable() const { return EWidgetVisibility::IsDerivedHitTestable(this->Visibility); }
    FORCEINLINE bool ShouldCheckForInputs() const { return this->IsHitTestable() || this->CanChildrenBeHitTestable(); }
    FORCEINLINE bool TransformsWidgetLayout() const { return EWidgetVisibility::TransformsWidgetLayout(this->Visibility); }
    FORCEINLINE bool IsVisible() const { return this->Visibility == EWidgetVisibility::Visible; }
    FORCEINLINE bool IsHidden() const { return this->Visibility == EWidgetVisibility::Hidden; }
    FORCEINLINE bool IsCollapsed() const { return this->Visibility == EWidgetVisibility::Collapsed; }
    FORCEINLINE bool IsDerivedHitTestInvisible() const { return this->Visibility == EWidgetVisibility::DerivedHitTestInvisible; }
    FORCEINLINE bool IsTransitiveHitTestInvisible() const { return this->Visibility == EWidgetVisibility::TransitiveHitTestInvisible; }
    FORCEINLINE bool IsIntransitiveHitTestInvisible() const { return this->Visibility == EWidgetVisibility::IntransitiveHitTestInvisible; }
                void SetVisibility(const EWidgetVisibility::Type InVisibility);

    //# Only if old and new are different.
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) { }

    //#
    //# Orphans this child from its parent widget.
    //# @param bDestroy If true, this child will be killed automatically by the butcher at his next sweep.
    //#
    virtual void RemoveFromParent(const bool bDestroy = true); // TODO WARNING Will currently always destroy the child.
    FORCEINLINE WParentBase* GetParent() { return this->Slot.Parent; }
    FORCEINLINE WParentBase* GetParentChecked() { WParentBase* Out = this->GetParent(); check( Out ); return Out; }
    FORCEINLINE WParentBase* GetParentAsserted() { WParentBase* Out = this->GetParent(); jassert( Out ); return Out; }
    FORCEINLINE const WParentBase* GetParent() const { return this->Slot.Parent; }
    FORCEINLINE const WParentBase* GetParentChecked() const { const WParentBase* Out = this->GetParent(); check( Out ); return Out; }
    FORCEINLINE const WParentBase* GetParentAsserted() const { const WParentBase* Out = this->GetParent(); jassert( Out ); return Out; }
    //# @return The most outer parent of this widget or the widget itself if no outer parent.
          WNode* GetMostOuterParent();
    const WNode* GetMostOuterParent() const;

    //#
    //# Searches for a node in this widget tree. Only searches nodes that are drawn.
    //# @return True if the target node exists in this widget tree and is visible.
    //#
    virtual bool FindNodeInVisiblePath(const WNode* InNode) const;

    //# @return The size of the current viewport in pixels.
    LIntVector2 GetViewportSize() const;
                virtual LViewport* GetViewport() const;
    FORCEINLINE virtual LViewport* GetViewportChecked() const { LViewport* Out = this->GetViewport(); check( Out ) return Out; }
    FORCEINLINE virtual LViewport* GetViewportAsserted() const { LViewport* Out = this->GetViewport(); jassert( Out ) return Out; }

    //# Virtual update method for the desired size. Automatically called. Do not call manually.
    virtual void UpdateDesiredSize() const { }
    //#
    //# Update the #DesiredSize of a widget inside the overridden #UpdateDesiredSize method with this one.
    //# Do not call this method from outside the #UpdateDesiredSize method.
    //#
    void SetDesiredSize(const LVector2& InSize) const;
    //# Internal usage only. Do not use.
    FORCEINLINE void SetDesiredSizeRaw(const LVector2& InSize) const { this->DesiredSize = InSize; return; }
    FORCEINLINE const LVector2& GetDesiredSize() const { return this->DesiredSize; }
    FORCEINLINE const LVector2& GetDesiredSizeSmart() const { return this->TransformsWidgetLayout() ? this->DesiredSize : LVector2::ZeroVector; }
    //# The min desired size. A widget will always be at least this size.
    FORCEINLINE void SetMinDesiredSize(const LVector2& InSize) { this->MinDesiredSize = InSize; return; }
    FORCEINLINE const LVector2& GetMinDesiredSize() const { return this->MinDesiredSize; }
    //# The max desired size. A widget will have at maximum this size. Zero means unbound. This includes max size of anchored nodes.
    FORCEINLINE void SetMaxDesiredSize(const LVector2& InSize) { this->MaxDesiredSize = InSize; return; }
    FORCEINLINE const LVector2& GetMaxDesiredSize() const { return this->MaxDesiredSize; }

    //# Virtual update method for the anchored size. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSize(const LViewport& Context) const;
    //# Virtual update method for the anchored size of a child. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const PURE_VIRTUAL()
    void SetAnchoredSize(const LVector2& InSize) const;
    void SetAnchoredSize(LVector2&& InSize) const;
    FORCEINLINE const LVector2& GetAnchoredSize() const { return this->AnchoredSize; }
    FORCEINLINE LVector2 CopyAnchoredSize() const { return this->AnchoredSize; }
    //# The anchored size that was lost during #MaxDesiredSize clamp.
    FORCEINLINE const LVector2& GetLostAnchoredSize() const { return this->LostAnchoredSize; }
    FORCEINLINE LVector2 CopyLostAnchoredSize() const { return this->LostAnchoredSize; }
    //# @return The anchored top-left corner of the widget relative to the given context's top-left corner.
    virtual LVector2 GetAnchoredTopLeftFromMostOuter(const LViewport& Context) const;
    LVector2 GetAnchoredAndTranslatedTopLeftFromMostOuter(const LViewport& Context) const;
    //# @return The anchored top-left corner of the direct child relative to the given context's top-left corner.
    virtual LVector2 GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const PURE_VIRTUAL(return { })

    FORCEINLINE bool IsSlotValid() const noexcept { return this->Slot.Parent != nullptr; }
    FORCEINLINE LWidgetSlot& GetMutableSlot() noexcept { return this->Slot; }
    FORCEINLINE LWidgetSlot& GetMutableSlotChecked() noexceptcheck { check( this->IsSlotValid() ) return this->GetMutableSlot(); }
    FORCEINLINE LWidgetSlot& GetMutableSlotAsserted() { jassert( this->IsSlotValid() ) return this->GetMutableSlot(); }
    FORCEINLINE LWidgetSlot const& GetSlot() const noexcept { return this->Slot; }
    FORCEINLINE LWidgetSlot const& GetSlotChecked() const noexceptcheck { check( this->IsSlotValid() ) return this->GetSlot(); }
    FORCEINLINE LWidgetSlot const& GetSlotAsserted() const { jassert( this->IsSlotValid() ) return this->GetSlot(); }
    FORCEINLINE TOptional<LMargin> GetMargin() const noexcept { if (this->Slot.Margin) { check( this->Slot.Parent ) return *this->Slot.Margin; } return { }; }
    FORCEINLINE TOptional<LMargin> GetMarginChecked() const noexceptcheck { TOptional Out{ this->GetMargin() }; check( Out.has_value() ); return Out; }
    FORCEINLINE TOptional<LMargin> GetMarginAsserted() const { TOptional Out{ this->GetMargin() }; jassert( Out.has_value() ); return Out; }
                bool SetMargin(const LMargin& InMargin) noexcept;
    FORCEINLINE bool SetMarginChecked(const LMargin& InMargin) noexceptcheck { const bool Out = this->SetMargin(InMargin); check( Out ); return Out; }
    FORCEINLINE bool SetMarginAsserted(const LMargin& InMargin) { const bool Out = this->SetMargin(InMargin); jassert( Out ); return Out; }
    FORCEINLINE void InvalidateSlotDangerous() noexceptcheck { check( this->IsSlotValid() ) this->Slot.Parent = nullptr; this->Slot.Content = nullptr; this->Slot.Margin = nullptr; }

    //#
    //# Prepare and use the factory for the given node. Only valid in the engine tick where the factory was requested
    //# for. A new factory has to be requested for every new widget node and if the Wsdsml syntax is used for a given
    //# node that is already living for an x amount of time.
    //#
    //# @tparam TNode The node to get the factory for.
    //# @return The factory for that node.
    //# @remark !!! Master thread only !!!
    //# @see    #TWidgetFactoryTy<TNode>
    //# @see    #Private::LWidgetFactoryUtility::MakeWidgetFactory<TNode>
    //#
    template<typename TNode>
    NODISCARD FORCEINLINE typename TNode::TWidgetFactory& GetFactory()
    {
        return ::Jafg::Private::LWidgetFactoryUtility::MakeWidgetFactory<TNode>(this);
    }

    FORCEINLINE auto GetAnchor()           ->       LAnchor& { return this->Anchor; }
    FORCEINLINE auto GetAnchor()     const -> const LAnchor& { return this->Anchor; }
    FORCEINLINE void SetAnchor(const LAnchor&      InAnchor) { this->Anchor = InAnchor; }
    FORCEINLINE void SetAnchor(const EAnchor::Type InAnchor) { this->Anchor = InAnchor; }

private:

    bool bAllowTick { true };
    EWidgetVisibility::Type Visibility { EWidgetVisibility::TransitiveHitTestInvisible };

    //#
    //# The slot that this widget is currently in.
    //#
    LWidgetSlot Slot;

    //#
    //# The desired size of this widget.
    //#
    mutable LVector2 DesiredSize;

    //#
    //# The minimum content area.
    //#
    LVector2 MinDesiredSize;

    //#
    //# The maximal content area. Zero means unbound. This includes max size of anchored nodes.
    //#
    LVector2 MaxDesiredSize;

    //#
    //# The anchored size of this widget.
    //#
    mutable LVector2 AnchoredSize;

    //#
    //# The anchored size that was lost during #MaxDesiredSize clamp.
    //#
    mutable LVector2 LostAnchoredSize;

    LAnchor Anchor{ EAnchor::TopLeft };
};

template<typename TInNode>
typename TInNode::TWidgetFactory& Private::LWidgetFactoryUtility::MakeWidgetFactory(const WNode* InNode)
{
    using TNode    = TInNode;
    using TFactory = typename TNode::TWidgetFactory;
    using TFacNode = typename TNode::TWidgetFactory::TNodeTy;

    static_assert(std::is_base_of_v<WNode, TNode>, "The node must be a widget node.");
    static_assert(std::is_base_of_v<WNode, TFacNode>, "The factory must be a widget factory.");

    check( DynamicCast<TNode>(InNode) )

    if (LWidgetFactory* Factory = FindOrNullWidgetFactory(InNode); Factory)
    {
        return *reinterpret_cast<TFactory*>(Factory);
    }

    TFactory* Factory = new TFactory();
    Factory->Node = const_cast<WNode*>(InNode);

    Private::AddWidgetFactory(Factory);

    return *Factory;
}

template<typename TNode>
template<typename T>
FORCEINLINE typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::SaveTo(T** Out)
{
    static_assert(std::is_base_of_v<WNode, T>);
    static_assert(std::is_base_of_v<T, TNodeTy>);
    if (Out)
    {
        *Out = this->GetNode();
    }

    return this->Self();
}

template<typename TNode>
FORCEINLINE typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::AddSibling(LWidgetFactory* InSibling)
{
    check( algo::contains(this->Siblings, InSibling) == false )
    this->Siblings.push_back(InSibling);
    return this->Self();
}

template<typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LClassOuter* Outer)
{
    return ConstructWidgetNode<TNode>(Outer, TNode::StaticClass());
}

template<typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LClassOuter* Outer, TSubclassOf<TNode> const& Class)
{
    TNode* Out{ ConstructDeferredWidgetNode<TNode>(Outer, Class) };
    MakeDeferredWidgetNodeFinal(Out);
    return Out;
}

template<typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LClassOuter* Outer)
{
    return ConstructDeferredWidgetNode<TNode>(Outer, TNode::StaticClass());
}

template<typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LClassOuter* Outer, TSubclassOf<TNode> const& Class)
{
    return StaticCastChecked<TNode>(NewDeferredObject(Outer, *Class.GetClass()));
}

FORCEINLINE void MakeDeferredWidgetNodeFinal(WNode* Node)
{
    MakeDeferredObjectFinal(Node);
}

#if !LAL_DO_CHECKS
FORCEINLINE LReply WNode::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    return LReply::Unhandled();
}

FORCEINLINE LReply WNode::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    return LReply::Unhandled();
}
#endif /* !LAL_DO_CHECKS */

} /* ~Namespace Jafg */

///////////////////////////////////////////////////////////////////////////////
// Widget style domain-specific-macro language (Wsdsml)
//
// Use these macros to create complex widget structures.
// Example use cases can be found here:
// @see User/Frontend/Osd/DebugScreen.cpp
///////////////////////////////////////////////////////////////////////////////

//# Make a new root node. In #UserWidgets only.
#define MakeRootNode(TRoot)                 (*this->ReplaceRoot(NewNodeNoFactory(TRoot))).GetFactory<TRoot>()
//# End the new root node declaration started in #MakeRootNode.
#define FinishWidgetStyling()               ;MakeDeferredWidgetNodeFinal(this->GetRoot());

//# Make a new node. Can be used everywhere.
#define NewNodeNoFactory(TNode)             (ConstructDeferredWidgetNode<TNode>(this->GetOuter()))
#define NewNodeNoFactoryCtx(Ctx, TNode)     (ConstructDeferredWidgetNode<TNode>((Ctx)->GetOuter()))

//# Make a new node. Can be used everywhere.
#define NewNode(TNode)                      (*ConstructDeferredWidgetNode<TNode>(this->GetOuter())).GetFactory<TNode>()
#define NewNodeCtx(Ctx, TNode)              (*ConstructDeferredWidgetNode<TNode>((Ctx)->GetOuter())).GetFactory<TNode>()
//# End the new node declaration started in #NewNodeNoFactory or #NewNode or #NewNodeCtx.
#define FinishWidget(Root)                  ;MakeDeferredWidgetNodeFinal(Root)
