// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::MwStatics
{

static constexpr i32  ChunkSize        { 32 };
static constexpr i32  ChunkSizeSquared { MwStatics::ChunkSize * MwStatics::ChunkSize };
static constexpr i32  ChunkSizeCubed   { MwStatics::ChunkSize * MwStatics::ChunkSize * MwStatics::ChunkSize };

//# Per chunk basis.
static constexpr i32  VoxelCount       { MwStatics::ChunkSizeCubed };

} /* ~Namespace Jafg::MyWorld */
