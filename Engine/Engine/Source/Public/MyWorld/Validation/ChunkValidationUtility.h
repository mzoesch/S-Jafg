// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "MyWorld/ChunkKey.h"

namespace Jafg::Validation
{

//#
//# Gets all chunks in a distance from a center as a box.
//# @param Center    The center (C).
//# @param Distance  The distance starting from 0.
//# @return The requested chunks:
//#     E.g., for a distance of 2:
//#          --> +X-Axis
//#       |  X X X X X
//#       |  X X X X X
//#       V  X X C X X
//#  +Y-Axis X X X X X
//#          X X X X X
//#
ENGINE_API TArray<LIntVector2> GetAllChunksFromCenterAsBox(const LIntVector2& Center, const i32 Distance);

//#
//# Same as above, but for 3D.
//#
ENGINE_API TArray<LChunkKey> GetAllChunksFromCenterAsBox(const LChunkKey& Center, const i32 Distance, const i32 Height, const i32 HeightOffset);

} /* ~Namespace Jafg::Validation */
