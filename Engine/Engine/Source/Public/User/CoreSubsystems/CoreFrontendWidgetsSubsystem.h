// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "CoreFrontendWidgetsSubsystem.generated.h"

namespace Jafg
{

class WFrontendScreen;

DECLARE_JAFG_CLASS()
class ENGINE_API JCoreFrontendWidgetsSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreFrontendWidgetsSubsystem)

    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    WFrontendScreen* FrontendScreen { nullptr };
};

} /* ~Namespace Jafg */
