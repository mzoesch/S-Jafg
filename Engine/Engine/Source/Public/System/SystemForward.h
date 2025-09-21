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

inline LPath LexToString(const EEnginePaths::Type InType)
{
    switch (InType)
    {
    case EEnginePaths::Textures:     { return "Content/Textures"; }
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

template <template <typename, typename> typename TEncoding, Lal::TStringBaseAllocatorConcept TAllocator>
    requires(Lal::TStringBaseEncodingConcept<TEncoding<typename TAllocator::T, typename TAllocator::SizeType>>)
class TEnginePathBase;

typedef TEnginePathBase<Lal::TStringBaseDefaultUtf8Traits, TArray<LJafgChar>> LEnginePath;

} /* ~Namespace Jafg */
