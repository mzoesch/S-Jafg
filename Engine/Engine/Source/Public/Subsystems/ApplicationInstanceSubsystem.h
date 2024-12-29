// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/Subsystem.h"
#include "ApplicationInstanceSubsystem.generated.h"

namespace Jafg
{

class LApplicationInstance;

/**
 * A subsystem that shares its lifetime with the application instance.
 * @see Engine/Framework/ApplicationInstance.h
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
class ENGINE_API JApplicationInstanceSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

    friend LApplicationInstance;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JApplicationInstanceSubsystem)
};

} /* Namespace Jafg */
