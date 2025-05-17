// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/RawInput.h"

namespace Jafg
{

struct LKeyEvent final
{
    FORCEINLINE constexpr LKeyEvent() noexcept
        : bIsPlatformRepeat(false) { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey) noexcept
        : bIsPlatformRepeat(false), Input(InKey) { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey, const f32 InValue) noexcept
        : bIsPlatformRepeat(false), Input(InKey, InValue) { }
    FORCEINLINE constexpr LKeyEvent(const LRawInput& InRawInput) noexcept
        : bIsPlatformRepeat(false), Input(InRawInput) { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey, const bool bIsPlatformRepeat) noexcept
        : bIsPlatformRepeat(bIsPlatformRepeat), Input(InKey) { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey, const f32 InValue, const bool bIsPlatformRepeat) noexcept
        : bIsPlatformRepeat(bIsPlatformRepeat), Input(InKey, InValue) { }
    FORCEINLINE constexpr LKeyEvent(const LRawInput& InRawInput, const bool bIsPlatformRepeat) noexcept
        : bIsPlatformRepeat(bIsPlatformRepeat), Input(InRawInput) { }

    //#
    //# Whether this key down event was triggered by a platform repeat. Meaning the repeat delay set on the platform
    //# directly by the user - based on the actual physical keyboard settings.
    //#
    FORCEINLINE constexpr bool IsPlatformRepeat() const noexcept { return this->bIsPlatformRepeat; }

    FORCEINLINE constexpr LRawInput GetRawInput() const noexcept { return { this->Input }; }
    FORCEINLINE constexpr LKey GetKey() const noexcept { return this->Input.Key; }
    FORCEINLINE constexpr f32 GetValue() const noexcept { return this->Input.Value; }

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{{{},{}}}", this->bIsPlatformRepeat, this->Input.ToString());
    }

private:

    bool bIsPlatformRepeat;
    LRawInput Input;
};

} /* ~Namespace Jafg */
