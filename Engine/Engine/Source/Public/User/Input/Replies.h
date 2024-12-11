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

/** Reply from a cursor query. */
class LCursorReply final : public TReplyBase<LCursorReply>
{
public:

    static LCursorReply Unhandled() { return { }; }

    LCursorReply() : TReplyBase<LCursorReply>(false) { }
    LCursorReply(const EMouseCursor::Type InCursorType) : TReplyBase<LCursorReply>(true), CursorType(InCursorType) { }

    FORCEINLINE auto GetCursorType() const -> EMouseCursor::Type { return this->CursorType; }

private:

    EMouseCursor::Type CursorType = EMouseCursor::Default;
};

/**
 * Reply from a query of widgets.
 * This reply describes the new state of all widgets if something of interest happened.
 */
class LReply final : public TReplyBase<LReply>
{
public:

    static LReply Handled()   { return { true  }; }
    static LReply Unhandled() { return { false }; }

    LReply() : TReplyBase<LReply>(false) { }
    LReply(const bool bInHandled) : TReplyBase<LReply>(bInHandled) { }
};

} /* ~Namespace Jafg */
