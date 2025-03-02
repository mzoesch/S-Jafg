// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class APawn;

class ENGINE_API LEye final
{
public:

    LEye() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LEye)
    ~LEye() noexcept = default;

    FORCEINLINE auto IsOwnerValid() const -> bool { return this->Owner != nullptr; }
    FORCEINLINE void SetOwner(APawn* InOwner) { this->Owner = InOwner; }
    FORCEINLINE auto GetOwner() const -> APawn* { return this->Owner; }

    LMatrix GetViewMatrix() const;
    FORCEINLINE auto GetRelativeFront() const -> const LVector& { return this->RelativeFront; }
    FORCEINLINE auto GetRelativeRight() const -> const LVector& { return this->RelativeRight; }
    FORCEINLINE auto GetRelativeUp() const -> const LVector& { return this->RelativeUp; }

    FORCEINLINE auto GetDegYFov() const -> float { return this->DegYFov; }
    FORCEINLINE void SetDegYFov(const float InDegYFov) { this->DegYFov = InDegYFov; }

private:

    APawn* Owner = nullptr;

    void UpdateRelativeVectors() const;
    mutable LVector RelativeFront = LVector::ForwardVector;
    mutable LVector RelativeRight = LVector::RightVector;
    mutable LVector RelativeUp    = LVector::UpVector;

    float DegYFov = 70.0f;
};

} /* ~Namespace Jafg */
