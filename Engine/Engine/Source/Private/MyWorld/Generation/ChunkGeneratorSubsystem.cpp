// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "ChunkGeneratorSubsystem.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/MyWorldStatics.h"

void Jafg::JChunkGeneratorSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->FnGenerator = FastNoise::NewFromEncodedNodeTree("DQAFAAAAAAAAQAgAAAAAAD8AAAAAAA==");
    this->UsableContainer = new float[MwStatics::VoxelCount];
    return;
}
