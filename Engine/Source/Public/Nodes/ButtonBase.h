// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#define JAFG_NODE_BUTTON_BOILERPLATE() \
    virtual LCursorReply OnCursorEnter() override{ return this->ButtonBase_OnCursorEnter(); } \
    virtual LCursorReply OnCursorLeave() override{ return this->ButtonBase_OnCursorLeave(); } \
    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override { return this->ButtonBase_OnKeyDown(Data, Event); } \
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override { return this->ButtonBase_OnKeyUp(Data, Event); }

namespace Jafg
{

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

//# To use your flags here you have to declare the #end member as in #EStyleBits.
template<typename UFlags, auto... BrushProj> requires std::is_base_of_v<Detail::LFlags, UFlags>
struct LStyleBase
{
    typedef typename UFlags::type flag_type;

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

template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush>
struct TButtonBaseStyle
{
    TBrush NormalBrush   {LRegionBrush{.Tint = {0x15,0x15,0x15}, .OutlineTint = Colors::Black}};
    TBrush HoverBrush    {LRegionBrush{.Tint = {0x1C,0x1C,0x1C}, .OutlineTint = Colors::White}};
    TBrush PressBrush    {LRegionBrush{.Tint = {0x24,0x24,0x24}, .OutlineTint = Colors::White}};
    TBrush SelectedBrush {LRegionBrush{.Tint = {0x24,0x24,0x24}, .OutlineTint = Colors::White}};
    TBrush DisabledBrush {LRegionBrush{.Tint = {0x0F,0x0F,0x0F}, .OutlineTint = Colors::Black}};
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

    typedef TBrush _ButtonBaseBrush;

    TButtonStyle<TBrush> Style;

    constexpr TButtonBase(TNode& InOwner) noexcept : Owner(InOwner) {}
    virtual ~TButtonBase() = default;

    EVENT_DECL(OnPrimaryPressDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnPrimaryReleaseDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnSecondaryPressDelegate, void(TNode& Self, LKeyEvent const& Event))
    EVENT_DECL(OnSecondaryReleaseDelegate, void(TNode& Self, LKeyEvent const& Event))
    //# These methods will only be called if the delegates are not bound.
    virtual void OnPrimaryPress() {}
    virtual void OnPrimaryRelease() {}
    virtual void OnSecondaryPress() {}
    virtual void OnSecondaryRelease() {}

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

    LCursorReply ButtonBase_OnCursorEnter()
    {
        if (this->bEnabled == false)
        {
            return {};
        }
        if (this->bUpdateBrushOnStateChange && this->bSelected == false)
        {
            this->Owner.*BrushProj = this->Style.HoverBrush;
        }
        if (this->Owner.OnCursorEnterEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnCursorEnterEvent.Invoke(this->Owner)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        return LCursorReply::Handled();
    }

    LCursorReply ButtonBase_OnCursorLeave()
    {
        if (this->bEnabled == false)
        {
            return {};
        }
        if (this->bUpdateBrushOnStateChange && this->bSelected == false)
        {
            this->Owner.*BrushProj = this->Style.NormalBrush;
        }
        if (this->Owner.OnCursorLeaveEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnCursorLeaveEvent.Invoke(this->Owner)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        return LCursorReply::Handled();
    }

    LReply ButtonBase_OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
    {
        if (this->bEnabled == false)
        {
            if (this->Owner.IsParentValid())
            {
                return this->Owner.GetParentChecked()->OnKeyDown(Data, Event);
            }
            return LReply::Unhandled();
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                this->Owner.*BrushProj = this->Style.PressBrush;
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyDownEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnPrimaryPressDelegate.IsValid())
            {
                this->OnPrimaryPressDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnPrimaryPress();
            }
            return Result;
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                this->Owner.*BrushProj = this->Style.PressBrush;
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyDownEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnSecondaryPressDelegate.IsValid())
            {
                this->OnSecondaryPressDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnSecondaryPress();
            }
            return Result;
        }

        if (this->Owner.OnKeyDownEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnKeyDownEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        if (this->Owner.IsParentValid())
        {
            return this->Owner.GetParentChecked()->OnKeyDown(Data, Event);
        }
        return LReply::Unhandled();
    }

    LReply ButtonBase_OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event)
    {
        if (this->bEnabled == false)
        {
            if (this->Owner.IsParentValid())
            {
                return this->Owner.GetParentChecked()->OnKeyUp(Data, Event);
            }
            return LReply::Unhandled();
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                this->Owner.*BrushProj = this->Style.HoverBrush;
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyUpEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnPrimaryReleaseDelegate.IsValid())
            {
                this->OnPrimaryReleaseDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnPrimaryRelease();
            }
            return Result;
        }

        if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
        {
            if (this->bUpdateBrushOnStateChange && this->bSelected == false)
            {
                this->Owner.*BrushProj = this->Style.HoverBrush;
            }
            LReply Result{LReply::Handled()};
            if (this->Owner.OnKeyUpEvent.IsValid())
            {
                if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
                {
                    Result = Reply;
                }
            }
            if (this->OnSecondaryReleaseDelegate.IsValid())
            {
                this->OnSecondaryReleaseDelegate.Invoke(this->Owner, Event);
            }
            else
            {
                this->OnSecondaryRelease();
            }
            return Result;
        }

        if (this->Owner.OnKeyUpEvent.IsValid())
        {
            if (auto Reply{this->Owner.OnKeyUpEvent.Invoke(this->Owner, Data, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
        }
        if (this->Owner.IsParentValid())
        {
            return this->Owner.GetParentChecked()->OnKeyUp(Data, Event);
        }
        return LReply::Unhandled();
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

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnPrimaryPress, OnPrimaryPressDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnPrimaryRelease, OnPrimaryReleaseDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnSecondaryPress, OnSecondaryPressDelegate)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnSecondaryRelease, OnSecondaryReleaseDelegate)

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
    decltype(auto) Member(this auto&& Self, TButtonStyle<Brush> const& Style) noexcept \
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
    template<typename decltype(TSelf::Member)::flag_type Flags, auto Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(In, Stem, Brush)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template Set<Flags, Proj>(Value); \
        return NODE_FACTORY_RESULT(); \
    } \
    template<typename decltype(TSelf::Member)::flag_type Flags, auto... Proj> \
    decltype(auto) JAFG_JOIN_OUTER_THREE(In, Stem, BrushChained)(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept \
    { \
        NODE_FACTORY_SELF().Member.template Chain<Flags, Proj...>(Values...); \
        return NODE_FACTORY_RESULT(); \
    }

    JAFG_NODE_FACTORY_STYLE_BOILERPLATE(, Style)

    // decltype(auto) Style(this auto&& Self, TButtonStyle<Brush> const& Style) noexcept
    // {
    //     NODE_FACTORY_SELF().Style = Style;
    //     return NODE_FACTORY_RESULT();
    // }
    // template<auto Proj>
    // decltype(auto) InAllBrushes(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept
    // {
    //     NODE_FACTORY_SELF().Style.template SetEverywhere<Proj>(Value);
    //     return NODE_FACTORY_RESULT();
    // }
    // template<auto... Proj>
    // decltype(auto) InAllBrushesChained(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept
    // {
    //     NODE_FACTORY_SELF().Style.template ChainEverywhere<Proj...>(Values...);
    //     return NODE_FACTORY_RESULT();
    // }
    // template<typename decltype(TSelf::Style)::flag_type Flags, auto Proj>
    // decltype(auto) InBrush(this auto&& Self, algo::proj_member_t<decltype(Proj)> const& Value) noexcept
    // {
    //     NODE_FACTORY_SELF().Style.template Set<Flags, Proj>(Value);
    //     return NODE_FACTORY_RESULT();
    // }
    // template<typename decltype(TSelf::Style)::flag_type Flags, auto... Proj>
    // decltype(auto) InBrushChained(this auto&& Self, algo::proj_member_t<decltype(Proj)> const&... Values) noexcept
    // {
    //     NODE_FACTORY_SELF().Style.template Chain<Flags, Proj...>(Values...);
    //     return NODE_FACTORY_RESULT();
    // }
};

} /* ~Namespace Jafg */

