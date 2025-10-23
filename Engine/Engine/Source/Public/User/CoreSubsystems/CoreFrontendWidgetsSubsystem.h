// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Subsystems/WorldSubsystem.h"
#include "CoreFrontendWidgetsSubsystem.generated.h"

namespace Jafg
{

class WFrontendScreen;

DECLARE_JAFG_CLASS()
class JCoreFrontendWidgetsSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCoreFrontendWidgetsSubsystem)

    virtual bool ShouldCreateSubsystem(LClassOuter const* Outer) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;

private:

    WFrontendScreen* FrontendScreen { nullptr };
};

} /* ~Namespace Jafg */
