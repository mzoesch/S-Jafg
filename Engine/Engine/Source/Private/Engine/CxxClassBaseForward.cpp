// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClass.h"

Jafg::LCxxObjectInitializer Jafg::GetDefaultObjectInitializer() noexcept
{
    check( Private::GetGlobalCxxRecordRegistry().AreNewPendingPackagesAllowed() )
    return LCxxObjectInitializer{Private::GetGlobalCxxRecordRegistry().GetMutableOuter()};
}
