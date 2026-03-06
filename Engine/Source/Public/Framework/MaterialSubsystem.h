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
    virtual void TearDown() override
    {
        this->PurgeUnused();
        Super::TearDown();
    }

    void ReloadMaterials();
    void PurgeUnused();

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
    FORCEINLINE auto const& GetSharedMaterialInstances() const noexcept { return this->MaterialInstances; }
    FORCEINLINE void RegisterSharedMaterialInstance(LString const& Identifier, LMaterialInstanceRef Instance) noexcept
    {
        if (this->MaterialInstances.contains(Identifier) == false)
        {
            this->MaterialInstances.emplace(Identifier, Instance);
        }
    }

    vk::PipelineColorBlendStateCreateInfo const& GetSolidColorBlending() const noexcept
    {
        static vk::PipelineColorBlendAttachmentState State{
            .blendEnable = vk::False,
            .srcColorBlendFactor = vk::BlendFactor::eZero,
            .dstColorBlendFactor = vk::BlendFactor::eZero,
            .colorBlendOp = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eZero,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp = vk::BlendOp::eAdd,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                            | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
            };
        static vk::PipelineColorBlendStateCreateInfo Info{
            .logicOpEnable = vk::False,
            .logicOp = vk::LogicOp::eClear,
            .attachmentCount = 1,
            .pAttachments = &State
            };
        return Info;
    }
    vk::PipelineColorBlendStateCreateInfo const& GetTranslucentBlending() const noexcept
    {
        static vk::PipelineColorBlendAttachmentState State{
            .blendEnable = vk::True,
            .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
            .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
            .colorBlendOp = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp = vk::BlendOp::eAdd,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                            | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
            };
        static vk::PipelineColorBlendStateCreateInfo Info{
            .logicOpEnable = vk::False,
            .logicOp = vk::LogicOp::eClear,
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
    std::unordered_map<LString, std::shared_ptr<LMaterialInstance>> MaterialInstances;
};

} /* ~Namespace Jafg */
