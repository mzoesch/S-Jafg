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
    ENGINE_API  void SetOwner(APawn* InOwner) noexcept;

    FORCEINLINE APawn* GetOwner() const noexcept { return this->Owner; }

    void UpdateViewMatrix();
    FORCEINLINE const LMatrix& GetViewMatrix() const noexcept { return this->CachedViewMatrix; }
    FORCEINLINE const LVector& GetRelativeFront() const noexcept { return this->RelativeFront; }
    FORCEINLINE const LVector& GetRelativeRight() const noexcept { return this->RelativeRight; }
    FORCEINLINE const LVector& GetRelativeUp() const noexcept { return this->RelativeUp; }

    FORCEINLINE f32  GetDegYFov() const noexcept { return this->DegYFov; }
    FORCEINLINE void SetDegYFov(const f32 InDegYFov) { this->DegYFov = InDegYFov; }

    FORCEINLINE f32  GetNearFrustum() const noexcept { return this->NearFrustum; }
    FORCEINLINE void SetNearFrustum(const f32 InNearFrustum) { this->NearFrustum = InNearFrustum; }
    FORCEINLINE f32  GetFarFrustum() const noexcept { return this->FarFrustum; }
    FORCEINLINE void SetFarFrustum(const f32 InFarFrustum) { this->FarFrustum = InFarFrustum; }

private:

    APawn*  Owner { nullptr };
    LMatrix CachedViewMatrix;

    void UpdateRelativeVectors() const;
    mutable LVector RelativeFront { LVector::ForwardVector };
    mutable LVector RelativeRight { LVector::RightVector };
    mutable LVector RelativeUp    { LVector::UpVector };

    f32 DegYFov { 90.0f };

    f32 NearFrustum { 0.1f };
    f32 FarFrustum  { 1.0f };
};

} /* ~Namespace Jafg */
