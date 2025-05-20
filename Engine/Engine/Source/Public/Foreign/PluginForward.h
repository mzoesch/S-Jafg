// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

struct LLoadedPlugin;

#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
namespace EPluginShutdownReason
{

enum Type
{
    //#
    //# Your plugin is unloaded, but the engine will live.
    //# Remove all identifiers to your plugin from the engine.
    //#
    Unload,

    //#
    //# Your plugin is unloaded, and the engine will shortly terminate.
    //# You do not have to make a clean exit. E.g., handles may not be
    //# removed, if applicable, as the engine is dead.
    //#
    EngineTearDown,

    //#
    //# Something happened that usually should never happen.
    //# It is best to figure out on your own what to do. Check global #GEngine status and decide what to do.
    //#
    Unspecified,
};

} /* ~Namespace EPluginShutdownReason */
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

//
// The following code this part of the standard engine.
// We consider the engine to be a very specific plugin itself. Even though it is never loaded dynamically at runtime.
// So the handle of the engine is one. An invalid handle is zero.
//

struct LLoadedPluginHandle final
{
    friend LLoadedPlugin;

    enum { EngineUuid = 1 };

    //#
    //# The handle to the "engine plugin".
    //#
    FORCEINLINE static constexpr LLoadedPluginHandle GetEnginePluginHandle() noexcept
    {
        return { EngineUuid };
    }

    FORCEINLINE constexpr LLoadedPluginHandle() noexcept = default;

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Handle != 0; }
    FORCEINLINE constexpr bool IsInvalid() const noexcept { return this->Handle == 0; }

    FORCEINLINE constexpr bool operator==(const LLoadedPluginHandle& Other) const noexcept { return this->Handle == Other.Handle; }
    FORCEINLINE constexpr bool operator!=(const LLoadedPluginHandle& Other) const noexcept { return this->Handle != Other.Handle; }

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsValid(); }

private:

    FORCEINLINE constexpr LLoadedPluginHandle(const u32 InHandle) noexcept : Handle(InHandle) { };

    u32 Handle { 0 };
};

} /* ~Namespace Jafg */
