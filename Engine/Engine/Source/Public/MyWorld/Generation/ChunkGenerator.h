// Copyright mzoesch. All rights reserved.

#pragma once

#include "MyWorld/CommonTypes.h"
#include "MyWorld/ChunkKey.h"

namespace Jafg
{

class AChunk;
class LWorld;
struct LSharedChunkArgs;

namespace ChunkGenerator
{

ENGINE_API void ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t*& InOutChunkData);
ENGINE_API void ReplaceSurface(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, AChunk* Target, voxel_t*& InOutChunkData);

} /* ~Namespace ChunkGenerator */

} /* ~Namespace Jafg */
