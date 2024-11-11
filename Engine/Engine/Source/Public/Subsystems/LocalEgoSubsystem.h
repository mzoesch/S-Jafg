// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "LocalEgoSubsystem.generated.h"

namespace Jafg
{

class LLocalEgo;
class APersonaController;

/**
 * A subsystem that shares its lifetime with the local ego.
 * @see User/LocalEgo.h
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API JLocalEgoSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JLocalEgoSubsystem)

public:

    /**
     * Called when the local ego possesses a new controller. Both pointers may be null.
     */
    virtual void OnNewPersonaControllerPossessed(APersonaController* InOld, APersonaController* InNew) { }

    LLocalEgo* GetLocalEgo() const;
};

} /* ~Namespace Jafg */
