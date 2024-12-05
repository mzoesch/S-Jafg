// Copyright mzoesch. All rights reserved.

#pragma once

/**
 * When you include this file, it will transitively include the platform-specific surface header files.
 * There is no need to check on which platform you are and conditionally include the correct header file - this header
 * will do that for you.
 */
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#include "Platform/SurfaceForward.h"
#include "User/Input/RawInput.h"

namespace Jafg
{

class LViewport;

/** Interface for a generic surface that the RHI may use to draw on. */
class ENGINE_API LSurface
{
public:

    LSurface() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LSurface)
    virtual ~LSurface() = default;

    virtual void Initialize();
    virtual void OnClear();
    virtual void OnUpdate();
    virtual void TearDown();

    virtual void PollInputs()       = 0;
    virtual void PollEvents()       = 0;

    virtual void SetInputMode(bool bShowCursor) = 0;

    FORCEINLINE       auto GetViewport() const -> LViewport* { return this->SurfaceViewport; }
    NODISCARD virtual auto GetWidth() const -> int32                    = 0;
    NODISCARD virtual auto GetHeight() const -> int32                   = 0;
    NODISCARD virtual auto GetDimensions() const -> TIntVector2<int32>  = 0;

    /** Whether the current surface does ever support VSync. */
    NODISCARD virtual bool CanVSync() const              = 0;
              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const               = 0;

    template<class T = LSurface>
    NODISCARD T* As() { return static_cast<T*>(this); }

    FORCEINLINE auto AddKeyDown(const LKey InKey) -> void
    {
        check( this->DownKeys.FindRef(InKey) == nullptr )
        this->DownKeys.Emplace(InKey);
    }
    FORCEINLINE auto AddKeyDown(const LKey InKey, const float InValue) -> void
    {
        check( this->DownKeys.FindRef(InKey) == nullptr )
        this->DownKeys.Emplace(InKey, InValue);
    }
    FORCEINLINE auto AddKeyDown(const LRawInput& InRawInput) -> void
    {
        check( this->DownKeys.FindRef(InRawInput.Key) == nullptr )
        this->DownKeys.Emplace(InRawInput);
    }
    FORCEINLINE auto GetCurrentlyPressedKeys()       ->       TdhArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetCurrentlyPressedKeys() const -> const TdhArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetLastFramePressedKeys()       ->       TdhArray<LRawInput>& { return this->LastFrameDownKeys; }
    FORCEINLINE auto GetLastFramePressedKeys() const -> const TdhArray<LRawInput>& { return this->LastFrameDownKeys; }

    FORCEINLINE bool IsShowMouseCursor() const { return this->bShowMouseCursor; }

protected:

    bool bShowMouseCursor    = false;
    bool bFirstMouseCallback = true;
    double LastMouseX = 0.0;
    double LastMouseY = 0.0;

private:

    /**
     * The viewport that is used to draw on this surface meaning the viewport that includes the whole surface screen.
     */
    LViewport* SurfaceViewport = nullptr;

    /** The keys that are currently down for this surface this frame. */
    TdhArray<LRawInput> DownKeys;
    /** The keys that were down for this surface last frame. */
    TdhArray<LRawInput> LastFrameDownKeys;
};

} /* ~Namespace Jafg */

#if PLATFORM_DESKTOP
    #include "Platform/DesktopPlatform.h"
#elif PLATFORM_WASM
    #include "Platform/PlatformWasm.h"
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */
