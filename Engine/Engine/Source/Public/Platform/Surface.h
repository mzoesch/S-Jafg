// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# When you include this file, it will transitively include the platform-specific surface header files.
//# There is no need to check on which platform you are and conditionally include the correct header file - this header
//# will do that for you.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#include "Core/Application.h"
#include "Platform/SurfaceForward.h"
#include "User/Input/RawInput.h"
#include "User/Input/InputMode.h"
#include "Platform/MouseCursor.h"
#include "Widgets/Viewport.h"

namespace Jafg
{

//#
//# Interface for a generic surface that the RHI may use to draw on.
//#
class LSurfaceBase
{
public:

    LSurfaceBase() = default;
    PROHIBIT_COPY(LSurfaceBase)
    DEFAULT_MOVE(LSurfaceBase)
    virtual ~LSurfaceBase() = default;

    template <class T = LSurfaceBase>
    NODISCARD FORCEINLINE T* As();
    template <class T = LSurfaceBase>
    NODISCARD FORCEINLINE const T* As() const;
    NODISCARD FORCEINLINE LSurface* AsSurface();
    NODISCARD FORCEINLINE const LSurface* AsSurface() const;

    //# Initialize should make the handle to a native surface screen valid or panic if not possible.
    virtual void Initialize();
    virtual void Tick();
    virtual void OnClear() { this->SurfaceViewport.OnClear(); }
    virtual void OnUpdate() { this->SurfaceViewport.Draw(); }
    //# TearDown should release the handle to the native surface screen or panic if not possible.
    virtual void TearDown();

    virtual bool IsValid() = 0;

    virtual void BeginNewFrame();
    virtual void PollInputs() = 0;
    virtual void PollEvents() = 0;
            void PollVirtualInputs();

    virtual     void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor);
    FORCEINLINE auto GetInputMode() const -> EInputMode::Type { return this->InputMode; }
    FORCEINLINE auto IsShowMouseCursor() const -> bool { return this->bShowCursor; }
    FORCEINLINE auto IsMouseLocationMeaningful() const -> bool { return this->bMouseLocationIsMeaningful; }
    FORCEINLINE auto GetMouseLocation() const -> LVector2 { return this->MouseLocation; }
    virtual     void SetMouseCursor(const EMouseCursor::Type InCursor) = 0;

    FORCEINLINE       auto GetViewport()       ->       LViewport& { return this->SurfaceViewport; }
    FORCEINLINE       auto GetViewport() const -> const LViewport& { return this->SurfaceViewport; }
    NODISCARD virtual auto GetWidth() const -> i32                    = 0;
    NODISCARD virtual auto GetHeight() const -> i32                   = 0;
    NODISCARD virtual auto GetDimensions() const -> TIntVector2<i32>  = 0;

    //# Whether the current surface does ever support VSync.
    NODISCARD virtual bool CanVSync() const              = 0;
              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const               = 0;

    FORCEINLINE void AddKeyDown(const LKey InKey);
    FORCEINLINE void AddKeyDown(const LKey InKey, const f32 InValue);
    FORCEINLINE void AddKeyDown(const LRawInput& InRawInput);
    FORCEINLINE void AddVirtualKeyDown(const LKey InKey) { this->VirtualInput.Emplace(InKey); }
    FORCEINLINE void AddVirtualKeyDown(const LKey InKey, const float InValue) { this->VirtualInput.Emplace(InKey, InValue); }
    FORCEINLINE void AddVirtualKeyDown(const LRawInput& InRawInput) { this->VirtualInput.Emplace(InRawInput); }
    FORCEINLINE auto GetCurrentlyPressedKeys()       ->       TArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetCurrentlyPressedKeys() const -> const TArray<LRawInput>& { return this->DownKeys;          }
    FORCEINLINE auto GetLastFramePressedKeys()       ->       TArray<LRawInput>& { return this->LastFrameDownKeys; }
    FORCEINLINE auto GetLastFramePressedKeys() const -> const TArray<LRawInput>& { return this->LastFrameDownKeys; }
    FORCEINLINE auto GetVirtualInput()       ->       TArray<LRawInput>& { return this->VirtualInput; }
    FORCEINLINE auto GetVirtualInput() const -> const TArray<LRawInput>& { return this->VirtualInput; }
    //# @return Whether the key is currently down.
    FORCEINLINE bool IsKeyDown(const LKey InKey) const { return this->GetCurrentlyPressedKeys().Contains(InKey); }
    FORCEINLINE bool IsKeyDown(const LRawInput& InRawInput) const { return this->IsKeyDown(InRawInput.Key); }
    //# @return Whether the key was just downed this frame.
    FORCEINLINE bool IsNewKeyDown(const LKey InKey) const;
    FORCEINLINE bool IsNewKeyDown(const LRawInput& InRawInput) const { return this->IsNewKeyDown(InRawInput.Key); }
    //# @return Whether the key was just released this frame.
    FORCEINLINE bool IsKeyUp(const LKey InKey) const;
    FORCEINLINE bool IsKeyUp(const LRawInput& InRawInput) const { return this->IsKeyUp(InRawInput.Key); }

    FORCEINLINE bool HasBufferedPlatformInput() const { return this->PlatformInput.IsEmpty() == false; }
    FORCEINLINE const TArray<LString>& GetBufferedPlatformInput() const { return this->PlatformInput; }
    FORCEINLINE LString GetBufferedPlatformInputAsStr() const;

    template <typename Predicate>
    FORCEINLINE void ForEachNewKeyDown(Predicate InPredicate);

protected:

    FORCEINLINE void AddBufferedPlatformInput(const char* InInput) { this->PlatformInput.Emplace(InInput); }
    FORCEINLINE void AddBufferedPlatformInput(const LString& InInput) { this->PlatformInput.Emplace(InInput); }
    FORCEINLINE void AddBufferedPlatformInput(LString&& InInput) { this->PlatformInput.Emplace(std::move(InInput)); }

#if PLATFORM_LINUX
    FORCEINLINE void SetPlatformSupportsRepeatedKey(const bool bInSupportsRepeatedKey) noexcept { this->bPlatformSupportsRepeatedKeyDown = bInSupportsRepeatedKey; }
    FORCEINLINE bool IsPlatformSupportsRepeatedKey() const noexcept { return this->bPlatformSupportsRepeatedKeyDown; }
    FORCEINLINE void SetRepeatedDelay(const f32 InDelay) noexcept { this->RepeatedDelay = InDelay; }
    FORCEINLINE f32  GetRepeatedDelay() const noexcept { return this->RepeatedDelay; }
    FORCEINLINE void SetRepeatedRate(const f32 InRate) noexcept { this->RepeatedRate = InRate; this->RepeatedBufferTime = InRate; }
    FORCEINLINE f32  GetRepeatedRate() const noexcept { return this->RepeatedRate; }
    FORCEINLINE void SetLastPressTimePoint(const Application::LHrcTimePoint& InTimePoint) noexcept { this->LastPressTimePoint = InTimePoint; this->RepeatedBufferTime = this->RepeatedRate; }
    FORCEINLINE void SetLastPressTimePoint(Application::LHrcTimePoint&& InTimePoint) noexcept { this->LastPressTimePoint = std::move(InTimePoint); }

    FORCEINLINE bool IsThisKeyRepeatedThisFrame(const LKey InKey) const noexcept { return this->bThisFrameRepeatedKeyDown && this->LastNewKey == InKey; }
    FORCEINLINE bool IsCurrenRepeatedKeyInQuestionValid() const noexcept { return this->LastNewKey != EKeys::Unresolved; }
    FORCEINLINE LKey GetCurrenRepeatedKeyInQuestion() const noexcept { return this->LastNewKey; }
    FORCEINLINE void SetCurrentRepeatedKeyInQuestion(const LKey InKey) noexcept { check( this->IsPlatformSupportsRepeatedKey() == false) this->LastNewKey = InKey; }
    virtual     void EmulateRepeatedContentForBufferedInput() = 0;
    virtual     void EmulateContentForBufferedInput(const LKey InKey) = 0;
#endif /* PLATFORM_LINUX */

    EInputMode::Type InputMode { EInputMode::UserInterface };
    bool bShowCursor { true };
    bool bMouseLocationIsMeaningful { false };
    LVector2 MouseLocation;

private:

    //#
    //# The viewport that is used to draw on this surface meaning the viewport that includes the whole surface screen.
    //#
    LViewport SurfaceViewport;
    bool bSurfaceViewportValid { false };

    //# The keys that are currently down for this surface this frame.
    TArray<LRawInput> DownKeys;

    //# The keys that were down for this surface last frame.
    TArray<LRawInput> LastFrameDownKeys;

    //#
    //# Virtual input for mock input.
    //# Only mocked if physical input is not available for said physical action.
    //#
    TArray<LRawInput> VirtualInput;

    //#
    //# This frame platform-localized input. Buffer is cleared every frame.
    //# So if you need this for later reference, you have to copy it.
    //#
    TArray<LString> PlatformInput;

#if PLATFORM_LINUX
    //#
    //# If this is false, the underlying platform does not support repeated key down events.
    //# This depends on the current desktop environment session. Primary X11 and Wayland.
    //# This is not an issue on Windows or macOS.
    //# If this is not supported, the engine will not send repeated key down events - except emulated Utf-8 input
    //# ones which may be inaccurate and not represent the actual user preferences of the underlying linux session.
    //#
    //# Not that this is not a good solution - but works for basic stuff. If the main loop lags, some input may be
    //# lost that would usually be repeated (when using the underlying operating system directly).
    //#
    bool bPlatformSupportsRepeatedKeyDown { true };
    Application::LHrcTimePoint LastPressTimePoint;
    f32 RepeatedBufferTime {  1.0f / 25.0f };
    f32 RepeatedDelay { 0.6f };
    f32 RepeatedRate  { 1.0f / 25.0f };
    bool bThisFrameRepeatedKeyDown { false };
    LKey LastNewKey { EKeys::Unresolved };
#endif /* PLATFORM_LINUX */
};

} /* ~Namespace Jafg */

void Jafg::LSurfaceBase::AddKeyDown(const LKey InKey)
{
    check( this->DownKeys.FindRef(InKey) == nullptr )
    this->DownKeys.Emplace(InKey);
    return;
}

void Jafg::LSurfaceBase::AddKeyDown(const LKey InKey, const float InValue)
{
    check( this->DownKeys.FindRef(InKey) == nullptr )
    this->DownKeys.Emplace(InKey, InValue);
    return;
}

void Jafg::LSurfaceBase::AddKeyDown(const LRawInput& InRawInput)
{
    check( this->DownKeys.FindRef(InRawInput.Key) == nullptr )
    this->DownKeys.Emplace(InRawInput);
    return;
}

template<class T>
NODISCARD FORCEINLINE T* Jafg::LSurfaceBase::As()
{
    static_assert(std::is_base_of_v<LSurfaceBase, T>, "T must be derived from LSurfaceBase");
    return static_cast<T*>(this);
}

template<class T>
NODISCARD FORCEINLINE const T* Jafg::LSurfaceBase::As() const
{
    static_assert(std::is_base_of_v<LSurfaceBase, T>, "T must be derived from LSurfaceBase");
    return static_cast<const T*>(this);
}

NODISCARD FORCEINLINE Jafg::LSurface* Jafg::LSurfaceBase::AsSurface()
{
    return this->As<LSurface>();
}

NODISCARD FORCEINLINE const Jafg::LSurface* Jafg::LSurfaceBase::AsSurface() const
{
    return this->As<LSurface>();
}

FORCEINLINE bool Jafg::LSurfaceBase::IsNewKeyDown(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) && (this->GetLastFramePressedKeys().Contains(InKey) == false);
}

FORCEINLINE bool Jafg::LSurfaceBase::IsKeyUp(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) == false && this->GetLastFramePressedKeys().Contains(InKey);
}

FORCEINLINE LString Jafg::LSurfaceBase::GetBufferedPlatformInputAsStr() const
{
    LString Out;

    for (const LString& Input : this->PlatformInput)
    {
        Out.Append(Input);
    }

    return Out;
}

template<typename Predicate>
FORCEINLINE void Jafg::LSurfaceBase::ForEachNewKeyDown(Predicate InPredicate)
{
    for (const LRawInput& Input : this->DownKeys)
    {
        if (this->IsNewKeyDown(Input.Key))
        {
            InPredicate(Input);
        }

        continue;
    }

    return;
}

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #include "Platform/SurfaceGlfw3.h"
#elif PLATFORM_USES_JAVA_SCRIPT_FRONTEND
    #include "Platform/SurfaceWasm.h"
#else /* PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
