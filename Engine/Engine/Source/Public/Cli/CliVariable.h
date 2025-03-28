// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

//#
//# A variable inside the cli of the engine.
//#
class LCliVariable final : public LCliObject
{
public:

    FORCEINLINE LCliVariable() = delete;
    FORCEINLINE LCliVariable(const LSimpleString& InIdentifier) : LCliObject(InIdentifier) { }
    FORCEINLINE LCliVariable(const LSimpleString& InIdentifier, const LString& InHelp) : LCliObject(InIdentifier, InHelp) { }
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
    FORCEINLINE void SetValue(const LString& InValue) { this->Value = InValue; }

private:

    LString Value;
};

} /* ~Namespace Jafg */
