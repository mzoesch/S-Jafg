// Copyright mzoesch. All rights reserved.

#include "Framework/Skybox.h"
#include "Framework/Frontend.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/ActorComponentForward.h"

namespace
{

struct LSkyBoxVertex: rhi::vertex_input_template<LSkyBoxVertex>
{
    NODISCARD static constexpr LStringView name() noexcept { return "Jafg::Detail::SkyboxVertex"sv; }

    LVec3F Position;

    NODISCARD static std::array<vk::VertexInputBindingDescription, 1> const& binding_descriptions() noexcept
    {
        static std::array Desc{vk::VertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(LSkyBoxVertex),
            .inputRate = vk::VertexInputRate::eVertex
            }};
        return Desc;
    }
    NODISCARD static std::array<vk::VertexInputAttributeDescription, 1> const& attribute_descriptions() noexcept
    {
        static std::array Desc{
            vk::VertexInputAttributeDescription{
                .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LSkyBoxVertex, Position)
                },
            };
        return Desc;
    }
};
static_assert(rhi::vertex_input<LSkyBoxVertex>);
Jafg::LVertexInputRegistrator<LSkyBoxVertex> _{};

constexpr std::array<LVec3F,8> SkyboxVertices{
    LVec3F{-1,-1,-1}, LVec3F{ 1,-1,-1}, LVec3F{ 1, 1,-1}, LVec3F{-1, 1,-1},
    LVec3F{-1,-1, 1}, LVec3F{ 1,-1, 1}, LVec3F{ 1, 1, 1}, LVec3F{-1, 1, 1},
    };
constexpr std::array<u16,36> SkyboxIndices{
    1,5,6, 6,2,1, // +X
    0,3,7, 7,4,0, // -X

    3,2,6, 6,7,3, // +Y
    0,4,5, 5,1,0, // -Y

    4,6,5, 6,4,7, // +Z
    0,1,2, 2,3,0, // -Z
    };

} /* ~Namespace <Anonymous> */

void Jafg::ASkybox::BeginLife()
{
    Super::BeginLife();

    auto& Frontend{this->GetMutableFrontend()};

    check(!*this->Vertices && !*this->Indices)
    this->Vertices = Frontend.Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(SkyboxVertices)::value_type) * SkyboxVertices.size()},
        .Data = SkyboxVertices.data(),
        }));
    this->Indices = Frontend.Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(SkyboxIndices)::value_type) * SkyboxIndices.size()},
        .Data = SkyboxIndices.data(),
        }));

    this->MaterialInstance = Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Skybox");
    this->MaterialInstance->Vk_SetSampler(this->GetMutableFrontend(), MaterialInstance->GetBinding("sampler")
        , Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerClampToEdge());
}

void Jafg::ASkybox::RenderSkybox(LActorRenderInfo const& Info)
{
    check(this->Texture)
    check(this->Texture->IsOnDevice())

    auto& Instance{*this->MaterialInstance};
    auto& Material{*Instance.Material};
    auto& Pipeline{Material.Pipeline};
    auto& MaterialTemplate{Material.Template};
    auto& Shader{MaterialTemplate.my_shader};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    // TODO: We want to make this prob. into a function because now it is duplicate code. @see StaticMesh.
    {
        /* Vulkan specs states at least 4. */
        std::array<vk::DescriptorSet, rhi::bound_descriptor_set_limit> DescriptorSetsToBind;
        u32 NumDescriptorSets{0};
        for (auto It{Shader.begin_space()}; It != Shader.end_space(); ++It)
        {
            checkCode
            (
                if (*It >= 4)
                {
                    LOG_FATAL(LogRhi, "Expected at most 4 descriptor sets to bind. But got [{}].", *It)
                }
            )

            if (auto* Set{Instance.Vk_FindUniqueDescriptorSet(*It, Info.Frame)}; Set)
            {
                DescriptorSetsToBind[*It] = **Set;
            }
            else
            {
                if (auto* Attribute{It->find_user_attribute("Shared", 1uz)}; Attribute != nullptr)
                {
                    if (Attribute->front() == "Jafg::UBO::WorldData"sv)
                    {
                        check(DescriptorSetsToBind[*It] == nullptr)
                        DescriptorSetsToBind[*It] = Info.WorldDataDescriptorSet;
                    }
                    else if (auto It2{Info.AdditionalSharedDescriptorSets.find(Attribute->front())}; It2 != Info.AdditionalSharedDescriptorSets.end())
                    {
                        unimplemented()
                    }
                    else
                    {
                        LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found. Unexpected shared attribute [{}]."
                            , Shader.Identifier, *It, Info.Frame, Attribute->front())
                    }
                }
                else
                {
                    LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found."
                        , Shader.Identifier, *It, Info.Frame)
                }
            }

            NumDescriptorSets = maths::max(NumDescriptorSets, *It + 1);
        }

        checkCode
        (
            for (vk::DescriptorSet const& SetToBind: DescriptorSetsToBind | algo::views::take(NumDescriptorSets))
            {
                check(!!SetToBind)
            }
        )

        if (NumDescriptorSets > 0)
        {
            Info.CommandBuffer.bindDescriptorSets2({
                /* TODO: Is this correct? The sets are vertex && fragment respectively -- not vertex | fragment. */
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .layout = *Pipeline.pipeline_layout,
                .firstSet = 0,
                .descriptorSetCount = NumDescriptorSets,
                .pDescriptorSets = DescriptorSetsToBind.data(),
                .dynamicOffsetCount = 0,
                .pDynamicOffsets = nullptr
                });
        }
    }

    Info.CommandBuffer.bindVertexBuffers(0, *this->Vertices, {0});
    Info.CommandBuffer.bindIndexBuffer(*this->Indices, 0, vk::IndexTypeValue<u16>::value);
    Info.CommandBuffer.drawIndexed(static_cast<u32>(SkyboxIndices.size()), 1, 0, 0, 0);
}

std::shared_ptr<Jafg::LTextureCube2> Jafg::ASkybox::SetSkyboxTexture(std::shared_ptr<LTextureCube2> Texture)
{
    auto Result{std::exchange(this->Texture, std::move(Texture))};

    if (this->Texture)
    {
        this->MaterialInstance->Vk_SetSampledTextureCube(this->GetMutableFrontend(), MaterialInstance->GetBinding("texture_cube"), *this->Texture);
    }

    return Result;
}
