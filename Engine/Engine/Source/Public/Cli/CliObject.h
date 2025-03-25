// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LCommandLineInterface;

typedef u32 LCliObjectUuid;

class LCliToken;
class LCliToken_String;
class LCliToken_Byte;
class LCliToken_Int;
class LCliToken_Float;
class LCliToken_Bool;
class LCliToken_Null;

class LCliToken
{
public:

    virtual ~LCliToken() = default;

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const = 0;

    virtual LString GetStringRepresentation() const = 0;

    virtual bool IsString() const { return false; }
    virtual bool IsByte() const { return false; }
    virtual bool IsInt() const { return false; }
    virtual bool IsFloat() const { return false; }
    virtual bool IsBool() const { return false; }
    virtual bool IsNull() const { return false; }
    virtual auto AsString() -> LCliToken_String* { return nullptr; }
    virtual auto AsByte() -> LCliToken_Byte* { return nullptr; }
    virtual auto AsInt() -> LCliToken_Int* { return nullptr; }
    virtual auto AsFloat() -> LCliToken_Float* { return nullptr; }
    virtual auto AsBool() -> LCliToken_Bool* { return nullptr; }
    virtual auto AsNull() -> LCliToken_Null* { return nullptr; }

    virtual bool IsCastableToString() const { return false; }
    virtual bool IsCastableToByte() const { return false; }
    virtual bool IsCastableToInt() const { return false; }
    virtual bool IsCastableToFloat() const { return false; }
    virtual bool IsCastableToBool() const { return false; }
    virtual auto CastToString() -> LString { checkNoEntry() return { }; }
    virtual auto CastToByte() -> u8 { checkNoEntry() return 0; }
    virtual auto CastToInt() -> i32 { checkNoEntry() return 0; }
    virtual auto CastToFloat() -> float { checkNoEntry() return 0.0f; }
    virtual auto CastToBool() -> bool { checkNoEntry() return false; }

    template <typename TToken>
    FORCEINLINE TToken&& MakePack()
    {
        this->bPack = true;
        return std::move(*static_cast<TToken*>(this));
    }

    //#
    //# A parameter pack is a parameter that accepts zero or more arguments of the same specified type.
    //#
    bool bPack = false;
};

//#
//# An Utf-8 encoded string.
//#
class LCliToken_String final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override;

    virtual LString GetStringRepresentation() const override { return this->Value; }

    FORCEINLINE virtual bool IsString() const override { return true; }
    FORCEINLINE virtual auto AsString() -> LCliToken_String* override { return this; }

    FORCEINLINE virtual bool IsCastableToString() const override { return true; }
    FORCEINLINE virtual auto CastToString() -> LString override { return this->Value; }

    LString Value = { };
};

//#
//# A 8-bit unsigned integer.
//#
class LCliToken_Byte final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override { return false; }

    virtual LString GetStringRepresentation() const override { return LString::SprintF("{}", this->Value); }

    FORCEINLINE virtual bool IsByte() const override { return true; }
    FORCEINLINE virtual auto AsByte() -> LCliToken_Byte* override { return this; }

    FORCEINLINE virtual bool IsCastableToString() const override { return true; }
    FORCEINLINE virtual auto CastToString() -> LString override { return LString::SprintF("{}", this->Value); }

    u8 Value = 0;
};

//#
//# A 32-bit signed integer.
//#
class LCliToken_Int final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override { return false; }

    virtual LString GetStringRepresentation() const override { return LString::SprintF("{}", this->Value); }

    FORCEINLINE virtual bool IsInt() const override { return true; }
    FORCEINLINE virtual auto AsInt() -> LCliToken_Int* override { return this; }

    FORCEINLINE virtual bool IsCastableToString() const override { return true; }
    FORCEINLINE virtual auto CastToString() -> LString override { return LString::SprintF("{}", this->Value); }

    i32 Value = 0;
};

//#
//# A 32-bit floating point number.
//#
class LCliToken_Float final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override { return false; }

    virtual LString GetStringRepresentation() const override { return LString::SprintF("{}", this->Value); }

    FORCEINLINE virtual bool IsFloat() const override { return true; }
    FORCEINLINE virtual auto AsFloat() -> LCliToken_Float* override { return this; }

    FORCEINLINE virtual bool IsCastableToString() const override { return true; }
    FORCEINLINE virtual auto CastToString() -> LString override { return LString::SprintF("{}", this->Value); }

    float Value = 0.0f;
};

//#
//# A boolean value.
//#
class LCliToken_Bool final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override { return false; }

    virtual LString GetStringRepresentation() const override { return this->Value ? "true" : "false"; }

    FORCEINLINE virtual bool IsBool() const override { return true; }
    FORCEINLINE virtual auto AsBool() -> LCliToken_Bool* override { return this; }

    FORCEINLINE virtual bool IsCastableToString() const override { return true; }
    FORCEINLINE virtual auto CastToString() -> LString override { return this->Value ? "true" : "false"; }

    bool Value = false;
};

//#
//# A command token that represents a null value.
//#
class LCliToken_Null final : public LCliToken
{
public:

    virtual bool IsInvocable(TdhArray<LCliToken*> InArgs, i32* InOutArgCursor) const override { return false; }

    virtual LString GetStringRepresentation() const override { return "null"; }

    FORCEINLINE virtual bool IsNull() const override { return true; }
    FORCEINLINE virtual auto AsNull() -> LCliToken_Null* override { return this; }
};

//#
//# Abstract base class for all command objects.
//#
class LCliObject
{
    friend LCommandLineInterface;

public:

    static constexpr LCliObjectUuid NoUuid { 0 };

    LCliObject() = default;
    LCliObject(const LSimpleString& InName) : Uuid(NoUuid), Identifier(InName) { }
    LCliObject(const LSimpleString& InName, const LString& InHelp) : Uuid(NoUuid), Identifier(InName), Help(InHelp) { }
    PROHIBIT_COPY(LCliObject)
    FORCEINLINE LCliObject(LCliObject&& InOther) noexcept = delete;
    FORCEINLINE LCliObject& operator=(LCliObject&& InOther) noexcept
    {
        this->Uuid = InOther.Uuid;
        InOther.Uuid = NoUuid;
        this->Identifier = std::move(InOther.Identifier);
        this->Help = std::move(InOther.Help);
        check( InOther.Uuid == NoUuid && InOther.Identifier.IsEmpty() && InOther.Help.IsEmpty() )
        return *this;
    }
    virtual ~LCliObject() = default;

    FORCEINLINE auto GetIdentifier(void) const -> LSimpleString { return this->Identifier; }
    FORCEINLINE void SetIdentifier(const LSimpleString& InIdentifier) { this->Identifier = InIdentifier; }
    FORCEINLINE auto GetHelp(void) const -> LString { return this->Help; }
    FORCEINLINE void SetHelp(const LString& InHelp) { this->Help = InHelp; }

private:

    LCliObjectUuid Uuid;
    LSimpleString Identifier;
    LString Help;
};

} /* ~Namespace Jafg */
