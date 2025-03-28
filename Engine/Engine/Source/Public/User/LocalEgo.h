// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/UserInput.h"
#include "Framework/Frontend.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Cli/CommandLineInterface.h"

namespace Jafg
{

class LObjectContext;
class APawn;
class LEngine;
class LWorld;
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

    LLocalEgo() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LLocalEgo)
    ~LLocalEgo() { check( this->Context.IsValid() == false ) }

    void Initialize();
    void Tick(const float DeltaTime);
    void OnLateTick(const float DeltaTime);
    void TearDown();

    FORCEINLINE bool IsValid() const { return this->bValid; }

    FORCEINLINE auto GetFrontend() -> LFrontend* { return &this->Frontend; }
    FORCEINLINE auto GetFrontend() const -> const LFrontend* { return &this->Frontend; }
    FORCEINLINE auto GetUserInput() -> LUserInput* { return &this->UserInput; }
    FORCEINLINE auto GetUserInput() const -> const LUserInput* { return &this->UserInput; }

    FORCEINLINE bool DoesPossess() const { return this->PersonaController != nullptr; }
    FORCEINLINE auto GetPossessed() const -> APersonaController* { return this->PersonaController; }
    FORCEINLINE auto GetCheckedPossessed() const -> APersonaController* { check( this->PersonaController ) return this->PersonaController; }
    FORCEINLINE auto GetPanickedPossessed() const -> APersonaController*;
    ENGINE_API  void Possess(APersonaController* InNewController);

    FORCEINLINE auto GetContext() -> LObjectContext& { return this->Context; }
    FORCEINLINE auto GetContext() const -> const LObjectContext& { return this->Context; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JLocalEgoSubsystem)

    void OnNewPawnPossessed(APawn* InOld, APawn* InNew);

    ENGINE_API LEngine* GetEngine();
    ENGINE_API LCommandLineInterface* GetCommandLineInterface();

private:

    void OnWorldBeginLife(LWorld* InNewWorld);

    bool bValid = false;

    LFrontend  Frontend;
    LUserInput UserInput;

    LDelegateHandle OnWorldBeginLifeHandle = nullptr;

    //# Currently possessed persona controller.
    APersonaController* PersonaController = nullptr;

    //#
    //# The context of the local ego. It is created when the local ego is instantiated
    //# and not destroyed until the local ego is killed.
    //#
    LObjectContext       Context = GlobalCarnifex;
    LSubsystemCollection Collection;

    LCliVariableHandle VariableHandle_UpdateFrustum;
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
