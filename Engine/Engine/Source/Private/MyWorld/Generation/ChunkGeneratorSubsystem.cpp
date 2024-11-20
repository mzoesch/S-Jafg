// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "ChunkGeneratorSubsystem.h"
#include "MyWorld/MyWorldStatics.h"

void Jafg::JChunkGeneratorSubsystem::OnInitialize(LSubsystemCollection& Collection)
{
    Super::OnInitialize(Collection);

    this->FnGenerator = FastNoise::NewFromEncodedNodeTree("DQAFAAAAAAAAQAgAAAAAAD8AAAAAAA==");
    this->UsableContainer = new float[MwStatics::VoxelCount];

    return;
}

void Jafg::JChunkGeneratorSubsystem::FixedTick(const float RunnableDeltaTime)
{
    Super::FixedTick(RunnableDeltaTime);
    checkSlow( Tasks::IsOnMasterThread() == false )

    return;
}
