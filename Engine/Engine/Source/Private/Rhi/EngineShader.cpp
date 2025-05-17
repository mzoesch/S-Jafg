// Copyright mzoesch. All rights reserved.

#include "Rhi/EngineShader.h"
#include "Engine/Engine.h"

bool Jafg::LEngineShader::Make(const LName InName)
{
    check( GEngine )
    check( this->bMeaningful == false )
    this->bMeaningful = true;

    return GEngine->AddShader(InName, this);
}

void Jafg::LEngineShader::OnFree()
{
    this->Program.Free();
    this->bMeaningful = false;

    return;
}
