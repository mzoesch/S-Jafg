// Copyright mzoesch. All rights reserved.

#include "Cli/CliType.h"
#include "Engine/Engine.h"

bool Jafg::LCliType::CanParse(const LCommandArgs& Args, i32* Cursor) const
{
    check( Cursor )
    if (this->OnParseTypeDelegate.IsBound())
    {
        return this->OnParseTypeDelegate.Invoke(Args, Cursor);
    }

    return GEngine->GetCommandLineInterface()->GetTypeAsserted(this->GetUuid())->OnParseTypeDelegate.Invoke(Args, Cursor);
}
