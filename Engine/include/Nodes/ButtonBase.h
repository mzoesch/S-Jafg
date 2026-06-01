// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"

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
            check(!this->Owner._check_MutableMouseEntered()) \
            checkCode(this->Owner._check_MutableMouseEntered() = true)\
            return {};\
        }\
        if (this->bUpdateBrushOnStateChange && !this->bSelected) \
        { \
            this->_ButtonBase_SetBrush(this->Style.HoverBrush); \
        } \
        return Super::OnCursorEnter(); \
    }

#define JAFG_NODE_BUTTON_BOILERPLATE_OnCursorLeave() \
    virtual void OnCursorLeave() override \
    { \
        if (!this->bEnabled)\
        {\
            check(this->Owner._check_MutableMouseEntered()) \
            checkCode(this->Owner._check_MutableMouseEntered() = false)\
            return;\
        }\
        if (this->bUpdateBrushOnStateChange && !this->bSelected) \
        { \
            this->_ButtonBase_SetBrush(this->Style.NormalBrush); \
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
};
ENUM_STRUCT_FLAGS(EStyleBits, EStyleFlags)

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
    template<UFlags Flags, auto... Proj>
    constexpr void Chain(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept
    {
        static_assert(sizeof...(Proj) == sizeof...(Values));
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
                            ((((Self.*Brush).*Proj) = Values), ...);
                        }
                    }
                }
            }};
            (Apply.template operator()<Seq>(BrushProj), ...);
        }(std::make_index_sequence<sizeof...(BrushProj)>{});
        return;
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

    constexpr TButtonBase(TNode& InOwner) noexcept : Owner{InOwner} {}
    virtual ~TButtonBase() = default;

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
                this->Owner.*BrushProj = this->Style.NormalBrush;
            }
            else
            {
                this->Owner.*BrushProj = this->Style.DisabledBrush;
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
                this->Owner.*BrushProj = this->Style.SelectedBrush;
            }
            else
            {
                this->Owner.*BrushProj = this->Style.NormalBrush;
            }
        }

        this->OnSelectedStateChanged();
        return;
    }
    //# Delegate that is called when the selected state changed.
    virtual void OnSelectedStateChanged() {}

    NODISCARD constexpr bool IsUpdateBrushOnStateChange() const noexcept { return this->bUpdateBrushOnStateChange; }
    constexpr void SetUpdateBrushOnStateChange(bool bInUpdate) noexcept { this->bUpdateBrushOnStateChange = bInUpdate; }

    FORCEINLINE void _ButtonBase_SetBrush(TBrush const& Brush) noexcept
    {
        this->Owner.*BrushProj = Brush;
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
                    this->Owner.*BrushProj = this->Style.SelectedBrush;
                }
                else
                {
                    this->Owner.*BrushProj = this->Style.NormalBrush;
                }
            }
            else
            {
                check(this->bSelected == false)
                this->Owner.*BrushProj = this->Style.DisabledBrush;
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

        if (Info.CursorLocation.has_value())
        {
            if (this->Owner.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
            {
                if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                    , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
                {
                    if (this->bUpdateBrushOnStateChange && !this->bSelected)
                    {
                        this->Owner.*BrushProj = this->Style.PressBrush;
                    }
                    auto Result{LNodeReply::Handled()};
                    if (this->Owner.OnKeyEventFocusedDelegate)
                    {
                        if (auto Reply{this->Owner.OnKeyEventFocusedDelegate(this->Owner, Info, Event)}; Reply.IsHandled())
                        {
                            Result = Reply;
                        }
                    }
                    return Result;
                }
            }

            if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                    , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
            {
                if (this->bUpdateBrushOnStateChange && !this->bSelected)
                {
                    this->Owner.*BrushProj = this->Style.HoverBrush;
                }
                auto Result{LNodeReply::Handled()};
                if (this->Owner.OnKeyEventFocusedDelegate)
                {
                    if (auto Reply{this->Owner.OnKeyEventFocusedDelegate(this->Owner, Info, Event)}; Reply.IsHandled())
                    {
                        Result = Reply;
                    }
                }
                return Result;
            }
        }

        if (this->Owner.OnKeyEventFocusedDelegate)
        {
            if (auto Reply{this->Owner.OnKeyEventFocusedDelegate(this->Owner, Info, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }

        return {};
    }

    TNode& Owner;
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
