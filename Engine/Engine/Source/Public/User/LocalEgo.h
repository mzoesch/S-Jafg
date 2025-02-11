// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LObjectContext;
class APawn;
class LHud;
class LWorld;
class LSurface;
class LUserInput;
class APersonaController;
struct LSubsystemCollection;

//#
//# Represents a from of an ego that is considered local (physically present) on the current machine where this
//# application instance is running, and it is responsible for handling input / output for it.
//# This includes inputs from physical devices and output to some kind of display device which may or may not be
//# proxied by a software surface.
//# This class is therefore never created on a dedicated server.
//#
class LLocalEgo final
{
public:

    LLocalEgo() = default;

    PROHIBIT_REALLOC_OF_ANY_FORM(LLocalEgo)

    void Initialize();
    void Tick(const float DeltaTime);
    void OnLateTick(const float DeltaTime);
    void TearDown();

    FORCEINLINE auto IsUserInputValid() const -> bool { return this->UserInput != nullptr; }
    FORCEINLINE auto GetUserInput() -> LUserInput* { return this->UserInput; }
    FORCEINLINE auto GetUserInput() const -> const LUserInput* { return this->UserInput; }
    FORCEINLINE auto IsHud() const -> bool { return this->Hud != nullptr; }
    FORCEINLINE auto GetHud() const -> LHud* { return this->Hud; }
    FORCEINLINE auto HasPrimarySurface() const -> bool { return this->SurfaceToDrawOn != nullptr; }
    FORCEINLINE auto GetPrimarySurface() const -> LSurface* { return this->SurfaceToDrawOn; }

    FORCEINLINE auto DoesPossess() const -> bool { return this->PersonaController != nullptr; }
    FORCEINLINE auto GetPossessed() const -> APersonaController* { return this->PersonaController; }
    FORCEINLINE auto GetCheckedPossessed() const -> APersonaController* { check( this->PersonaController ) return this->PersonaController; }
    FORCEINLINE auto GetPanickedPossessed() const -> APersonaController*;
                void Possess(APersonaController* InNewController);

    FORCEINLINE auto GetContext() const -> LObjectContext* { return this->Context; }

    void OnNewPawnPossessed(APawn* InOld, APawn* InNew) const;

protected:

    void OnWorldBeginLife(LWorld* InNewWorld);

private:

    LUserInput* UserInput = nullptr;
    LHud*       Hud = nullptr;
    LSurface*   SurfaceToDrawOn = nullptr;

    LDelegateHandle     OnWorldBeginLifeHandle = nullptr;
    APersonaController* PersonaController = nullptr;

    //#
    //# The context of the local ego. It is created when the local ego is instantiated
    //# and not destroyed until the local ego is killed.
    //#
    LObjectContext* Context = nullptr;
    LSubsystemCollection* Collection = nullptr;
};

} /* ~Namespace Jafg */

Jafg::APersonaController* Jafg::LLocalEgo::GetPanickedPossessed() const
{
    if (this->PersonaController)
    {
        return this->PersonaController;
    }
    panic( "No persona controller possessed by the local ego." )
    return nullptr;
}
