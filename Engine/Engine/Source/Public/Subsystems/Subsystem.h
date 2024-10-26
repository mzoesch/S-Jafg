// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JSubsystem : public JObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSubsystem)

public:


    virtual void Initialize(LSubsystemCollection& Collection) { }
    virtual void TearDown() { }
};

} /* Namespace Jafg */
