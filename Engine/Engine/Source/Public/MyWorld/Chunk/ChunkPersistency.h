// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace EChunkPersistency
{

enum Type : u8
{
    Persistent,
    Transient,
};

} /* ~Namespace EChunkPersistency */

inline LSimpleString LexToString(const EChunkPersistency::Type ChunkPersistency)
{
    switch (ChunkPersistency)
    {
    case EChunkPersistency::Persistent:
    {
        return "Persistent";
    }
    case EChunkPersistency::Transient:
    {
        return "Transient";
    }
    default:
    {
        checkNoEntry()
        return "Unknown";
    }
    }
}

} /* ~Namespace Jafg */
