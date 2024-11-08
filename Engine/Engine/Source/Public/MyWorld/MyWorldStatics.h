// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg::MwStatics
{

static constexpr int32  ChunkSize        { 32 };
static constexpr int32  ChunkSizeSquared { MwStatics::ChunkSize * MwStatics::ChunkSize };
static constexpr int32  ChunkSizeCubed   { MwStatics::ChunkSize * MwStatics::ChunkSize * MwStatics::ChunkSize };

/** Per chunk basis. */
static constexpr int32  VoxelCount       { MwStatics::ChunkSizeCubed };

} /* ~Namespace Jafg::MyWorld */
