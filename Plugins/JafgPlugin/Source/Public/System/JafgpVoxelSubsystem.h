// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "JafgpVoxelSubsystem.generated.h"

namespace Jafgp
{

DECLARE_JAFG_CLASS()
class JJafgpVoxelSubsystem final : public Jafg::JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JJafgpVoxelSubsystem)

public:

    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
    virtual void TearDown() override;
};

} /* ~Namespace Jafgp */
