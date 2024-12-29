// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Cli/CliObject.h"

bool Jafg::LCliToken_String::IsInvocable(TdhArray<LCliToken*> InArgs, int32* InOutArgCursor) const
{
    if (this->bPack)
    {
        *InOutArgCursor = InArgs.GetSize();
        return true;
    }

    check( InArgs[*InOutArgCursor] )

    if (InArgs[*InOutArgCursor]->IsCastableToString())
    {
        *InOutArgCursor += 1;
        return true;
    }

    return false;
}
