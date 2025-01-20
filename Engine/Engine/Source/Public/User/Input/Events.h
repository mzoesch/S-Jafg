// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/input/InputTypes.h"
#include "User/Input/RawInput.h"

namespace Jafg
{

struct LKeyEvent final
{
    LKeyEvent() = default;
    LKeyEvent(const LKey InKey) { this->Input = LRawInput(InKey); }
    LKeyEvent(const LKey InKey, const float InValue) { this->Input = LRawInput(InKey, InValue); }
    LKeyEvent(const LRawInput& InRawInput) { this->Input = InRawInput; }
    LKeyEvent(const LKey InKey, const bool bIsPlatformRepeat) { this->Input = LRawInput(InKey); this->bIsPlatformRepeat = bIsPlatformRepeat; }
    LKeyEvent(const LKey InKey, const float InValue, const bool bIsPlatformRepeat) { this->Input = LRawInput(InKey, InValue); this->bIsPlatformRepeat = bIsPlatformRepeat; }
    LKeyEvent(const LRawInput& InRawInput, const bool bIsPlatformRepeat) { this->Input = InRawInput; this->bIsPlatformRepeat = bIsPlatformRepeat; }

    //#
    //# Whether this key down event was triggered by a platform repeat. Meaning the repeat delay set on the platform
    //# directly by the user - based on the actual physical keyboard settings.
    //#
    FORCEINLINE auto IsPlatformRepeat() const -> bool { return this->bIsPlatformRepeat; }

    FORCEINLINE auto GetRawInput() const -> LRawInput { return { this->Input }; }
    FORCEINLINE auto GetKey() const -> LKey { return this->Input.Key; }
    FORCEINLINE auto GetValue() const -> float { return this->Input.Value; }

private:

    bool bIsPlatformRepeat = false;
    LRawInput Input;
};

} /* ~Namespace Jafg */
