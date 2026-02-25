// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/Material.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

class JShaderSubsystem;
class JTextureSubsystem;
struct LTexture2;
struct LFetchedShader;

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
    LMaterialInstanceRef GetInstanceFromMaterialName(LString const& MaterialName) { return this->GetInstance(this->GetMaterial(MaterialName)); }

    ///////////////////////////////////////////////////////////////////////////////
    // Only for unique layouts.
    //# @param FetchedMaterial Optional field if the caller already has the fetched material at hand.
    void SetMaterialInstanceField(LMaterialInstance& Instance, LString const& Key, LString const& Value, LFetchedShader const& InShader) const noexcept;
    void SetCombinedImageSampler(LMaterialInstance& Instance, LStringView Where, LTexture2 const& Texture) const;

private:

    JTextureSubsystem* TextureSubsystem{};
    JShaderSubsystem* ShaderSubsystem{};

    TArray<LFetchedMaterial> FetchedMaterials;
    std::unordered_map<LString, std::shared_ptr<LMaterial>> Materials;
};

} /* ~Namespace Jafg */
