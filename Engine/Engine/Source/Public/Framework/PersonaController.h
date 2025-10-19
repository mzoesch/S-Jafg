// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Controller.h"
#include "Platform/SurfaceForward.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LLocalEgo;
class LEye;
class APawn;

DECLARE_JAFG_CLASS()
class APersonaController final : public AController
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APersonaController)

public:

    virtual void EndLife() override;

    FORCEINLINE bool IsSurfaceValid() const noexcept { return this->Surface != nullptr; }
    FORCEINLINE LSurface* GetSurface() noexcept { return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceChecked() noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface* GetSurfaceAsserted() noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurface() const noexcept { return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceChecked() const noexceptcheck { check( this->Surface ); return this->Surface; }
    FORCEINLINE LSurface const* GetSurfaceAsserted() const noexceptcheck { jassert( this->Surface ); return this->Surface; }
    FORCEINLINE void SetSurface(LSurface* InSurface) noexcept { this->Surface = InSurface; }

    FORCEINLINE bool DoesPossess() const noexcept { return this->Pawn != nullptr; }
    FORCEINLINE APawn* GetPossessed() noexcept { return this->Pawn; }
    FORCEINLINE APawn* GetPossessedChecked() noexceptcheck { check( this->DoesPossess() ) return this->Pawn; }
    FORCEINLINE APawn* GetPossessedAsserted() noexceptcheck { jassert( this->DoesPossess() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPossessed() const noexcept { return this->Pawn; }
    FORCEINLINE APawn const* GetPossessedChecked() const noexceptcheck { check( this->DoesPossess() ) return this->Pawn; }
    FORCEINLINE APawn const* GetPossessedAsserted() const noexceptcheck { jassert( this->DoesPossess() ) return this->Pawn; }
    ENGINE_API  void Possess(APawn* NewPawn, const bool bKillOld = true);

private:

    LSurface* Surface{ nullptr };
    LEye* EyeReference{ nullptr };
    APawn* Pawn{ nullptr };
};

} /* ~Namespace Jafg */
