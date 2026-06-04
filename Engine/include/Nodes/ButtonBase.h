// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"

#define JAFG_NODE_BUTTON_BOILERPLATE_Construct() \
    virtual void Construct() override \
    { \
        Super::Construct(); \
        this->ButtonBase_Construct(); \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE_SweepFocus() \
    virtual ::Jafg::LNodeReply SweepFocus(::Jafg::LNodeSweepInfo const& Info, LVec2F const& Location) override \
    {\
        if (auto Result{Super::SweepFocus(Info, Location)}; Result.IsHandled()) \
        {\
            if (!this->bEnabled) \
            { \
                return ::Jafg::LNodeReply::Handled(); \
            } \
            return Result; \
        } \
        return {}; \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE_OnCursorEnter() \
    virtual LNodeReply OnCursorEnter() override \
    { \
        if (!this->bEnabled)\
        {\
            check(!this->_ButtonBase_Owner._check_MutableMouseEntered()) \
            checkCode(this->_ButtonBase_Owner._check_MutableMouseEntered() = true)\
            return {};\
        }\
        if (this->bUpdateBrushOnStateChange && !this->bSelected) \
        { \
            this->_ButtonBase_SetBrush(::Jafg::EStyleBits::Hover); \
        } \
        return Super::OnCursorEnter(); \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE_OnCursorLeave() \
    virtual void OnCursorLeave() override \
    { \
        if (!this->bEnabled)\
        {\
            check(this->_ButtonBase_Owner._check_MutableMouseEntered()) \
            checkCode(this->_ButtonBase_Owner._check_MutableMouseEntered() = false)\
            return;\
        }\
        if (this->bUpdateBrushOnStateChange && !this->bSelected) \
        { \
            this->_ButtonBase_SetBrush(::Jafg::EStyleBits::Normal); \
        } \
        Super::OnCursorLeave(); \
        return; \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE_OnKeyEventFocused() \
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override \
    {\
        return this->ButtonBase_OnKeyEventFocused(Info, Event); \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE() \
    JAFG_NODE_BUTTON_BOILERPLATE_Construct() \
    JAFG_NODE_BUTTON_BOILERPLATE_SweepFocus() \
    JAFG_NODE_BUTTON_BOILERPLATE_OnCursorEnter() \
    JAFG_NODE_BUTTON_BOILERPLATE_OnCursorLeave() \
    JAFG_NODE_BUTTON_BOILERPLATE_OnKeyEventFocused()

namespace Jafg
{

struct LRegionBrush;

enum struct EStyleBits
{
    Identity = 0x0 << 0,
    Normal = 0x1 << 0,
    Hover = 0x1 << 1,
    Press = 0x1 << 2,
    Selected = 0x1 << 3,
    Disabled = 0x1 << 4,

    count = 5,
    ActiveCombi = Hover | Press | Selected,
    InactiveCombi = Normal | Disabled,
};
ENUM_STRUCT_FLAGS(EStyleBits, EStyleFlags)

inline constexpr LStringView LexToString(EStyleBits Bit) noexcept
{
    switch (Bit)
    {
    case EStyleBits::Identity: return "Identity";
    case EStyleBits::Normal: return "Normal";
    case EStyleBits::Hover: return "Hover";
    case EStyleBits::Press: return "Press";
    case EStyleBits::Selected: return "Selected";
    case EStyleBits::Disabled: return "Disabled";
    case EStyleBits::ActiveCombi: return "ActiveCombi";
    case EStyleBits::InactiveCombi: return "InactiveCombi";
    default: std::unreachable();
    }
}

//# To use your flags here you have to declare the #count member as done in #EStyleBits.
template<typename UFlags, auto... BrushProj> requires std::is_base_of_v<Detail::LFlags, UFlags>
struct LStyleBase
{
    typedef typename UFlags::type flag_type;

    constexpr void ForEachEverywhere(this auto&& Self, auto&& F) noexcept
    {
        ([&](auto&& Brush) -> void{ (F(Self.*Brush));}(BrushProj), ...);
    }

    template<auto Proj>
    constexpr void SetEverywhere(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept
    {
        ((((Self.*BrushProj).*Proj) = Value), ...);
    }
    template<auto... Proj>
    constexpr void ChainEverywhere(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept
    {
        static_assert(sizeof...(Proj) == sizeof...(Values));
        ([&](auto&& Brush) -> void{ ((((Self.*Brush).*Proj) = Values), ...);}(BrushProj), ...);
        return;
    }

    template<UFlags Flags, auto Proj>
    constexpr void Set(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept
    {
        [&]<std::size_t... Seq>(std::index_sequence<Seq...>)
        {
            auto Apply{[&]<std::size_t Idx>(auto&& Brush)
            {
                for (auto FlagIdx{0uz}; FlagIdx < std::to_underlying(flag_type::count); ++FlagIdx)
                {
                    if (!!(Flags & UFlags{0x01 << FlagIdx}))
                    {
                        if (Idx == FlagIdx)
                        {
                            ((Self.*Brush).*Proj) = Value;
                        }
                    }
                }
            }};
            (Apply.template operator()<Seq>(BrushProj), ...);
        }(std::make_index_sequence<sizeof...(BrushProj)>{});
        return;
    }

    template<auto P, typename Obj, typename V>
    constexpr void Assign(Obj&& Obj_, V&& Value)
    {
        Obj_.*P = std::forward<V>(Value);
    }

    template<std::size_t N, auto First, auto... Rest>
    inline static constexpr auto nth_value = []() {
    if constexpr (N == 0) return First;
    else return nth_value<N - 1, Rest...>;
    }();

    template<UFlags Flags, auto... Proj>
    constexpr void Chain(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept
    {
        static_assert(sizeof...(Proj) == sizeof...(Values));

        // Pack Values into a tuple so we can index into them
        auto ValTuple = std::forward_as_tuple(Values...);

        [&]<std::size_t... Seq>(std::index_sequence<Seq...>)
        {
            // For each brush member pointer (indexed by BrushSeq)
            auto Apply{[&]<std::size_t BrushSeq>(auto&& Brush)
            {
                for (auto FlagIdx{0uz}; FlagIdx < std::to_underlying(flag_type::count); ++FlagIdx)
                {
                    if (!!(Flags & UFlags{0x01 << FlagIdx}))
                    {
                        if (BrushSeq == FlagIdx)
                        {
                            constexpr auto ProjTuple = std::make_tuple(std::integral_constant<decltype(Proj), Proj>{}...);
                            [&]<std::size_t... ProjSeq>(std::index_sequence<ProjSeq...>)
                            {
                                auto assign_one = [&]<std::size_t N>() {
                                    constexpr auto MPtr = std::tuple_element_t<N, decltype(ProjTuple)>::value;
                                    (Self.*Brush).*MPtr = std::get<N>(ValTuple);
                                };
                                (assign_one.template operator()<ProjSeq>(), ...);
                            }(std::make_index_sequence<sizeof...(Proj)>{});
                        }
                    }
                }
            }};
            (Apply.template operator()<Seq>(BrushProj), ...);
        }(std::make_index_sequence<sizeof...(BrushProj)>{});
    }
};

namespace Detail
{

template<typename TBrush>
struct TButtonBaseStyle
{
    TBrush NormalBrush;
    TBrush HoverBrush;
    TBrush PressBrush;
    TBrush SelectedBrush;
    TBrush DisabledBrush;
};

} /* ~Namespace Detail */

template<typename TBrush>
struct TButtonStyle : Detail::TButtonBaseStyle<TBrush>
    , LStyleBase<EStyleFlags,
        &Detail::TButtonBaseStyle<TBrush>::NormalBrush,
        &Detail::TButtonBaseStyle<TBrush>::HoverBrush,
        &Detail::TButtonBaseStyle<TBrush>::PressBrush,
        &Detail::TButtonBaseStyle<TBrush>::SelectedBrush,
        &Detail::TButtonBaseStyle<TBrush>::DisabledBrush
        >
{
    typedef EStyleFlags flag_type;
};

template<typename T, typename U>
FORCEINLINE constexpr void ApplyStyleBit(U& Style, T& Brush, EStyleBits Bit) noexcept
{
    switch (Bit)
    {
    case EStyleBits::Normal:
    {
        Brush = Style.NormalBrush;
        break;
    }
    case EStyleBits::Hover:
    {
        Brush = Style.HoverBrush;
        break;
    }
    case EStyleBits::Press:
    {
        Brush = Style.PressBrush;
        break;
    }
    case EStyleBits::Selected:
    {
        Brush = Style.SelectedBrush;
        break;
    }
    case EStyleBits::Disabled:
    {
        Brush = Style.DisabledBrush;
        break;
    }
    default:
    {
        std::unreachable();
    }
    }

    return;
}

//# Inherit from this to access common button logic.
template<typename TNode, typename TBrush, auto BrushProj> requires std::is_base_of_v<LRegionBrush, TBrush>
class TButtonBase
{
    typedef TButtonBase Derived;

public:

    //# The default visibility for buttons.
    inline static constexpr auto DefaultVisibility{ENodeVisibility::DerivedHitTestInvisible};

    typedef TBrush _ButtonBaseBrush;

    TButtonStyle<TBrush> Style;

    constexpr TButtonBase(TNode& InOwner) noexcept : _ButtonBase_Owner{InOwner} {}
    virtual ~TButtonBase() = default;

    EVENT_DECL(OnBrushChangedEvent, void(TNode& Self, EStyleBits Bit))
    inline virtual void OnBrushChanged(EStyleBits Bit) noexcept
    {
        if (this->OnBrushChangedEvent.IsValid())
        {
            this->OnBrushChangedEvent(this->_ButtonBase_Owner, Bit);
        }

        return;
    }

    NODISCARD constexpr bool IsEnabled() const noexcept { return this->bEnabled; }
    constexpr void SetEnabled(bool bInEnabled) noexcept
    {
        check(this->bSelected == false)
        if (this->bEnabled == bInEnabled)
        {
            return;
        }
        this->bEnabled = bInEnabled;

        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                this->_ButtonBase_SetBrush(EStyleBits::Normal);
            }
            else
            {
                this->_ButtonBase_SetBrush(EStyleBits::Disabled);
            }
        }

        this->OnEnabledStateChanged();
        return;
    }
    //# Delegate that is called when the enabled state changed.
    virtual void OnEnabledStateChanged() {}

    NODISCARD constexpr bool IsSelectable() const noexcept { return this->bSelectable; }
    constexpr void SetSelectable(bool bInSelectable) noexcept { check(this->bSelected == false) this->bSelectable = bInSelectable; }

    NODISCARD constexpr bool IsSelected() const noexcept { return this->bSelected; }
    constexpr void SetSelected(bool bInSelected) noexcept
    {
        check(this->bEnabled)
        check(this->bSelectable)
        if (this->bSelected == bInSelected)
        {
            return;
        }
        this->bSelected = bInSelected;

        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bSelected)
            {
                this->_ButtonBase_SetBrush(EStyleBits::Selected);
            }
            else
            {
                this->_ButtonBase_SetBrush(EStyleBits::Normal);
            }
        }

        this->OnSelectedStateChanged();
        return;
    }
    //# Delegate that is called when the selected state changed.
    virtual void OnSelectedStateChanged() {}

    NODISCARD constexpr bool IsUpdateBrushOnStateChange() const noexcept { return this->bUpdateBrushOnStateChange; }
    constexpr void SetUpdateBrushOnStateChange(bool bInUpdate) noexcept { this->bUpdateBrushOnStateChange = bInUpdate; }

    FORCEINLINE void _ButtonBase_SetBrush(EStyleBits Bits) noexcept
    {
        ApplyStyleBit(this->Style, this->_ButtonBase_Owner.*BrushProj, Bits);
        this->OnBrushChanged(Bits);
    }

protected:

    virtual void ButtonBase_Construct()
    {
        if (this->bUpdateBrushOnStateChange)
        {
            if (this->bEnabled)
            {
                if (this->bSelected)
                {
                    this->_ButtonBase_SetBrush(EStyleBits::Selected);
                }
                else
                {
                    this->_ButtonBase_SetBrush(EStyleBits::Normal);
                }
            }
            else
            {
                check(this->bSelected == false)
                this->_ButtonBase_SetBrush(EStyleBits::Disabled);
            }
        }
        return;
    }

    LNodeReply ButtonBase_OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
    {
        if (!this->bEnabled)
        {
            return {};
        }

        auto Result{LNodeReply::Unhandled()};

        if (Info.CursorLocation && this->_ButtonBase_Owner.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
        {
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
            {
                if (this->bUpdateBrushOnStateChange && !this->bSelected)
                {
                    this->_ButtonBase_SetBrush(EStyleBits::Press);
                }
                Result = LNodeReply::Handled();
            }
            else if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                    , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
            {
                if (this->bUpdateBrushOnStateChange && !this->bSelected)
                {
                    this->_ButtonBase_SetBrush(EStyleBits::Hover);
                }
                Result = LNodeReply::Handled();
            }
        }

        if (this->_ButtonBase_Owner.OnKeyEventFocusedDelegate)
        {
            if (auto Reply{this->_ButtonBase_Owner.OnKeyEventFocusedDelegate(this->_ButtonBase_Owner, Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        return Result;
    }

    TNode& _ButtonBase_Owner;
    bool bEnabled:1 { true };
    bool bSelectable:1 {};
    bool bSelected:1 {};
    //# TODO bool: Auto select on press.
    //# TODO bool: Call delegates if selected
    bool bUpdateBrushOnStateChange:1 { true };
};

template<typename TNode>
struct TFactoryButtonBase : NODE_FACTORY_PARENT(TNode)
{
    NODE_FACTORY_BODY(TNode)

    typedef typename TNode::_ButtonBaseBrush Brush;

    decltype(auto) Enabled(this auto&& Self, bool bEnabled) noexcept
    {
        NODE_FACTORY_SELF().SetEnabled(bEnabled);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Selectable(this auto&& Self, bool bSelectable) noexcept
    {
        NODE_FACTORY_SELF().SetSelectable(bSelectable);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Selected(this auto&& Self, bool bSelected) noexcept
    {
        NODE_FACTORY_SELF().SetSelected(bSelected);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) UpdateBrushOnStateChange(this auto&& Self, bool bUpdate) noexcept
    {
        NODE_FACTORY_SELF().SetUpdateBrushOnStateChange(bUpdate);
        return NODE_FACTORY_RESULT();
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnBrushChanged, OnBrushChangedEvent)

#define JAFG_NODE_FACTORY_STYLE_BOILERPLATE(Stem, Member) \
    decltype(auto) Member(this auto&& Self, decltype(std::remove_cvref_t<decltype(Self)>::TFactoredNode::Member) const& Style) noexcept \
    { \
        NODE_FACTORY_SELF().Member = Style; \
        return NODE_FACTORY_RESULT(); \
    } \
    template<auto Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(InAll, Stem, Brushes)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template SetEverywhere<Proj>(Value); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<auto... Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(InAll, Stem, BrushesChained)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template ChainEverywhere<Proj...>(Values...); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename decltype(TFactoredNode::Member)::flag_type Flags, auto Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(In, Stem, Brush)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template Set<Flags, Proj>(Value); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename decltype(TFactoredNode::Member)::flag_type Flags, auto... Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(In, Stem, BrushChained)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template Chain<Flags, Proj...>(Values...); \
        return NODE_FACTORY_RESULT(); \
    }

    JAFG_NODE_FACTORY_STYLE_BOILERPLATE(, Style)
};

} /* ~Namespace Jafg */
