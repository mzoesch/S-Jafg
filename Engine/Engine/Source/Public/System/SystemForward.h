// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/Path.h"

namespace Jafg
{

namespace EEnginePaths
{

enum Type : u8
{
    //# Custom path
    None = 0,

    //# Custom path relative to a module.
    CustomModule,

    //# Custom path relative to the engine.
    CustomEngine,

    //# Content/Textures/Voxels
    Voxels,

    //# Content/Textures/Interface
    Interface,

    //# Content/Textures/Blends
    Blends,

    //# Content/Shaders
    Shaders,

    //# Content/Fonts
    Fonts,
};

} /* ~Namespace EEnginePaths */

inline LString LexToString(const EEnginePaths::Type InType)
{
    switch (InType)
    {
    case EEnginePaths::CustomModule: { return ""; }
    case EEnginePaths::CustomEngine: { return ""; }
    case EEnginePaths::Voxels:       { return "Content/Textures/Voxels"; }
    case EEnginePaths::Interface:    { return "Content/Textures/Interface"; }
    case EEnginePaths::Blends:       { return "Content/Textures/Blends"; }
#if PLATFORM_WASM
    case EEnginePaths::Shaders:      { return "Content/Shaders/Gles3"; }
#else /* PLATFORM_WASM */
    case EEnginePaths::Shaders:      { return "Content/Shaders/NativeGl"; }
#endif /* !PLATFORM_WASM */
    case EEnginePaths::Fonts:        { return "Content/Fonts"; }
    default:                         { panic( "Could not resolve engine path type." )  return ""; }
    }
}

template <typename InTPathTy> class LEnginePathBase;
using LEnginePath = LEnginePathBase<LPath>;

} /* ~Namespace Jafg */
