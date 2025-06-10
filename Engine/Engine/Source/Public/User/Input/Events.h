// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/RawInput.h"

namespace Jafg
{

struct LKeyEvent final
{
    FORCEINLINE constexpr LKeyEvent() noexcept
        : Input() { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey) noexcept
        : Input(InKey) { }
    FORCEINLINE constexpr LKeyEvent(const LKey InKey, const f32 InValue) noexcept
        : Input(InKey, InValue) { }
    FORCEINLINE constexpr LKeyEvent(const LRawInput& InRawInput) noexcept
        : Input(InRawInput) { }

    //#
    //# Whether this key down event was triggered by a platform repeat. Meaning the repeat delay set on the platform
    //# directly by the user - based on the actual physical keyboard settings.
    //#
    FORCEINLINE constexpr bool IsPlatformRepeat() const noexcept { return this->Input.bRepeated; }

    FORCEINLINE constexpr LRawInput GetRawInput() const noexcept { return { this->Input }; }
    FORCEINLINE constexpr LKey GetKey() const noexcept { return this->Input.Key; }
    FORCEINLINE constexpr f32 GetValue() const noexcept { return this->Input.Value; }

    FORCEINLINE LString ToString() const { return this->Input.ToString() ;}

private:

    LRawInput Input;
};

} /* ~Namespace Jafg */
