// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/MouseCursor.h"

namespace Jafg
{

class LReplyBase
{
public:

    LReplyBase() : bHandled(false) { }
    LReplyBase(const bool bInHandled) : bHandled(bInHandled) { }
    LReplyBase(const LReplyBase&) = default;
    LReplyBase(LReplyBase&&) = default;
    LReplyBase& operator=(const LReplyBase&) = default;
    LReplyBase& operator=(LReplyBase&&) = default;
    virtual ~LReplyBase() = default;

    FORCEINLINE bool IsHandled() const { return this->bHandled; }

protected:

    bool bHandled;
};

template <typename TReplyTy>
class TReplyBase : public LReplyBase
{
public:

    TReplyBase() : LReplyBase() { }
    TReplyBase(const bool bInHandled) : LReplyBase(bInHandled) { }
    virtual ~TReplyBase() override = default;

protected:

    TReplyTy& Self() { return static_cast<TReplyTy&>(*this); }
};

//#
//# Reply from a cursor query.
//#
class LCursorReply final : public TReplyBase<LCursorReply>
{
public:

    static LCursorReply Handled()   { return { EMouseCursor::Default }; }
    static LCursorReply Unhandled() { return { }; }

    LCursorReply() : TReplyBase<LCursorReply>(false) { }
    LCursorReply(const EMouseCursor::Type InCursorType)
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
    LCursorReply(const EMouseCursor::Type InCursorType, WWidgetNode* InFocusedWidget, const bool bInLooseFocus = false)
        : TReplyBase<LCursorReply>(true), CursorType(InCursorType), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    LCursorReply(WWidgetNode* InFocusedWidget, const bool bInLooseFocus = false)
        : TReplyBase<LCursorReply>(true), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    FORCEINLINE auto GetFocusedWidget() const -> WWidgetNode* { return this->FocusedWidget; }
    FORCEINLINE auto ShouldLooseFocus() const -> bool { return this->bLooseFocus; }
    FORCEINLINE auto GetCursorType() const -> EMouseCursor::Type { return this->CursorType; }

private:

    EMouseCursor::Type CursorType = EMouseCursor::Default;

    //#
    //# The widget to focus. Null if this reply does not affect any focus.
    //#
    WWidgetNode* FocusedWidget = nullptr;

    //#
    //# True if the current focused widget should lose focus. If #FocusedWidget is null, no widget will be focused.
    //#
    bool bLooseFocus = false;
};

//#
//# Reply from a query of widgets.
//# This reply describes the new state of all widgets if something of interest happened.
//#
class LReply final : public TReplyBase<LReply>
{
public:

    static LReply Handled()   { return { true  }; }
    static LReply HandledWithFocusLost() { return { nullptr, true }; }
    static LReply Unhandled() { return { false }; }

    //# Unhandled.
    LReply() : TReplyBase<LReply>(false) { }

    //# Handled. Will not affect focused widgets.
    LReply(const bool bInHandled) : TReplyBase<LReply>(bInHandled) { }

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
    LReply(WWidgetNode* InFocusedWidget, const bool bInLooseFocus = false)
        : TReplyBase<LReply>(true), FocusedWidget(InFocusedWidget), bLooseFocus(bInLooseFocus) { }

    FORCEINLINE auto IsFocusedWidgetValid() const -> bool { return this->FocusedWidget != nullptr; }
    FORCEINLINE auto GetFocusedWidget() const -> WWidgetNode* { return this->FocusedWidget; }
    FORCEINLINE auto ShouldLooseFocus() const -> bool { return this->bLooseFocus; }

private:

    //#
    //# The widget to focus. Null if this reply does not affect any focus.
    //#
    WWidgetNode* FocusedWidget = nullptr;

    //#
    //# True if the current focused widget should lose focus. If #FocusedWidget is null, no widget will be focused.
    //#
    bool bLooseFocus = false;
};

} /* ~Namespace Jafg */
