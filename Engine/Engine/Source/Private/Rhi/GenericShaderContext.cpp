// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Rhi/GenericShaderContext.h"

void Jafg::LGenericShaderContext::Make()
{
    if (this->IsMeaningful())
    {
        panic( "Tried to override a meaningful shader context." )
        return;
    }

    this->bMeaningful = true;

    return;
}

void Jafg::LGenericShaderContext::Free()
{
    if (this->bMeaningful)
    {
        this->bMeaningful = false;
        this->OnFree();
    }

    return;
}
