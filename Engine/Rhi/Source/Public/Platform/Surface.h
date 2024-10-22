// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Player/RawInput.h"

namespace Jafg
{

class LPlayerInput;

MAKE_EXTERNAL_TEMPLATE_DHARRAY(RHI, LRawInput)

/** Interface for a generic surface that the RHI may use to draw on. */
class RHI_API LSurface
{
    friend LPlayerInput;

public:

    LSurface() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LSurface)
    virtual ~LSurface() = default;

    virtual void Initialize()       = 0;
    virtual void OnClear()          = 0;
    virtual void OnUpdate()         = 0;
    virtual void TearDown()         = 0;

    virtual void PollInputs()       = 0;
    virtual void PollEvents()       = 0;

    virtual void SetInputMode(const bool bShowCursor) = 0;

    NODISCARD virtual auto GetWidth() const -> int32                    = 0;
    NODISCARD virtual auto GetHeight() const -> int32                   = 0;
    NODISCARD virtual auto GetDimensions() const -> TIntVector2<int32>  = 0;

              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const               = 0;

    template<class T = LSurface>
    NODISCARD T* As() { return static_cast<T*>(this); }

    FORCEINLINE auto AddKeyDown(const LKey InKey) -> void { this->DownKeys.Emplace(InKey); }
    FORCEINLINE auto AddKeyDown(const LKey InKey, const float InValue) -> void { this->DownKeys.Emplace(InKey, InValue); }
    FORCEINLINE auto GetCurrentlyPressedKeys() const -> const TdhArray<LRawInput>& { return this->DownKeys; }

private:

    /** The keys that are currently down for this surface this frame. */
    TdhArray<LRawInput> DownKeys;
    /** The keys that were down for this surface last frame. */
    TdhArray<LRawInput> LastFrameDownKeys;
};

} /* ~Namespace Jafg */

#if PLATFORM_DESKTOP
    #include "Platform/DesktopPlatform.h"
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */
