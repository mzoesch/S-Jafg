// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/PlatformMisc.h"

namespace Jafg
{

ENGINE_API LPlatformMisc* GPlatformMisc = nullptr;

void PlatformMisc::InvalidateCachedValues()
{
    delete GPlatformMisc;
    GPlatformMisc = new LPlatformMisc();
    GPlatformMisc->MakeCachedValuesValid();
    checkSlow( GPlatformMisc )

    return;
}

void LPlatformMisc::MakeCachedValuesValid()
{
    this->NumberOfPhysicalViewports = PlatformMisc::GetNumberOfPhysicalViewports();
    if (PlatformMisc::SetPhysicalViewports() == false)
    {
        LOG_ERROR(LogPlatformMisc, "Failed to get physical viewports.")
    }

    return;
}

} /* ~Namespace Jafg */
