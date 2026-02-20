// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Platform/SurfaceForward.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LLocalEgo;
class LEye;
class APawn;
class LUserInput;

DECLARE_JAFG_CLASS()
class ENGINE_API APersonaController : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(APersonaController)

public:

    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override;

    FORCEINLINE constexpr bool IsLocallyPossessed() const noexcept { return this->IsOwningSurfaceValid(); }

    FORCEINLINE constexpr bool IsOwningSurfaceValid() const noexcept { return this->Surface != nullptr; }
    FORCEINLINE LSurface* GetOwningSurface() noexcept { return this->Surface; }
    FORCEINLINE LSurface const* GetOwningSurface() const noexcept { return this->Surface; }
    FORCEINLINE LSurface* GetOwningSurfaceChecked() noexcept{ check(this->Surface) return this->Surface; }
    FORCEINLINE LSurface const* GetOwningSurfaceChecked() const noexcept { check(this->Surface) return this->Surface; }
    FORCEINLINE LSurface* GetOwningSurfaceAsserted() noexcept { jassert(this->Surface) return this->Surface; }
    FORCEINLINE LSurface const* GetOwningSurfaceAsserted() const noexcept { jassert(this->Surface) return this->Surface; }
    //# Jafg internal method. Do not use. @see #LSurface::PossessController
    FORCEINLINE void _SetOwningSurface(LSurface* InSurface) noexcept { this->Surface = InSurface; }

    FORCEINLINE bool IsOwnedPawnValid() const noexcept { return this->Pawn != nullptr; }
    FORCEINLINE APawn* GetOwnedPawn() noexcept { return this->Pawn; }
    FORCEINLINE APawn const* GetOwnedPawn() const noexcept { return this->Pawn; }
    FORCEINLINE APawn* GetOwnedPawnChecked() noexcept { check(this->IsOwnedPawnValid()) return this->Pawn; }
    FORCEINLINE APawn const* GetOwnedPawnChecked() const noexcept { check(this->IsOwnedPawnValid()) return this->Pawn; }
    FORCEINLINE APawn* GetOwnedPawnAsserted() noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn; }
    FORCEINLINE APawn const* GetOwnedPawnAsserted() const noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn; }
    void PossessPawn(APawn* New, const bool bKillOld = true);

private:

    LSurface* Surface{};
    APawn* Pawn{};
};

} /* ~Namespace Jafg */
