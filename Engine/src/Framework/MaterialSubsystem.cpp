// Copyright mzoesch. All rights reserved.

#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Engine/Engine.h"
#include "Framework/ShaderSubsystem.h"
#include "User/LocalEgo.h"
#include "Stats/Stats.h"
#include "Rhi/GraphicsPipelineFactory.h"
#include "Rhi/Objects.h"
#include "Rhi/VisualInstance.h"
#include "Engine/WorldData.h"

namespace
{

struct UnresolvedMaterial final
{
    LPath Path;
    LString Identifier;

    TArray<LString> Inherits;
    std::optional<LString> Shader;

    std::optional<vk::PipelineInputAssemblyStateCreateInfo> PipelineInputAssemblyState;
    std::optional<vk::PipelineRasterizationStateCreateInfo> PipelineRasterizationState;
    // std::optional<vk::PipelineMultisampleStateCreateInfo> PipelineMultisampleState;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> PipelineDepthStencilState;
    std::optional<rhi::material_template::pipeline_color_blend_state_wrapper> PipelineColorBlendState;

    TArray<rhi::material_template::property> Properties;

    NODISCARD FORCEINLINE constexpr bool IsFullyResolved() const noexcept
    {
        return this->Shader
            && this->PipelineInputAssemblyState
            && this->PipelineRasterizationState
            // && this->PipelineMultisampleState
            && this->PipelineDepthStencilState
            && this->PipelineColorBlendState
            ;
    }
};

template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
void from_json(BasicJsonType const& j, ::UnresolvedMaterial& Material)
{
    const ::UnresolvedMaterial nlohmann_json_default_obj{};
    if (j.contains("inherits"))
    {
        if (j.at("inherits").is_string())
        {
            LString Parent;
            j.at("inherits").get_to(Parent);
            Material.Inherits.emplace_back(std::move(Parent));
        }
        else
        {
            j.at("inherits").get_to(Material.Inherits);
        }
    }

#define DETAIL_ENGINE_POPULATE_MEMBER(Json, Member) \
    if (j.contains(Json)) \
    { \
        Material.Member.emplace(); \
        j.at(Json).get_to(*Material.Member); \
    }
    DETAIL_ENGINE_POPULATE_MEMBER("shader", Shader)
    DETAIL_ENGINE_POPULATE_MEMBER("pipelineInputAssemblyState", PipelineInputAssemblyState)
    DETAIL_ENGINE_POPULATE_MEMBER("pipelineRasterizationState", PipelineRasterizationState)
    // DETAIL_ENGINE_POPULATE_MEMBER("pipelineMultisampleState", PipelineMultisampleState)
    DETAIL_ENGINE_POPULATE_MEMBER("pipelineDepthStencilState", PipelineDepthStencilState)
    DETAIL_ENGINE_POPULATE_MEMBER("pipelineColorBlendState", PipelineColorBlendState)
#undef DETAIL_ENGINE_POPULATE_MEMBER

    if (j.contains("properties"))
    {
        for (auto const& [Key, Value]: j.at("properties").items())
        {
            auto& Ref{Material.Properties.emplace_back(Key)};
            Ref.value = [](auto&& Value) -> LString
            {
                if (Value.is_string())
                {
                    return Value.template get<LString>();
                }
                if (Value.is_number_float())
                {
                    return algo::sprintf("{}", Value.template get<f64>());
                }
                if (Value.is_number())
                {
                    return algo::sprintf("{}", Value.template get<i64>());
                }
                if (Value.is_boolean())
                {
                    if (auto b{Value.template get<bool>()}; b)
                    {
                        return "true";
                    }
                    return "false";
                }

                LOG_FATAL(LogSerialization, "Unsupported property value type [{}].", Value.type_name())
            }(Value);
        }
    }
}

struct LSccFinder final
{
    TArray<UnresolvedMaterial> const& Materials;

    std::size_t Index{};
    TArray<LStringView> Stack;
    std::unordered_set<LStringView> OnStack;
    std::unordered_map<LStringView, std::size_t> Indices;
    std::unordered_map<LStringView, std::size_t> LowLink;

    TArray<TArray<LStringView>> Sccs;

    explicit LSccFinder(TArray<UnresolvedMaterial> const& Materials) noexcept : Materials{Materials}
    {
        for (auto& Material: this->Materials)
        {
            if (!this->Indices.contains(Material.Identifier))
            {
                this->Connect(Material);
            }
        }
    }

    void Connect(UnresolvedMaterial const& Material) noexcept
    {
        this->Indices[Material.Identifier] = this->Index;
        this->LowLink[Material.Identifier] = this->Index;
        ++this->Index;
        this->Stack.emplace_back(Material.Identifier);
        this->OnStack.insert(Material.Identifier);

        for (auto const& Parent: Material.Inherits)
        {
            if (!this->Indices.contains(Parent))
            {
                auto It{algo::find(this->Materials, Parent, &UnresolvedMaterial::Identifier)};
                if (It == this->Materials.end())
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: No such shader.", Parent)
                }
                this->Connect(*It);
                this->LowLink[Material.Identifier] = std::min(this->LowLink[Material.Identifier], this->LowLink[Parent]);
            }
            else if (this->OnStack.contains(Parent))
            {
                if (auto It{algo::find(this->Materials, Parent, &UnresolvedMaterial::Identifier)}; It == this->Materials.end())
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: No such shader.", Parent)
                }
                this->LowLink[Material.Identifier] = std::min(this->LowLink[Material.Identifier], this->Indices[Parent]);
            }
        }

        if (this->LowLink[Material.Identifier] == this->Indices[Material.Identifier])
        {
            TArray<LStringView> Scc;
            while (true)
            {
                auto Top{this->Stack.back()};
                this->Stack.pop_back();
                this->OnStack.erase(Top);
                Scc.emplace_back(Top);
                if (Top == Material.Identifier)
                {
                    break;
                }
            }
            this->Sccs.emplace_back(std::move(Scc));
        }
    }
};

void Populate(UnresolvedMaterial* Material, UnresolvedMaterial const& From) noexcept
{
    check(Material)
#define DETAIL_ENGINE_POPULATE_MEMBER(Member) \
    if (!Material->Member) \
    { \
        Material->Member = From.Member; \
    }
    DETAIL_ENGINE_POPULATE_MEMBER(Shader)
    DETAIL_ENGINE_POPULATE_MEMBER(PipelineInputAssemblyState)
    DETAIL_ENGINE_POPULATE_MEMBER(PipelineRasterizationState)
    // DETAIL_ENGINE_POPULATE_MEMBER(PipelineMultisampleState)
    DETAIL_ENGINE_POPULATE_MEMBER(PipelineDepthStencilState)
    DETAIL_ENGINE_POPULATE_MEMBER(PipelineColorBlendState)
#undef DETAIL_ENGINE_POPULATE_MEMBER

    for (auto& Property: From.Properties)
    {
        if (!algo::contains(Material->Properties, Property.key, &rhi::material_template::property::key))
        {
            Material->Properties.emplace_back(Property);
        }
    }
    checkCode
    (
        std::unordered_set<LString> Set;
        for (auto& Property: Material->Properties)
        {
            if (!Set.insert(Property.key).second)
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Duplicate property key [{}] after populating from [{}]."
                    , Material->Path, Property.key, From.Path)
            }
        }
    )
}

void TopDownPopulate(TArray<UnresolvedMaterial*> const& Unresolved) noexcept
{
    if (Unresolved.empty())
    {
        return;
    }

    auto IsLeaf{[&Unresolved](UnresolvedMaterial const& Material)
    {
        return !algo::any_of(Unresolved, [&Material](auto* Other)
        {
            return algo::contains(Other->Inherits, Material.Identifier);
        });
    }};
    auto GetResolved{[Unresolved](auto const& Identifier) -> UnresolvedMaterial&
    {
        auto It{algo::find(Unresolved, Identifier, &UnresolvedMaterial::Identifier)};
        if (It == Unresolved.end())
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No such shader.", Identifier)
        }
        return **It;
    }};

    TArray<UnresolvedMaterial*> Siblings;
    TArray<UnresolvedMaterial*> Unrelated;
    for (auto* Material: Unresolved)
    {
        check(Material)

        if (IsLeaf(*Material))
        {
            Siblings.emplace_back(Material);
        }
        else
        {
            Unrelated.emplace_back(Material);
        }
    }

    ::TopDownPopulate(Unrelated);

    for (UnresolvedMaterial* Sibling: Siblings)
    {
        for (auto const& Parent: Sibling->Inherits)
        {
            ::Populate(&GetResolved(Sibling->Identifier), GetResolved(Parent));
        }
    }
}

TArray<rhi::material_template> ResolveInheritance(Jafg::JShaderSubsystem& ShaderSubsystem, TArray<UnresolvedMaterial> Materials) noexcept
{
    if (Materials.empty())
    {
        return {};
    }

    TArray<UnresolvedMaterial*> Unresolved; Unresolved.reserve(Materials.size());
    for (auto& Material: Materials)
    {
        Unresolved.emplace_back(&Material);
    }

    ::TopDownPopulate(Unresolved);

    TArray<rhi::material_template> Result; Result.reserve(Unresolved.size());
    algo::for_each(Unresolved, [&ShaderSubsystem, &Result](UnresolvedMaterial* Material)
    {
        check(Material)
        if (Material->IsFullyResolved())
        {
            LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Successfully resolved material.", Material->Path)
            Result.push_back({
                .my_shader = ShaderSubsystem.GetShader(*Material->Shader),
                .identifier = std::move(Material->Identifier),
                .pipeline_input_assembly_state = *Material->PipelineInputAssemblyState,
                .pipeline_rasterization_state = *Material->PipelineRasterizationState,
                // .pipeline_multisample_state = *Material->PipelineMultisampleState,
                .pipeline_depth_stencil_state = *Material->PipelineDepthStencilState,
                .pipeline_color_blend_state = *Material->PipelineColorBlendState,
                .properties = Material->Properties,
                });
        }
        else
        {
            LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Material is not fully resolved. Skipping.", Material->Path)
        }
    });

    return Result;
}

} /* ~Namespace <Anonymous> */

rhi::vk_binding Jafg::LMaterialInstance::GetBinding(LStringView Key) const noexcept
{
    check(this->Material.get())
    auto& Shader{this->Material->Template.my_shader};

    for (auto& Parameter: Shader.Parameters)
    {
        if (Parameter.Name == Key)
        {
            return {
                .type = Parameter.as_descriptor_type(),
                .space = Parameter.Space,
                .index = Parameter.Index,
                };
        }
    }
    LOG_FATAL(LogMaterialSubsystem, "[{}]: No such set in any layout [{}]."
        , Shader.Identifier, Key
        )
}

void Jafg::LMaterialInstance::Vk_SetField(LFrontend& Frontend, rhi::vk_binding Where, LStringView Value)
{
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Setting value [{}] for set identifier [{}::{}]."
        , this->Material->Template.identifier, Value, Where.space, Where.index
        )

    switch (Where.type)
    {
    case vk::DescriptorType::eSampler:
    {
        if (Value == "Jafg.LinearRepeatSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerRepeat());
        }
        else if (Value == "Jafg.LinearMirroredRepeatSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.LinearClampToEdgeSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerClampToEdge());
        }
        else if (Value == "Jafg.LinearClampToBorderSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerClampToBorder());
        }
        else if (Value == "Jafg.NearestRepeatSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerRepeat());
        }
        else if (Value == "Jafg.NearestMirroredRepeatSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.NearestClampToEdgeSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerClampToEdge());
        }
        else if (Value == "Jafg.NearestClampToBorderSampler")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerClampToBorder());
        }
        else
        {
            LOG_FATAL(LogMaterialSubsystem
                , "[{}]: No such sampler identifier [{}]. Failed to set value [{}] for set identifier [{}::{}]."
                , this->Material->Template.identifier, Value, Value, Where.space, Where.index
                )
        }
        break;
    }
    case vk::DescriptorType::eSampledImage:
    {
        this->Vk_SetSampledImage(Frontend, Where, *Frontend.GetSubsystemChecked<JTextureSubsystem>()->FromAsset(Value));
        break;
    }
    default:
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: Unsupported descriptor type [{}] for set identifier [{}::{}]. Failed to set value [{}]."
            , this->Material->Template.my_shader.Identifier, vk::to_string(Where.type), Where.space, Where.index, Value
            )
    }
    }
}

void Jafg::LMaterialInstance::Vk_SetSampler(LFrontend const& Frontend, rhi::vk_binding Where, vk::Sampler const& Sampler)
{
    vk::DescriptorImageInfo ImageInfo{
        .sampler = Sampler,
        .imageView = nullptr,
        .imageLayout = vk::ImageLayout::eUndefined,
        };
    check(ImageInfo.imageView == nullptr && ImageInfo.imageLayout == vk::ImageLayout::eUndefined)
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = *this->Vk_GetUniqueDescriptorSet(Where.space),
        .dstBinding = Where.index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eSampler,
        .pImageInfo = &ImageInfo,
        },};
    /* TODO: If we wanna be pedantic we should add this write to a frame buffer and update in a batch like fashion. But how cares rn. */
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
}

void Jafg::LMaterialInstance::Vk_SetSampledImage(LFrontend const& Frontend, rhi::vk_binding Where, LTexture2 const& Texture)
{
    vk::DescriptorImageInfo ImageInfo{
        .sampler = nullptr,
        .imageView = Texture.GetImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = *this->Vk_GetUniqueDescriptorSet(Where.space),
        .dstBinding = Where.index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eSampledImage,
        .pImageInfo = &ImageInfo,
        },};
    /* TODO: Same as above. */
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
}

bool Jafg::JMaterialSubsystem::ShouldCreateSubsystem() const noexcept
{
    if (this->GetFrontend().IsHeadless())
    {
        return false;
    }
    return Super::ShouldCreateSubsystem();
}

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    auto& Frontend{this->GetFrontend()};

    Collection.InitializeDependency<JTextureSubsystem>(this);
    Collection.InitializeDependency<JShaderSubsystem>(this);

    this->TextureSubsystem = Collection.GetSubsystemChecked<JTextureSubsystem>();
    this->ShaderSubsystem = Collection.GetSubsystemChecked<JShaderSubsystem>();

    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared bindless descriptor set layout with capacity [{}].", UBO::Bindless::name(), this->TextureSubsystem->GetBindlessTextureArrayCapacity())
    this->SharedDescriptorSetLayouts.emplace(UBO::Bindless::name(), UBO::Bindless::build(Frontend.Vk_GetDevice(), this->TextureSubsystem->GetBindlessTextureArrayCapacity()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared bindless descriptor set layout", UBO::WorldData::name())
    this->SharedDescriptorSetLayouts.emplace(UBO::WorldData::name(), UBO::WorldData::build(Frontend.Vk_GetDevice()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared bindless descriptor set layout", UBO::VisualShared::name())
    this->SharedDescriptorSetLayouts.emplace(UBO::VisualShared::name(), UBO::VisualShared::build(Frontend.Vk_GetDevice()));

    STAT_QUICK_CYCLE_START("Fetching materials")
    LOG_VERBOSE(LogMaterialSubsystem, "Fetching materials.")

    check(this->MaterialTemplates.empty())
    TArray<UnresolvedMaterial> Materials; // Of course this is just temp. We want to replace this with jasset.
    for (LPath& Material: finder::retrieve_files<finder::recursive_directory_iterator>("Content/Materials", ".*\\.json"))
    {
        UnresolvedMaterial& Result{Materials.emplace_back()};
        json::parse(finder::read_file(Material), nullptr, false).get_to(Result);
        Result.Identifier = Material.stem().string(); // Also do not use stem but rel to content.
        Result.Path = std::move(Material);

        check(algo::count(Materials, Result.Identifier, &UnresolvedMaterial::Identifier) == 1)
    }
    for (LSccFinder Finder{Materials}; auto& Scc: Finder.Sccs)
    {
        if (Scc.size() > 1)
        {
            LOG_FATAL(LogMaterialSubsystem, "Found inheritance cycle in shaders: [{}].", algo::join(Scc))
        }
    }
    for (auto& Material: Materials)
    {
        if (algo::contains(Material.Inherits, Material.Identifier))
        {
            LOG_FATAL(LogMaterialSubsystem
                , "[{}]: Reflexive inheritance is not allowed. [{}] inherits from [{}]."
                , Material.Path, Material.Identifier, algo::join(Material.Inherits)
                )
        }
    }
    this->MaterialTemplates = ::ResolveInheritance(*this->ShaderSubsystem, std::move(Materials));
    LOG_VERBOSE(LogMaterialSubsystem, "Fetched [{}] materials.", this->MaterialTemplates.size())

    // Kinda awkward here. Is there a way around?
    this->TextureSubsystem->_Vk_AllocateBindlessPool();
    this->TextureSubsystem->_Vk_UpdateSamplers();
}

void Jafg::JMaterialSubsystem::PurgeUnused()
{
    std::erase_if(this->MaterialInstances, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "[{}]: Puring unreferenced material instance.", E.first)
            return true;
        }
        return false;
    });
    std::erase_if(this->Materials, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "[{}]: Puring unreferenced material.", E.first)
            return true;
        }
        return false;
    });
}

Jafg::LMaterialRef Jafg::JMaterialSubsystem::GetMaterial(LStringView Name) noexcept
{
    if (auto It{this->Materials.find(Name)}; It != this->Materials.end())
    {
        return It->second;
    }

    check(this->ShaderSubsystem && this->TextureSubsystem)
    auto& Frontend{this->GetLocalEgo().GetFrontend()};

    rhi::graphics_pipeline_factory Factory;

    auto Result{std::make_shared<LMaterial>(this->GetMaterialTemplate(Name))};
    auto& Material{*Result};
    auto& Template{Material.Template};
    auto& Shader{Template.my_shader};

    {
        std::unordered_map<LPath, vk::ShaderModule> CodePaths;
        algo::for_each(Shader.EntryPoints, [&](auto const& Entrypoint)
        {
            if (!CodePaths.contains(Entrypoint.Code))
            {
                auto Code{finder::read_binary_file(Entrypoint.Code)};
                auto Result{Frontend.Vk_GetDevice().createShaderModule({
                    .codeSize = Code.size(),
                    .pCode = reinterpret_cast<u32 const*>(Code.data()),
                    })};
                check(Result.has_value())
                CodePaths[Entrypoint.Code] = *Factory.shader_modules.emplace_back(std::move(*Result));
            }
        });
        for (auto const& Entrypoint: Shader.EntryPoints)
        {
            Factory.shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = Entrypoint.Stage,
                .module = CodePaths.at(Entrypoint.Code),
                .pName = Entrypoint.Name.c_str(),
                });
        }
    }

    if (auto& VertEntry{*algo::find_checked(Shader.EntryPoints, vk::ShaderStageFlagBits::eVertex, &rhi::reflected_shader::entry_point::Stage)};
        std::holds_alternative<rhi::reflected_shader::entry_point::custom_type>(VertEntry.Parameters))
    {
        auto& type{std::get<rhi::reflected_shader::entry_point::custom_type>(VertEntry.Parameters)};
        if (auto It{type.UserAttributes.find("CxxName")}; It != type.UserAttributes.end())
        {
            if (It->second.size() != 1)
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Custom vertex input type [{}] must have exactly one CxxName user attribute argument."
                    , Name, type.Name)
            }
            Factory.pipeline_vertex_input_state = this->ShaderSubsystem->GetVertexInput(It->second.front()).Provider();
        }
        else
        {
            Factory.pipeline_vertex_input_state = this->ShaderSubsystem->GetVertexInput(type.Name).Provider();
        }
    }
    else if (std::holds_alternative<rhi::reflected_shader::entry_point::generated_type>(VertEntry.Parameters))
    {
    }
    else
    {
        std::unreachable();
    }

    Factory.pipeline_input_assembly_state = Template.pipeline_input_assembly_state;
    Factory.pipeline_rasterization_state = Template.pipeline_rasterization_state;
    // Factory.pipeline_multisample_state = Template.pipeline_multisample_state;
    if (Template.my_shader.Identifier == "Shaders/Jafg.Mesh.Outline")
    {
        Factory.pipeline_multisample_state = vk::PipelineMultisampleStateCreateInfo{
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable = vk::False,
            };
    }
    else
    {
        Factory.pipeline_multisample_state = vk::PipelineMultisampleStateCreateInfo{
            .rasterizationSamples = Frontend.Vk_GetMaxMsaaSampleCount(), // TODO: user prefs.
            .sampleShadingEnable = vk::False, /* TODO whats this?? */ // TODO: make part of material template.
            };
    }
    Factory.pipeline_depth_stencil_state = Template.pipeline_depth_stencil_state;
    {
        Factory.pipeline_color_blend_attachment_states = Template.pipeline_color_blend_state.pipeline_color_blend_attachment_states;
        Factory.pipeline_color_blend_state = vk::PipelineColorBlendStateCreateInfo{
            .logicOpEnable = Template.pipeline_color_blend_state.logic_op_enable,
            .logicOp = Template.pipeline_color_blend_state.logic_op,
            .attachmentCount = static_cast<u32>(Factory.pipeline_color_blend_attachment_states.size()),
            .pAttachments = Factory.pipeline_color_blend_attachment_states.data(),
            };
    }

    std::unordered_map<u32, TArray<vk::DescriptorSetLayoutBinding>> DescriptorSetLayoutBindings;
    std::unordered_map<LString, u32> SharedDescriptorSetLayoutBindings;
    for (auto& DescriptorTableSlot: Shader.Parameters
        | algo::views::filter([](auto& Parameter)
        {
            return Parameter.Kind == rhi::reflected_shader::binding_type::descriptor_table_slot;
        }))
    {
        if (auto It{DescriptorTableSlot.UserAttributes.find("Shared")}; It != DescriptorTableSlot.UserAttributes.end())
        {
            if (It->second.size() != 1)
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Shared descriptor table slot [{}] must have exactly one Shared user attribute argument."
                    , Name, DescriptorTableSlot.Name)
            }
            if (auto SharedIt{SharedDescriptorSetLayoutBindings.find(It->second.front())}; SharedIt != SharedDescriptorSetLayoutBindings.end())
            {
                if (SharedIt->second != DescriptorTableSlot.Space)
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Shared descriptor table slot [{}] has conflicting space bindings [{} != {}]."
                        , Name, DescriptorTableSlot.Name, SharedIt->second, DescriptorTableSlot.Space)
                }
            }
            else
            {
                SharedDescriptorSetLayoutBindings.emplace(It->second.front(), DescriptorTableSlot.Space);
            }
        }
        else
        {
            if (auto Flags{Shader.shader_stage_flags_for(DescriptorTableSlot)}; Flags != vk::ShaderStageFlags{})
            {
                DescriptorSetLayoutBindings[DescriptorTableSlot.Space].emplace_back(vk::DescriptorSetLayoutBinding{
                    .binding = DescriptorTableSlot.Index,
                    .descriptorType = DescriptorTableSlot.as_descriptor_type(),
                    .descriptorCount = DescriptorTableSlot.binding_count(),
                    .stageFlags = Flags,
                    .pImmutableSamplers = nullptr,
                    });
            }
        }
    }
    for (auto const& [Space, Bindings]: DescriptorSetLayoutBindings)
    {
        auto Result{Frontend.Vk_GetDevice().createDescriptorSetLayout({
            .bindingCount = static_cast<u32>(Bindings.size()),
            .pBindings = Bindings.data(),
            })};
        check(Result.has_value())
        Factory.unique_descriptor_set_layouts.emplace_back(Space, std::move(*Result));
    }
    for (auto const& [Name, Space]: SharedDescriptorSetLayoutBindings)
    {
        checkCode
        (
            if (!this->SharedDescriptorSetLayouts.contains(Name)) /* #at throws always. This is just for a more human-readable error message during development. */
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: No such shared descriptor set layout at space [{}].", Name, Space)
            }
        )
        Factory.shared_descriptor_set_layouts.emplace_back(Space, this->SharedDescriptorSetLayouts.at(Name));
    }

    for (auto& PushConstant: Shader.Parameters
        | algo::views::filter([](auto& Parameter)
        {
            return Parameter.Kind == rhi::reflected_shader::binding_type::push_constant_buffer;
        }))
    {
        if (auto Flags{Shader.shader_stage_flags_for(PushConstant)}; Flags != vk::ShaderStageFlags{})
        {
            check(std::holds_alternative<rhi::reflected_shader::binding_definition::constant_buffer>(PushConstant.Type)) // TODO: Support more pcs
            auto& ConstantBuffer = std::get<rhi::reflected_shader::binding_definition::constant_buffer>(PushConstant.Type);
            Factory.push_constant_ranges.emplace_back(vk::PushConstantRange{
                .stageFlags = Flags,
                .offset = ConstantBuffer.Offset,
                .size = ConstantBuffer.Size,
                });
        }
    }

    check(Template.color_attachment_formats.empty())
    check(Template.depth_attachment_format == vk::Format::eUndefined)
    check(Template.stencil_attachment_format == vk::Format::eUndefined)

    if (Template.my_shader.Identifier == "Shaders/Jafg.Mesh.Outline")
    {
        Factory.color_attachment_formats = {vk::Format::eR8Unorm};
    }
    else
    {
        Factory.color_attachment_formats = {Frontend.Vk_GetSurfaceFormat().format};
    }

    Factory.depth_attachment_format = Frontend.Vk_GetPreferredDepthFormat();
    Factory.stencil_attachment_format = vk::Format::eUndefined;

    Material.Pipeline = Factory.build(Frontend.Vk_GetDevice());
    this->Materials.emplace(Name, Result);
    return Result;
}

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::GetInstance(LMaterialRef Material)
{
    check(this->ShaderSubsystem)

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    auto& MaterialTemplate{Material->Template};
    auto& Shader{MaterialTemplate.my_shader};

    auto Instance{std::make_shared<LMaterialInstance>(LMaterialInstance{.Material=Material})};

    std::unordered_map<u32, rhi::reflected_shader::update_frequency> UpdateFrequencies;
    for (auto& Parameter: Shader.Parameters)
    {
        if (Parameter.Kind != rhi::reflected_shader::binding_type::descriptor_table_slot)
        {
            continue;
        }
        if (Parameter.find_user_attribute("Shared"))
        {
            continue;
        }

        if (auto* Array{Parameter.find_user_attribute("UpdateFrequency", 1uz)})
        {
            rhi::reflected_shader::update_frequency Frequency{rhi::reflected_shader::from_string(Array->front())};
            if (auto It{UpdateFrequencies.find(Parameter.Space)}; It != UpdateFrequencies.end())
            {
                if (It->second != Frequency)
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Conflicting update frequencies for space [{}]. [{} != {}]"
                        , Shader.Identifier, Parameter.Space, static_cast<u32>(It->second), static_cast<u32>(Frequency))
                }
            }
            else
            {
                UpdateFrequencies.emplace(Parameter.Space, Frequency);
            }
        }
        else
        {
            /*
             * Theoretically we could dervie the frequency from one of the bindings inside a set
             * but we do not do that intentiionally. We ant to be explicit.
             */
            LOG_FATAL(LogMaterialSubsystem, "[{}]: No update frequency for space [{}@{}::{}]."
                , Shader.Identifier, Parameter.Name, Parameter.Space, Parameter.Index)
        }
    }

    TArray<vk::DescriptorSetLayout> UniqueLayouts;
    for (auto const& [Space, Frequency]: UpdateFrequencies)
    {
        if (Frequency == rhi::reflected_shader::update_frequency::per_frame)
        {
            check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)
            for (auto FramesInFlight{0uz}; FramesInFlight < Frontend.Vk_GetNumberOfFramesInFlight(); ++FramesInFlight)
            {
                UniqueLayouts.emplace_back(*Material->Pipeline.get_unique_layout(Space));
            }
        }
        else if (Frequency == rhi::reflected_shader::update_frequency::rarely)
        {
            UniqueLayouts.emplace_back(Material->Pipeline.get_unique_layout(Space));
        }
        else
        {
            std::unreachable();
        }
    }
    if (!UniqueLayouts.empty())
    {
        auto UniqueSets{rhi::vk_allocate(Frontend.Vk_GetDevice(), vk::DescriptorSetAllocateInfo{
            .descriptorPool = Frontend.Vk_GetDescriptorPool(),
            .descriptorSetCount = static_cast<u32>(UniqueLayouts.size()),
            .pSetLayouts = UniqueLayouts.data(),
            })};

        auto Idx{0uz};
        for (auto const& [Space, Frequency]: UpdateFrequencies)
        {
            if (Frequency == rhi::reflected_shader::update_frequency::per_frame)
            {
                for (auto FrameInFlight{0uz}; FrameInFlight < Frontend.Vk_GetNumberOfFramesInFlight(); ++FrameInFlight)
                {
                    Instance->FrequentDescriptorSets[FrameInFlight].emplace_back(Space, std::move(UniqueSets[Idx++]));
                }
            }
            else if (Frequency == rhi::reflected_shader::update_frequency::rarely)
            {
                Instance->InfrequentDescriptorSets.emplace_back(Space, std::move(UniqueSets[Idx++]));
            }
            else
            {
                std::unreachable();
            }
        }
        check(Idx == UniqueLayouts.size())
    }

    if (!UniqueLayouts.empty())
    {
        for (auto& Parameter: Shader.Parameters)
        {
            if (Parameter.Kind != rhi::reflected_shader::binding_type::descriptor_table_slot)
            {
                continue;
            }
            if (Parameter.find_user_attribute("Shared"))
            {
                continue;
            }

            std::visit([&](auto&& Type)
            {
                auto SetBinding{[&]<typename T>(std::optional<LString> const& CxxName = {})
                {
                    auto SetResource{[&](std::optional<u32> Frame)
                    {
                        auto& DescriptorSetInstance{Instance->Vk_GetUniqueDescriptorSet(Parameter.Space, Frame)};
                        if (DescriptorSetInstance.Resources.size() < Parameter.Index + 1)
                        {
                            DescriptorSetInstance.Resources.resize(static_cast<std::size_t>(Parameter.Index + 1));
                        }
                        *DescriptorSetInstance.Resources[Parameter.Index] = T{};

                        if constexpr (std::same_as<T, rhi::mapped_device_buffer>)
                        {
                            rhi::mapped_device_buffer& Buffer{DescriptorSetInstance.Resources[Parameter.Index].AsBuffer()};
                            if (!Parameter.CxxName)
                            {
                                LOG_FATAL(LogMaterialSubsystem, "[{}]: No CxxName for buffer set [{}::{}].", Shader.Identifier, Parameter.Space, Parameter.Index)
                            }
                            if (auto& CxxParameter{this->ShaderSubsystem->GetBufferObject(*CxxName)}; !CxxParameter.bSkipAutoAllocation)
                            {
                                check(CxxParameter.DefaultCount > 0)
                                check(!!CxxParameter.BufferCreateInfo)
                                check(!Buffer.data())
                                LOG_TRACE(LogMaterialSubsystem, "[{}]: Auto-allocating buffer [{}@{}::{}] with elem count of [{}]."
                                    , Shader.Identifier, *CxxName, Parameter.Space, Parameter.Index, CxxParameter.DefaultCount)
                                Buffer = Frontend.Vk_CreateMappedBuffer(CxxParameter.BufferCreateInfo(CxxParameter.DefaultCount));
                            }
                        }
                    }};

                    if (auto Frequency{UpdateFrequencies.at(Parameter.Space)}; Frequency == rhi::reflected_shader::update_frequency::rarely)
                    {
                        SetResource(std::nullopt);
                    }
                    else if (Frequency == rhi::reflected_shader::update_frequency::per_frame)
                    {
                        check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)
                        for (auto FrameInFlight{0uz}; FrameInFlight < Frontend.Vk_GetNumberOfFramesInFlight(); ++FrameInFlight)
                        {
                            SetResource(FrameInFlight);
                        }
                    }
                    else
                    {
                        LOG_FATAL(LogMaterialSubsystem, "[{}]: Unsupported update frequency for set [{}::{}]."
                            , Shader.Identifier, Parameter.Space, Parameter.Index)
                    }
                }};

                if constexpr (std::is_same_v<std::decay_t<decltype(Type)>, rhi::reflected_shader::binding_definition::resource>)
                {
                    std::visit([&](auto&& base_shape)
                    {
                        if constexpr (std::same_as<std::decay_t<decltype(base_shape)>, rhi::reflected_shader::binding_definition::resource::texture2D>)
                        {
                            SetBinding.template operator()<LTexture2Ref>();
                        }
                        else if constexpr (std::same_as<std::decay_t<decltype(base_shape)>, rhi::reflected_shader::binding_definition::resource::structured_buffer>)
                        {
                            SetBinding.template operator()<rhi::mapped_device_buffer>(base_shape.CxxName);
                        }
                        else
                        {
                            LOG_FATAL(LogMaterialSubsystem, "[{}]: Unsupported resource type for set [{}::{}].", Shader.Identifier, Parameter.Space, Parameter.Index)
                        }
                    }, Type.base_shape);
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(Type)>, rhi::reflected_shader::binding_definition::sampler_state>)
                {
                    SetBinding.template operator()<UBO::Bindless::Sampler>();
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(Type)>, rhi::reflected_shader::binding_definition::constant_buffer>)
                {
                    SetBinding.template operator()<rhi::mapped_device_buffer>(Parameter.CxxName);
                }
                else
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Unsupported Cxx parameter type for set [{}::{}].", Shader.Identifier, Parameter.Space, Parameter.Index)
                }
            }, Parameter.Type);
        }
    }

    checkCode
    (
        std::unordered_map<u32, u32> InfrequentSpaces;
        for (auto& Set: Instance->InfrequentDescriptorSets)
        {
            if (InfrequentSpaces.contains(Set.Space))
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Duplicate space [{}] in infrequent descriptor sets.", Shader.Identifier, Set.Space)
            }
            InfrequentSpaces.emplace(Set.Space, 1);
        }
        std::unordered_map<u32, u32> Frequent;
        for (auto& Array: Instance->FrequentDescriptorSets)
        {
            for (auto const& Set: Array)
            {
                if (InfrequentSpaces.contains(Set.Space))
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Space [{}] is both infrequent and frequent.", Shader.Identifier, Set.Space)
                }
                if (auto It{Frequent.find(Set.Space)}; It != Frequent.end())
                {
                    ++It->second;
                }
                else
                {
                    Frequent.emplace(Set.Space, 1);
                }
            }
        }
        for (auto const& [Space, Count]: Frequent)
        {
            if (Count != Frontend.Vk_GetNumberOfFramesInFlight())
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Space [{}] is frequent but not present in all frames [{} != {}]."
                    , Shader.Identifier, Space, Count, Frontend.Vk_GetNumberOfFramesInFlight())
            }
        }
    )

    for (auto const& [Key, Value]: MaterialTemplate.properties)
    {
        Instance->Vk_SetField(this->GetFrontend(), Instance->GetBinding(Key), Value);
    }

    return Instance;
}
