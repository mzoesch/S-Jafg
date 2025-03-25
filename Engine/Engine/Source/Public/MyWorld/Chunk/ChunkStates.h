// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace EChunkState
{

enum Type : u8
{
    //#
    //# Marks an invalid state. A chunk must never have this state.
    //#
    Invalid,
    Freed,

    PreSpawned,
    Spawned,
    Shaped,
    SurfaceReplaced,
    Active,

    //#
    //# Special states below here.
    //# These entries have nothing to do with generation and are considered
    //# special as they will not interfere with the subsystem.
    //# A chunk must never have this state as it is only used to classify
    //# states together - like a barrier.
    //#

    /*----------------------------------------------------------------------------*/
    Special,
    /*----------------------------------------------------------------------------*/

    //#
    //#  Chunk has been marked as a pending kill and is no longer visible to the LEye in the world (the mesh
    //#  has been cleared to save memory).
    //#  The validation system will remove this chunk from the world in the near future.
    //#
    PendingKill,

    //#
    //# If the chunk itself decides that it wants to die and be separated from all its friends in the world.
    //# The corresponding AActor will probably be killed at the end of the tick, where the state change was broadcasted.
    //#
    Kill,
};

} /* ~Namespace EChunkState */

inline LSimpleString LexToString(const EChunkState::Type ChunkState)
{
    switch (ChunkState)
    {
    case EChunkState::Invalid:
    {
        return "Invalid";
    }
    case EChunkState::PreSpawned:
    {
        return "PreSpawned";
    }
    case EChunkState::Spawned:
    {
        return "Spawned";
    }
    case EChunkState::Shaped:
    {
        return "Shaped";
    }
    case EChunkState::SurfaceReplaced:
    {
        return "SurfaceReplaced";
    }
    case EChunkState::Active:
    {
        return "Active";
    }
    default:
    {
        checkNoEntry()
        return "Unknown";
    }
    }
}

} /* ~Namespace Jafg */
