// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "LocalEgoSubsystem.generated.h"

namespace Jafg
{

class LLocalEgo;
class APersonaController;
class APawn;

//#
//# A subsystem that shares its lifetime with the local ego.
//# @see User/LocalEgo.h
//#
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class JLocalEgoSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JLocalEgoSubsystem)

public:

    //#
    //# Called when the local ego possesses a new controller. Both pointers may be null.
    //#
    virtual void OnNewPersonaControllerPossessed(APersonaController* InOld, APersonaController* InNew) { }

    //#
    //# Called when the current controller possesses a new pawn. Both pointers may be null.
    //#
    virtual void OnNewPawnPossessed(APawn* InOld, APawn* InNew) { }
};

} /* ~Namespace Jafg */
