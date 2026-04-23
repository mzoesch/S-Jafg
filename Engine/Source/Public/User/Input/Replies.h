// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/MouseCursor.h"

namespace Jafg
{

class WNode;

class LReplyBase
{
public:

    FORCEINLINE constexpr LReplyBase() noexcept : bHandled(false) { }
    FORCEINLINE constexpr LReplyBase(bool bInHandled) noexcept : bHandled(bInHandled) { }
    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LReplyBase)
    FORCEINLINE constexpr virtual ~LReplyBase() noexcept = default;

    FORCEINLINE constexpr bool IsHandled() const noexcept { return this->bHandled; }

protected:

    bool bHandled;
};

class LPrimitiveReply : public LReplyBase
{
public:

    using LReplyBase::LReplyBase;

    FORCEINLINE static constexpr LPrimitiveReply Handled() noexcept { return LPrimitiveReply{ true }; }
    FORCEINLINE static constexpr LPrimitiveReply Unhandled() noexcept { return LPrimitiveReply{ false }; }
};

template <typename TReplyTy>
class TReplyBase : public LReplyBase
{
public:

    FORCEINLINE constexpr TReplyBase() noexcept : LReplyBase() { }
    FORCEINLINE constexpr TReplyBase(const bool bInHandled) noexcept : LReplyBase(bInHandled) { }
    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(TReplyBase)
    FORCEINLINE virtual ~TReplyBase() noexcept override = default;

protected:

    FORCEINLINE constexpr TReplyTy& Self() noexcept { return static_cast<TReplyTy&>(*this); }
};

//#
//# Reply from a cursor query.
//#
class LCursorReply final : public TReplyBase<LCursorReply>
{
public:

    FORCEINLINE static constexpr LCursorReply Handled()   noexcept { return { EMouseCursor::None }; }
    FORCEINLINE static constexpr LCursorReply Unhandled() noexcept { return { }; }

    constexpr LCursorReply() noexcept : TReplyBase<LCursorReply>(false) { }
    constexpr LCursorReply(const EMouseCursor::Type InCursorType) noexcept
        : TReplyBase<LCursorReply>(true), CursorType(InCursorType) { }

    //#
    //# @param InCursorType    The cursor type to set. EMouseCursor::None to do not affect the current state.
    //# @param InFocusedWidget The widget to focus. Null if this reply does not affect any focus.
    //# @param bInLooseFocus   Whether to lose focus on the current focused widget.
    //#                        Note that if this is true and the #InFocusedWidget is the current focused widget, the
    //#                        focus will be lost and gained again in the same frame. If you do not want this behavior,
    //#                        set #bInLooseFocus to false. Now only if the #InFocusedWidget is different from the
    //#                        current focused widget, the focus will be lost for the current focused widget - if both
    //#                        are the same, nothing will happen.
    //#                        This is usually the most save way to handle focus changes and the recommended way.
    //#
    constexpr LCursorReply(const EMouseCursor::Type InCursorType, WNode* InFocusedWidget, const bool bInLooseFocus = false) noexcept
        : TReplyBase<LCursorReply>(true), CursorType(InCursorType), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    constexpr LCursorReply(WNode* InFocusedWidget, const bool bInLooseFocus = false) noexcept
        : TReplyBase<LCursorReply>(true), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    FORCEINLINE constexpr bool IsFocusedWidgetValid() const noexcept { return this->FocusedWidget != nullptr; }
    FORCEINLINE constexpr auto GetFocusedWidget() const noexcept -> WNode* { return this->FocusedWidget; }
    FORCEINLINE constexpr bool ShouldLooseFocus() const noexcept { return this->bLooseFocus; }
    FORCEINLINE constexpr auto GetCursorType() const noexcept -> EMouseCursor::Type { return this->CursorType; }

private:

    EMouseCursor::Type CursorType { EMouseCursor::None };

    //#
    //# The widget to focus. Null if this reply does not affect any focus.
    //#
    WNode* FocusedWidget { nullptr };

    //#
    //# True if the current focused widget should lose focus. If #FocusedWidget is null, no widget will be focused.
    //#
    bool bLooseFocus { false };
};

//#
//# Reply from a query of widgets.
//# This reply describes the new state of all widgets if something of interest happened.
//#
class LReply final : public TReplyBase<LReply>
{
public:

    FORCEINLINE static constexpr LReply Handled() noexcept { return { true }; }
    FORCEINLINE static constexpr LReply HandledWithFocusLost() noexcept { return { nullptr, true }; }
    FORCEINLINE static constexpr LReply Unhandled() noexcept { return { false }; }

    //# Unhandled.
    FORCEINLINE constexpr LReply() noexcept : TReplyBase<LReply>(false) { }

    //# Handled. Will not affect focused widgets.
    FORCEINLINE constexpr LReply(const bool bInHandled) noexcept : TReplyBase<LReply>(bInHandled) { }

    //#
    //# @param InFocusedWidget The widget to focus. Null if this reply does not affect any focus.
    //# @param bInLooseFocus   Whether to lose focus on the current focused widget.
    //#                        Note that if this is true and the #InFocusedWidget is the current focused widget, the
    //#                        focus will be lost and gained again in the same frame. If you do not want this behavior,
    //#                        set #bInLooseFocus to false. Now only if the #InFocusedWidget is different from the
    //#                        current focused widget, the focus will be lost for the current focused widget - if both
    //#                        are the same, nothing will happen.
    //#                        This is usually the most save way to handle focus changes and the recommended way.
    //#
    FORCEINLINE constexpr LReply(WNode* InFocusedWidget, const bool bInLooseFocus = false) noexcept
        : TReplyBase<LReply>(true), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LReply)

    FORCEINLINE virtual ~LReply() noexcept override = default;

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsHandled(); }

    FORCEINLINE constexpr bool   IsFocusedWidgetValid() const { return this->FocusedWidget != nullptr; }
    FORCEINLINE constexpr WNode* GetFocusedWidget() const { return this->FocusedWidget; }
    FORCEINLINE constexpr bool   ShouldLooseFocus() const { return this->bLooseFocus; }

private:

    //#
    //# The widget to focus. Null if this reply does not affect any focus.
    //#
    WNode* FocusedWidget { nullptr };

    //#
    //# True if the current focused widget should lose focus. If #FocusedWidget is null, no widget will be focused.
    //#
    bool bLooseFocus { false };
};

} /* ~Namespace Jafg */
