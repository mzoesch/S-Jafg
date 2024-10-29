// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/Subsystem.h"
#include "HudSubsystem.generated.h"

namespace Jafg
{

/**
 * A subsystem that shares its lifetime with the local player hud.
 * Important delegates will be broadcasted to all children of this subsystem.
 * Widget construction and destruction should be done here.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class JHudSubsystem : public JSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JHudSubsystem)

public:

    virtual bool ShouldCreateSubsystem(const Private::LObjectContext* InOuter) const override
    {
        return false;
    }

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void Tick(const float DeltaTime);
    virtual void TearDown() override;
};

} /* ~Namespace Jafg. */
