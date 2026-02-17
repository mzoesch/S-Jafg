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

    FORCEINLINE bool IsLocallyPossessed() const noexcept { return this->IsSurfaceValid(); }

    FORCEINLINE bool IsSurfaceValid() const noexcept { return this->Surface != nullptr; }
    FORCEINLINE LSurface* GetSurface() noexcept { return this->Surface; }
    FORCEINLINE LSurface const* GetSurface() const noexcept { return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceChecked() noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceChecked() const noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceAsserted() noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceAsserted() const noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE void SetSurface(LSurface* InSurface) noexcept { this->Surface = InSurface; }

    FORCEINLINE bool IsPawnValid() const noexcept { return this->Pawn != nullptr; }
    FORCEINLINE APawn* GetPawn() noexcept { return this->Pawn; }
    FORCEINLINE APawn const* GetPawn() const noexcept { return this->Pawn; }
    FORCEINLINE APawn* GetPawnChecked() noexceptcheck { check( this->IsPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPawnChecked() const noexceptcheck { check( this->IsPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn* GetPawnAsserted() noexceptcheck { jassert( this->IsPawnValid() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPawnAsserted() const noexceptcheck { jassert( this->IsPawnValid() ) return this->Pawn; }
    void PossessPawn(APawn* New, const bool bKillOld = true);

private:

    LSurface* Surface{};
    APawn* Pawn{};
};

} /* ~Namespace Jafg */
