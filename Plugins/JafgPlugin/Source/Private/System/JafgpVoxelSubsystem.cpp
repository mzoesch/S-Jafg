// Copyright mzoesch. All rights reserved.

#include "System/JafgpVoxelSubsystem.h"

void Jafgp::JJafgpVoxelSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LOG_ERROR(LogTemporal, "Hello")

    return;
}

void Jafgp::JJafgpVoxelSubsystem::TearDown()
{
    Super::TearDown();

    LOG_ERROR(LogTemporal, "Hello")

    return;
}
