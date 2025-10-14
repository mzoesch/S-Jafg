// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if !LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #error "Plugins are not supported on this platform."
#endif /* !PLATFORM_SUPPORTS_SHARED_LIBRARIES */

#include "Engine/EngineCompileTimeConstants.h"

#if !JAFG_WITH_FOREIGN_SUPPORT
    #error "Plugins are not supported in this build configuration."
#endif /* !JAFG_WITH_FOREIGN_SUPPORT */

#include "Foreign/PluginForward.h"
#include "Foreign/PluginLifetime.h"

namespace Jafg
{

class LEngine;
class LPluginLifetime;

namespace EPluginLoadReturnCode
{

enum Type
{
    Success,

    ///////////////////////////////////////////////////////////////////////////////
    // Failure states.

    Failure,
    AlreadyLoaded,
    NotLoaded,
    NotFound,
    NoBin,
    PlatformError,
    NoLifetimeHandle,
    NoLifetime,
};

NODISCARD FORCEINLINE constexpr bool IsSuccess(const Type InValue) noexcept { return InValue == EPluginLoadReturnCode::Success; }
NODISCARD FORCEINLINE constexpr bool IsFailure(const Type InValue) noexcept { return InValue >= EPluginLoadReturnCode::Failure; }

} /* ~Namespace EPluginLoadReturnCode */
ENGINE_API LString LexToString(const EPluginLoadReturnCode::Type InValue);

struct LFetchedPlugin;
struct LLoadedPluginHandle;
struct LLoadedPlugin;

struct LFetchedPlugin final
{
    FORCEINLINE LFetchedPlugin() = default;

    //# The absolute path to the .jaf.root.plugin config file.
    LPath   AbsolutePath;

    //# Version as specified in the config.
    LString Version;

    //#
    //# Identifier used to refer to this plugin. Must be unique among all plugins.
    //# This identifier can be freely changed.
    //#
    LString Identifier;

    //#
    //# Optional identifier for the lifetime used for dynamic linking.
    //# This cannot be changed without recompiling the plugin binary.
    //#
    //# @note If not specified, the #Identifier is used.
    //#
    LString LifetimeIdentifier;

    //# Native identifier used for internal purposes only. Cannot be changed.
    LString NativeIdentifier;

    //#
    //# Optional Friendly name used for display purposes only.
    //#
    //# @note If not specified, the #Identifier is used.
    //#
    LString FriendlyName;

    //# Relative (to the config file) or absolute path to the binary implementing this plugin.
    LPath Bin;
};

struct LLoadedPlugin final
{
    friend LEngine;

    enum { InvalidUuid = 0 };

    FORCEINLINE LLoadedPlugin() = default;
    FORCEINLINE LLoadedPlugin(LFetchedPlugin const& InFetched, LPath const& InBinPath)
        : Fetched(InFetched), BinPath(InBinPath)
    {
    }

    FORCEINLINE LLoadedPlugin(LLoadedPlugin const& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin(LLoadedPlugin&& Other) noexcept
    {
        this->Fetched = std::move(Other.Fetched);
        this->BinPath = std::move(Other.BinPath);
        this->Uuid = Other.Uuid;
        this->NativeHandle = Other.NativeHandle;
        this->Lifetime = std::move(Other.Lifetime);
        Other.Uuid = InvalidUuid;
        Other.NativeHandle = nullptr;
        check( Other.Lifetime.get() == nullptr )

        return;
    }

    FORCEINLINE LLoadedPlugin& operator=(const LLoadedPlugin& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin& operator=(LLoadedPlugin&& Other) noexcept
    {
        this->Fetched = std::move(Other.Fetched);
        this->BinPath = std::move(Other.BinPath);
        this->Uuid = Other.Uuid;
        this->NativeHandle = Other.NativeHandle;
        this->Lifetime = std::move(Other.Lifetime);
        Other.Uuid = InvalidUuid;
        Other.NativeHandle = nullptr;
        check( Other.Lifetime.get() == nullptr )

        return *this;
    }

    ENGINE_API ~LLoadedPlugin();

    FORCEINLINE bool operator==(const LLoadedPlugin& Other) const noexcept { return this->BinPath == Other.BinPath; }

    FORCEINLINE constexpr bool IsLoaded() const noexcept { return this->NativeHandle != nullptr; }

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Fetched.AbsolutePath.empty() == false; }
    FORCEINLINE const LPath&   GetAbsolutePath() const noexcept { return this->Fetched.AbsolutePath; }
    FORCEINLINE const LString& GetIdentifier() const noexcept { return this->Fetched.Identifier; }
    FORCEINLINE const LString& GetFriendlyName() const noexcept { return this->Fetched.FriendlyName; }
    FORCEINLINE const LPath&   GetBin() const noexcept { return this->Fetched.Bin; }
    FORCEINLINE const LPath&   GetPathToBin() const noexcept { return this->BinPath; }

    FORCEINLINE constexpr LLoadedPluginHandle GetHandle() const noexcept { return { this->Uuid }; }

    FORCEINLINE LPluginLifetime* GetLifetime() noexcept { return this->Lifetime.get(); }
    FORCEINLINE LPluginLifetime const* GetLifetime() const noexcept { return this->Lifetime.get(); }

private:

    EPluginLoadReturnCode::Type OpenLibrary();
    void PrepareLibraryClose(const EPluginShutdownReason::Type InReason);
    EPluginLoadReturnCode::Type CloseLibrary(const EPluginShutdownReason::Type InReason);

    LFetchedPlugin Fetched;
    LPath BinPath;

    u32 Uuid{ LLoadedPlugin::InvalidUuid };

    void* NativeHandle { nullptr };

    TUnique<LPluginLifetime> Lifetime;
};

} /* ~Namespace Jafg */
