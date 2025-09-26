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

namespace Jafg
{

class LEngine;
class LPluginLifetime;
class LObjectContext;

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

    LPath AbsolutePath;
    LString Version;
    LString Identifier;
    LString FriendlyName;
    LPath Bin;
};

struct LLoadedPlugin final
{
    friend LEngine;

    enum { InvalidUuid = 0 };

    FORCEINLINE LLoadedPlugin() = default;
    FORCEINLINE LLoadedPlugin(const LFetchedPlugin& InFetched, const LPath& InBinPath)
        : Fetched(InFetched), BinPath(InBinPath)
    {
    }

    FORCEINLINE LLoadedPlugin(const LLoadedPlugin& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin(LLoadedPlugin&& Other) noexcept
    {
        this->Fetched = std::move(Other.Fetched);
        this->BinPath = std::move(Other.BinPath);
        this->Handle = Other.Handle;
        this->Lifetime = Other.Lifetime;
        this->Uuid = Other.Uuid;
        this->ObjectContext = std::move(Other.ObjectContext);
        Other.Handle = nullptr;
        Other.Lifetime = nullptr;
        Other.Uuid = InvalidUuid;

        return;
    }

    FORCEINLINE LLoadedPlugin& operator=(const LLoadedPlugin& Other) noexcept = delete;
    FORCEINLINE LLoadedPlugin& operator=(LLoadedPlugin&& Other) noexcept
    {
        this->Fetched = std::move(Other.Fetched);
        this->BinPath = std::move(Other.BinPath);
        this->Handle = Other.Handle;
        this->Lifetime = Other.Lifetime;
        this->Uuid = Other.Uuid;
        this->ObjectContext = std::move(Other.ObjectContext);
        Other.Handle = nullptr;
        Other.Lifetime = nullptr;
        Other.Uuid = InvalidUuid;

        return *this;
    }

    ENGINE_API ~LLoadedPlugin();

    FORCEINLINE bool operator==(const LLoadedPlugin& Other) const { return this->BinPath == Other.BinPath; }

    FORCEINLINE constexpr bool IsLoaded() const { return this->Handle != nullptr; }

    FORCEINLINE constexpr bool IsValid() const { return this->Fetched.AbsolutePath.IsEmpty() == false; }
    FORCEINLINE const LPath&   GetAbsolutePath() const { return this->Fetched.AbsolutePath; }
    FORCEINLINE const LString& GetIdentifier() const { return this->Fetched.Identifier; }
    FORCEINLINE const LString& GetFriendlyName() const { return this->Fetched.FriendlyName; }
    FORCEINLINE const LPath&   GetBin() const { return this->Fetched.Bin; }
    FORCEINLINE const LPath&   GetPathToBin() const { return this->BinPath; }

    FORCEINLINE constexpr LLoadedPluginHandle GetHandle() const { return { this->Uuid }; }

private:

    EPluginLoadReturnCode::Type OpenLibrary();
    void PrePareLibraryClose(const EPluginShutdownReason::Type InReason);
    EPluginLoadReturnCode::Type CloseLibrary(const EPluginShutdownReason::Type InReason);

    LFetchedPlugin Fetched;
    LPath BinPath;

    u32 Uuid { InvalidUuid };

    void* Handle { nullptr };
    LPluginLifetime* Lifetime { nullptr };

    Smart::TUnique<LObjectContext> ObjectContext { nullptr };
};

} /* ~Namespace Jafg */
