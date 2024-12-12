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
#include "User/Input/InputMode.h"
#include "Platform/MouseCursor.h"

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

    template <class T = LSurface>
    NODISCARD T* As() { return static_cast<T*>(this); }

    virtual void Initialize();
    virtual void OnClear();
    virtual void OnUpdate();
    virtual void TearDown();

    virtual void PollInputs() = 0;
    virtual void PollEvents() = 0;

    virtual     void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor);
    FORCEINLINE auto GetInputMode() const -> EInputMode::Type { return this->InputMode; }
    FORCEINLINE auto IsShowMouseCursor() const -> bool { return this->bShowCursor; }
    FORCEINLINE auto IsMouseLocationMeaningful() const -> bool { return this->bMouseLocationIsMeaningful; }
    FORCEINLINE auto GetMouseLocation() const -> LVector2 { return this->MouseLocation; }
    virtual     void SetMouseCursor(const EMouseCursor::Type InCursor) = 0;

    FORCEINLINE       auto GetViewport() const -> LViewport* { return this->SurfaceViewport; }
    NODISCARD virtual auto GetWidth() const -> int32                    = 0;
    NODISCARD virtual auto GetHeight() const -> int32                   = 0;
    NODISCARD virtual auto GetDimensions() const -> TIntVector2<int32>  = 0;

    /** Whether the current surface does ever support VSync. */
    NODISCARD virtual bool CanVSync() const              = 0;
              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const               = 0;

    FORCEINLINE auto AddKeyDown(const LKey InKey) -> void;
    FORCEINLINE auto AddKeyDown(const LKey InKey, const float InValue) -> void;
    FORCEINLINE auto AddKeyDown(const LRawInput& InRawInput) -> void;
    FORCEINLINE auto GetCurrentlyPressedKeys()       ->       TdhArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetCurrentlyPressedKeys() const -> const TdhArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetLastFramePressedKeys()       ->       TdhArray<LRawInput>& { return this->LastFrameDownKeys; }
    FORCEINLINE auto GetLastFramePressedKeys() const -> const TdhArray<LRawInput>& { return this->LastFrameDownKeys; }
    /** @return Whether the key is currently down. */
    bool IsKeyDown(const LKey InKey) const;
    FORCEINLINE bool IsKeyDown(const LRawInput& InRawInput) const { return this->IsKeyDown(InRawInput.Key); }
    /** @return Whether the key was just downed this frame. */
    bool IsNewKeyDown(const LKey InKey) const;
    FORCEINLINE bool IsNewKeyDown(const LRawInput& InRawInput) const { return this->IsNewKeyDown(InRawInput.Key); }
    /** @return Whether the key was just released this frame. */
    bool IsNewKeyUp(const LKey InKey) const;
    FORCEINLINE bool IsNewKeyUp(const LRawInput& InRawInput) const { return this->IsNewKeyUp(InRawInput.Key); }

protected:

    EInputMode::Type InputMode = EInputMode::UserInterface;
    bool bShowCursor = false;
    bool bMouseLocationIsMeaningful = false;
    LVector2 MouseLocation = LVector2::ZeroVector;

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

void Jafg::LSurface::AddKeyDown(const LKey InKey)
{
    check( this->DownKeys.FindRef(InKey) == nullptr )
    this->DownKeys.Emplace(InKey);
    return;
}

void Jafg::LSurface::AddKeyDown(const LKey InKey, const float InValue)
{
    check( this->DownKeys.FindRef(InKey) == nullptr )
    this->DownKeys.Emplace(InKey, InValue);
    return;
}

void Jafg::LSurface::AddKeyDown(const LRawInput& InRawInput)
{
    check( this->DownKeys.FindRef(InRawInput.Key) == nullptr )
    this->DownKeys.Emplace(InRawInput);
    return;
}

#if PLATFORM_DESKTOP
    #include "Platform/DesktopPlatform.h"
#elif PLATFORM_WASM
    #include "Platform/PlatformWasm.h"
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */
