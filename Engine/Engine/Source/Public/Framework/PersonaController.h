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

    DEFAULT_OBJECT_CONSTRUCTOR(APersonaController)

public:

    virtual void EndLife() override;

    FORCEINLINE bool IsLocallyPossessed() const noexcept { return this->IsSurfaceValid(); }

    FORCEINLINE bool IsSurfaceValid() const noexcept { return this->Surface != nullptr; }
    FORCEINLINE LSurface* GetSurface() noexcept { return this->Surface; }
    FORCEINLINE LSurface const* GetSurface() const noexcept { return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceChecked() noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceChecked() const noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceAsserted() noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceAsserted() const noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE void SetSurface(LSurface* InSurface) noexcept { this->Surface = InSurface; }

    FORCEINLINE bool IsPossessedPawnValid() const noexcept { return this->Pawn != nullptr; }
    FORCEINLINE APawn* GetPossessedPawn() noexcept { return this->Pawn; }
    FORCEINLINE APawn const* GetPossessedPawn() const noexcept { return this->Pawn; }
    FORCEINLINE APawn* GetPossessedPawnChecked() noexceptcheck { check( this->IsPossessedPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPossessedPawnChecked() const noexceptcheck { check( this->IsPossessedPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn* GetPossessedPawnAsserted() noexceptcheck { jassert( this->IsPossessedPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPossessedPawnAsserted() const noexceptcheck { jassert( this->IsPossessedPawnValid() ) return this->Pawn; }
    void PossessPawn(APawn* New, const bool bKillOld = true);

private:

    LSurface* Surface{ nullptr };
    APawn* Pawn{ nullptr };
};

} /* ~Namespace Jafg */
