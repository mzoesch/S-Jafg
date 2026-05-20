// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# When you include this file, it will transitively include the platform-specific surface header files.
//# There is no need to check on which platform you are and conditionally include the correct header file - this header
//# will do that for you.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#include "Platform/SurfaceForward.h"
#include "User/Input/RawInput.h"
#include "User/Input/InputMode.h"
#include "Framework/FrontendForward.h"
#include "Nodes/Viewport.h"

namespace Jafg
{

class LEngine;
class LLocalEgo;

struct LSurfaceCreateInfo
{
    bool bFullScreen           = false;
    bool bResizable            = true;
    bool bBorderless           = false;
    // bool bUseNativeResolution  = true;
    // TODO: Desired monitor?

    //# Jafg officially supports the minimal dimensions of 640x475px up to the maximum for "normal" use cases.
    LVec2u32 DesiredDimensionsPx{ 1280, 720 };
    LString HumanReadableName{ "Transient" };
};

//# Interface for a generic surface that the RHI may use to draw on.
class LSurfaceBase
{
public:

    inline explicit LSurfaceBase(LSurfaceCreateInfo const& Info) noexcept
        : SurfaceViewport{*this->AsSurface(), this->SurfaceExtent}
    {
        LOG_VERBOSE(LogSurface, "Creating surface [{}].", Info.HumanReadableName)
        this->HumanReadableName = Info.HumanReadableName;
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LSurfaceBase)
    virtual ~LSurfaceBase() = default;

    template<typename T = LSurfaceBase>
    NODISCARD FORCEINLINE T* As() { static_assert(std::is_base_of_v<LSurfaceBase, T>); return static_cast<T*>(this); }
    template<typename T = LSurfaceBase>
    NODISCARD FORCEINLINE T const* As() const { static_assert(std::is_base_of_v<LSurfaceBase, T>); return static_cast<T const*>(this); }
    NODISCARD FORCEINLINE LSurface* AsSurface();
    NODISCARD FORCEINLINE LSurface const* AsSurface() const;

    FORCEINLINE void SetHumanReadableName(LString const& S) noexcept { this->HumanReadableName = S; }
    FORCEINLINE LString const& GetHumanReadableName() const noexcept { return this->HumanReadableName; }

    void BeginNewFrame();
    void PollPlatformEvents() PURE_VIRTUAL()
    void Tick();
    void OnRender() PURE_VIRTUAL()

    FORCEINLINE void SetInputMode(EInputMode InMode) noexcept PURE_VIRTUAL()
    FORCEINLINE EInputMode GetInputMode() const noexcept { return this->InputMode; }
    FORCEINLINE bool IsShowMouseCursor() const noexcept { return static_cast<bool>(this->InputMode & EInputModeBits::ShowMouseCursor); }

    FORCEINLINE bool HasMouseLocation() const noexcept { return this->MouseLocation.has_value(); }
    FORCEINLINE auto const& GetMouseLocation() const noexcept { return this->MouseLocation; }
    FORCEINLINE LVec2F GetMouseLocationValue() const noexcept { check( this->MouseLocation.has_value() ) return this->MouseLocation.value(); }

    FORCEINLINE LViewport& GetViewport() noexcept { return this->SurfaceViewport; }
    FORCEINLINE LViewport const& GetViewport() const noexcept { return this->SurfaceViewport; }

    //# In physical pixels.
    NODISCARD FORCEINLINE constexpr rhi::extent2 GetSurfaceExtent() const noexcept { return this->SurfaceExtent; }

    NODISCARD bool CanEverVSync() const noexcept PURE_VIRTUAL()
              void SetVSync(const bool bEnabled) PURE_VIRTUAL()
    NODISCARD bool IsVSync() const noexcept PURE_VIRTUAL()
    NODISCARD bool CanEverResize() const PURE_VIRTUAL()
              void SetResizable(bool bResizable) PURE_VIRTUAL()
    NODISCARD bool IsResizable() const noexcept PURE_VIRTUAL()

    ENGINE_API void UpdateKeyState(LRawInput const& InRawInput);

    template<typename... TArgs> requires std::is_constructible_v<LRawInput, TArgs&&...>
    FORCEINLINE void EmplaceVirtualKey(TArgs&&... Args) noexcept { this->VirtualInput.emplace_back(std::forward<TArgs>(Args)...); }
    FORCEINLINE auto& GetVirtualInput() noexcept { return this->VirtualInput; }
    FORCEINLINE auto const& GetVirtualInput() const noexcept { return this->VirtualInput; }

    FORCEINLINE auto& GetMutableRawInputsDangerous() noexcept { return this->RawInputs; }
    FORCEINLINE auto const& GetRawInputs() const noexcept { return this->RawInputs; }
    FORCEINLINE auto& GetMutableUnconsumedInputsDangerous() noexcept { return this->UnconsumedInputs; }
    FORCEINLINE auto const& GetUnconsumedInputs() const noexcept { return this->UnconsumedInputs; }
    FORCEINLINE bool HasPlatformKeyState(LPhysicalKey Key, ERawInputStateFlags Flags) const noexcept
    {
        check(Key != LPhysicalKey{})
        if (auto It{algo::find(this->RawInputs, Key, &LRawInput::PhysicalKey)}; It != this->RawInputs.end())
        {
            return (It->State & Flags) != ERawInputStateBits::Identity;
        }
        return false;
    }
    FORCEINLINE bool HasConsumableKeyState(LPhysicalKey Key, ERawInputStateFlags Flags) const noexcept
    {
        check(Key != LPhysicalKey{})
        if (auto It{algo::find(this->UnconsumedInputs, Key, &LRawInput::PhysicalKey)}; It != this->UnconsumedInputs.end())
        {
            return (It->State & Flags) != ERawInputStateBits::Identity;
        }
        return false;
    }
    FORCEINLINE void ConsumeKey(LPhysicalKey Key) noexcept
    {
        check(Key != LPhysicalKey{})
        algo::erase_exactly_once_checked(&this->UnconsumedInputs, Key, &LRawInput::PhysicalKey);
    }
    FORCEINLINE decltype(auto) ConsumeKey(algo::iterator_t<TArray<LRawInput>> It) noexcept
    {
        return this->UnconsumedInputs.erase(It);
    }

    FORCEINLINE bool HasBufferedPlatformInput() const { return !this->PlatformInput.empty(); }
    FORCEINLINE TArray<LString> const& GetRawBufferedPlatformInput() const { return this->PlatformInput; }
    FORCEINLINE LString GetBufferedPlatformInput() const noexcept
    {
        std::stringstream Result;
        for (auto& Input : this->PlatformInput) { Result << Input; }
        return Result.str();
    }
    FORCEINLINE LString ConsumeBufferedPlatformInput() noexcept
    {
        LString Result{this->GetBufferedPlatformInput()};
        this->PlatformInput.clear();
        return Result;
    }

    ENGINE_API LEngine const& GetEngine() const noexcept;
    ENGINE_API LEngine& GetMutableEngine() noexcept;
    ENGINE_API LLocalEgo const& GetLocalEgo() const noexcept;
    ENGINE_API LLocalEgo& GetMutableLocalEgo() noexcept;
    ENGINE_API LFrontend const& GetFrontend() const noexcept;
    ENGINE_API LFrontend& GetMutableFrontend() noexcept;

protected:

    FORCEINLINE void AddBufferedPlatformInput(LString InInput) noexcept { this->PlatformInput.emplace_back(std::move(InInput)); }

    //# Input mode. The mouse cursor visibility might be ignored on some platform configurations.
    EInputMode InputMode{ EInputModeBits::ShowMouseCursor };
    //# Mouse inside surface.
    bool bMouseInsideSurface{};
    //# The mouse location if available. In some platform configurations, this value might always be missing.
    std::optional<LVec2F> MouseLocation;

    rhi::extent2 SurfaceExtent;

private:

    LString HumanReadableName{ "Transient" };
    //# The viewport that is used to draw on this surface meaning the viewport that includes the whole surface screen.
    LViewport SurfaceViewport;

    //#
    //# The current raw inputs.
    //# - ERawInputStateBits::Press decay after one frame to ERawInputStateBits::Hold.
    //# - ERawInputStateBits::Repeat decay after one frame.
    //# - ERawInputStateBits::Release will result in the removal of the key from this array after one frame.
    //#
    TArray<LRawInput> RawInputs;
    //# The current frame raw inputs that are still unconsumed.
    TArray<LRawInput> UnconsumedInputs;

    //#
    //# Virtual input for mock input.
    //# Only mocked if physical input is not available for said physical action.
    //# This array is cleared every frame.
    //#
    TArray<LRawInput> VirtualInput;

    //#
    //# This frame platform-localized input. Buffer is cleared every frame.
    //# So if you need this for later reference, you have to copy it.
    //#
    TArray<LString> PlatformInput;
};

} /* ~Namespace Jafg */

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #include "Platform/SurfaceGlfw3.h"
#elif PLATFORM_USES_JAVA_SCRIPT_FRONTEND
    #include "Platform/SurfaceWasm.h"
#else /* PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

NODISCARD FORCEINLINE Jafg::LSurface* Jafg::LSurfaceBase::AsSurface()
{
    return this->As<LSurface>();
}

NODISCARD FORCEINLINE Jafg::LSurface const* Jafg::LSurfaceBase::AsSurface() const
{
    return this->As<LSurface>();
}
