// Copyright mzoesch. All rights reserved.

#include "Rhi/EngineShader.h"
#include "Engine/Engine.h"

uint32 Jafg::LEngineShader::Make()
{
    check( this->bMeaningful == false )
    this->bMeaningful = true;

    GEngine->AddShader(this);
    check( this->Uuid != 0 )

    return this->Uuid;
}
