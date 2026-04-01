// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "SupremePolicies.generated.h"

namespace Jafg
{

class APawn;
class APersonaController;

DECLARE_JAFG_CLASS()
class ENGINE_API ASupremePolicies : public AWorldObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ASupremePolicies)

public:

    //#
    //# The first hook to customize world initialization.
    //# AActors added in this phase have to be deferred as they will be automatically finalized later.
    //# This hook run before the #LTrack::OnWorldPreInit.
    //#
    virtual void OnWorldPreInit() {}

    //#
    //# Called late in the world initialization phase, after all subsystems have been initialized.
    //# But still before #LTrack::OnWorldPostInit.
    //#
    // virtual void OnWorldLateInit() {}

    //#
    //# The last hook to customize world initialization.
    //#
    virtual void OnWorldPostInit() {}

    //#
    //# Called when an incoming connection request is received.
    //# TODO: Add some more context information here when we build the networking stack.
    //#
    //# @return The persona controller to use for this connection. If nullptr is returned, the connection is rejected.
    //#         The persona controller must be deferred.
    //#
    virtual TJxxUnique<APersonaController> OnIncomingConnectionRequest(EIncomingConnectionRequest IncomingConnection, LString* OutRejectionReason = nullptr);

    //#
    //# Called after a persona controller has been created.
    //#
    virtual void OnPersonaControllerCreated(APersonaController& Pc);

    //#
    //# Called whenever a pawn is spawned/used for a persona controller.
    //# @return A valid pointer to a possessable pawn.
    //#
    virtual TJxxUnique<APawn> GetPawnForPersonaController(APersonaController const& Pc);

    //# Whether a pawn should be automatically created for a new persona controller.
    bool bCreatePawn{ true };

    //# The default class for all persona controllers unless overridden by #OnIncomingConnectionRequest.
    TSubclassOf<APersonaController> PersonaControllerClass;
    //# The default class for all spawned pawns unless overridden by #GetPawnForPersonaController.
    TSubclassOf<APawn> DefaultPawnClass;
};

} /* ~Namespace Jafg */
