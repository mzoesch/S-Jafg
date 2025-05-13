// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
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
class LWidgetFactory;
class WParentBase;
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
    template <typename TNode>
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
struct LWidgetSlot
{
    //#
    //# The parent of this slot and the owner of the memory.
    //#
    WParentBase* Parent;

    //#
    //# The content of this slot. We interpret all names in this and derived structs as of the view of the content.
    //#
    WNode* Content;

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

    friend WNode;
    friend Private::LWidgetFactoryUtility;
    template <typename TNode>
    friend class TWidgetFactory;
    template <typename TNode>
    friend class TWidgetFactoryParentBase;

    FORCEINLINE WNode* GetNodeRaw() const noexcept { check( this->Node ) return this->Node; }

    FORCEINLINE bool HasAnySibling() const noexcept { return this->Siblings.IsEmpty() == false; }
    FORCEINLINE auto GetSiblings() const noexcept -> const TArray<LWidgetFactory*>& { return this->Siblings; }

private:

    FORCEINLINE TArray<LWidgetFactory*>& GetMutableSiblingsDangerous() noexcept { return this->Siblings; }

    WNode* Node = nullptr;
    TArray<LWidgetFactory*> Siblings;
};

//#
//# Base class of all widget factories that can be used with the declarative syntax defined by Wsdsml.
//#
template <typename TNode>
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

    FORCEINLINE TNodeTy* GetNode() const { return CheckedStaticCast<TNodeTy>(this->GetNodeRaw()); }

    //# @return A pointer to self.
    FORCEINLINE TFactoryRetTy& Self() noexcept { return *static_cast<TFactoryRetTy*>(this); }
    //# @return A pointer to this.
    FORCEINLINE TNodeTy*       This() { return this->GetNode(); }

    FORCEINLINE TFactoryRetTy& Anchor(const LAnchor&      InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Anchor(const EAnchor::Type InAnchor) { this->This()->SetAnchor(InAnchor); return this->Self(); }

    FORCEINLINE TFactoryRetTy& MinDesiredSize(const LVector2& InSize) { this->This()->SetMinDesiredSize(InSize); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Visibility(const EWidgetVisibility::Type InVisibility) { this->This()->SetVisibility(InVisibility); return this->Self(); }

    template <typename T> FORCEINLINE auto SaveTo(T** Out) -> TFactoryRetTy&;
    template <typename T> FORCEINLINE auto operator>>(T** Out) -> TFactoryRetTy& { return this->SaveTo(std::forward<T*&>(Out)); }

    FORCEINLINE TFactoryRetTy& AddSibling(LWidgetFactory* InSibling);
    FORCEINLINE TFactoryRetTy& operator+(LWidgetFactory& InSibling) { return this->AddSibling(&InSibling); }
};

//#
//# The body of every factory deriving from TWidgetFactory<TNode>.
//#
#define GENERATED_FACTORY_BODY(TSuper)                   \
    using Super         = TSuper<TNode>;                 \
    using TFactoryRetTy = typename Super::TFactoryRetTy; \

//#
//# Constructs a new widget node in the given context
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template <typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LObjectContext* InContext);
template <typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LObjectContext* InContext, const TSubclassOf<TNode>& InClass);

//#
//# Constructs a new deferred widget node in the given context.
//# @see NewNode(TNode) (Wsdsml)
//# @see User/Frontend/DebugScreen.cpp (for usage example)
//#
template <typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LObjectContext* InContext);
template <typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LObjectContext* InContext, const TSubclassOf<TNode>& InClass);

//# Call this method to finalize a widget that was deferred.
FORCEINLINE void MakeDeferredWidgetNodeFinal(WNode* InNode);

struct LWidgetNodeData
{
    LName DerivedClass;
};

//#
//# The base class for everything that can be interpreted as a visual element.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactory, EClassFlags::Abstract)
class ENGINE_API WNode : public JObjectBase
{
    GENERATED_CLASS_BODY()

    friend WUserWidget;
    friend LWidgetConstructor;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WNode)

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

    virtual void Draw(LViewport& Context) const { check( this->ShouldNowDraw() ) }

    //#
    //# Use this method to pass arbitrary typesafe data to the widget.
    //# @return True, if the data was used successfully.
    //#
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
    bool IsFocusWidgetTransitive() const; /* Warning: Slow. */
    virtual bool IsFocusWidgetTransitive(const LViewport* InViewport) const;

    FORCEINLINE bool ShouldNowTick() const { return this->bAllowTick && EWidgetVisibility::IsTicked(this->Visibility); }
    FORCEINLINE bool GetRawShouldTick() const { return this->bAllowTick; }
    FORCEINLINE void SetShouldTick(const bool bInShouldTick) { this->bAllowTick = bInShouldTick; }
    FORCEINLINE bool ShouldNowDraw() const { return EWidgetVisibility::IsDrawn(this->Visibility); }
    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE bool IsWidgetVisible() const { return this->ShouldNowDraw(); }
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
    virtual     void RemoveFromParent(const bool bDestroy = true);
                auto GetParent() const -> WParentBase*;
    FORCEINLINE auto GetParentChecked() const -> WParentBase* { WParentBase* Out = this->GetParent(); check( Out ); return Out; }
    FORCEINLINE auto GetParentAsserted() const -> WParentBase* { WParentBase* Out = this->GetParent(); jassert( Out ); return Out; }

    //#
    //# Searches for a node in this widget tree. Only searches nodes that are drawn.
    //# @return True, if the target node exists in this widget tree and is visible.
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
    FORCEINLINE void SetDesiredSizeRaw(const LVector2& InSize) const { this->DesiredSize = InSize; }
    FORCEINLINE const LVector2& GetDesiredSize() const { return this->DesiredSize; }
    FORCEINLINE const LVector2& GetDesiredSizeSmart() const { return this->TransformsWidgetLayout() ? this->DesiredSize : LVector2::ZeroVector; }
    FORCEINLINE const LVector2& GetMinDesiredSize() const { return this->MinDesiredSize; }
    //# The min desired size. A widget will always be at least this size.
    FORCEINLINE void SetMinDesiredSize(const LVector2& InSize) { this->MinDesiredSize = InSize; return; }

    //# Virtual update method for the anchored size. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSize(const LViewport& Context) const;
    //# Virtual update method for the anchored size of a child. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const PURE_VIRTUAL()
    FORCEINLINE void SetAnchoredSize(const LVector2& InSize) const { this->AnchoredSize = InSize; }
    FORCEINLINE void SetAnchoredSize(LVector2&& InSize) const { this->AnchoredSize = std::move(InSize); }
    FORCEINLINE auto GetAnchoredSize() const -> LVector2 { return this->AnchoredSize; }
    //# @return The anchored top left corner of the widget relative to the given context's top left corner.
    virtual LVector2 GetAnchoredTopLeftFromMostOuter(const LViewport& Context) const;
    //# @return The anchored top left corner of the direct child relative to the given context's top left corner.
    virtual LVector2 GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const PURE_VIRTUAL(return { })

    FORCEINLINE bool IsSlotValid() const { return this->Slot != nullptr; }
    FORCEINLINE LWidgetSlot* GetSlot() const { return this->Slot; }
    FORCEINLINE LWidgetSlot* GetSlotChecked() const { LWidgetSlot* Out = this->GetSlot(); check( Out ); return Out; }
    FORCEINLINE LWidgetSlot* GetSlotAsserted() const { LWidgetSlot* Out = this->GetSlot(); jassert( Out ); return Out; }
                TOptional<LMargin> GetMargin() const;
    FORCEINLINE TOptional<LMargin> GetMarginChecked() const { TOptional<LMargin> Out = this->GetMargin(); check( Out.IsSet() ); return Out; }
    FORCEINLINE TOptional<LMargin> GetMarginAsserted() const { TOptional<LMargin> Out = this->GetMargin(); jassert( Out.IsSet() ); return Out; }
                bool SetMargin(const LMargin& InMargin);
    FORCEINLINE bool SetMarginChecked(const LMargin& InMargin) { const bool Out = this->SetMargin(InMargin); check( Out ); return Out; }
    FORCEINLINE bool SetMarginAsserted(const LMargin& InMargin) { const bool Out = this->SetMargin(InMargin); jassert( Out ); return Out; }

    FORCEINLINE LWidgetSlot** GetMutableSlotDangerousDoNotUseForInternalStuffOnlyOrIfYouWantYourOwnParentClass() { return &this->Slot; }

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
    template <typename TNode>
    NODISCARD FORCEINLINE typename TNode::TWidgetFactory& GetFactory()
    {
        return ::Jafg::Private::LWidgetFactoryUtility::MakeWidgetFactory<TNode>(this);
    }

    FORCEINLINE auto GetAnchor()           ->       LAnchor& { return this->Anchor; }
    FORCEINLINE auto GetAnchor()     const -> const LAnchor& { return this->Anchor; }
    FORCEINLINE void SetAnchor(const LAnchor&      InAnchor) { this->Anchor = InAnchor; }
    FORCEINLINE void SetAnchor(const EAnchor::Type InAnchor) { this->Anchor = InAnchor; }

    template <typename T> FORCEINLINE bool IsA() const;
    template <typename T> FORCEINLINE bool IsA(const T** OutObject) const;
                          FORCEINLINE bool IsA(const LObjectClass* InStaticClass) const { return this->GetVTableSlow()->DerivesFrom(InStaticClass); }
    template <typename T> FORCEINLINE       T* As() { return DynamicCast<T>(this); }
    template <typename T> FORCEINLINE const T* As() const { return DynamicCast<T>(this); }
    template <typename T> FORCEINLINE       T* AsChecked() { T* Out = this->As<T>(); check( Out ) return Out; }
    template <typename T> FORCEINLINE const T* AsChecked() const { const T* Out = this->As<T>(this); check( Out ) return Out; }
    template <typename T> FORCEINLINE       T* AsAsserted() { T* Out = this->As<T>(); jassert( Out ) return Out; }
    template <typename T> FORCEINLINE const T* AsAsserted() const { const T* Out = this->As<T>(); jassert( Out ) return Out; }

    LEngine*   GetEngine() const;
    LLocalEgo* GetLocalEgo() const;

private:

    bool bAllowTick { true };
    EWidgetVisibility::Type Visibility { EWidgetVisibility::TransitiveHitTestInvisible };

    //#
    //# The slot that this widget is currently in. Might be null if the widget is a standalone.
    //# This class is not the owner of this slot. But the parent holding the child is.
    //#
    LWidgetSlot* Slot { nullptr };

    //#
    //# The desired size of this widget.
    //#
    mutable LVector2 DesiredSize;

    //#
    //# The minimum content area.
    //#
    LVector2 MinDesiredSize;

    //#
    //# The anchored size of this widget.
    //#
    mutable LVector2 AnchoredSize;

    LAnchor Anchor { EAnchor::TopLeft };
};

template <typename TInNode>
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

template <typename TNode>
template <typename T>
FORCEINLINE typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::SaveTo(T** Out)
{
    check( Out )
    static_assert(std::is_base_of_v<WNode, T>);
    static_assert(std::is_base_of_v<T, TNodeTy>);
    *Out = this->GetNode();
    return this->Self();
}

template <typename TNode>
FORCEINLINE typename TWidgetFactory<TNode>::TFactoryRetTy& TWidgetFactory<TNode>::AddSibling(LWidgetFactory* InSibling)
{
    check( this->Siblings.Contains(InSibling) == false )
    this->Siblings.Add(InSibling);
    return this->Self();
}

template <typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LObjectContext* InContext)
{
    return ConstructWidgetNode<TNode>(InContext, TNode::StaticClass());
}

template<typename TNode>
FORCEINLINE TNode* ConstructWidgetNode(LObjectContext* InContext, const TSubclassOf<TNode>& InClass)
{
    const TNode* Out = ConstructDeferredWidgetNode<TNode>(InContext, InClass);
    MakeDeferredWidgetNodeFinal(Out);
    return Out;
}

template <typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LObjectContext* InContext)
{
    return ConstructDeferredWidgetNode<TNode>(InContext, TNode::StaticClass());
}

template<typename TNode>
FORCEINLINE TNode* ConstructDeferredWidgetNode(LObjectContext* InContext, const TSubclassOf<TNode>& InClass)
{
    return CheckedStaticCast<TNode>(NewDeferredObject(InContext, InClass));
}

FORCEINLINE void MakeDeferredWidgetNodeFinal(WNode* InNode)
{
    check( InNode )
    MakeDeferredObjectFinal(InNode);
    return;
}

template<typename T>
FORCEINLINE bool WNode::IsA() const
{
    static_assert(std::is_base_of_v<WNode, T>, "T must derive from WNode");
    return this->GetVTableSlow()->DerivesFrom(T::StaticClass());
}

template<typename T>
FORCEINLINE bool WNode::IsA(const T** OutObject) const
{
    static_assert(std::is_base_of_v<WNode, T>, "T must derive from WNode");
    if (this->IsA<T>())
    {
        if (OutObject)
        {
            *OutObject = CheckedStaticCast<T>(this);
        }

        return true;
    }

    return false;
}

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
#define NewNodeNoFactory(TNode)             (*ConstructDeferredWidgetNode<TNode>(this->GetOuter()))
#define NewNodeNoFactoryCtx(Ctx, TNode)     (*ConstructDeferredWidgetNode<TNode>((Ctx)->GetOuter()))

//# Make a new node. Can be used everywhere.
#define NewNode(TNode)                      (*ConstructDeferredWidgetNode<TNode>(this->GetOuter())).GetFactory<TNode>()
#define NewNodeCtx(Ctx, TNode)              (*ConstructDeferredWidgetNode<TNode>((Ctx)->GetOuter())).GetFactory<TNode>()
//# End the new node declaration started in #NewNodeNoFactory or #NewNode or #NewNodeCtx.
#define FinishWidget(Root)                  ;MakeDeferredWidgetNodeFinal(Root)
