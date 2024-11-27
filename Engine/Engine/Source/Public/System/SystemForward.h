// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/Path.h"

namespace Jafg
{

namespace EEnginePaths
{

enum Type : uint8
{
    None = 0,

    /** Custom path relative to a module. */
    CustomModule,

    /** Custom path relative to the engine. */
    CustomEngine,

    /** Content/Textures/Voxels */
    Voxels,

    /** Content/Textures/Blends */
    Blends,

    /** Content/Shaders */
    Shaders,

    /** Content/Fonts */
    Fonts,
};

} /* ~Namespace EEnginePaths */

inline LSimpleString LexToString(const EEnginePaths::Type InType)
{
    switch (InType)
    {
    case EEnginePaths::CustomModule: { return ""; }
    case EEnginePaths::Voxels:       { return "Content/Textures/Voxels"; }
    case EEnginePaths::Blends:       { return "Content/Textures/Blends"; }
    case EEnginePaths::Shaders:      { return "Content/Shaders"; }
    case EEnginePaths::Fonts:        { return "Content/Fonts"; }
    default:                         { panic( "Could not resolve engine path type." )  return ""; }
    }
}

template <typename InTPathTy> class LEnginePathBase;
using LEnginePath = LEnginePathBase<LPath>;

} /* ~Namespace Jafg */
