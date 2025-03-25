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

    FORCEINLINE bool IsOwnerValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE void SetOwner(APawn* InOwner) noexcept { this->Owner = InOwner; }
    FORCEINLINE auto GetOwner() const noexcept -> APawn* { return this->Owner; }

    void UpdateViewMatrix();
    FORCEINLINE const LMatrix& GetViewMatrix() const noexcept { return this->CachedViewMatrix; }
    FORCEINLINE const LVector& GetRelativeFront() const noexcept { return this->RelativeFront; }
    FORCEINLINE const LVector& GetRelativeRight() const noexcept { return this->RelativeRight; }
    FORCEINLINE const LVector& GetRelativeUp() const noexcept { return this->RelativeUp; }

    FORCEINLINE auto GetDegYFov() const noexcept -> float { return this->DegYFov; }
    FORCEINLINE void SetDegYFov(const float InDegYFov) { this->DegYFov = InDegYFov; }

private:

    APawn*  Owner = nullptr;
    LMatrix CachedViewMatrix = LMatrix::Identity;

    void UpdateRelativeVectors() const;
    mutable LVector RelativeFront = LVector::ForwardVector;
    mutable LVector RelativeRight = LVector::RightVector;
    mutable LVector RelativeUp    = LVector::UpVector;

    float DegYFov = 70.0f;
};

} /* ~Namespace Jafg */
