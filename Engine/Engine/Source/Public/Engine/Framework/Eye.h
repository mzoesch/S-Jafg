// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class APawn;

class ENGINE_API LEye final
{
public:

    LEye() = delete;
    FORCEINLINE explicit LEye(APawn* InOwningPawn) : OwningPawn(InOwningPawn) { check( this->OwningPawn ) }
    PROHIBIT_REALLOC_OF_ANY_FORM(LEye)
    ~LEye() = default;

    FORCEINLINE auto HasOwningPawn() const -> bool { return this->OwningPawn != nullptr; }
    FORCEINLINE auto GetOwningPawn() const -> APawn* { return this->OwningPawn; }

    LMatrix GetViewMatrix() const;
    FORCEINLINE auto GetRelativeFront() const -> const LVector& { return this->RelativeFront; }
    FORCEINLINE auto GetRelativeRight() const -> const LVector& { return this->RelativeRight; }
    FORCEINLINE auto GetRelativeUp() const -> const LVector& { return this->RelativeUp; }

    FORCEINLINE auto GetDegYFov() const -> float { return this->DegYFov; }
    FORCEINLINE void SetDegYFov(const float InDegYFov) { this->DegYFov = InDegYFov; }

private:

    APawn* OwningPawn = nullptr;

    void UpdateRelativeVectors() const;
    mutable LVector RelativeFront = LVector::ForwardVector;
    mutable LVector RelativeRight = LVector::RightVector;
    mutable LVector RelativeUp    = LVector::UpVector;

    float DegYFov            = 70.0f;
};

} /* ~Namespace Jafg */
