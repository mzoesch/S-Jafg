// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::MwStatics
{

inline constexpr i32 ChunkSize        { 32 };
inline constexpr i32 ChunkSizeSquared { MwStatics::ChunkSize * MwStatics::ChunkSize };
inline constexpr i32 ChunkSizeCubed   { MwStatics::ChunkSize * MwStatics::ChunkSize * MwStatics::ChunkSize };

//# Per chunk basis.
inline constexpr i32 VoxelCount       { MwStatics::ChunkSizeCubed };

} /* ~Namespace Jafg::MyWorld */
