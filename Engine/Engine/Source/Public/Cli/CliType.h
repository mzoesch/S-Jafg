// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

struct LCommandArgs;

typedef TFunction<bool(const LCommandArgs& Args, i32* Cursor)> LOnParseTypeDelegate;

//#
//# A variable type inside the cli of the engine. May be used for type checking within commands and variables.
//#
class LCliType final : public LCliObject
{
public:

    FORCEINLINE LCliType() = delete;
    FORCEINLINE LCliType(const LSimpleString& InIdentifier) : LCliObject(InIdentifier) { this->ExpandUuid(); }
    FORCEINLINE LCliType(const LSimpleString& InIdentifier, LOnParseTypeDelegate&& InOnParseType) : LCliObject(InIdentifier), OnParseType(std::move(InOnParseType)) { }
    FORCEINLINE LCliType(const LSimpleString& InIdentifier, const LString& InHelp, LOnParseTypeDelegate&& InOnParseType) : LCliObject(InIdentifier, InHelp), OnParseType(std::move(InOnParseType)) { }
    PROHIBIT_COPY(LCliType)
    FORCEINLINE LCliType(LCliType&& InOther) noexcept
    {
        this->OnParseType = std::move(InOther.OnParseType);
        this->LCliObject::operator=(std::move(InOther));
        return;
    }
    FORCEINLINE LCliType& operator=(LCliType&& InOther) noexcept
    {
        this->OnParseType = std::move(InOther.OnParseType);
        this->LCliObject::operator=(std::move(InOther));
        return *this;
    }

    ENGINE_API bool CanParse(const LCommandArgs& Args, i32* Cursor) const;

private:

    LOnParseTypeDelegate OnParseType;
};

} /* ~Namespace Jafg */
