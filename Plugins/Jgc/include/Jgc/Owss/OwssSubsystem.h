// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldSubsystem.h"
#include "OwssSubsystem.generated.h"

namespace Jgc
{

//#
//# A subsystem that is only created in an owss world.
//#
DECLARE_JAFG_CLASS()
class JGC_API JOwssSubsystem : public Jafg::JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JOwssSubsystem)

    virtual bool ShouldCreateSubsystem() const noexcept override;
};

} /* ~Namespace Jgc */
