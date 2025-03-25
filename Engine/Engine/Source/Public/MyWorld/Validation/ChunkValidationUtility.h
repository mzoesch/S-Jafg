// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "MyWorld/ChunkKey.h"

namespace Jafg::Validation
{

//#
//# Gets all chunks in a distance from a center as a box.
//# @param Center    The center (C).
//# @param Distance  The distance starting from 0.
//# @param OutChunks The requested chunks:
//#     E.g., for a distance of 2:
//#          --> +X-Axis
//#       |  X X X X X
//#       |  X X X X X
//#       V  X X C X X
//#  +Y-Axis X X X X X
//#          X X X X X
//#
void GetAllChunksFromCenterAsBox(const LChunkKey2& Center, const i32 Distance, TdhArray<LChunkKey2>& OutChunks);

} /* ~Namespace Jafg::Validation */
