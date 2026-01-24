// Copyright mzoesch. All rights reserved.

#include "Engine/CxxClass.h"

Jafg::LCxxObjectInitializer Jafg::GetDefaultObjectInitializer(LCxxClass& Class) noexcept
{
    check( Private::GetGlobalCxxRecordRegistry().IsCDRRegistrationAllowed() )
    return LCxxObjectInitializer{Private::GetGlobalCxxRecordRegistry().GetMutableOuter(), Class};
}
