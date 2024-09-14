// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace WorldStatics
{

static constexpr int32  ChunkSize        { 32 };
static constexpr int32  ChunkSizeSquared { WorldStatics::ChunkSize * WorldStatics::ChunkSize };
static constexpr int32  ChunkSizeCubed   { WorldStatics::ChunkSize * WorldStatics::ChunkSize * WorldStatics::ChunkSize };

/** Per chunk basis. */
static constexpr int32  VoxelCount       { WorldStatics::ChunkSizeCubed };

}
