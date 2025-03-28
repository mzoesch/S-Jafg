// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

class LCliType;
struct LCommandArgs;

//# @return True if the argument can be parsed as a valid type.
typedef TFunction<bool(const LCommandArgs& Args, i32* Cursor)> LOnParseTypeDelegate;
//# @param  OutValue Is always valid.
//# @return True if the #OutValue was set changed.
typedef TFunction<bool(const LCommandArgs& InValue, LString* OutValue)> LOnValueSetDelegate;

//#
//# A variable type inside the cli of the engine. May be used for type checking within commands and variables.
//#
class LCliType final : public LCliObject
{
public:

    FORCEINLINE LCliType() = delete;
    FORCEINLINE LCliType(const LString& InIdentifier, const LString& InDefault, LOnParseTypeDelegate&& InOnParseType, LOnValueSetDelegate&& InOnValueSet = nullptr)
    : LCliObject(InIdentifier), Default(InDefault), OnParseTypeDelegate(std::move(InOnParseType)), OnValueSetDelegate(std::move(InOnValueSet)) { }
    FORCEINLINE LCliType(const LString& InIdentifier, const LString& InHelp, const LString& InDefault, LOnParseTypeDelegate&& InOnParseType, LOnValueSetDelegate&& InOnValueSet = nullptr)
    : LCliObject(InIdentifier, InHelp), Default(InDefault), OnParseTypeDelegate(std::move(InOnParseType)), OnValueSetDelegate(std::move(InOnValueSet)) { }
    PROHIBIT_COPY(LCliType)
    FORCEINLINE LCliType(LCliType&& InOther) noexcept
    {
        this->Default = std::move(InOther.Default);
        this->OnParseTypeDelegate = std::move(InOther.OnParseTypeDelegate);
        this->OnValueSetDelegate = std::move(InOther.OnValueSetDelegate);
        this->LCliObject::operator=(std::move(InOther));
        return;
    }
    FORCEINLINE LCliType& operator=(LCliType&& InOther) noexcept
    {
        this->Default = std::move(InOther.Default);
        this->OnParseTypeDelegate = std::move(InOther.OnParseTypeDelegate);
        this->OnValueSetDelegate = std::move(InOther.OnValueSetDelegate);
        this->LCliObject::operator=(std::move(InOther));
        return *this;
    }

    //#
    //# Use this for referring to an already registered type.
    //#
    FORCEINLINE static LCliType Type(const LString& InIdentifier) { return LCliType(InIdentifier); }
    FORCEINLINE static LCliType Type(const LString& InIdentifier, const LString& InHelp) { return LCliType(InIdentifier, InHelp); }

    //#
    //# Set a variable typesafe through the engine with this method.
    //#
    FORCEINLINE bool SetVariable(const LCommandArgs& InValue, LString* OutValue) const
    {
        return this->OnValueSetDelegate(InValue, OutValue);
    }

    ENGINE_API  bool CanParse(const LCommandArgs& Args, i32* Cursor) const;

    FORCEINLINE auto GetDefault() const -> const LString& { return this->Default; }
    FORCEINLINE bool IsTypeDelegateValid() const { return this->OnParseTypeDelegate.IsValid(); }
    FORCEINLINE bool IsValueSetDelegateValid() const { return this->OnValueSetDelegate.IsValid(); }

private:

    FORCEINLINE explicit LCliType(const LString& InIdentifier) : LCliObject(InIdentifier) { this->ExpandToUuid(); }
    FORCEINLINE explicit LCliType(const LString& InIdentifier, const LString& InHelp) : LCliObject(InIdentifier, InHelp) { this->ExpandToUuid(); }
    LString Default;
    LOnParseTypeDelegate OnParseTypeDelegate;
    LOnValueSetDelegate OnValueSetDelegate;
};

} /* ~Namespace Jafg */
