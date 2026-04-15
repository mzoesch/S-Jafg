// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "Nodes/Whitespace.h"
#include "User/Input/Replies.h"
#include "User/Input/Events.h"
#include "Platform/SurfaceForward.h"
#include "Nodes/InterfaceTypes.h"
#include "User/UserPreferencesForward.h"
#include "Framework/FrontendForward.h"
#include "Node.generated.h"

namespace Jafg
{

class JNodeData;
class LLocalEgo;
class WNode;
class WParent;
class LViewport;
struct LWidgetConstructor;
struct LMappedDeviceBuffer;
struct LNodeRenderInfo;

//#
//# How to anchor a child to its parent if the parent can have children.
//# This enum can be used for simple and complex anchoring.
//#
namespace EAnchor
{

enum Type : u8
{
    //# Default behavior. Usually this is the VTop | HLeft.
    Identity   = 0x0 << 0,

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
        LVec4F Anchors;
    };

    FORCEINLINE constexpr LAnchor() noexcept : Anchors(maths::zero_vector<LVec4F>) { }
    FORCEINLINE constexpr LAnchor(f32 InUniformAnchors) noexcept
        : Anchors(InUniformAnchors, InUniformAnchors, InUniformAnchors, InUniformAnchors)
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(f32 InHorizontalUniform, f32 InVerticalUniform) noexcept
        : Anchors(InHorizontalUniform, InVerticalUniform, InHorizontalUniform, InVerticalUniform)
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(f32 InMinX, f32 InMinY, f32 InMaxX, f32 InMaxY) noexcept
        : Anchors(InMinX, InMinY, InMaxX, InMaxY)
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(EAnchor::Type InAnchors) noexcept
        : Anchors(maths::zero_vector<LVec4F>)
    {
        this->ApplyConstraints(InAnchors);
    }
    FORCEINLINE constexpr LAnchor(LAnchor const& InOther, EAnchor::Type InConstraints) noexcept
        : Anchors(InOther.Anchors)
    {
        this->ApplyConstraints(InConstraints);
    }
    FORCEINLINE constexpr LAnchor(LVec4F const& InOther, const EAnchor::Type InConstraints) noexcept
        : Anchors(InOther)
    {
        this->ApplyConstraints(InConstraints);
    }
    FORCEINLINE constexpr LAnchor(LVec4F const& InAnchors) noexcept : Anchors(InAnchors) { }
    FORCEINLINE constexpr LAnchor(LAnchor const& InOther) noexcept : Anchors(InOther.Anchors) { }
    FORCEINLINE constexpr LAnchor(LAnchor&& InOther) noexcept : Anchors(std::move(InOther.Anchors)) { }
    FORCEINLINE constexpr LAnchor& operator=(LAnchor const& InOther) noexcept { this->Anchors = InOther.Anchors; return *this; }
    FORCEINLINE constexpr LAnchor& operator=(LAnchor&& InOther) noexcept { this->Anchors = std::move(InOther.Anchors); return *this; }

    constexpr bool IsNormalized() const noexcept
    {
        return
            this->Anchors.x >= 0.0f && this->Anchors.y >= 0.0f && this->Anchors.z >= 0.0f && this->Anchors.w >= 0.0f
            && this->Anchors.x <= 1.0f && this->Anchors.y <= 1.0f && this->Anchors.z <= 1.0f && this->Anchors.w <= 1.0f
            && this->Anchors.x + this->Anchors.z <= 1.0f
            && this->Anchors.y + this->Anchors.w <= 1.0f;
    }
    constexpr void Normalize() noexcept
    {
#if JAFG_DO_CHECKS
        LAnchor Old{*this};
#endif /* JAFG_DO_CHECKS */

        this->Anchors.x = maths::clamp(this->Anchors.x, 0.0f, 1.0f);
        this->Anchors.y = maths::clamp(this->Anchors.y, 0.0f, 1.0f);
        this->Anchors.z = maths::clamp(this->Anchors.z, 0.0f, 1.0f);
        this->Anchors.w = maths::clamp(this->Anchors.w, 0.0f, 1.0f);

        this->MaxX = maths::min(this->MaxX, 1.0f - this->MinX);
        this->MaxY = maths::min(this->MaxY, 1.0f - this->MinY);

#if JAFG_DO_CHECKS
        if (Old != *this)
        {
            LOG_WARNING(LogWidgetFramework, "Anchor was not normalized correctly [{} -> {}].", Old.ToString(), this->ToString())
        }
#endif /* JAFG_DO_CHECKS */

        return;
    }

    inline constexpr void ApplyConstraints(EAnchor::Type InConstraints) noexcept;

    FORCEINLINE constexpr bool IsPushedHorizontal() const noexcept { return this->MinX > 0.0; }
    FORCEINLINE constexpr bool IsPushedVertical() const noexcept { return this->MinY > 0.0; }
    FORCEINLINE constexpr bool IsStretchedHorizontal() const noexcept { return this->MaxX > 0.0; }
    FORCEINLINE constexpr bool IsStretchedVertical() const noexcept { return this->MaxY > 0.0; }

    FORCEINLINE constexpr bool IsPushed() const noexcept { return this->IsPushedHorizontal() || this->IsPushedVertical(); }
    FORCEINLINE constexpr bool IsStretched() const noexcept { return this->IsStretchedHorizontal() || this->IsStretchedVertical(); }

    FORCEINLINE constexpr bool operator==(LAnchor const& InOther) const noexcept { return this->Anchors == InOther.Anchors; }
    FORCEINLINE LString ToString() const noexcept { return maths::to_string(this->Anchors); }
};

namespace Anchors
{

inline static constexpr LAnchor VTop    { 0.0, 0.0, 0.0, 0.0 };
inline static constexpr LAnchor VCenter { 0.0, 0.5, 0.0, 0.0 };
inline static constexpr LAnchor VBottom { 0.0, 1.0, 0.0, 0.0 };
inline static constexpr LAnchor HLeft   { 0.0, 0.0, 0.0, 0.0 };
inline static constexpr LAnchor HCenter { 0.5, 0.0, 0.0, 0.0 };
inline static constexpr LAnchor HRight  { 1.0, 0.0, 0.0, 0.0 };
inline static constexpr LAnchor VFill   { 0.0, 0.0, 0.0, 1.0 };
inline static constexpr LAnchor HFill   { 0.0, 0.0, 1.0, 0.0 };

} /* ~Namespace Anchors */

inline constexpr void LAnchor::ApplyConstraints(EAnchor::Type InConstraints) noexcept
{
    if (InConstraints & EAnchor::VTop)    { this->Anchors += Anchors::VTop.Anchors; }
    if (InConstraints & EAnchor::VCenter) { this->Anchors += Anchors::VCenter.Anchors; }
    if (InConstraints & EAnchor::VBottom) { this->Anchors += Anchors::VBottom.Anchors; }
    if (InConstraints & EAnchor::HLeft)   { this->Anchors += Anchors::HLeft.Anchors; }
    if (InConstraints & EAnchor::HCenter) { this->Anchors += Anchors::HCenter.Anchors; }
    if (InConstraints & EAnchor::HRight)  { this->Anchors += Anchors::HRight.Anchors; }
    if (InConstraints & EAnchor::VFill)   { this->Anchors += Anchors::VFill.Anchors; }
    if (InConstraints & EAnchor::HFill)   { this->Anchors += Anchors::HFill.Anchors; }

    check(this->IsNormalized())

    return;
}

enum struct ENodeVisibility : u8
{
    //#
    //# Visible, takes up space in the node layout and is hit-testable.
    //# Nodes in this state will be ticked.
    //#
    Visible,

    //#
    //# Not visible, takes up space in the node layout and is not hit-testable.
    //# Nodes in this state will not be ticked.
    //#
    Hidden,

    //#
    //# Not visible, does not take up space in the node layout and is not hit-testable.
    //# Nodes in this state will not be ticked.
    //#
    Collapsed,

    //#
    //# Visible, takes up space in the node layout and is hit-testable, but all children are not.
    //# Nodes in this state will be ticked.
    //#
    DerivedHitTestInvisible,

    //#
    //# Visible, takes up space in the node layout and is not hit-testable.
    //# Nodes in this state will be ticked.
    //#
    TransitiveHitTestInvisible,

    //#
    //# Visible, takes up space in the node layout and is itself not hit-testable, but all children are.
    //# Nodes in this state will be ticked.
    //#
    IntransitiveHitTestInvisible,
};
namespace NodeVisibility
{

FORCEINLINE constexpr bool IsDrawn(ENodeVisibility Visibility) noexcept
{
    return Visibility != ENodeVisibility::Hidden
        && Visibility != ENodeVisibility::Collapsed;
}

FORCEINLINE constexpr bool IsTicked(ENodeVisibility Visibility) noexcept
{
    return IsDrawn(Visibility);
}

FORCEINLINE constexpr bool IsHitTestable(ENodeVisibility Visibility) noexcept
{
    return Visibility == ENodeVisibility::Visible
        || Visibility == ENodeVisibility::DerivedHitTestInvisible;
}

FORCEINLINE constexpr bool IsDerivedHitTestable(ENodeVisibility Visibility) noexcept
{
    return Visibility == ENodeVisibility::Visible
        || Visibility == ENodeVisibility::IntransitiveHitTestInvisible;
}

FORCEINLINE constexpr bool TransformsWidgetLayout(ENodeVisibility Visibility) noexcept
{
    return Visibility != ENodeVisibility::Collapsed;
}

} /* ~Namespace EWidgetVisibility */
inline LString LexToString(ENodeVisibility Visibility)
{
    switch (Visibility)
    {
    case ENodeVisibility::Visible: { return "Visible"; }
    case ENodeVisibility::Hidden: { return "Hidden"; }
    case ENodeVisibility::Collapsed: { return "Collapsed"; }
    case ENodeVisibility::DerivedHitTestInvisible: { return "DerivedHitTestInvisible"; }
    case ENodeVisibility::TransitiveHitTestInvisible: { return "TransitiveHitTestInvisible"; }
    case ENodeVisibility::IntransitiveHitTestInvisible: { return "IntransitiveHitTestInvisible"; }
    default: checkNoEntry() return { "<unknown>" };
    }
}

namespace Detail
{

struct LViewport2OuterProj
{
    NODISCARD inline constexpr decltype(auto) operator()(auto& Viewport) const noexcept
    {
        return Viewport.GetOuter();
    }
};

struct LBeginStylingFnResult;
struct LNodeFactoryBase
{
    friend LBeginStylingFnResult;

    constexpr LNodeFactoryBase() noexcept = delete;
    constexpr LNodeFactoryBase(WNode& InNode) noexcept : Node{InNode} {}
    PROHIBIT_COPY(LNodeFactoryBase)
    LNodeFactoryBase(LNodeFactoryBase&& O) noexcept
        : Node{O.Node}
        , Siblings{std::move(O.Siblings)}
#if JAFG_DO_CHECKS
        , _bReleased{O._bReleased}
        , _bDecommissioned{O._bDecommissioned}
#endif /* JAFG_DO_CHECKS */
    {
        check(O.Siblings.empty())
        checkCode(O._bReleased = true)
        checkCode(O._bDecommissioned = true)
    }
    LNodeFactoryBase& operator=(LNodeFactoryBase&& Rhs) noexcept = delete;
    ~LNodeFactoryBase()
    {
        /* A factory does not have to be decommissioned in order to be destroyed. */
        check(this->_bReleased && this->Siblings.empty())
    }

#if JAFG_DO_CHECKS
    FORCEINLINE constexpr void _Release() noexcept { check(this->_bReleased == false) this->_bReleased = true; }
    FORCEINLINE constexpr bool _IsReleased() const noexcept { return this->_bReleased; }
    FORCEINLINE constexpr void _Decommission() noexcept { check(this->_bDecommissioned == false) this->_bDecommissioned = true; }
    FORCEINLINE constexpr bool _IsDecommissioned() const noexcept { return this->_bDecommissioned; }
#endif /* JAFG_DO_CHECKS */

    FORCEINLINE auto& GetRawNode() noexcept { check(this->_IsDecommissioned() == false) return this->Node; }
    FORCEINLINE auto const& GetRawNode() const noexcept { check(this->_IsDecommissioned() == false) return this->Node; }

    FORCEINLINE auto& GetMutableSiblings() noexcept { check(this->_IsDecommissioned() == false) return this->Siblings; }
    FORCEINLINE auto const& GetSiblings() const noexcept { check(this->_IsDecommissioned() == false) return this->Siblings; }

private:

    WNode& Node;
    TArray<WNode*> Siblings;
#if JAFG_DO_CHECKS
    bool _bReleased{};
    bool _bDecommissioned{};
#endif /* JAFG_DO_CHECKS */
};

} /* ~Namespace Detail */

#define NODE_FACTORY_PARENT(Node) public Node::Super::LFactory
#define NODE_FACTORY_BODY(Node) typedef Node TSelf;
#define DETAIL_JAFG_NODE_FACTORY_SELF() (*StaticCastChecked<typename std::remove_cvref_t<decltype(Self)>::TSelf>(&Self.GetRawNode()))
#define NODE_FACTORY_SELF() check(Self._IsDecommissioned() == false) DETAIL_JAFG_NODE_FACTORY_SELF()
#define NODE_FACTORY_RESULT() std::forward<decltype(Self)>(Self)

struct LNodeSweepData final
{
    //#
    //# The translation that is recommended for children of a #WParentBase to use while sweeping.
    //# This translation should be removed if a parent widget finished its sweep logic.
    //# This value is reset every frame.
    //# @remark Use the #LViewportSweepTranslation for easy RAII style translation logic.
    //#
    const LVec2F Translation;
};

struct LNodeKeyEventData final
{
    LFrontend const& Frontend;
    LSurface& Surface;
    LViewport& Viewport;
    WNode& Node;

    const LVec2F Translation;
};

//# Initializer for the dynamic ctors for WNodes.
typedef Detail::TCxxDynamicInit<LViewport, Detail::LViewport2OuterProj> LNodeDynamicInit;
//# Initializer for the static ctors for WNodes.
template<typename TCxxClass>
using TNodeStaticInit = Detail::TCxxStaticInitBase<LViewport, TCxxClass, Detail::LViewport2OuterProj>;

namespace Detail
{

struct LOuter2ViewportProj
{
    NODISCARD inline constexpr LNodeDynamicInit operator()(LCxxDynamicInit const& Init) const noexcept;
};

struct LBeginStylingFnResult final
{
public:

    inline constexpr LBeginStylingFnResult(WParent& P) noexcept : Parent{P} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LBeginStylingFnResult)
    inline ~LBeginStylingFnResult();

    WParent& Parent;
    std::optional<i32> Where;
    TUnique<LNodeFactoryBase> Factory;

    decltype(auto) At(this auto&& Self, i32 InIndex) noexcept
    {
        Self.Where = InIndex;
        return std::forward<decltype(Self)>(Self);
    }

    //# Set the root with a dynamic type specified at runtime.
    template<typename TNode = WNode> requires std::is_base_of_v<WNode, TNode>
    inline typename TNode::LFactory& Root(LCxxClass const& Class);
    //# Set the root with the provided static type directly.
    template<typename TNode, typename... TArgs>
        requires std::is_base_of_v<WNode, TNode>
              // && std::is_constructible_v<TNode, TNodeStaticInit<TNode> const&, TArgs&&...>
    inline typename TNode::LFactory& StaticRoot(TArgs&&... Args) noexcept;
};

} /* ~Namespace Detail */

#define JAFG_NODE_FACTORY_DELEGATE_BINDINGS(Prefix, Event) \
    decltype(auto) Prefix(this auto&& Self, LNullptrTy) noexcept \
    { \
        NODE_FACTORY_SELF().Event.Bind(nullptr); \
        return NODE_FACTORY_RESULT(); \
    } \
    decltype(auto) Prefix(this auto&& Self, decltype(std::remove_cvref_t<decltype(Self)>::TSelf::Event) const& InEvent) noexcept \
    { \
        NODE_FACTORY_SELF().Event.Bind(InEvent); \
        return NODE_FACTORY_RESULT(); \
    } \
    decltype(auto) Prefix(this auto&& Self, decltype(std::remove_cvref_t<decltype(Self)>::TSelf::Event)&& InEvent) noexcept \
    { \
        NODE_FACTORY_SELF().Event.Bind(std::move(InEvent)); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename TFunctor> \
    decltype(auto) Prefix(this auto&& Self, TFunctor&& Functor) \
        requires(decltype(std::remove_cvref_t<decltype(Self)>::TSelf::Event)::template IsInvocableWith_v<TFunctor>) \
    { \
        NODE_FACTORY_SELF().Event.Bind(std::forward<TFunctor>(Functor)); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename TFunctor> \
    decltype(auto) Prefix(this auto&& Self, TFunctor* Functor) \
        requires(decltype(std::remove_cvref_t<decltype(Self)>::TSelf::Event)::template IsInvocableWith_v<TFunctor>) \
    { \
        NODE_FACTORY_SELF().Event.Bind(Functor); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename TObj, typename TMemberFunctor> \
    decltype(auto) Prefix(this auto&& Self, TObj* Object, TMemberFunctor MemberFunctor) \
        requires(decltype(std::remove_cvref_t<decltype(Self)>::TSelf::Event)::template IsInvocableWithMember_v<TObj, TMemberFunctor>) \
    { \
        NODE_FACTORY_SELF().Event.Bind(Object, MemberFunctor); \
        return NODE_FACTORY_RESULT(); \
    }

struct LFactoryNode : public Detail::LNodeFactoryBase
{
    NODE_FACTORY_BODY(WNode)

    decltype(auto) Visibility(this auto&& Self, ENodeVisibility Visibility) noexcept
    {
        NODE_FACTORY_SELF().SetVisibility(Visibility);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Anchor(this auto&& Self, LAnchor const& Anchor) noexcept
    {
        NODE_FACTORY_SELF().SetAnchor(Anchor);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Anchor(this auto&& Self, EAnchor::Type Anchor) noexcept
    {
        NODE_FACTORY_SELF().SetAnchor(Anchor);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) MinDesiredSize(this auto&& Self, LWidgetSize2 Size) noexcept
    {
        NODE_FACTORY_SELF().SetMinDesiredSize(Size);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) MaxDesiredSize(this auto&& Self, LWidgetSize2 Size) noexcept
    {
        NODE_FACTORY_SELF().SetMaxDesiredSize(Size);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Delegate(this auto&& Self, TFunction<void(LFactoryNode& Factory)>& Delegate) noexcept
    {
        if (Delegate.IsValid())
        {
            Delegate(Self);
        }
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Data(this auto&& Self, JNodeData& Data) noexcept
    {
        NODE_FACTORY_SELF().AddData(Data);
        return NODE_FACTORY_RESULT();
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnKeyDown, OnKeyDownEvent)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnKeyUp, OnKeyUpEvent)

    template<typename T> requires std::is_base_of_v<WNode, T>
    decltype(auto) SaveTo(this auto&& Self, T** Out) noexcept
    {
        if (Out)
        {
            *Out = StaticCastChecked<T>(&Self.GetRawNode());
        }
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Unique(this auto&& Self) noexcept
    {
        auto& Cache{Self.GetRawNode()};
#if JAFG_DO_CHECKS
        Self._Release();
        Self._Decommission();
#endif /* JAFG_DO_CHECKS */
        return TJxxUnique<typename std::remove_cvref_t<decltype(Self)>::TSelf>(
            static_cast<typename std::remove_cvref_t<decltype(Self)>::TSelf*>(&Cache)
            );
    }

    inline decltype(auto) operator+(this auto&& Self, LNodeFactoryBase&& F) noexcept;
};

//# Pass arbitrary data typesafe down the hierarchy. Using this often is a good indicator for bad design.
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API JNodeData : public JCxxClass
{
    GENERATED_CLASS_BODY()
protected:
    DEFAULT_OBJECT_CONSTRUCTORS(JNodeData)
};

ENGINE_API  f32 InSpt(LViewport const& Viewport, LWidgetSize1 Size) noexcept;
FORCEINLINE f32 InSpt(WNode const& Node, LWidgetSize1 Size) noexcept;
ENGINE_API  LVec2F InSpt(LViewport const& Viewport, LWidgetSize2 Size) noexcept;
FORCEINLINE LVec2F InSpt(WNode const& Node, LWidgetSize2 Size) noexcept;

ENGINE_API  f32 InSptFromRelative(LViewport const& Viewport, f32 Relative) noexcept;
FORCEINLINE f32 InSptFromRelative(WNode const& Node, f32 Relative) noexcept;
ENGINE_API  LVec2F InSptFromRelative(LViewport const& Viewport, LVec2F Relative) noexcept;
FORCEINLINE LVec2F InSptFromRelative(WNode const& Node, LVec2F Relative) noexcept;

//#
//# The base class for everything that can be interpreted as a visual element.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryNode, ECxxClassFlags::Abstract)
class ENGINE_API WNode : public JCxxClass
{
    GENERATED_CLASS_BODY()

    friend LWidgetConstructor;

protected:

    inline explicit WNode(LNodeDynamicInit const& Init) noexcept;
    template<typename TCxxClass>
    inline explicit WNode(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{TCxxStaticInit<TCxxClass>{.Outer=std::invoke(typename TNodeStaticInit<TCxxClass>::Proj{}, Init.Outer)}}
        , AttachedViewport{Init.Outer}{}

public:

#if JAFG_DO_CHECKS
    virtual ~WNode() override
    {
        check(this->Parent == nullptr)
    }
#endif /* JAFG_DO_CHECKS */

    // JCxxClass implementation
    virtual void BeginLife() override final { Super::BeginLife();  this->Construct(); }
    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override final
    {
        this->Destruct();
        Super::OnGarbage(Reason);
    }
    // ~JCxxClass implementation

    //#
    //# Called when this widget is constructed. This does not mean being drawn to a canvas. A widget might be
    //# constructed but never drawn on a canvas in their entire lifespan. This method replaces the #BeginLife super
    //# method.
    //#
    virtual void Construct() {}

    //#
    //# Called when this widget is being ticked.
    //# See #EWidgetVisibility for more information about when to tick a widget.
    //#
    virtual void Tick() {}

    //#
    //# Called when this widget is being destructed. This does not mean being removed from its parent. This method
    //# replaces the #EndLife super method.
    //#
    virtual void Destruct()
    {
        check(Tasks::IsOnMasterThread())
        if (this->Parent)
        {
            checkCode(_check_Destruct())
            this->Parent = nullptr;
        }
    }

    //#
    //# The paint function for a widget. Only called if the widget is visible and paintable.
    //# Do not update any values of any widgets when inside this method.
    //# Automatically called by the owning viewport. Do not call manually.
    //#
    virtual void Draw(LNodeRenderInfo const& Info) const { check(this->ShouldNowDraw()) }

    //#
    //# Use this method to pass arbitrary typesafe data to the widget.
    //# @return True, if the data was used successfully handled.
    //#
    virtual bool AddData(JNodeData& Data) { return false; }

    bool IsInBounds(LNodeSweepData const& Data, LVec2F const& Location) const;
    virtual LCursorReply SweepMouse(LNodeSweepData const& Data, LVec2F const& Location);

    TFunction<LCursorReply(WNode& Node)> OnCursorEnterEvent;
    TFunction<LCursorReply(WNode& Node)> OnCursorMovedEvent;
    TFunction<LCursorReply(WNode& Node)> OnCursorLeaveEvent;
    virtual LCursorReply OnCursorEnter() { if (this->OnCursorEnterEvent.IsValid()) { return this->OnCursorEnterEvent.Invoke(*this); } return LCursorReply::Handled(); }
    virtual LCursorReply OnCursorMoved(const LVec2F& InLocation) { if (this->OnCursorMovedEvent.IsValid()) { return this->OnCursorMovedEvent.Invoke(*this); } return LCursorReply::Handled(); }
    virtual LCursorReply OnCursorLeave() { if (this->OnCursorLeaveEvent.IsValid()) { return this->OnCursorLeaveEvent.Invoke(*this); } return LCursorReply::Handled(); }

    virtual LReply SweepFocusTest(LNodeSweepData const& Data, LVec2F const& Location);

    //#
    //# Called if this widget is being focused / unfocused. Extra care is given by the owing viewport for special
    //# events, only receivable by a focused widget. This includes e.g., platform buffered input.
    //# You may decide to bubble this event up or down.
    //# Note that the #WParent node will not bubble this event in any direction as it is usually meant for the
    //# most inner node only.
    //#
    virtual void OnFocusReceived() {}
    virtual void OnFocusLost() {}

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
    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event);
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event);

    TFunction<LReply(WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event)> OnKeyDownEvent;
    TFunction<LReply(WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event)> OnKeyUpEvent;

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
    virtual LReply OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) { return LReply::Unhandled(); }
    virtual LReply OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) { return LReply::Unhandled(); }

    //#
    //# @return Whether this is the focused widget.
    //# @remark This method is fairly slow. Consider young the #IsFocusWidget with a viewport parameter if cached.
    //#
    bool IsFocusWidget() const;
    //#
    //# @param InViewport The viewport to check for or null.
    //# @return Whether this is the focused widget.
    //#
    bool IsFocusWidget(LViewport const* InViewport) const;

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

    FORCEINLINE bool ShouldNowTick() const { return this->bAllowTick && NodeVisibility::IsTicked(this->Visibility); }
    FORCEINLINE bool GetRawShouldTick() const { return this->bAllowTick; }
    FORCEINLINE void SetShouldTick(const bool bInShouldTick) { this->bAllowTick = bInShouldTick; }
    FORCEINLINE bool ShouldNowDraw() const { return NodeVisibility::IsDrawn(this->Visibility); }
    FORCEINLINE auto GetVisibility() const { return this->Visibility; }
    FORCEINLINE bool IsPainted() const { return this->ShouldNowDraw(); }
    FORCEINLINE bool IsHitTestable() const { return NodeVisibility::IsHitTestable(this->Visibility); }
    FORCEINLINE bool CanChildrenBeHitTestable() const { return NodeVisibility::IsDerivedHitTestable(this->Visibility); }
    FORCEINLINE bool ShouldCheckForInputs() const { return this->IsHitTestable() || this->CanChildrenBeHitTestable(); }
    FORCEINLINE bool TransformsWidgetLayout() const { return NodeVisibility::TransformsWidgetLayout(this->Visibility); }
    FORCEINLINE bool IsVisible() const { return this->Visibility == ENodeVisibility::Visible; }
    FORCEINLINE bool IsHidden() const { return this->Visibility == ENodeVisibility::Hidden; }
    FORCEINLINE bool IsCollapsed() const { return this->Visibility == ENodeVisibility::Collapsed; }
    FORCEINLINE bool IsDerivedHitTestInvisible() const { return this->Visibility == ENodeVisibility::DerivedHitTestInvisible; }
    FORCEINLINE bool IsTransitiveHitTestInvisible() const { return this->Visibility == ENodeVisibility::TransitiveHitTestInvisible; }
    FORCEINLINE bool IsIntransitiveHitTestInvisible() const { return this->Visibility == ENodeVisibility::IntransitiveHitTestInvisible; }
                void SetVisibility(ENodeVisibility InVisibility);

    //# Only if old and new are different.
    virtual void OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility) {}
    //# Called if the surface was resized.
    virtual void OnSurfaceResize() {}

    //# Mark this node and alls its children as garbage and remove them, from their parent.
    virtual void RemoveFromParent2();

    FORCEINLINE bool IsParentValid() const noexcept { return this->Parent != nullptr; }
    FORCEINLINE WParent* GetParent() { return this->Parent; }
    FORCEINLINE WParent* GetParentChecked() { auto* Out{this->GetParent()}; check(Out); return Out; }
    FORCEINLINE WParent const* GetParent() const { return this->Parent; }
    FORCEINLINE WParent const* GetParentChecked() const { auto const* Out{this->GetParent()}; check(Out); return Out; }
    //# @return The most outer parent of this widget or the widget itself if no outer parent.
    WNode* GetMostOuterParent() noexcept;
    WNode const* GetMostOuterParent() const noexcept;

    //#
    //# Searches for a node in this widget tree. Only searches nodes that are drawn.
    //# @return True if the target node exists in this widget tree and is visible.
    //#
    virtual bool FindNodeInVisiblePath(WNode const* Node) const;

    //# @return The size of the current viewport in pixels.
    LVec2u32 GetViewportSize() const;
    FORCEINLINE constexpr LViewport& GetViewport() noexcept { return this->AttachedViewport; }
    FORCEINLINE constexpr LViewport const& GetViewport() const noexcept { return this->AttachedViewport; }

    //# Virtual update method for the desired size. Automatically called. Do not call manually.
    virtual void UpdateDesiredSize() const {}
    //#
    //# Update the #DesiredSize of a widget inside the overridden #UpdateDesiredSize method with this one.
    //# Do not call this method from outside the #UpdateDesiredSize method.
    //#
    FORCEINLINE void SetDesiredSize(LWidgetSize2 Size) const noexcept { this->SetDesiredSizeInSpt(InSpt(*this, Size)); }
    void SetDesiredSizeInSpt(LVec2F Size) const noexcept;
    //# Internal usage only. Do not use unless you are a really smart person.
    FORCEINLINE void SetDesiredSizeUnsanitized(LVec2F Size) const { this->DesiredSize_v2 = std::move(Size); return; }
    FORCEINLINE LVec2F const& GetDesiredSize_v2() const { return this->DesiredSize_v2; }
    FORCEINLINE LVec2F const& GetDesiredSizeSmart_v2() const { return this->TransformsWidgetLayout() ? this->DesiredSize_v2 : maths::zero_vector<LVec2F>; }
    //# The min desired size. A widget will always be at least this size.
    FORCEINLINE void SetMinDesiredSize(LWidgetSize2 Size) { this->MinDesiredSize = std::move(Size); return; }
    FORCEINLINE LWidgetSize2 const& GetMinDesiredSize() const { return this->MinDesiredSize; }
    //# The max desired size. A widget will have at maximum this size. Zero means unbound. This includes max size of anchored nodes.
    FORCEINLINE void SetMaxDesiredSize(LWidgetSize2 Size) { this->MaxDesiredSize = std::move(Size); return; }
    FORCEINLINE LWidgetSize2 const& GetMaxDesiredSize() const { return this->MaxDesiredSize; }

    //# Virtual update method for the anchored size. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSize(LViewport const& Context) const;
    //# Virtual update method for the anchored size of a child. Automatically called. Do not call manually.
    virtual LVec2F GetAnchoredSizeForChild(LViewport const& Viewport, WNode const* InDirectChild) const PURE_VIRTUAL()
    void SetAnchoredSize(LVec2F const& Size) const noexcept;
    FORCEINLINE LVec2F const& GetAnchoredSize_v2() const noexcept { return this->AnchoredSize_v2; }
    FORCEINLINE LVec2F CopyAnchoredSize_v2() const noexcept { return this->AnchoredSize_v2; }
    //# The anchored size that was lost during #MaxDesiredSize clamp.
    FORCEINLINE LVec2F const& GetLostAnchoredSize_v2() const noexcept { return this->LostAnchoredSize_v2; }
    FORCEINLINE LVec2F CopyLostAnchoredSize_v2() const noexcept { return this->LostAnchoredSize_v2; }
    //# @return The anchored top-left corner of the widget relative to the given context's top-left corner.
    virtual LVec2F GetAnchoredTopLeftFromMostOuter(LViewport const& Viewport) const;
    LVec2F GetAnchoredAndTranslatedTopLeftFromMostOuter(LVec2F const& Translation) const;
    //# @return The anchored top-left corner of the direct child relative to the given context's top-left corner.
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Viewport, WNode const* InDirectChild) const PURE_VIRTUAL(return { })

    TOptional<LMargin> GetMargin() const noexcept;
    FORCEINLINE TOptional<LMargin> GetMarginChecked() const noexcept { TOptional Out{this->GetMargin()}; check(Out.has_value()); return Out; }
    FORCEINLINE TOptional<LMargin> GetMarginAsserted() const noexcept { TOptional Out{this->GetMargin()}; jassert(Out.has_value()); return Out; }

    FORCEINLINE constexpr LAnchor& GetAnchor() noexcept { return this->Anchor; }
    FORCEINLINE constexpr LAnchor const& GetAnchor() const noexcept { return this->Anchor; }
    FORCEINLINE constexpr void SetAnchor(LAnchor const& InAnchor) noexcept { this->Anchor = InAnchor; }
    FORCEINLINE constexpr void SetAnchor(EAnchor::Type  InAnchor) noexcept { this->Anchor = InAnchor; }

    ENGINE_API LFrontend& GetFrontend() const noexcept;

    //# Internal function of Jafg. Do not call yourself.
    void _SetParentDangerous(WParent* InParent) noexcept { this->Parent = InParent; }

private:

#if JAFG_DO_CHECKS
    void _check_Destruct();
#endif /* JAFG_DO_CHECKS */

    bool bAllowTick{ true };
    ENodeVisibility Visibility{ ENodeVisibility::TransitiveHitTestInvisible };

    //# Parent of this widget.
    WParent* Parent{};

    LViewport& AttachedViewport;

    //# The desired size of this widget in pt.
    mutable LVec2F DesiredSize_v2;

    //# The minimum content area.
    LWidgetSize2 MinDesiredSize;

    //# The maximal content area. Zero means unbound. This includes max size of anchored nodes.
    LWidgetSize2 MaxDesiredSize;

    //# The anchor to use.
    LAnchor Anchor{ EAnchor::TopLeft };
    //# The anchored size of this widget in pt.
    mutable LVec2F AnchoredSize_v2;
    //# The anchored size that was lost during #MaxDesiredSize clamp in pt.
    mutable LVec2F LostAnchoredSize_v2;
};

inline decltype(auto) LFactoryNode::operator+(this auto&& Self, LNodeFactoryBase&& F) noexcept
{
    Self.GetMutableSiblings().emplace_back(&F.GetRawNode());

    // If this hits, but usage is correct append siblings to self.
    // But I do not see a case where a correct usage would lead to this.
    check(F.GetSiblings().empty())

    checkCode(F._Release())
    checkCode(F._Decommission())
    return NODE_FACTORY_RESULT();
}

namespace Detail
{

//# Do not use NewObject for WNodes; instead use these.
inline constexpr NewDeferredObjectFn<LNodeDynamicInit, TNodeStaticInit, TDeferredObjectExec, WNode> ConstructNodeImpl{};

struct BeginStylingFn final
{
    template<typename TParent> requires std::is_base_of_v<WParent, TParent>
    LBeginStylingFnResult operator()(TParent& Parent) const
    {
        return {Parent};
    }
};

struct LNewNodeFnResult final
{
    LViewport& Viewport;

    template<typename TNode = WNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class() const
    {
        return this->Class<TNode>(TNode::StaticClass());
    }
    template<typename TNode = WNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class(LCxxClass const& Class) const
    {
        return typename TNode::LFactory{*ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Viewport,.Class=Class}).release()};
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class(TSubclassOf<TNode> Class) const
    {
        return typename TNode::LFactory{*ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Viewport,.Class=Class.GetClassOrDefault()}).release()};
    }
};

struct NewNodeFn final
{
    LNewNodeFnResult operator()(LViewport& Viewport) const
    {
        return {Viewport};
    }
};

} /* ~Namespace Detail */

//#
//# Call this if you want to begin styling a new WNode.
//# If you want to create a new Node that is top level to a #Viewport use @see #WUserWidget.
//#
inline constexpr Detail::BeginStylingFn BeginStyling{};
//# Call this inside #BeginStyling to create new child/sibling nodes.
inline constexpr Detail::NewNodeFn NewNode{};
//# Just some boilerplate helpers. Completely optional.
#define NewStaticNode(NodeClass) ::Jafg::NewNode(this->GetViewport()).Class<NodeClass>()
#define NewStaticNodeVp(Vp, NodeClass) ::Jafg::NewNode(Vp).Class<NodeClass>()
#define NewSubNode(Subclass) ::Jafg::NewNode(this->GetViewport()).Class(Subclass)
#define NewSubNodeVp(Vp, Subclass) ::Jafg::NewNode(Vp).Class(Subclass)

FORCEINLINE f32 InSpt(WNode const& Node, LWidgetSize1 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    check( Size.Type == EWidgetSize::Points )
    return InSptFromRelative(Node.GetViewport(), Size.Size);
}

FORCEINLINE LVec2F InSpt(WNode const& Node, LWidgetSize2 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    check( Size.Type == EWidgetSize::Points )
    return InSptFromRelative(Node.GetViewport(), Size.Size);
}

FORCEINLINE f32 InSptFromRelative(WNode const& Node, f32 Relative) noexcept
{
    return InSptFromRelative(Node.GetViewport(), Relative);
}

FORCEINLINE LVec2F InSptFromRelative(WNode const& Node, LVec2F Relative) noexcept
{
    return InSptFromRelative(Node.GetViewport(), Relative);
}

#if !JAFG_DO_CHECKS
FORCEINLINE LReply WNode::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    return LReply::Unhandled();
}

FORCEINLINE LReply WNode::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    return LReply::Unhandled();
}
#endif /* !JAFG_DO_CHECKS */

} /* ~Namespace Jafg */

#include "Nodes/Viewport.h"
