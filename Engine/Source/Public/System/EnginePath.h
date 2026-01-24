// Copyright mzoesch. All rights reserved.

#pragma once

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

template<typename TOut = LPath>
inline TOut LexToString(EEnginePaths::Type InType) noexcept
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

namespace Jafg
{

class JUserPreferences;

//#
//# A path that is always relative to a content folder. The content folder that is used is then determined when
//# resolving the absolute path. This allows the user to add their own plugin content folders that have a higher
//# priority than the engine content folder. Absolute paths will then be redirected to the plugin content folder.
//#
template <typename TRet, typename TSuper>
class TEnginePathBase : public TSuper
{
public:

    typedef TRet   Ret;
    typedef TSuper Super;

    using Super::Super;

    FORCEINLINE explicit TEnginePathBase(const EEnginePaths::Type Type) noexcept
        : Super(LexToString<TSuper>(Type)) { }
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type, LString const& Source) noexcept
        : Super(LexToString<TSuper>(Type) / static_cast<std::string>(Source)) { }
    template<typename TSource, typename = std::filesystem::__detail::_JafgPath<TSource>>
    FORCEINLINE TEnginePathBase(const EEnginePaths::Type Type, TSource const& Source) noexcept
        : Super(LexToString<TSuper>(Type) / std::basic_string_view{Source}) { }

    FORCEINLINE LPath ResolvePath() const noexcept
    {
        //#
        //# Currently there is no user preference to override the engine content folder and add priorities.
        //# But this logic will be added here.
        //#
        return LPath{this->generic_string()};
    }
    FORCEINLINE LPath ResolvePathToAbsolute() const noexcept
    {
        return absolute(this->ResolvePath());
    }
};

} /* ~Namespace Jafg */

typedef Jafg::TEnginePathBase<LPath, LPath> LEnginePath;

template<>
struct std::formatter<LEnginePath> : std::formatter<std::string>
{
    FORCEINLINE auto format
    (
        LEnginePath const& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        const LString PathStr{Path.string()};
        return std::formatter<std::string>::format(PathStr, InContext);
    }
};
