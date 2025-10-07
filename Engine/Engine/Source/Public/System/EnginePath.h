// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/SystemForward.h"

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
    template<typename TSource, typename = std::filesystem::__detail::_LalPath<TSource>>
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

template <>
struct std::formatter<Jafg::LEnginePath> : std::formatter<std::string_view>
{
    FORCEINLINE auto format
    (
        const Jafg::LEnginePath& Path,
        std::format_context& InContext
    ) const -> std::format_context::iterator
    {
        return std::formatter<std::string_view>::format(std::string_view{std::to_address(Path.native().begin()), Path.native().size()}, InContext);
    }
};
