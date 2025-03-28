// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

typedef TFunction<bool(const LString& InValue, LString* OutValue)> LOnVariableChangedDelegate;

//#
//# A variable inside the cli of the engine.
//#
class LCliVariable final : public LCliObject
{
public:

    FORCEINLINE LCliVariable() = delete;
    FORCEINLINE LCliVariable(const LString& InIdentifier) : LCliObject(InIdentifier) { }
    FORCEINLINE LCliVariable(const LString& InIdentifier, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier), OnVariableChangedDelegate(std::move(InDelegate)) { }
    FORCEINLINE LCliVariable(const LString& InIdentifier, const LString& InHelp, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier, InHelp), OnVariableChangedDelegate(std::move(InDelegate)) { }
    PROHIBIT_COPY(LCliVariable)
    FORCEINLINE LCliVariable(LCliVariable&& InOther) noexcept
    {
        this->Value = std::move(InOther.Value);
        this->LCliObject::operator=(std::move(InOther));
        return;
    }
    FORCEINLINE LCliVariable& operator=(LCliVariable&& InOther) noexcept
    {
        this->Value = std::move(InOther.Value);
        this->LCliObject::operator=(std::move(InOther));
        return *this;
    }

    FORCEINLINE const LString& GetValue() const { return this->Value; }
    FORCEINLINE bool SetValue(const LString& InValue)
    {
        if (this->Value == InValue)
        {
            return false;
        }

        if (this->OnVariableChangedDelegate)
        {
            return this->OnVariableChangedDelegate(InValue, &this->Value);
        }

        this->Value = InValue;
        return true;
    }

private:

    LString Value;
    LOnVariableChangedDelegate OnVariableChangedDelegate;
};

} /* ~Namespace Jafg */
