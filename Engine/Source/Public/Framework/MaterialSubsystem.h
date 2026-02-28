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
    // BEGIN Only for unique layouts.

    //# @param FetchedMaterial Optional field if the caller already has the fetched material at hand.
    struct LBinding{ vk::DescriptorType Type; u32 Layout; u32 Set; };
    LBinding GetBinding(LMaterialInstance& Instance, LStringView Key);

    void SetMaterialInstanceField(LMaterialInstance& Instance, LBinding Where, LString const& Value) const noexcept;
    void SetSampler(LMaterialInstance& Instance, LBinding Where, vk::Sampler const& Sampler) const;
    void SetSampledImage(LMaterialInstance& Instance, LBinding Where, LTexture2 const& Texture) const;

    // END Only for unique layouts.
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto const& GetFetchedMaterials() const noexcept { return this->FetchedMaterials; }
    FORCEINLINE auto const& GetMaterials() const noexcept { return this->Materials; }

    vk::PipelineColorBlendStateCreateInfo const& GetSolidColorBlending() const noexcept
    {
        static vk::PipelineColorBlendAttachmentState State{
            .blendEnable = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                            | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
            };

        static vk::PipelineColorBlendStateCreateInfo Info{
            .logicOpEnable = vk::False,
            .attachmentCount = 1,
            .pAttachments = &State
            };

        return Info;
    }


private:

    JTextureSubsystem* TextureSubsystem{};
    JShaderSubsystem* ShaderSubsystem{};

    TArray<std::unique_ptr<LFetchedMaterial>> FetchedMaterials;
    std::unordered_map<LString, std::shared_ptr<LMaterial>> Materials;
};

} /* ~Namespace Jafg */
