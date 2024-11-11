// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

/**
 * Uniquely defines a voxel in the application.
 */
typedef uint32 voxel_t;

/**
 * Uniquely defines a voxel in the application.
 * Use this sparingly and never store this type in a variable, only use it for loops or similar.
 */
typedef int64 voxel_t_signed;

namespace ECompileTimeVoxels
{

enum Type : voxel_t
{
    Null = 0,
    Air  = 0, // Just temp change back to one.
    Max  = 1,
    Num  = 2,
};

} /* ~Namespace EDefinedVoxels */

} /* ~Namespace Jafg */
