// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

namespace EEnginePaths
{

enum Type : u8
{
    //# Custom path
    None = 0,

    //# Content/Textures
    Textures,

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

template <typename TOut = LPath>
inline TOut LexToString(const EEnginePaths::Type InType)
{
    switch (InType)
    {
    case EEnginePaths::Textures:     { return TOut{"Content/Textures"}; }
    case EEnginePaths::Voxels:       { return TOut{"Content/Textures/Voxels"}; }
    case EEnginePaths::Interface:    { return TOut{"Content/Textures/Interface"}; }
    case EEnginePaths::Blends:       { return TOut{"Content/Textures/Blends"}; }
#if PLATFORM_WASM
    case EEnginePaths::Shaders:      { return "Content/Shaders/Gles3"; }
#else /* PLATFORM_WASM */
    case EEnginePaths::Shaders:      { return TOut{"Content/Shaders/NativeGl"}; }
#endif /* !PLATFORM_WASM */
    case EEnginePaths::Fonts:        { return TOut{"Content/Fonts"}; }
    default:                         { panic( "Could not resolve engine path type." )  return ""; }
    }
}

template <typename TRet, typename TSuper>
class TEnginePathBase;

typedef TEnginePathBase<LPath, LPath> LEnginePath;

} /* ~Namespace Jafg */
