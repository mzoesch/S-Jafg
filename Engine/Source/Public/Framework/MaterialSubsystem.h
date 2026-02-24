// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "Rhi/Material.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JMaterialSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JMaterialSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    void ReloadMaterials();

    FORCEINLINE TArray<LMaterial> const& GetMaterials() const noexcept { return this->Materials; }

private:

    TArray<LMaterial> Materials;
};

} /* ~Namespace Jafg */
