// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "User/Input/Events.h"
#include "Platform/SurfaceForward.h"
#include "User/UserPreferencesForward.h"
#include "Framework/FrontendForward.h"
#include "Platform/Cursor.h"
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
struct LRenderInfo;
struct LNodeRenderInfo;

namespace Detail
{

template<typename T>
FORCEINLINE constexpr decltype(T::Size) GetNodeSizeInStaticPoints(T const& Size, LViewport const& Viewport) noexcept;

} /* ~Namespace Detail */

enum struct ENodeSize : u8
{
    //# Static points may vary for each monitor and/or platform. But they behave consistently during an application launch.
    StaticPoints,
    //# Points behave differently depending on user preferences and the size of the viewport they are painted in.
    Points,
};
//# One-dimensional widget size.
struct LNodeSize1 final
{
    ENodeSize Type{ ENodeSize::Points };
    f32 Size;

    FORCEINLINE constexpr LNodeSize1() noexcept : Size{} {}
    FORCEINLINE constexpr LNodeSize1(ENodeSize Type, f32 Size) noexcept : Type{Type}, Size{Size} {}
    FORCEINLINE constexpr LNodeSize1(LNodeSize1 const& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return;
    }
    FORCEINLINE constexpr LNodeSize1& operator=(LNodeSize1 const& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return *this;
    }

    NODISCARD FORCEINLINE constexpr bool operator==(LNodeSize1 const& Other) const noexcept
    {
        return this->Type == Other.Type && this->Size == Other.Size;
    }
    NODISCARD FORCEINLINE constexpr LNodeSize1 operator+(LNodeSize1 const& Other) const noexcept
    {
        check(this->Type == Other.Type)
        return LNodeSize1{this->Type, this->Size + Other.Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize1 operator-(LNodeSize1 const& Other) const noexcept
    {
        check(this->Type == Other.Type)
        return LNodeSize1{this->Type, this->Size - Other.Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize1 operator-() const noexcept
    {
        return LNodeSize1{this->Type, -this->Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize1 operator*(f32 Scalar) const noexcept
    {
        return LNodeSize1{this->Type, this->Size * Scalar};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize1 operator/(f32 Scalar) const noexcept
    {
        return LNodeSize1{this->Type, this->Size / Scalar};
    }

    NODISCARD FORCEINLINE constexpr f32 InStaticPoints(LViewport const& Viewport) const noexcept
    {
        return Detail::GetNodeSizeInStaticPoints(*this, Viewport);
    }
};
//# Two-dimensional widget size.
struct LNodeSize2 final
{
    ENodeSize Type{ ENodeSize::StaticPoints };
    LVec2F Size;

    FORCEINLINE constexpr LNodeSize2() noexcept : Size{maths::zero_vector<LVec2F>} {}
    FORCEINLINE constexpr LNodeSize2(LNodeSize2 const& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = WidgetSize2.Size;
        return;
    }
    FORCEINLINE constexpr LNodeSize2& operator=(LNodeSize2 const& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = WidgetSize2.Size;
        return *this;
    }
    FORCEINLINE constexpr LNodeSize2(ENodeSize Type, f32 X, f32 Y) noexcept : Type{Type}, Size{X, Y} {}
    FORCEINLINE constexpr LNodeSize2(ENodeSize Type, LVec2F Size) noexcept : Type{Type}, Size{Size} {}
    FORCEINLINE constexpr LNodeSize2(LNodeSize1 X, f32 Y) noexcept : Type{X.Type}, Size{X.Size, Y} {}
    FORCEINLINE constexpr LNodeSize2(f32 X, LNodeSize1 Y) noexcept : Type{Y.Type}, Size{X, Y.Size} {}

    NODISCARD FORCEINLINE constexpr bool operator==(LNodeSize2 const& Other) const noexcept
    {
        return this->Type == Other.Type && this->Size == Other.Size;
    }
    NODISCARD FORCEINLINE constexpr LNodeSize2 operator+(LNodeSize2 const& Other) const noexcept
    {
        check(this->Type == Other.Type)
        return {this->Type, this->Size + Other.Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize2 operator-(LNodeSize2 const& Other) const noexcept
    {
        check(this->Type == Other.Type)
        return {this->Type, this->Size - Other.Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize2 operator-() const noexcept
    {
        return {this->Type, -this->Size};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize2 operator*(f32 Scalar) const noexcept
    {
        return {this->Type, this->Size * Scalar};
    }
    NODISCARD FORCEINLINE constexpr LNodeSize2 operator/(f32 Scalar) const noexcept
    {
        return {this->Type, this->Size / Scalar};
    }

    NODISCARD FORCEINLINE constexpr LVec2F InStaticPoints(LViewport const& Viewport) const noexcept
    {
        return Detail::GetNodeSizeInStaticPoints(*this, Viewport);
    }
};

struct LWhitespace final
{
    ENodeSize Type{ ENodeSize::StaticPoints };
    LVec4F Size;

    FORCEINLINE constexpr LWhitespace() noexcept : Size{maths::zero_vector<LVec4F>} {}

    FORCEINLINE constexpr LWhitespace(ENodeSize Type, f32 Scalar) noexcept : Type{Type}, Size{Scalar, Scalar, Scalar, Scalar} {}
    FORCEINLINE constexpr LWhitespace(ENodeSize Type, f32 Horizontal, f32 Vertical) noexcept : Type{Type}, Size{Horizontal, Vertical, Horizontal, Vertical} {}
    FORCEINLINE constexpr LWhitespace(ENodeSize Type, f32 Left, f32 Top, f32 Right, f32 South) noexcept : Type{Type}, Size{Left, Top, Right, South} {}
    FORCEINLINE constexpr LWhitespace(ENodeSize Type, LVec4F Size) noexcept : Type{Type}, Size{Size} {}

    FORCEINLINE constexpr LWhitespace(LNodeSize1 Scalar) noexcept : Type{Scalar.Type}, Size{Scalar.Size} {}
    FORCEINLINE constexpr LWhitespace(LNodeSize1 Horizontal, f32 Vertical) noexcept : Type{Horizontal.Type}, Size{Horizontal.Size, Vertical, Horizontal.Size, Vertical} {}
    FORCEINLINE constexpr LWhitespace(f32 Horizontal, LNodeSize1 Vertical) noexcept : Type{Vertical.Type}, Size{Horizontal, Vertical.Size, Horizontal, Vertical.Size} {}
    FORCEINLINE constexpr LWhitespace(LNodeSize1 Left, f32 Top, f32 Right, f32 South) noexcept : Type{Left.Type}, Size{Left.Size, Top, Right, South} {}

    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LWhitespace)

    FORCEINLINE constexpr ~LWhitespace() noexcept = default;

    FORCEINLINE constexpr LWhitespace operator*(f32 Scalar) const noexcept { return {this->Type, this->Size * Scalar}; }
    FORCEINLINE constexpr LWhitespace operator/(f32 Scalar) const noexcept { return {this->Type, this->Size / Scalar}; }
    FORCEINLINE constexpr bool operator==(LWhitespace const& Rhs) const noexcept { return Type == Rhs.Type && maths::eq_e(this->Size, Rhs.Size); }

    //# All return values are always: first horizontal, then vertical.
    FORCEINLINE constexpr LNodeSize1 GetLeftOffset() const noexcept { return {this->Type, this->Size.x}; }
    FORCEINLINE constexpr LNodeSize1 GetTopOffset() const noexcept { return {this->Type, this->Size.y}; }
    FORCEINLINE constexpr LNodeSize2 GetTopLeftOffset() const noexcept { return {this->Type, this->Size.x, this->Size.y}; }
    FORCEINLINE constexpr LNodeSize1 GetRightOffset() const noexcept { return {this->Type, this->Size.z}; }
    FORCEINLINE constexpr LNodeSize1 GetBottomOffset() const noexcept { return {this->Type, this->Size.w}; }
    FORCEINLINE constexpr LNodeSize2 GetBottomRightOffset() const noexcept { return {this->Type, this->Size.z, this->Size.w}; }
    FORCEINLINE constexpr LNodeSize1 GetDesiredSizeX() const noexcept { return {this->Type, this->Size.x + this->Size.z}; }
    FORCEINLINE constexpr LNodeSize1 GetDesiredSizeY() const noexcept { return {this->Type, this->Size.y + this->Size.w}; }
    FORCEINLINE constexpr LNodeSize2 GetDesiredSize() const noexcept { return {this->Type, this->Size.x + this->Size.z, this->Size.y + this->Size.w}; }

    FORCEINLINE constexpr LVec4F InStaticPoints(LViewport const& Viewport) const noexcept
    {
        return Detail::GetNodeSizeInStaticPoints(*this, Viewport);
    }
};
typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

//#
//# How to anchor a child to its parent if the parent can have children.
//# This enum can be used for simple and complex anchoring.
//#
enum struct EAnchor
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
    //# Common compounds.
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
//#
//# The anchor for complex anchoring only.
//# This struct may not be used when dealing with widgets that only support a simple layout flow.
//#
struct LAnchor final
{
    union
    {
#if JAFG_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif /* JAFG_WITH_GCC */
        struct
        {
            f32 MinX;
            f32 MinY;
            f32 MaxX;
            f32 MaxY;
        };
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */

        //#
        //# Minimum to maximum.
        //# Left, Top, Right, Bottom.
        //#
        LVec4F Anchors;
    };

    FORCEINLINE constexpr LAnchor() noexcept : Anchors(maths::zero_vector<LVec4F>) { }
    FORCEINLINE constexpr LAnchor(f32 InUniformAnchors) noexcept
        : Anchors{InUniformAnchors, InUniformAnchors, InUniformAnchors, InUniformAnchors}
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(f32 InHorizontalUniform, f32 InVerticalUniform) noexcept
        : Anchors{InHorizontalUniform, InVerticalUniform, InHorizontalUniform, InVerticalUniform}
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(f32 InMinX, f32 InMinY, f32 InMaxX, f32 InMaxY) noexcept
        : Anchors{InMinX, InMinY, InMaxX, InMaxY}
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(EAnchor InAnchors) noexcept
        : Anchors{maths::zero_vector<LVec4F>}
    {
        this->ApplyConstraints(InAnchors);
    }
    FORCEINLINE constexpr LAnchor(LAnchor const& InOther, EAnchor InConstraints) noexcept
        : Anchors{InOther.Anchors}
    {
        this->ApplyConstraints(InConstraints);
    }
    FORCEINLINE constexpr LAnchor(LVec4F const& InOther, const EAnchor InConstraints) noexcept
        : Anchors(InOther)
    {
        this->ApplyConstraints(InConstraints);
    }
    FORCEINLINE constexpr LAnchor(LVec4F const& InAnchors) noexcept
        : Anchors{InAnchors}
    {
        check(this->IsNormalized())
    }
    FORCEINLINE constexpr LAnchor(LAnchor const& InOther) noexcept
        : Anchors{InOther.Anchors}
    {
        check(this->IsNormalized())
    }

    FORCEINLINE constexpr LAnchor& operator=(LAnchor const& Rhs) noexcept { this->Anchors = Rhs.Anchors; return *this; }
    FORCEINLINE constexpr LAnchor operator+(LAnchor const& Rhs) const noexcept { return LAnchor{this->Anchors + Rhs.Anchors}; }

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

    inline constexpr void ApplyConstraints(EAnchor InConstraints) noexcept;

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
inline constexpr void LAnchor::ApplyConstraints(EAnchor InConstraints) noexcept
{
    auto Constraints{std::to_underlying(InConstraints)};

    if (Constraints & std::to_underlying(EAnchor::VTop))    { this->Anchors += Anchors::VTop.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::VCenter)) { this->Anchors += Anchors::VCenter.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::VBottom)) { this->Anchors += Anchors::VBottom.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::HLeft))   { this->Anchors += Anchors::HLeft.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::HCenter)) { this->Anchors += Anchors::HCenter.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::HRight))  { this->Anchors += Anchors::HRight.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::VFill))   { this->Anchors += Anchors::VFill.Anchors; }
    if (Constraints & std::to_underlying(EAnchor::HFill))   { this->Anchors += Anchors::HFill.Anchors; }

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

struct LNodeReply final
{
    FORCEINLINE constexpr LNodeReply() noexcept
        : bHandled{}, bConsumesEvent{true}, Cursor{ECursor::None} {}
    FORCEINLINE constexpr LNodeReply(bool bInHandled, bool bInConsumesEvent = true) noexcept
        : bHandled{bInHandled}, bConsumesEvent{bInConsumesEvent}, Cursor{ECursor::None} {}
    FORCEINLINE constexpr LNodeReply(std::optional<TClassStorage<WNode>> InFocus, bool bInConsumesEvent = true) noexcept
        : bHandled{true}, bConsumesEvent{bInConsumesEvent}, Cursor{ECursor::None}, Focus{std::move(InFocus)} {}
    FORCEINLINE constexpr LNodeReply(ECursor InCursor, std::optional<TClassStorage<WNode>> InFocus = {}, bool bInConsumesEvent = true) noexcept
        : bHandled{true}, bConsumesEvent{bInConsumesEvent}, Cursor{InCursor}, Focus{std::move(InFocus)} {}
    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LNodeReply)
    FORCEINLINE constexpr ~LNodeReply() noexcept = default;

    FORCEINLINE static constexpr LNodeReply Handled(bool bConsumesEvent = true) noexcept { return LNodeReply{true, bConsumesEvent}; }
    FORCEINLINE static constexpr LNodeReply Unhandled() noexcept { return {}; }

    FORCEINLINE constexpr bool IsHandled() const noexcept { return this->bHandled; }
    FORCEINLINE constexpr bool DoesConsume() const noexcept { return this->bConsumesEvent; }
    FORCEINLINE constexpr ECursor GetCursor() const noexcept { return this->Cursor; }
    FORCEINLINE constexpr bool IsFocusValid() const noexcept { return this->Focus.has_value(); }
    FORCEINLINE constexpr TClassStorage<WNode> GetFocus() noexcept { check(this->IsFocusValid()) return *this->Focus; }
    FORCEINLINE constexpr TClassStorage<WNode> const& GetFocus() const noexcept { check(this->IsFocusValid()) return *this->Focus; }

private:

    bool bHandled;
    bool bConsumesEvent;
    ECursor Cursor;
    std::optional<TClassStorage<WNode>> Focus;
};

namespace Detail
{

//#
//# Defines the state flags a node can be in. Not all combinations are valid.
//# @see #WNode::_check_StateInvariant
//#
enum struct ENodeStateBits
{
    //# The node has nothing.
    Identity = 0x0 << 0,
    //#
    //# The node is advised to either
    //# - let events fallthrough to children (even if they are itself not in a state to handle any events)
    //# - or decide on their own to either handle events or let them bubble down.
    //#
    Fallthrough = 0x1 << 0,
    //#
    //# Events have been dispatched for this node. Event counterparts will be called later on (this is guaranteed by jafg).
    //# Events that dispatch must be fallthrough.
    //#
    Dispatched = 0x1 << 1,
    //#
    //# Node is focused. The viewport gives special care for them.
    //# @see #LViewport
    //#
    Focused = 0x1 << 2,
};
ENUM_STRUCT_FLAGS(ENodeStateBits, ENodeStateFlags)
inline constexpr ENodeStateFlags NodeStateSwept{ENodeStateBits::Fallthrough | ENodeStateBits::Dispatched};

struct LViewport2OuterProj
{
    NODISCARD inline constexpr decltype(auto) operator()(auto& Viewport) const noexcept
    {
        return Viewport.GetOuter();
    }
};

} /* ~Namespace Detail */

#define NODE_FACTORY_PARENT(Node) public Node::Super::LFactory
#define NODE_FACTORY_BODY(Node) typedef Node TFactoredNode;
#define DETAIL_JAFG_NODE_FACTORY_SELF() (*StaticCastChecked<typename std::remove_cvref_t<decltype(Self)>::TFactoredNode>(&Self.GetRawNode()))
#define NODE_FACTORY_SELF() check(Self._IsDecommissioned() == false) DETAIL_JAFG_NODE_FACTORY_SELF()
#define NODE_FACTORY_RESULT() std::forward<decltype(Self)>(Self)

struct LNodeSweepInfo final
{
    //# The translation that is recommended for children of a #WParent to use while sweeping.
    const LVec2F Translation{ maths::zero_vector<LVec2F> };
    //# Hint for an additional translation for all children.
    const LVec2F ChildTranslationHint{ maths::zero_vector<LVec2F> };
    //# Hint to not sweep children.
    const bool bSweepChildren{ true };
};

struct LNodeKeyEventInfo final
{
    LFrontend const& Frontend;
    LSurface& Surface;
    LViewport& Viewport;
    std::optional<std::reference_wrapper<WNode>> FocusedNode;

    std::optional<LVec2F> CursorLocation;

    const LVec2F Translation;
};

//# Initializer for the dynamic ctors for WNodes.
typedef Detail::TCxxDynamicInit<LViewport, Detail::LViewport2OuterProj> LNodeDynamicInit;
//# Initializer for the static ctors for WNodes.
template<typename TCxxClass>
using TNodeStaticInit = Detail::TCxxStaticInitBase<LViewport, TCxxClass, Detail::LViewport2OuterProj>;

//# Generic API to for allowing subclasses of nodes to be injected in the construction of a node.
template<typename TNode> requires std::is_base_of_v<WNode, TNode>
struct TNodeInjection final
{
    typedef TNode TBaseInjectableNode;
    TSubclassOf<TNode> Class;
    std::move_only_function<void(typename TNode::LFactory& Factory)> Delegate;
};
#define JAFG_NODE_FACTORY_INJECTION(BaseNode, Injection, TargetInjection) \
        template<typename TNode> requires std::is_base_of_v<BaseNode, TNode> \
        decltype(auto) Injection(this auto&& Self, TSubclassOf<TNode> Class) noexcept \
        { \
            NODE_FACTORY_SELF().TargetInjection.Class = Class; \
            return NODE_FACTORY_RESULT(); \
        } \
        template<typename TNode, typename TFunc> requires std::is_base_of_v<BaseNode, TNode> \
        decltype(auto) Injection(this auto&& Self, TSubclassOf<TNode> Class, TFunc Delegate) noexcept \
            requires std::is_invocable_r_v<void, TFunc, typename TNode::LFactory&> \
        { \
            Self.Injection(Class); \
            NODE_FACTORY_SELF().TargetInjection.Delegate = [D=std::move(Delegate)] \
            (typename decltype(DETAIL_JAFG_NODE_FACTORY_SELF().TargetInjection)::TBaseInjectableNode::LFactory& F) \
            { \
                check(F.GetRawNode().template IsA<TNode>()) \
                std::invoke(D, static_cast<typename TNode::LFactory&>(F)); \
            }; \
            return NODE_FACTORY_RESULT(); \
        }

namespace Detail
{

struct LOuter2ViewportProj
{
    NODISCARD inline constexpr LNodeDynamicInit operator()(LCxxDynamicInit const& Init) const noexcept;
};

struct LBeginStylingFnResult final
{
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
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    inline typename TNode::LFactory& Root(LJxxClass const& Class);
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    inline typename TNode::LFactory& Root(TNodeInjection<TNode> const& Injection)
    {
        return this->Root<TNode>(Injection.Class.GetClassOrDefault());
    }

    //# Set the root with the provided static type directly.
    template<typename TNode, typename... TArgs>
        requires std::is_base_of_v<WNode, TNode>
              // && std::is_constructible_v<TNode, TNodeStaticInit<TNode> const&, TArgs&&...>
    inline typename TNode::LFactory& StaticRoot(TArgs&&... Args) noexcept;
};

} /* ~Namespace Detail */

#define JAFG_NODE_FACTORY_DELEGATE_BINDINGS(f, callable) \
        decltype(auto) f(this auto&& Self, decltype(std::remove_cvref_t<decltype(Self)>::TFactoredNode::callable) Event) noexcept \
    { \
        NODE_FACTORY_SELF().callable = std::move(Event); \
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
        NODE_FACTORY_SELF().Anchor = Anchor;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Anchor(this auto&& Self, EAnchor Anchor) noexcept
    {
        NODE_FACTORY_SELF().Anchor = Anchor;
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) MinMaxDesiredSize(this auto&& Self, LNodeSize2 Size) noexcept
    {
        Self.MinDesiredSize(Size);
        Self.MaxDesiredSize(Size);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) MinDesiredSize(this auto&& Self, LNodeSize2 Size) noexcept
    {
        NODE_FACTORY_SELF().MinDesiredSize = Size;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) MaxDesiredSize(this auto&& Self, LNodeSize2 Size) noexcept
    {
        NODE_FACTORY_SELF().MaxDesiredSize = Size;
        return NODE_FACTORY_RESULT();
    }

    template<typename T>
    decltype(auto) Inject(this auto&& Self, TNodeInjection<T>& Injection) noexcept
    {
        if (Injection.Delegate)
        {
            check(Injection.Class.GetClassOrDefault().template DerivesFrom<T>())
            Self.Delegate(Injection.Delegate);
        }
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) Data(this auto&& Self, JNodeData& Data) noexcept
    {
        NODE_FACTORY_SELF().AddData(Data);
        return NODE_FACTORY_RESULT();
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnCursorEnter, OnCursorEnterEvent)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnCursorMoved, OnCursorMovedEvent)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnCursorLeave, OnCursorLeaveEvent)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnKeyEventFocused, OnKeyEventFocusedDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnKeyEventUnfocused, OnKeyEventUnfocusedDelegate)
};

//# Pass arbitrary data typesafe down the hierarchy. Using this often is a good indicator for bad design.
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class ENGINE_API JNodeData : public JCxxClass
{
    GENERATED_CLASS_BODY()
protected:
    DEFAULT_OBJECT_CONSTRUCTORS(JNodeData)
};

//#
//# The base class for everything that can be interpreted as a visual element.
//# Generally speaking, inheriting from this class directly is not recommended.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryNode, EJxxClassBits::Abstract)
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
        check(!this->Parent)
    }
#endif /* JAFG_DO_CHECKS */

    // JCxxClass implementation
    virtual void BeginLife() override final
    {
        Super::BeginLife();
        this->Construct();
    }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override final
    {
        this->Destruct();
        Super::OnGarbage(Reason);
    }
    // ~JCxxClass implementation

    typedef TRaiiDelegateHandle<TMulticastDelegate<bool()>> LRaiiViewportHandle;
    typedef TRaiiDelegateHandle<TMulticastDelegate<bool(LRenderInfo const& Info)>> LRaiiPreDrawHandle;
    typedef TRaiiDelegateHandle<TMulticastDelegate<bool(LNodeRenderInfo const& Info)>> LRaiiDrawHandle;


    //#
    //# Called when this widget is constructed. This method replaces the #BeginLife super method.
    //#
    inline virtual void Construct() {}

    //#
    //# Called when this widget is being ticked.
    //# @see #EWidgetVisibility for more information about when to tick a widget.
    //#
    inline virtual void Tick() {}

    //#
    //# Called when this widget is being destructed. This method replaces the #EndLife super method.
    //#
    inline virtual void Destruct()
    {
        check(Tasks::IsOnMasterThread())
        checkCode(_check_Destruct())
        this->_ResetFocusState();
        this->Parent = nullptr;
    }

    //#
    //# The paint function for a widget. Only called if the widget is visible and paintable.
    //# Do not update any values of any widgets when inside this method. This methods might get called from any thread
    //# and might be called multiple times per tick.
    //# Automatically called by the owning viewport if applicable. Do not call manually.
    //#
    virtual void Draw(LNodeRenderInfo const& Info) const { check(this->ShouldNowDraw()) }

    //# Use this method to pass arbitrary typesafe data to the widget.
    virtual algo::reply AddData(JNodeData& Data) { return {}; }

    //# Aabb test this node.
    NODISCARD FORCEINLINE constexpr bool AabbTest(LNodeSweepInfo const& Info, LVec2F const& Location) const noexcept
    {
        return this->TransformsWidgetLayout() && maths::aabb_point({
            .Offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
            .Extent = this->GetAnchoredSize_v2()
            }, Location);
    }
    //# Convenience overload for #AabbTest to reduce boilerplate.
    NODISCARD FORCEINLINE constexpr bool AabbTest(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location) const noexcept
    {
        return Location && this->AabbTest(Info, *Location);
    }
    //# Convenience overload for #AabbTest to reduce boilerplate.
    NODISCARD FORCEINLINE constexpr bool AabbTest(LNodeKeyEventInfo const& Info) const noexcept
    {
        return Info.CursorLocation && this->AabbTest({.Translation = Info.Translation}, *Info.CursorLocation);
    }

    //# Sweep this node and all its children from bottom to top for focus.
    FORCEINLINE virtual LNodeReply SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location);
    //# Public internal method for jafg. Do not use.
    FORCEINLINE void _ResetFocusState() noexcept;

    //# A non bubbled event that is called when a node receives a focus.
    virtual void OnFocusReceived()
    {
        check(!this->_check_bFocused)
        checkCode(this->_check_bFocused = true)
        check(!(this->NodeState & Detail::ENodeStateBits::Focused))
        checkCode(this->_check_StateInvariant())
        this->NodeState |= Detail::ENodeStateBits::Focused;
        checkCode(this->_check_StateInvariant())
    }
    //# Called each tick for a focused node regardless if the parent node allways ticking.
    virtual void OnFocusTick()
    {
        check(this->_check_bFocused)
        check(this->NodeState & Detail::ENodeStateBits::Focused)
    }
    //# Always called if the node loses focus. This is guaranteed.
    virtual void OnFocusLost()
    {
        check(this->_check_bFocused)
        checkCode(this->_check_bFocused = false)
        check(this->NodeState & Detail::ENodeStateBits::Focused)
        checkCode(this->_check_StateInvariant())
        this->NodeState &= ~Detail::ENodeStateFlags{Detail::ENodeStateBits::Focused};
        checkCode(this->_check_StateInvariant())
    }

    //# Sweep an optional cursor location over this node.
    virtual LNodeReply Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location);
    //# Public internal method for jafg. Do not use.
    FORCEINLINE void _RemoveFallthroughState() noexcept
    {
        if (this->NodeState & Detail::ENodeStateBits::Fallthrough)
        {
            check(!(this->NodeState & Detail::ENodeStateBits::Dispatched))
            this->NodeState &= ~Detail::ENodeStateFlags{Detail::ENodeStateBits::Fallthrough};
        }
    }
    //# Public internal method for jafg. Do not use.
    FORCEINLINE void _RemoveDispatchedState() noexcept
    {
        if (this->NodeState & Detail::ENodeStateBits::Dispatched)
        {
            check(this->NodeState & Detail::ENodeStateBits::Fallthrough)
            this->NodeState &= ~Detail::ENodeStateFlags{Detail::ENodeStateBits::Dispatched};
            this->OnCursorLeave();
        }
    }

    //#
    //# Called if the cursor has entered/moved/left this node.
    //# The following is guaranteed:
    //#  - #OnCursorEnter is always called before #OnCursorMoved or #OnCursorLeave.
    //#  - #OnCursorMoved is only called between the #OnCursorEnter and #OnCursorLeave.
    //#  - #OnCursorLeave is always called if #OnCursorEnter was called previously.
    //#
    TFunction2<LNodeReply(WNode& Node)> OnCursorEnterEvent;
    TFunction2<LNodeReply(WNode& Node)> OnCursorMovedEvent;
    TFunction2<void(WNode& Node)> OnCursorLeaveEvent;
    virtual LNodeReply OnCursorEnter()
    {
        check(!this->_check_bMouseEntered)
        checkCode(this->_check_bMouseEntered = true)
        if (this->OnCursorEnterEvent) if (auto Reply{this->OnCursorEnterEvent(*this)}; Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Handled();
    }
    virtual LNodeReply OnCursorMoved(LVec2F const& Location)
    {
        check(this->_check_bMouseEntered)
        if (this->OnCursorMovedEvent) if (auto Reply{this->OnCursorMovedEvent(*this)}; Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Handled();
    }
    virtual void OnCursorLeave()
    {
        check(this->_check_bMouseEntered)
        checkCode(this->_check_bMouseEntered = false)
        if (this->OnCursorLeaveEvent)
        {
            this->OnCursorLeaveEvent(*this);
        }
        return;
    }

    //#
    //# Only called if focused. Should not be bubbled.
    //# @note If not handled, then #OnKeyEventUnfocused might still get called even if this node was focused.
    //#
    TFunction2<LNodeReply(WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)> OnKeyEventFocusedDelegate;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
    {
        if (this->OnKeyEventFocusedDelegate) if (auto Reply{this->OnKeyEventFocusedDelegate(*this, Info, Event)}; Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Unhandled();
    }

    //#
    //# Called from top to bottom, but should be handled in reversed order (bottom to top) for all input receivable
    //# nodes, if their #Visibility allows it, for all consumable user inputs.
    //#
    TFunction2<LNodeReply(WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)> OnKeyEventUnfocusedDelegate;
    virtual LNodeReply OnKeyEventUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
    {
        if (this->OnKeyEventUnfocusedDelegate) if (auto Reply{this->OnKeyEventUnfocusedDelegate(*this, Info, Event)}; Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Unhandled();
    }

    //# @return Whether this is the focused widget.
    NODISCARD FORCEINLINE constexpr bool IsFocusWidget() const noexcept;
    //# @return Whether this is the focused widget or any of its children.
    NODISCARD FORCEINLINE virtual bool IsFocusWidgetTransitive() const noexcept; /* Warning: Slow. */

    //# Public internal method for jafg. Do not use.
    FORCEINLINE constexpr Detail::ENodeStateFlags _GetNodeState() const noexcept { return this->NodeState; }

    FORCEINLINE constexpr bool ShouldNowTick() const noexcept { return this->bAllowTick && NodeVisibility::IsTicked(this->Visibility); }
    FORCEINLINE constexpr bool GetRawShouldTick() const noexcept { return this->bAllowTick; }
    FORCEINLINE constexpr void SetShouldTick(bool bTick) noexcept { this->bAllowTick = bTick; }
    FORCEINLINE constexpr bool ShouldNowDraw() const noexcept { return NodeVisibility::IsDrawn(this->Visibility); }
    FORCEINLINE constexpr auto GetVisibility() const noexcept { return this->Visibility; }
    FORCEINLINE constexpr bool IsPainted() const noexcept { return this->ShouldNowDraw(); }
    FORCEINLINE constexpr bool IsHitTestable() const noexcept { return NodeVisibility::IsHitTestable(this->Visibility); }
    FORCEINLINE constexpr bool CanChildrenBeHitTestable() const noexcept { return NodeVisibility::IsDerivedHitTestable(this->Visibility); }
    FORCEINLINE constexpr bool ShouldCheckForInputs() const noexcept { return this->IsHitTestable() || this->CanChildrenBeHitTestable(); }
    FORCEINLINE constexpr bool TransformsWidgetLayout() const noexcept { return NodeVisibility::TransformsWidgetLayout(this->Visibility); }
    FORCEINLINE constexpr bool IsVisible() const noexcept { return this->Visibility == ENodeVisibility::Visible; }
    FORCEINLINE constexpr bool IsHidden() const noexcept { return this->Visibility == ENodeVisibility::Hidden; }
    FORCEINLINE constexpr bool IsCollapsed() const noexcept { return this->Visibility == ENodeVisibility::Collapsed; }
    FORCEINLINE constexpr bool IsDerivedHitTestInvisible() const noexcept { return this->Visibility == ENodeVisibility::DerivedHitTestInvisible; }
    FORCEINLINE constexpr bool IsTransitiveHitTestInvisible() const noexcept { return this->Visibility == ENodeVisibility::TransitiveHitTestInvisible; }
    FORCEINLINE constexpr bool IsIntransitiveHitTestInvisible() const noexcept { return this->Visibility == ENodeVisibility::IntransitiveHitTestInvisible; }
    void SetVisibility(ENodeVisibility InVisibility);

    //# Only if old and new are different.
    virtual void OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility) {}
    //# Called if the surface was resized.
    virtual void OnSurfaceResize() {}

    //# Mark this node and alls its children as garbage and remove them from their parent.
    virtual void RemoveFromParent2();
    //# Removes this node from the node tree, so that it can be reparented.
    NODISCARD FORCEINLINE auto RemoveFromTree(this auto&& Self) noexcept
    {
        auto Up{Self.RemoveFromTreeImpl()};
        auto Result{TJxxUnique<std::remove_cvref_t<decltype(Self)>>{
            StaticCast<std::remove_cvref_t<decltype(Self)>>(Up.get())
            }};
        Up.release();
        return Result;
    }

    FORCEINLINE bool IsParentValid() const noexcept { return this->Parent != nullptr; }
    FORCEINLINE WParent* GetParent() { return this->Parent; }
    FORCEINLINE WParent* GetParentChecked() { auto* Out{this->GetParent()}; check(Out); return Out; }
    FORCEINLINE WParent const* GetParent() const { return this->Parent; }
    FORCEINLINE WParent const* GetParentChecked() const { auto const* Out{this->GetParent()}; check(Out); return Out; }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetParentUntil() noexcept;
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetParentUntilChecked() noexcept
    {
        auto* Result{this->GetParentUntil<TNode>()};
        check(Result)
        return Result;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode const* GetParentUntil() const noexcept;
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode const* GetParentUntilChecked() const noexcept
    {
        auto* Result{this->GetParentUntil<TNode>()};
        check(Result)
        return Result;
    }

    //# @return The most outer parent of this widget or the widget itself if no outer parent.
    WNode& GetMostOuterParent() noexcept;
    WNode const& GetMostOuterParent() const noexcept;

    //#
    //# Searches for a node in this widget tree. Only searches nodes that are drawn.
    //# @return True if the target node exists in this widget tree and is visible.
    //#
    NODISCARD virtual bool IsNodeInVisiblePath(WNode const& Node) const { return this == &Node && this->ShouldNowDraw(); }
    NODISCARD inline WNode const* FindNodeInVisiblePath(TSubclassOf<WNode> Class) const noexcept { return this->FindNodeInVisiblePathImpl(Class); }
    NODISCARD inline WNode* FindNodeInVisiblePath(TSubclassOf<WNode> Class) noexcept { return this->FindNodeInVisiblePathImpl(Class); }
    NODISCARD FORCEINLINE WNode const* FindNodeInVisiblePathChecked(TSubclassOf<WNode> Class) const
    {
        auto const* Result{this->FindNodeInVisiblePath(Class)};
        check(Result)
        return Result;
    }
    NODISCARD FORCEINLINE WNode* FindNodeInVisiblePathChecked(TSubclassOf<WNode> Class)
    {
        auto* Result{this->FindNodeInVisiblePath(Class)};
        check(Result)
        return Result;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode const* FindNodeInVisiblePath() const noexcept
    {
        return StaticCast<TNode>(this->FindNodeInVisiblePath(TNode::StaticClass()));
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode* FindNodeInVisiblePath() noexcept
    {
        return StaticCast<TNode>(this->FindNodeInVisiblePath(TNode::StaticClass()));
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode const* FindNodeInVisiblePathChecked() const noexcept
    {
        auto const* Result{this->FindNodeInVisiblePath<TNode>()};
        check(Result)
        return Result;
    }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    NODISCARD FORCEINLINE TNode* FindNodeInVisiblePathChecked() noexcept
    {
        auto* Result{this->FindNodeInVisiblePath<TNode>()};
        check(Result)
        return Result;
    }
    NODISCARD virtual inline WNode const* FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) const noexcept
    {
        if (this->ShouldNowDraw() && this->IsA(Class))
        {
            return this;
        }
        return nullptr;
    }
    NODISCARD virtual inline WNode* FindNodeInVisiblePathImpl(TSubclassOf<WNode> Class) noexcept
    {
        if (this->ShouldNowDraw() && this->IsA(Class))
        {
            return this;
        }
        return nullptr;
    }

    FORCEINLINE constexpr LViewport& GetViewport() noexcept { return this->AttachedViewport; }
    FORCEINLINE constexpr LViewport const& GetViewport() const noexcept { return this->AttachedViewport; }

    //# Virtual update method for the desired size. Automatically called. Do not call manually.
    virtual void UpdateDesiredSize() const {}
    //#
    //# Update the #DesiredSize of a widget inside the overridden #UpdateDesiredSize method with this one.
    //# Do not call this method from outside the #UpdateDesiredSize method.
    //#
    FORCEINLINE void SetDesiredSize(LNodeSize2 Size) const noexcept { this->SetDesiredSizeInSpt(Size.InStaticPoints(this->AttachedViewport)); }
    void SetDesiredSizeInSpt(LVec2F Size) const noexcept;
    //# Internal usage only. Do not use unless you are a really smart person.
    FORCEINLINE void SetDesiredSizeUnsanitized(LVec2F Size) const { this->DesiredSize_v2 = Size; }
    FORCEINLINE LVec2F const& GetDesiredSize_v2() const { return this->DesiredSize_v2; }
    FORCEINLINE LVec2F const& GetDesiredSizeSmart_v2() const { return this->TransformsWidgetLayout() ? this->DesiredSize_v2 : maths::zero_vector<LVec2F>; }
    //# The min desired size. A widget will always be at least this size.
    LNodeSize2 MinDesiredSize;
    //# The max desired size. A widget will have at maximum this size. Zero means unbound. This includes max size of anchored nodes.
    LNodeSize2 MaxDesiredSize;

    //# Virtual update method for the anchored size. Automatically called. Do not call manually.
    virtual void UpdateAnchoredSize() const;
    void SetAnchoredSize(LVec2F const& Size) const noexcept;
    FORCEINLINE LVec2F const& GetAnchoredSize_v2() const noexcept { return this->AnchoredSize_v2; }
    FORCEINLINE LVec2F CopyAnchoredSize_v2() const noexcept { return this->AnchoredSize_v2; }
    //# The anchored size that was lost during #MaxDesiredSize clamp.
    FORCEINLINE LVec2F const& GetLostAnchoredSize_v2() const noexcept { return this->LostAnchoredSize_v2; }
    FORCEINLINE LVec2F CopyLostAnchoredSize_v2() const noexcept { return this->LostAnchoredSize_v2; }
    //# @return The anchored top-left corner of the widget relative to the given context's top-left corner.
    virtual LVec2F GetAnchoredTopLeftFromMostOuter() const;
    LVec2F GetAnchoredAndTranslatedTopLeftFromMostOuter(LVec2F const& Translation) const;

    std::optional<LMargin> GetMargin() const noexcept;
    FORCEINLINE std::optional<LMargin> GetMarginChecked() const noexcept { std::optional Out{this->GetMargin()}; check(Out.has_value()); return Out; }
    FORCEINLINE std::optional<LMargin> GetMarginAsserted() const noexcept { std::optional Out{this->GetMargin()}; jassert(Out.has_value()); return Out; }

    //# The anchor to use.
    LAnchor Anchor{ EAnchor::TopLeft };

    ENGINE_API LFrontend const& GetFrontend() const noexcept;
    ENGINE_API LFrontend& GetMutableFrontend() const noexcept;

    //# Internal function of Jafg. Do not call yourself.
    void _SetParentDangerous(WParent* InParent) noexcept { this->Parent = InParent; }

#if JAFG_DO_CHECKS
    void _check_StateInvariant();
    void _check_Destruct();
    FORCEINLINE bool& _check_MutableMouseEntered() noexcept { return this->_check_bMouseEntered; }
#endif /* JAFG_DO_CHECKS */

private:

    NODISCARD TJxxUnique<WNode> RemoveFromTreeImpl() noexcept;

#if JAFG_DO_CHECKS
    bool _check_bFocused{};
    bool _check_bMouseEntered{};
#endif /* JAFG_DO_CHECKS */
    bool bAllowTick{ true };
    Detail::ENodeStateFlags NodeState{ Detail::ENodeStateBits::Identity };
    ENodeVisibility Visibility{ ENodeVisibility::TransitiveHitTestInvisible };

    //# Parent of this widget.
    WParent* Parent{};

    LViewport& AttachedViewport;

    //# The desired size of this widget in pt.
    mutable LVec2F DesiredSize_v2{ maths::zero_vector<LVec2F> };

    //# The anchored size of this widget in pt.
    mutable LVec2F AnchoredSize_v2{ maths::zero_vector<LVec2F> };
    //# The anchored size that was lost during #MaxDesiredSize clamp in pt.
    mutable LVec2F LostAnchoredSize_v2{ maths::zero_vector<LVec2F> };
};

NODISCARD inline decltype(auto) Detail::LNodeFactoryBase::Unique(this auto&& Self) noexcept
{
    auto& Cache{Self.GetRawNode()};
#if JAFG_DO_CHECKS
    Self._Release();
    Self._Decommission();
#endif /* JAFG_DO_CHECKS */
    return TJxxUnique<typename std::remove_cvref_t<decltype(Self)>::TFactoredNode>(
        static_cast<typename std::remove_cvref_t<decltype(Self)>::TFactoredNode*>(&Cache)
        );
}

NODISCARD inline decltype(auto) Detail::LNodeFactoryBase::UniqueXs(this auto&& Self, TArray<TJxxUnique<WNode>>* Xs) noexcept
{
    check(Xs)
    for (auto* Sibling : Self.GetSiblings())
    {
        Xs->emplace_back(Sibling);
    }
    algo::orphan(&Self.GetMutableSiblings());
    return std::forward<decltype(Self)>(Self).Unique();
}

template<typename T> requires std::is_base_of_v<WNode, T>
inline decltype(auto) Detail::LNodeFactoryBase::SaveTo(this auto&& Self, T** Out) noexcept
{
    check(Out)
    *Out = StaticCastChecked<T>(&Self.GetRawNode());
    return std::forward<decltype(Self)>(Self);
}

inline decltype(auto) Detail::LNodeFactoryBase::operator+(this auto&& Self, LNodeFactoryBase&& F) noexcept
{
    Self.GetMutableSiblings().emplace_back(&F.GetRawNode());
    for (auto* Sibling : F.GetSiblings())
    {
        check(Sibling)
        Self.GetMutableSiblings().emplace_back(Sibling);
    }
    algo::orphan(&F.GetMutableSiblings());
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

    template<typename TNode, typename... TArgs> requires std::is_base_of_v<WNode, TNode>
        // && std::is_constructible_v<TNode, TNodeStaticInit<TNode> const&, TArgs&&...>
    typename TNode::LFactory Class(TArgs&&... Args) const
    {
        return typename TNode::LFactory{
            *ConstructNodeImpl(TNodeStaticInit<TNode>{.Outer=this->Viewport}, std::forward<TArgs>(Args)...).release()
            };
    }

    template<typename TNode=WNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class(LJxxClass const& Class) const
    {
        return typename TNode::LFactory{
            *ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Viewport,.Class=Class}).release()
            };
    }
    template<typename TNode=WNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class(TSubclassOf<TNode> Class) const
    {
        return typename TNode::LFactory{
            *ConstructNodeImpl(CastTo<TNode>{}, {.Outer=this->Viewport,.Class=Class.GetClassOrDefault()}).release()
            };
    }
    template<typename TNode=WNode> requires std::is_base_of_v<WNode, TNode>
    typename TNode::LFactory Class(TNodeInjection<TNode> const& Injection) const
    {
        return this->Class<TNode>(Injection.Class);
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
#define NewStaticNode(NodeClass, ...) ::Jafg::NewNode(this->GetViewport()).Class<NodeClass>(__VA_ARGS__)
#define NewDynamicNode(Subclass) ::Jafg::NewNode(this->GetViewport()).Class(Subclass)

} /* ~Namespace Jafg */

#include "Nodes/Viewport.h"

namespace Jafg
{

namespace Detail
{

template<typename T>
FORCEINLINE constexpr decltype(T::Size) GetNodeSizeInStaticPoints(T const& Size, LViewport const& Viewport) noexcept
{
    if (Size.Type == ENodeSize::StaticPoints)
    {
        return Size.Size;
    }
    EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    check(Scale != EApplicationScale::Auto)
    return Size.Size * LexToFloat(Scale);
}

inline constexpr LNodeDynamicInit LOuter2ViewportProj::operator()(LCxxDynamicInit const& Init) const noexcept
{
    check(Init.Outer.GetUserData())
    return {.Outer=*static_cast<LViewport*>(Init.Outer.GetUserData()),.Class=Init.Class};
}

} /* ~Namespace Detail */

inline WNode::WNode(LNodeDynamicInit const& Init) noexcept
    : Super{LCxxDynamicInit{.Outer=std::invoke(LNodeDynamicInit::Proj{}, Init.Outer),.Class=Init.Class}}
    , AttachedViewport{Init.Outer}
{
}

FORCEINLINE LNodeReply WNode::SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location)
{
    if (!this->IsHitTestable()) { return {}; }
    if (!this->AabbTest(Info, Location)) { return {}; }
    if (this->GetViewport().GetFocusedWidget() == this) { return LNodeReply::Handled(false); } return LNodeReply{TClassStorage{this}, false};
}

FORCEINLINE void WNode::_ResetFocusState() noexcept
{
    if ((this->NodeState & Detail::ENodeStateBits::Focused) != Detail::ENodeStateBits::Identity)
    {
        this->AttachedViewport.ChangeFocusImpl({});
        check(!(this->NodeState & Detail::ENodeStateBits::Focused))
    }
    return;
}

NODISCARD FORCEINLINE constexpr bool WNode::IsFocusWidget() const noexcept
{
    return this->AttachedViewport.GetFocusedWidget() == this;
}

NODISCARD FORCEINLINE bool WNode::IsFocusWidgetTransitive() const noexcept
{
    return this->IsFocusWidget();
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE TNode const* LViewport::GetFocusedWidget() const
{
    return DynamicCast<TNode>(this->FocusedWidget.get());
}
FORCEINLINE constexpr WNode const* LViewport::GetFocusedWidget() const
{
    return this->FocusedWidget.get();
}

} /* ~Namespace Jafg */

FORCEINLINE constexpr Jafg::LNodeSize1 operator ""_spt(unsigned long long Value) noexcept
{
    return {Jafg::ENodeSize::StaticPoints, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize1 operator ""_spt(long double Value) noexcept
{
    return {Jafg::ENodeSize::StaticPoints, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize1 operator ""_pt(unsigned long long Value) noexcept
{
    return {Jafg::ENodeSize::Points, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize1 operator ""_pt(long double Value) noexcept
{
    return {Jafg::ENodeSize::Points, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize2 operator ""_spt2(unsigned long long Value) noexcept
{
    return {Jafg::ENodeSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize2 operator ""_spt2(long double Value) noexcept
{
    return {Jafg::ENodeSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize2 operator ""_pt2(unsigned long long Value) noexcept
{
    return {Jafg::ENodeSize::Points, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LNodeSize2 operator ""_pt2(long double Value) noexcept
{
    return {Jafg::ENodeSize::Points, static_cast<f32>(Value), static_cast<f32>(Value)};
}
