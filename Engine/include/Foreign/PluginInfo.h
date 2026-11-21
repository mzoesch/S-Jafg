// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

struct LPluginInfoImpl;

//#
//# Identifies a content namespace (the content dir of the engine or of a plugin). Subsystems use it as the key of
//# their resources (textures, materials, meshes, etc.).
//#
//# Clients requesting resources must never dereference it. Only subsystems do so, when they load a resource that
//# is not cached yet. It must not be a nullptr when requesting resources.
//#
//# Infos are owned by the engine and are never freed avoiding ABA problems.
//# Plugins pointing to the same content dir share the same info. A
//# plugin that is loaded again gets its previous info back.
//#
//# @see LLoadedPlugin::GetInfo
//#
typedef LPluginInfoImpl const* LPluginInfo;

namespace Detail
{

//# Owns all infos. Engine internal. Do not use.
class ENGINE_API LPluginInfoRegistry final
{
public:

    UTILITY_CLASS(LPluginInfoRegistry)

    //#
    //# The info of this content dir. Created, if there is none yet.
    //# @param Content Has to be an existing directory.
    //#
    NODISCARD static std::shared_ptr<LPluginInfoImpl const> FindOrAdd(LPath const& Content);

    //# The info with the deepest content dir that contains the path. Nullptr if there is none.
    NODISCARD static LPluginInfo FindContaining(LPath const& Path);
};

} /* ~Namespace Detail */

//#
//# Splits a path into the namespace that contains it and the path relative to the content dir of that namespace.
//# Fatal if no namespace contains the path.
//#
NODISCARD ENGINE_API std::pair<LPluginInfo, LString> SplitIntoNamespace(LPath const& Path);

struct LPluginInfoImpl final
{
    friend Detail::LPluginInfoRegistry;

    PROHIBIT_REALLOC_OF_ANY_FORM(LPluginInfoImpl)

    NODISCARD FORCEINLINE operator LPath const&() const noexcept
    {
        check(!this->ContentPath.empty())
        return this->ContentPath;
    }

    NODISCARD FORCEINLINE operator LString const&() const noexcept
    {
        check(!this->ContentPathString.empty())
        return this->ContentPathString;
    }

    NODISCARD FORCEINLINE operator LStringView() const noexcept
    {
        check(!this->ContentPathString.empty())
        return this->ContentPathString;
    }

    NODISCARD FORCEINLINE constexpr LPath const& operator*() const noexcept
    {
        check(!this->ContentPath.empty())
        return this->ContentPath;
    }

    NODISCARD LPath operator/(auto&& Path) const noexcept
        requires requires(LPath const& P) { P / Path; }
    {
        check(!this->ContentPath.empty())
        return this->ContentPath / Path;
    }

private:

    //# Only the registry creates infos. @see Detail::LPluginInfoRegistry
    LPluginInfoImpl() = default;

    void Initialize(LPath Path)
    {
        check(this->ContentPath.empty())
        this->ContentPath = canonical(Path);
        this->ContentPathString = this->ContentPath.generic_string();
    }

    LPath ContentPath;
    LString ContentPathString;
};

} /* ~Namespace Jafg */

//# The content dir.
template<>
struct std::formatter<Jafg::LPluginInfoImpl>: std::formatter<LString>
{
    auto format(Jafg::LPluginInfoImpl const& Info, std::format_context& Context) const
    {
        return std::formatter<LString>::format(static_cast<LString const&>(Info), Context);
    }
};
