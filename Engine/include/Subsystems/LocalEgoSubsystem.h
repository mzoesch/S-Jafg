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
DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class JLocalEgoSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JLocalEgoSubsystem)

public:

    //#
    //# Called when the local ego changed their PC.
    //# @note If #New is null, then the local ego does not possess.
    //#
    virtual void OnNewPersonaController(APersonaController* New) {}
};

} /* ~Namespace Jafg */
