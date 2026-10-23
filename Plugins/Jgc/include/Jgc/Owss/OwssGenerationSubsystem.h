// Copyright mzoesch. All rights reserved.

#pragma once

#include "Jgc/Owss/OwssSubsystem.h"
#include "Rhi/Material.h"
#include "OwssGenerationSubsystem.generated.h"

namespace Jgc
{

class AVehicleComponent;

DECLARE_JAFG_CLASS()
class JGC_API JOwssGenerationSubsystem : public Jafg::JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JOwssGenerationSubsystem)

    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;

public:

    virtual void GenerateAsteroids(LWorldVec3 Where);
    virtual AVehicleComponent& SpawnShip(LWorldVec3 Where);

private:

    Jafg::LMaterialInstanceRef MaterialInstance;

    bool bGeneratedAsteroids{};
};

} /* ~Namespace Jgc */
