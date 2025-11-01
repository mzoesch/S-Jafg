// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class APawn;

//# TODO: Remove this class and integrate its functionality into APawn directly.
class LEye final
{
public:

    LEye() noexcept = default;
    PROHIBIT_ANY_REALLOC_OTHER_THAN_CDR(LEye) noexceptcheck
    {
        check( CDR.OwningPawn == nullptr )

        this->DegYFov = CDR.DegYFov;
        this->NearFrustum = CDR.NearFrustum;
        this->FarFrustum  = CDR.FarFrustum;

        return;
    }
    ~LEye() noexcept = default;

    FORCEINLINE bool IsOwningPawnValid() const noexcept { return this->OwningPawn != nullptr; }
    FORCEINLINE APawn* GetOwningPawn() noexcept { return this->OwningPawn; }
    FORCEINLINE APawn const* GetOwningPawn() const noexcept { return this->OwningPawn; }
    FORCEINLINE APawn* GetOwningPawnChecked() noexceptcheck { check( this->IsOwningPawnValid() ) return this->OwningPawn; }
    FORCEINLINE APawn const* GetOwningPawnChecked() const noexceptcheck { check( this->IsOwningPawnValid() ) return this->OwningPawn; }
    FORCEINLINE APawn* GetOwningPawnAsserted() noexceptcheck { jassert( this->IsOwningPawnValid() ) return this->OwningPawn; }
    FORCEINLINE APawn const* GetOwningPawnAsserted() const noexceptcheck { jassert( this->IsOwningPawnValid() ) return this->OwningPawn; }
    void SetOwningPawn(APawn* InOwner) noexcept;

    void UpdateViewMatrix();
    FORCEINLINE const LMatrix& GetViewMatrix() const noexcept { return this->CachedViewMatrix; }
    FORCEINLINE const LVector& GetRelativeFront() const noexcept { return this->RelativeFront; }
    FORCEINLINE const LVector& GetRelativeRight() const noexcept { return this->RelativeRight; }
    FORCEINLINE const LVector& GetRelativeUp() const noexcept { return this->RelativeUp; }

    FORCEINLINE f32  GetDegYFov() const noexcept { return this->DegYFov; }
    FORCEINLINE void SetDegYFov(const f32 InDegYFov) noexcept { this->DegYFov = InDegYFov; }

    FORCEINLINE f32  GetNearFrustum() const noexcept { return this->NearFrustum; }
    FORCEINLINE void SetNearFrustum(const f32 InNearFrustum) noexcept { this->NearFrustum = InNearFrustum; }
    FORCEINLINE f32  GetFarFrustum() const noexcept { return this->FarFrustum; }
    FORCEINLINE void SetFarFrustum(const f32 InFarFrustum) noexcept { this->FarFrustum = InFarFrustum; }

private:

    APawn* OwningPawn{ nullptr };

    void UpdateRelativeVectors() const;
    LMatrix CachedViewMatrix;
    mutable LVector RelativeFront;
    mutable LVector RelativeRight;
    mutable LVector RelativeUp;

    f32 DegYFov{ 90.0f };

    f32 NearFrustum{ 0.1f };
    f32 FarFrustum{ 1.0f };
};

} /* ~Namespace Jafg */
