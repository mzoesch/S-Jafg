// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Frontend.h"
#include "Framework/SubsystemCollection.h"
#include "Framework/LocalEgoSubsystem.h"
#include "Cli/CliHandles.h"
#include "Cli/CommandLineInterface.h"
#include "User/Input/UserInputRegistry.h"

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
    void Tick(f32 Dt)
    {
        if (!this->Frontend.IsHeadless())
        {
            this->Frontend.Tick();
        }
    }
    void TearDown();

    FORCEINLINE LFrontend& GetFrontend() noexcept { return this->Frontend; }
    FORCEINLINE LFrontend const& GetFrontend() const noexcept { return this->Frontend; }

    FORCEINLINE LUserInputRegistry& GetUserInputRegistry() noexcept { return this->UserInputRegistry; }
    FORCEINLINE LUserInputRegistry const& GetUserInputRegistry() const noexcept { return this->UserInputRegistry; }

    FORCEINLINE LClassOuter* GetOuter() noexcept { return &this->Outer; }
    FORCEINLINE const LClassOuter* GetOuter() const noexcept { return &this->Outer; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JLocalEgoSubsystem)

    ENGINE_API LEngine const& GetEngine() const;
    ENGINE_API LEngine& GetMutableEngine();
    ENGINE_API LCommandLineInterface const& GetCommandLineInterface() const;
    ENGINE_API LCommandLineInterface& GetMutableCommandLineInterface();

    FORCEINLINE bool GetVariable_UpdateFrustum() const { bool bOut = false; this->VariableHandle_UpdateFrustum.GetValue(&bOut); return bOut; }
    FORCEINLINE auto GetVariableHandle_UpdateFrustum() const -> const LCliVariableHandle& { return this->VariableHandle_UpdateFrustum; }
    FORCEINLINE bool GetVariable_VisualizeFrustum() const { bool bOut = false; this->VariableHandle_VisualizeFrustum.GetValue(&bOut); return bOut; }
    FORCEINLINE auto GetVariableHandle_VisualizeFrustum() const -> const LCliVariableHandle& { return this->VariableHandle_VisualizeFrustum; }
    FORCEINLINE f32  GetVariable_FrustumNearPlane() const { f32 Out = 0.0f; this->VariableHandle_FrustumNearPlane.GetValue(&Out); return Out; }
    FORCEINLINE auto GetVariableHandle_FrustumNearPlane() const -> const LCliVariableHandle& { return this->VariableHandle_FrustumNearPlane; }
    FORCEINLINE f32  GetVariable_FrustumFarPlane() const { f32 Out = 0.0f; this->VariableHandle_FrustumFarPlane.GetValue(&Out); return Out; }
    FORCEINLINE auto GetVariableHandle_FrustumFarPlane() const -> const LCliVariableHandle& { return this->VariableHandle_FrustumFarPlane; }

#if JAFG_DO_CHECKS
    FORCEINLINE bool IsDecommissioned() const noexcept { return this->bDecommissioned; }
#endif /* JAFG_DO_CHECKS */

private:

#if JAFG_DO_CHECKS
    bool bDecommissioned{};
#endif /* JAFG_DO_CHECKS */

    LFrontend Frontend;
    LUserInputRegistry UserInputRegistry;

    //#
    //# The context of the local ego. It is created when the local ego is instantiated
    //# and not destroyed until the local ego is killed.
    //#
    LClassOuter Outer{ "LocalEgo" };
    LSubsystemCollection Collection{ "LocalEgo" };

    LCliVariableHandle VariableHandle_UpdateFrustum;
    LCliVariableHandle VariableHandle_VisualizeFrustum;
    LCliVariableHandle VariableHandle_FrustumNearPlane;
    LCliVariableHandle VariableHandle_FrustumFarPlane;
};

} /* ~Namespace Jafg */
