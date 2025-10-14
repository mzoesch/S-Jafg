// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "User/Input/UserInput.h"
#include "Framework/Frontend.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Cli/CliHandles.h"
#include "Cli/CommandLineInterface.h"

namespace Jafg
{

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
    ~LLocalEgo() noexcept = default;

    void Initialize();
    void Tick(const f32 DeltaTime);
    void OnLateTick(const f32 DeltaTime);
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

    FORCEINLINE LClassOuter* GetOuter() noexcept { return &this->Outer; }
    FORCEINLINE const LClassOuter* GetOuter() const noexcept { return &this->Outer; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JLocalEgoSubsystem)

    void OnNewPawnPossessed(APawn* InOld, APawn* InNew);

    ENGINE_API LEngine* GetEngine();
    ENGINE_API LCommandLineInterface* GetCommandLineInterface();

    FORCEINLINE bool GetVariable_UpdateFrustum() const { bool bOut = false; this->VariableHandle_UpdateFrustum.GetValue(&bOut); return bOut; }
    FORCEINLINE auto GetVariableHandle_UpdateFrustum() const -> const LCliVariableHandle& { return this->VariableHandle_UpdateFrustum; }
    FORCEINLINE bool GetVariable_VisualizeFrustum() const { bool bOut = false; this->VariableHandle_VisualizeFrustum.GetValue(&bOut); return bOut; }
    FORCEINLINE auto GetVariableHandle_VisualizeFrustum() const -> const LCliVariableHandle& { return this->VariableHandle_VisualizeFrustum; }
    FORCEINLINE f32  GetVariable_FrustumNearPlane() const { f32 Out = 0.0f; this->VariableHandle_FrustumNearPlane.GetValue(&Out); return Out; }
    FORCEINLINE auto GetVariableHandle_FrustumNearPlane() const -> const LCliVariableHandle& { return this->VariableHandle_FrustumNearPlane; }
    FORCEINLINE f32  GetVariable_FrustumFarPlane() const { f32 Out = 0.0f; this->VariableHandle_FrustumFarPlane.GetValue(&Out); return Out; }
    FORCEINLINE auto GetVariableHandle_FrustumFarPlane() const -> const LCliVariableHandle& { return this->VariableHandle_FrustumFarPlane; }

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
    LClassOuter Outer{"LocalEgo"};
    LSubsystemCollection Collection{ "LocalEgo" };

    LCliVariableHandle VariableHandle_UpdateFrustum;
    LCliVariableHandle VariableHandle_VisualizeFrustum;
    LCliVariableHandle VariableHandle_FrustumNearPlane;
    LCliVariableHandle VariableHandle_FrustumFarPlane;

    LCliVariableHandle VariableHandle_VerifyChunks;
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
