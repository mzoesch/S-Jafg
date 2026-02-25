// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Material.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

struct LTexture2;

DECLARE_JAFG_CLASS()
class ENGINE_API JMaterialSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JMaterialSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    void ReloadMaterials();

    //# Get a fetched material.
    LFetchedMaterial const& GetFetchedMaterial(LString const& Name) const noexcept;
    //# Get a fetched material that is allocated on the device and device ready.
    LMaterialRef GetMaterial(LString const& Name) noexcept;
    //# Get the instance of a material to specify unique data.
    LMaterialInstanceRef GetInstance(LMaterialRef Material);

    //# Only for unique layouts.
    void SetCombinedImageSampler(LMaterialInstanceRef MaterialInstance, LStringView Where, LTexture2 const& Texture) const;

private:

    TArray<LFetchedMaterial> FetchedMaterials;
    std::unordered_map<LString, std::shared_ptr<LMaterial>> Materials;
    std::unordered_map<LString, std::shared_ptr<LMaterialInstance>> MaterialInstances;
};

} /* ~Namespace Jafg */
