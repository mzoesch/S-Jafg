// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Framework/Lackey.h"
#include "SupremePolicies.generated.h"

namespace Jafg
{

class APawn;
class APawnStart;
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
    enum EConnectionRequest
    {
        Local,
#if JAFG_WITH_EDITOR
        Editor,
#endif /* JAFG_WITH_EDITOR */
        Proxy,
    };
    virtual std::expected<TJxxUnique<APersonaController>, LString> OnIncomingConnectionRequest(EConnectionRequest Req);

    //#
    //# Called after a persona controller has been created.
    //#
    virtual void OnPersonaControllerCreated(APersonaController& Pc);

    //#
    //# Called whenever a pawn is spawned/used for a persona controller.
    //# @return A valid pointer to a possessable pawn.
    //#
    virtual TJxxUnique<APawn> GetPawnForPersonaController(APersonaController const& Pc);
    //# @return The start for the pawn, or nullptr for default behavior.
    virtual APawnStart const* GetPawnStartForPawn(APawn const& Pawn);

    //# Whether a pawn should be automatically created for a new persona controller.
    bool bCreatePawn{ true };
    //# Whether pawn starts should be ignored when spawning pawns for persona controllers.
    bool bIgnorePawnStarts{ false };

    //# The default class for all persona controllers unless overridden by #OnIncomingConnectionRequest.
    TSubclassOf<APersonaController> PersonaControllerClass;
#if JAFG_WITH_EDITOR
    TSubclassOf<APersonaController> EditorPersonaControllerClass;
#endif /* JAFG_WITH_EDITOR */
    //# The default class for all spawned pawns unless overridden by #GetPawnForPersonaController.
    TSubclassOf<APawn> DefaultPawnClass;
#if JAFG_WITH_EDITOR
    TSubclassOf<APawn> EditorDefaultPawnClass;
#endif /* JAFG_WITH_EDITOR */
};

} /* ~Namespace Jafg */
