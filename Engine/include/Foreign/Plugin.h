// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#if !JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES
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
    friend LEngine;

    //# The absolute path to the .jafg.root.plugin config file.
    LPath AbsolutePath;

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

    //#
    //# Optional description of the plugin used for display purposes only.
    //#
    LString Description;

    //#
    //# Author of the plugin used for display purposes only.
    //#
    LString Author;

    //#
    //# Whether this plugin can be dynamically unloaded at runtime.
    //#
    bool bDynUnloadable{ true };

    //# Relative (to the config file) or absolute path to the binary implementing this plugin.
    LPath Bin;

    //# Whether this plugin has content.
    bool bHasContent{};
    //# If #bHasContent is set this will point a the valid content dir of this plugin.
    LPath Content;
};

struct LLoadedPlugin final
{
    friend LEngine;

    enum : u8 { InvalidUuid = 0 };

    FORCEINLINE LLoadedPlugin() = default;
    FORCEINLINE LLoadedPlugin(LFetchedPlugin InFetched, LPath InBinPath)
        : Fetched{std::move(InFetched)}, BinPath{std::move(InBinPath)}
    {
    }

    FORCEINLINE LLoadedPlugin(LLoadedPlugin const& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin(LLoadedPlugin&& Other) noexcept
        : Fetched{std::move(Other.Fetched)}, BinPath{std::move(Other.BinPath)}, Uuid{Other.Uuid}
        , NativeHandle{Other.NativeHandle}, bStarted{std::exchange(Other.bStarted, false)}, Lifetime{std::move(Other.Lifetime)}
        , ContentOverride{std::exchange(Other.ContentOverride, std::nullopt)}, Info{std::move(Other.Info)}
    {
        Other.Uuid = InvalidUuid;
        Other.NativeHandle = nullptr;
        check(Other.Lifetime.get() == nullptr)
    }

    FORCEINLINE LLoadedPlugin& operator=(const LLoadedPlugin& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin& operator=(LLoadedPlugin&& Other) noexcept
    {
        this->Fetched = std::move(Other.Fetched);
        this->BinPath = std::move(Other.BinPath);
        this->Uuid = std::exchange(Other.Uuid, InvalidUuid);
        this->NativeHandle = std::exchange(Other.NativeHandle, nullptr);
        this->bStarted = std::exchange(Other.bStarted, false);
        this->Lifetime = std::move(Other.Lifetime);
        this->ContentOverride = std::exchange(Other.ContentOverride, std::nullopt);
        this->Info = std::move(Other.Info);
        check(Other.Lifetime.get() == nullptr)

        return *this;
    }

    ENGINE_API ~LLoadedPlugin();

    FORCEINLINE bool operator==(const LLoadedPlugin& Other) const noexcept { return this->BinPath == Other.BinPath; }

    FORCEINLINE constexpr bool IsLoaded() const noexcept { return this->NativeHandle != nullptr; }

    NODISCARD FORCEINLINE constexpr bool        IsValid() const noexcept { return !this->Fetched.AbsolutePath.empty(); }
    NODISCARD FORCEINLINE constexpr auto const& GetAbsolutePath() const noexcept { return this->Fetched.AbsolutePath; }
    NODISCARD FORCEINLINE constexpr auto const& GetVersion() const noexcept { return this->Fetched.Version; }
    NODISCARD FORCEINLINE constexpr auto const& GetIdentifier() const noexcept { return this->Fetched.Identifier; }
    NODISCARD FORCEINLINE constexpr auto const& GetLifetimeIdentifier() const noexcept { return this->Fetched.LifetimeIdentifier; }
    NODISCARD FORCEINLINE constexpr auto const& GetNativeIdentifier() const noexcept { return this->Fetched.NativeIdentifier; }
    NODISCARD FORCEINLINE constexpr auto const& GetFriendlyName() const noexcept { return this->Fetched.FriendlyName; }
    NODISCARD FORCEINLINE constexpr auto const& GetDescription() const noexcept { return this->Fetched.Description; }
    NODISCARD FORCEINLINE constexpr auto const& GetAuthor() const noexcept { return this->Fetched.Author; }
    NODISCARD FORCEINLINE constexpr auto        IsDynUnloadable() const noexcept { return this->Fetched.bDynUnloadable; }
    NODISCARD FORCEINLINE constexpr auto const& GetRelativeBinPath() const noexcept { return this->Fetched.Bin; }
    NODISCARD FORCEINLINE constexpr auto        HasContent() const noexcept { return this->Fetched.bHasContent; }
    NODISCARD FORCEINLINE constexpr auto const& GetDefaultContent() const noexcept { return this->Fetched.Content; }
    NODISCARD FORCEINLINE constexpr auto const& GetFetched() const noexcept { return this->Fetched; }

    NODISCARD FORCEINLINE constexpr auto const& GetAbsoluteBin() const noexcept { return this->BinPath; }

    FORCEINLINE constexpr LLoadedPluginHandle GetHandle() const noexcept { return { this->Uuid }; }

    NODISCARD FORCEINLINE constexpr bool IsStarted() const noexcept { return this->bStarted; }

    FORCEINLINE LPluginLifetime* GetLifetime() noexcept { return this->Lifetime.get(); }
    FORCEINLINE LPluginLifetime const* GetLifetime() const noexcept { return this->Lifetime.get(); }

    //#
    //# Overriding is only allowed before startup of said plugin and before #GetInfo was called the first time.
    //# @return True, if override was successful.
    //#
    bool OverrideContentDir(LPath Path) const noexcept
    {
        if (!this->bStarted && !this->Info)
        {
            this->ContentOverride = std::move(Path);
            return true;
        }
        return false;
    }

    //#
    //# Whether this plugin dynamically has overridden their content dir or was overridden by the engine.
    //#
    NODISCARD FORCEINLINE constexpr bool IsContentOverridden() const noexcept { return !!this->ContentOverride; }
    NODISCARD LPath const& GetContent() const noexcept
    {
        check(this->HasContent())
        if (this->IsContentOverridden())
        {
            return *this->ContentOverride;
        }
        return this->GetDefaultContent();
    }

    //#
    //# The info of the content dir of this plugin. Use it to request resources from subsystems. @see LPluginInfo
    //#
    //# The first call freezes the content dir: #OverrideContentDir fails afterward. So override it before calling
    //# this, e.g., in #LPluginLifetime::OnStartup. If the plugin did not call it during startup, the engine calls it
    //# right after #LPluginLifetime::OnStartup.
    //#
    //# Requires #HasContent.
    //#
    NODISCARD ENGINE_API LPluginInfo GetInfo() const;

private:

    EPluginLoadReturnCode::Type OpenLibrary();
    void PrepareLibraryClose(EPluginShutdownReason::Type InReason);
    EPluginLoadReturnCode::Type CloseLibrary(EPluginShutdownReason::Type InReason);

    LFetchedPlugin Fetched;
    LPath BinPath;

    u32 Uuid{ LLoadedPlugin::InvalidUuid };

    void* NativeHandle{};
    bool bStarted{};

    TUnique<LPluginLifetime> Lifetime;
    //#
    //# Some plugins might want to override their content dir at runtime. They may do this with that variable.
    //# @see OverrideContentDir
    //#
    mutable std::optional<LPath> ContentOverride;

    //#
    //# Set by the first call of #GetInfo. Owned by the engine, which never frees it. Shared with all other plugins
    //# pointing to the same content dir.
    //#
    mutable std::shared_ptr<LPluginInfoImpl const> Info;
};

} /* ~Namespace Jafg */
