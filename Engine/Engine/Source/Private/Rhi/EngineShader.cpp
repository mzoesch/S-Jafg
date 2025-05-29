// Copyright mzoesch. All rights reserved.

#include "Rhi/EngineShader.h"
#include "Engine/Engine.h"

bool Jafg::LEngineShader::Make(const LName InName)
{
    check( GEngine )
    check( this->IsValid() == false )
    this->bValid = true;

    return GEngine->AddShader(InName, this);
}

void Jafg::LEngineShader::OnFree()
{
    LOG_VERBOSE(LogRhi, "Freeing shader [{}] from graphical processing unit.", this->Program.GetId())

    this->Program.Free();
    this->bValid = false;

    if (GEngine)
    {
        GEngine->RemoveShader(this);
    }

    return;
}
