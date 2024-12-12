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
    LKeyEvent(const LRawInput& InRawInput) { this->Input = InRawInput; }

    FORCEINLINE auto GetRawInput() const -> LRawInput { return { this->Input }; }
    FORCEINLINE auto GetKey() const -> LKey { return this->Input.Key; }
    FORCEINLINE auto GetValue() const -> float { return this->Input.Value; }

private:

    LRawInput Input;
};

} /* ~Namespace Jafg */
