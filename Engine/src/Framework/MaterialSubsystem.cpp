// Copyright mzoesch. All rights reserved.

#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Engine/Engine.h"
#include "Framework/ShaderSubsystem.h"
#include "User/LocalEgo.h"
#include "Rhi/GraphicsPipelineFactory.h"
#include "Rhi/Objects.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/TextureCube.h"
#include "Rhi/RenderTarget.h"
#include "Engine/WorldData.h"
#include "User/UserPreferences.h"

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
                    LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits from [{}]. No such material.", Material.Path, Parent)
                }
                this->Connect(*It);
                this->LowLink[Material.Identifier] = std::min(this->LowLink[Material.Identifier], this->LowLink[Parent]);
            }
            else if (this->OnStack.contains(Parent))
            {
                if (auto It{algo::find(this->Materials, Parent, &UnresolvedMaterial::Identifier)}; It == this->Materials.end())
                {
                    LOG_FATAL(LogShaderSubsystem, "[{}]: Inherits from [{}]. No such material.", Material.Path, Parent)
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

TArray<rhi::material_template> ResolveInheritance(Jafg::JShaderSubsystem& ShaderSubsystem, Jafg::LPluginInfo Namespace, TArray<UnresolvedMaterial> Materials) noexcept
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
    algo::for_each(Unresolved, [Namespace, &ShaderSubsystem, &Result](UnresolvedMaterial* Material)
    {
        check(Material)
        if (Material->IsFullyResolved())
        {
            LOG_VERBOSE(LogMaterialSubsystem, "[{}/{}]: Successfully resolved material.", *Namespace, Material->Identifier)
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
            LOG_VERBOSE(LogMaterialSubsystem, "[{}/{}]: Material is not fully resolved. Skipping.", *Namespace, Material->Identifier)
        }
    });

    return Result;
}

struct Property_SampledImage
{
    //# The identifier of the namespace. @see Jafg::LEngine::FindPluginInfo
    LString Namespace;
    LString Name;
};
SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(Property_SampledImage)
{
    j.at("namespace").get_to(field.Namespace);
    j.at("name").get_to(field.Name);
}

} /* ~Namespace <Anonymous> */

namespace Jafg
{

template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
static void from_json(BasicJsonType const& j, LPbrMaterialTemplate& Material)
{
    const LPbrMaterialTemplate nlohmann_json_default_obj{};

    if (j.contains("material"))
    {
        if (j.at("material").is_string())
        {
            check(Material.Material.first.empty())
            j.at("material").get_to(Material.Material.second);
        }
        else
        {
            j.at("material").get_to(Material.Material);
        }
    }

    if (j.contains("albedo_factor"))
    {
        LVec4F AlbedoFactor;
        j.at("albedo_factor").get_to(AlbedoFactor);
        Material.AlbedoFactor = LLinearColor{AlbedoFactor.x, AlbedoFactor.y, AlbedoFactor.z, AlbedoFactor.w};
    }
    if (j.contains("occlusion_factor"))
    {
        j.at("occlusion_factor").get_to(Material.OcclusionFactor);
    }
    if (j.contains("roughness_factor"))
    {
        j.at("roughness_factor").get_to(Material.RoughnessFactor);
    }
    if (j.contains("metallic_factor"))
    {
        j.at("metallic_factor").get_to(Material.MetallicFactor);
    }
    if (j.contains("emissive_factor"))
    {
        j.at("emissive_factor").get_to(Material.EmissiveFactor);
    }

    if (j.contains("alpha_cutoff"))
    {
        j.at("alpha_cutoff").get_to(Material.AlphaCutoff);
    }
    if (j.contains("normal_scale"))
    {
        j.at("normal_scale").get_to(Material.NormalScale);
    }

    if (j.contains("textures"))
    {
        j.at("textures").get_to(Material.Textures);
    }

    if (j.contains("sampler"))
    {
        j.at("sampler").get_to(Material.Sampler);
    }

    if (j.contains("flags"))
    {
        j.at("flags").get_to(Material.Flags);
    }

    if (j.contains("uv_scale"))
    {
        j.at("uv_scale").get_to(Material.UVScale);
    }
    if (j.contains("uv_offset"))
    {
        j.at("uv_offset").get_to(Material.UVOffset);
    }
}

} /* ~Namespace Jafg */

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
        if (Value == "Jafg.LinearRepeat")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerRepeat());
        }
        else if (Value == "Jafg.LinearMirroredRepeat")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.LinearClampToEdge")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerClampToEdge());
        }
        else if (Value == "Jafg.LinearClampToBorder")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetLinearSamplerClampToBorder());
        }
        else if (Value == "Jafg.NearestRepeat")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerRepeat());
        }
        else if (Value == "Jafg.NearestMirroredRepeat")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.NearestClampToEdge")
        {
            this->Vk_SetSampler(Frontend, Where, Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerClampToEdge());
        }
        else if (Value == "Jafg.NearestClampToBorder")
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
        Property_SampledImage Image;
        json json = json::parse(Value, nullptr, false);
        if (json.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "Invalid json: [{}].", Value)
        }
        json.get_to(Image);
        check(GEngine)
        this->Vk_SetSampledImage(Frontend, Where, *Frontend.GetSubsystemChecked<JTextureSubsystem>()
            ->FromTextureView(GEngine->FindPluginInfoChecked(Image.Namespace), Image.Name));
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

void Jafg::LMaterialInstance::Vk_SetSampledTextureCube(LFrontend const& Frontend, rhi::vk_binding Where, LTextureCube2 const& Texture)
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

LPath const& Jafg::LPbrMaterialTemplate::GetTexture(ETextureType Type, ETextureQualityBits Quality) const noexcept
{
    auto GetQualityBelow{[](ETextureQualityBits Q) -> std::optional<ETextureQualityBits>
    {
        switch (Q)
        {
        case ETextureQualityBits::Potato:    return std::nullopt;
        case ETextureQualityBits::Toaster:   return ETextureQualityBits::Potato;
        case ETextureQualityBits::Mobile:    return ETextureQualityBits::Toaster;
        case ETextureQualityBits::Standard:  return ETextureQualityBits::Mobile;
        case ETextureQualityBits::Ultra:     return ETextureQualityBits::Standard;
        case ETextureQualityBits::Cinematic: return ETextureQualityBits::Ultra;
        }
        LOG_FATAL(LogMaterialSubsystem, "Unsupported quality [{}].", static_cast<u32>(Q))
    }};
    auto GetQualityAbove{[](ETextureQualityBits Q) -> std::optional<ETextureQualityBits>
    {
        switch (Q)
        {
        case ETextureQualityBits::Potato:    return ETextureQualityBits::Toaster;
        case ETextureQualityBits::Toaster:   return ETextureQualityBits::Mobile;
        case ETextureQualityBits::Mobile:    return ETextureQualityBits::Standard;
        case ETextureQualityBits::Standard:  return ETextureQualityBits::Ultra;
        case ETextureQualityBits::Ultra:     return ETextureQualityBits::Cinematic;
        case ETextureQualityBits::Cinematic: return std::nullopt;
        }
        LOG_FATAL(LogMaterialSubsystem, "Unsupported quality [{}].", static_cast<u32>(Q))
    }};

    auto GetForQuality{[&](ETextureQualityBits Q) -> std::optional<LPath const*>
    {
        for (auto const& [CandidateQuality, Bindings]: this->Textures)
        {
            if (!!(CandidateQuality & Q))
            {
                switch (Type)
                {
                case ETextureType::Albedo:   return &Bindings.Albedo;
                case ETextureType::Normal:   return &Bindings.Normal;
                case ETextureType::Arm:      return &Bindings.Arm;
                case ETextureType::Emissive: return &Bindings.Emissive;
                }
            }
        }
        return std::nullopt;
    }};

    ETextureQualityBits Cursor{Quality};
    while (true)
    {
        if (auto Path{GetForQuality(Cursor)}; Path && !(*Path)->empty())
        {
            return **Path;
        }
        if (auto Next{GetQualityBelow(Cursor)}; Next)
        {
            Cursor = *Next;
        }
        else
        {
            break;
        }
    }
    Cursor = Quality;
    while (true)
    {
        if (auto Next{GetQualityAbove(Cursor)}; Next)
        {
            Cursor = *Next;
        }
        else
        {
            break;
        }
        if (auto Path{GetForQuality(Cursor)}; Path && !(*Path)->empty())
        {
            return **Path;
        }
    }

    static LPath Empty{};
    return Empty;
}

bool Jafg::LPbrMaterialTemplate::IsEmptyAtTexture(ETextureType Type) const noexcept
{
    for (auto const& Bindings: this->Textures | std::views::values)
    {
        switch (Type)
        {
        case ETextureType::Albedo:   if (!Bindings.Albedo.empty())   { return false; } break;
        case ETextureType::Normal:   if (!Bindings.Normal.empty())   { return false; } break;
        case ETextureType::Arm:      if (!Bindings.Arm.empty())      { return false; } break;
        case ETextureType::Emissive: if (!Bindings.Emissive.empty()) { return false; } break;
        }
    }

    return true;
}

void Jafg::LPbrMaterialInstance::FreeFromBindlessArray(JMaterialSubsystem* CachedSubsystem)
{
    check(this->IsBindless())

    if (!CachedSubsystem)
    {
        CachedSubsystem = Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystem<JMaterialSubsystem>();
    }
    if (CachedSubsystem)
    {
        CachedSubsystem->RemoveInstanceFromGlobalBindlessArray(this);
    }
}

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    auto& Frontend{this->GetFrontend()};

    Collection.InitializeDependency<JTextureSubsystem>(this);
    Collection.InitializeDependency<JShaderSubsystem>(this);

    STAT_FUNCTION()

    this->GetMutableEngine().OnForeignPluginLoaded.EmplaceStrong([this](LLoadedPlugin& Plugin)
    {
        if (Plugin.HasContent())
        {
            this->RetrieveMaterialsFrom(Plugin.GetInfo());
        }

        return false;
    });

    this->TextureSubsystem = Collection.GetSubsystemChecked<JTextureSubsystem>();
    this->ShaderSubsystem = Collection.GetSubsystemChecked<JShaderSubsystem>();

    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared descriptor set layout with capacity [{}].", UBO::Bindless::name(), this->TextureSubsystem->GetBindlessTextureArrayCapacity())
    this->SharedDescriptorSetLayouts.emplace(UBO::Bindless::name(), UBO::Bindless::build(Frontend.Vk_GetDevice(), this->TextureSubsystem->GetBindlessTextureArrayCapacity()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared descriptor set layout", UBO::WorldData::name())
    this->SharedDescriptorSetLayouts.emplace(UBO::WorldData::name(), UBO::WorldData::build(Frontend.Vk_GetDevice()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared descriptor set layout", UBO::VisualShared::name())
    this->SharedDescriptorSetLayouts.emplace(UBO::VisualShared::name(), UBO::VisualShared::build(Frontend.Vk_GetDevice()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared descriptor set layout.", SSBO::BindlessPbrMaterial::name())
    this->SharedDescriptorSetLayouts.emplace(SSBO::BindlessPbrMaterial::name(), SSBO::BindlessPbrMaterial::build(Frontend.Vk_GetDevice()));
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Registering shared descriptor set layout.", SSBO::BindlessPbrLight::name())
    this->SharedDescriptorSetLayouts.emplace(SSBO::BindlessPbrLight::name(), SSBO::BindlessPbrLight::build(Frontend.Vk_GetDevice()));

    LOG_VERBOSE(LogMaterialSubsystem, "Fetching materials.")
    this->RetrieveMaterialsFrom(JafgInfo);

    Tasks::Make(ENamedThreads::Master, ETaskTime::Early, [this]{ this->PrepareBindless(); });
    // TODO: Kinda awkward here. Is there a way around?
    this->TextureSubsystem->_Vk_AllocateBindlessPool();
    this->TextureSubsystem->_Vk_UpdateSamplers();
}

void Jafg::JMaterialSubsystem::PurgeUnused()
{
    LOG_TRACE(LogMaterialSubsystem, "Purging unreferenced material instances and materials.")
    for (auto& Cache: this->Instances | std::views::values)
    {
        std::erase_if(Cache, [](auto& E) -> bool
        {
            if (E.second.use_count() == 1)
            {
                LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Puring unreferenced material instance.", E.first)
                return true;
            }
            return false;
        });
    }
    for (auto& Cache: this->Materials | std::views::values)
    {
        std::erase_if(Cache, [](auto& E) -> bool
        {
            if (E.second.use_count() == 1)
            {
                LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Puring unreferenced material.", E.first)
                return true;
            }
            return false;
        });
    }
}

void Jafg::JMaterialSubsystem::RegisterSharedInstance(LPluginInfo Namespace, LStringView Name, LMaterialInstanceRef Instance) noexcept
{
    check(Namespace)

    auto& Cache{this->Instances[Namespace]};
    if (Cache.contains(Name))
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}/{}]: Material instance with this name already exists.", *Namespace, Name)
    }
    Cache.emplace(LString{Name}, std::move(Instance));
}

std::optional<Jafg::LMaterialInstanceRef> Jafg::JMaterialSubsystem::FindSharedInstance(LPluginInfo Namespace, LStringView Name) const noexcept
{
    check(Namespace)

    if (auto It{this->Instances.find(Namespace)}; It != this->Instances.end())
    {
        if (auto Instance{It->second.find(Name)}; Instance != It->second.end())
        {
            return Instance->second;
        }
    }
    return {};
}

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::FindOrAllocateSharedInstance(LPluginInfo Namespace, LStringView Name) noexcept
{
    if (auto Instance{this->FindSharedInstance(Namespace, Name)})
    {
        return *Instance;
    }
    auto Result{this->GetInstanceFromName(Namespace, Name)};
    this->RegisterSharedInstance(Namespace, Name, Result);
    return Result;
}

rhi::material_template const& Jafg::JMaterialSubsystem::GetMaterialTemplate(LPluginInfo Namespace, LStringView Identifier) const noexcept
{
    check(Namespace)

    auto const It{this->Templates.find(Namespace)};
    if (It == this->Templates.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: No such namespace.", *Namespace)
    }
    auto const Template{algo::find(It->second, Identifier, &rhi::material_template::identifier)};
    if (Template == It->second.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}/{}]: No such material.", *Namespace, Identifier)
    }
    return *Template;
}

Jafg::LMaterialRef Jafg::JMaterialSubsystem::GetMaterial(LPluginInfo Namespace, LStringView Name) noexcept
{
    check(Namespace)

    auto& Cache{this->Materials[Namespace]};
    if (auto It{Cache.find(Name)}; It != Cache.end())
    {
        return It->second;
    }

    checkCode
    (
        if (auto It{this->PbrTemplates.find(Namespace)}; It != this->PbrTemplates.end())
        {
            if (auto Pbr{algo::find(It->second, Name, &LPbrMaterialTemplate::Identifier)}; Pbr != It->second.end())
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}/{}]: Material is a pbr material. Use JMaterialSubsystem::GetPbrInstance instead.", *Namespace, Name)
            }
        }
    )

    check(this->ShaderSubsystem && this->TextureSubsystem)
    auto& Frontend{this->GetLocalEgo().GetFrontend()};

    rhi::graphics_pipeline_factory Factory;

    auto Result{std::make_shared<LMaterial>(this->GetMaterialTemplate(Namespace, Name))};
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
    //# TODO: Make the attachments part of the material template instead of deriving them from the shader.
    auto GetAuxiliaryColorFormat{[&]() -> std::optional<vk::Format>
    {
        LStringView Identifier{Template.my_shader.Identifier};
        if (Identifier == "Shaders/Jafg.Mesh.Outline")
        {
            return vk::Format::eR8Unorm;
        }
        if (Identifier == "Shaders/Jafg.Mesh.Depth")
        {
            return LRenderTarget::SceneDepthFormat;
        }
        if (Identifier == "Shaders/Jafg.Ssao" || Identifier == "Shaders/Jafg.SsaoBlur")
        {
            return LRenderTarget::AmbientOcclusionFormat;
        }
        return {};
    }};
    //# Depth only pipelines render without any color attachment. E.g. into the shadow atlas.
    bool const bDepthOnly{Template.my_shader.Identifier == "Shaders/Jafg.Mesh.Shadow"};
    //# Auxiliary targets are never multi sampled.
    if (GetAuxiliaryColorFormat() || bDepthOnly)
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
                LOG_FATAL(LogMaterialSubsystem, "[{}]: No such shared descriptor set layout found at space [{}].", Name, Space)
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

    if (bDepthOnly)
    {
        Factory.color_attachment_formats = {};
        if (!Factory.pipeline_color_blend_attachment_states.empty())
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: Depth only materials must not have any color blend attachments. Inherit from Materials/Base/NoColorAttachment."
                , Name)
        }
    }
    else if (auto Format{GetAuxiliaryColorFormat()})
    {
        Factory.color_attachment_formats = {*Format};
    }
    else
    {
        Factory.color_attachment_formats = {Frontend.Vk_GetSurfaceFormat().format};
    }

    //# The fullscreen passes of the ambient occlusion render without any depth attachment.
    bool const bWithoutDepth{Template.my_shader.Identifier == "Shaders/Jafg.Ssao" || Template.my_shader.Identifier == "Shaders/Jafg.SsaoBlur"};
    Factory.depth_attachment_format = bWithoutDepth ? vk::Format::eUndefined : Frontend.Vk_GetPreferredDepthFormat();
    Factory.stencil_attachment_format = vk::Format::eUndefined;

    Material.Pipeline = Factory.build(Frontend.Vk_GetDevice());
    Cache.emplace(LString{Name}, Result);
    return Result;
}

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::GetInstance(LMaterialRef Material)
{
    check(this->ShaderSubsystem)

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    auto& MaterialTemplate{Material->Template};
    auto& Shader{MaterialTemplate.my_shader};

    auto Instance{std::make_shared<LMaterialInstance>(Material)};

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
             * Theoretically we could derive the frequency from one of the bindings inside a set,
             * but we do not do that intentionally. We want to be explicit.
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
                        else if constexpr (std::same_as<std::decay_t<decltype(base_shape)>, rhi::reflected_shader::binding_definition::resource::textureCube2D>)
                        {
                            SetBinding.template operator()<LTextureCube2>();
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

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::GetInstanceFromName(LPluginInfo Namespace, LStringView Name)
{
    return this->GetInstance(this->GetMaterial(Namespace, Name));
}

Jafg::LPbrMaterialInstanceRef Jafg::JMaterialSubsystem::GetPbrInstance(LPluginInfo Namespace, LStringView Name)
{
    check(Namespace)

    if (auto It{this->PbrTemplates.find(Namespace)}; It != this->PbrTemplates.end())
    {
        if (auto Pbr{algo::find(It->second, Name, &LPbrMaterialTemplate::Identifier)}; Pbr != It->second.end())
        {
            if (auto It{algo::find(this->PbrInstances, &*Pbr, [](LPbrMaterialInstanceRef const& Element)
            {
                check(Element)
                return &Element->Template;
            })}; It != this->PbrInstances.end())
            {
                return *It;
            }

            auto& Prefs{GetSingleton<JUserPreferences>()};

            LPbrMaterialInstanceRef Result;
            if (Pbr->Material.second.empty())
            {
                check(Pbr->Material.first.empty())
                Result = this->PbrInstances.emplace_back(std::make_shared<LPbrMaterialInstance>(
                    this->FindOrAllocateSharedInstance(JafgInfo, "Materials/Pbr"), *Pbr));
            }
            else if (Pbr->Material.first.empty())
            {
                Result = this->PbrInstances.emplace_back(std::make_shared<LPbrMaterialInstance>(
                    this->FindOrAllocateSharedInstance(Namespace, Pbr->Material.second), *Pbr));
            }
            else
            {
                check(GEngine)
                Result = this->PbrInstances.emplace_back(std::make_shared<LPbrMaterialInstance>(
                    this->FindOrAllocateSharedInstance(GEngine->FindPluginInfoChecked(Pbr->Material.first), Pbr->Material.second), *Pbr));
            }
            check(!Result->IsBindless())

            Result->Data.AlbedoFactor = Pbr->AlbedoFactor.ToDeviceReadyVector4();
            Result->Data.OcclusionFactor = Pbr->OcclusionFactor;
            Result->Data.RoughnessFactor = Pbr->RoughnessFactor;
            Result->Data.MetallicFactor = Pbr->MetallicFactor;
            Result->Data.EmissiveFactor = Pbr->EmissiveFactor;

            Result->Data.AlphaCutoff = Pbr->AlphaCutoff;
            Result->Data.NormalScale = Pbr->NormalScale;

            //# TODO: Optimize later on:
            //      - albedo: VK_FORMAT_BC7_SRGB_BLOCK
            //      - normal: VK_FORMAT_BC5_UNORM_BLOCK
            //      - metallic-roughness: VK_FORMAT_BC7_UNORM_BLOCK (better quality) or VK_FORMAT_BC1_RGB_UNORM_BLOCK (lower memory usage)
            LString const FallbackAlbedo{algo::sprintf("{}.png", Name)};
            if (auto& Texture{Pbr->GetTexture(LPbrMaterialTemplate::ETextureType::Albedo, *Prefs.TextureQuality)}; !Texture.empty())
            {
                Result->LoadedTextures.emplace_back(this->TextureSubsystem->FromFile(
                      Namespace
                    , Texture.generic_string()
                    , {.Format=vk::Format::eR8G8B8A8Srgb}
                    , {.Samples=vk::SampleCountFlagBits::e1}
                    ));
                this->TextureSubsystem->AddTextureToGlobalBindlessArray(&*Result->LoadedTextures.back());
                Result->Data.Albedo = Result->LoadedTextures.back()->GetBindlessIndex();
            }
            else if (Pbr->IsEmptyAtTexture(LPbrMaterialTemplate::ETextureType::Albedo) && finder::is_regular_file(*Namespace / FallbackAlbedo))
            {
                Result->LoadedTextures.emplace_back(this->TextureSubsystem->FromFile(
                      Namespace
                    , FallbackAlbedo
                    , {.Format=vk::Format::eR8G8B8A8Srgb}
                    , {.Samples=vk::SampleCountFlagBits::e1}
                    ));
                this->TextureSubsystem->AddTextureToGlobalBindlessArray(&*Result->LoadedTextures.back());
                Result->Data.Albedo = Result->LoadedTextures.back()->GetBindlessIndex();
            }
            else
            {
                Result->Data.Albedo = rhi::invalid_resource_index;
            }

            if (auto& Texture{Pbr->GetTexture(LPbrMaterialTemplate::ETextureType::Normal, *Prefs.TextureQuality)}; !Texture.empty())
            {
                Result->LoadedTextures.emplace_back(this->TextureSubsystem->FromFile(
                      Namespace
                    , Texture.generic_string()
                    , {.Format=vk::Format::eR8G8B8A8Unorm}
                    , {.Samples=vk::SampleCountFlagBits::e1}
                    ));
                this->TextureSubsystem->AddTextureToGlobalBindlessArray(&*Result->LoadedTextures.back());
                Result->Data.Normal = Result->LoadedTextures.back()->GetBindlessIndex();
            }
            else
            {
                Result->Data.Normal = rhi::invalid_resource_index;
            }

            if (auto& Texture{Pbr->GetTexture(LPbrMaterialTemplate::ETextureType::Arm, *Prefs.TextureQuality)}; !Texture.empty())
            {
                Result->LoadedTextures.emplace_back(this->TextureSubsystem->FromFile(
                      Namespace
                    , Texture.generic_string()
                    , {.Format=vk::Format::eR8G8B8A8Unorm}
                    , {.Samples=vk::SampleCountFlagBits::e1}
                    ));
                this->TextureSubsystem->AddTextureToGlobalBindlessArray(&*Result->LoadedTextures.back());
                Result->Data.Arm = Result->LoadedTextures.back()->GetBindlessIndex();
            }
            else
            {
                Result->Data.Arm = rhi::invalid_resource_index;
            }

            if (auto& Texture{Pbr->GetTexture(LPbrMaterialTemplate::ETextureType::Emissive, *Prefs.TextureQuality)}; !Texture.empty())
            {
                Result->LoadedTextures.emplace_back(this->TextureSubsystem->FromFile(
                      Namespace
                    , Texture.generic_string()
                    , {.Format=vk::Format::eR8G8B8A8Srgb}
                    , {.Samples=vk::SampleCountFlagBits::e1}
                    ));
                this->TextureSubsystem->AddTextureToGlobalBindlessArray(&*Result->LoadedTextures.back());
                Result->Data.Emissive = Result->LoadedTextures.back()->GetBindlessIndex();
            }
            else
            {
                Result->Data.Emissive = rhi::invalid_resource_index;
            }

            Result->Data.Sampler = Pbr->Sampler;
            Result->Data.Flags = Pbr->Flags;

            Result->Data.UVScale = Pbr->UVScale;
            Result->Data.UVOffset = Pbr->UVOffset;

            this->AddInstanceToGlobalBindlessArray(Result.get());
            return Result;
        }
    }
    LOG_FATAL(LogMaterialSubsystem, "[{}/{}]: No such PBR material.", *Namespace, Name)
}

void Jafg::JMaterialSubsystem::AddInstanceToGlobalBindlessArray(LPbrMaterialInstance* Instance)
{
    check(Instance)
    check(Instance->Instance)
    check(!Instance->IsBindless())

    if (auto R{this->_Vk_AddInstanceToGlobalBindlessArray(Instance)})
    {
        Instance->Index = *R;
    }
    else
    {
        Instance->Index = rhi::invalid_resource_index;
    }
    if (!Instance->IsBindless())
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: Failed to make instance bindless. Out of binding points.", Instance->Instance->Material->Template.identifier)
    }

    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Binding resource to global bindless material array slot [{}].", Instance->Instance->Material->Template.identifier, Instance->GetBindlessIndex())
}

std::optional<rhi::resource_index_t> Jafg::JMaterialSubsystem::_Vk_AddInstanceToGlobalBindlessArray(LPbrMaterialInstance* Instance)
{
    check(Instance)

    u64 Idx{this->FreeBindlessPbrMaterialArraySlots.allocate()};
    if (Idx == algo::dynamic_bit_set::npos)
    {
        return {};
    }
    check(Idx < this->PbrInstanceLimit) // Unlikely. Here then reallocate or mark to reallocate on draw?

    return static_cast<rhi::resource_index_t>(Idx);
}

void Jafg::JMaterialSubsystem::RemoveInstanceFromGlobalBindlessArray(LPbrMaterialInstance* Instance)
{
    checkNoEntry()
}

void Jafg::JMaterialSubsystem::UpdateMaterialsFor(std::size_t Sync)
{
    auto& Buffer{this->PbrInstanceBuffers[Sync]};
    check(Buffer.data())

    u8 const Bit{static_cast<u8>(1uz << Sync)};

    for (auto& Instance: this->PbrInstances)
    {
        if (Instance->Dirty & Bit)
        {
            check(Instance->IsBindless())
            Instance->Data.upload_at(Buffer, Instance->GetBindlessIndex());
            Instance->Dirty &= static_cast<u8>(~Bit);
        }
    }
}

void Jafg::JMaterialSubsystem::RetrieveMaterialsFrom(LPluginInfo Namespace)
{
    check(Namespace)

    if (this->Templates.contains(Namespace))
    {
        check(this->PbrTemplates.contains(Namespace))
        LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Materials are already known. Skipping.", *Namespace)
        return;
    }
    check(!this->PbrTemplates.contains(Namespace))

    LPath const& Path{**Namespace};

    STAT_CUSTOM_ZONE_FMT("RetrieveMaterialsFrom{}", Path.string())

    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Retrieving materials.", *Namespace)

    TArray<UnresolvedMaterial> Materials;
    for (LPath& Material: finder::retrieve_files<finder::recursive_directory_iterator>(Path, "^.*\\.material\\.json$"))
    {
        UnresolvedMaterial& Result{Materials.emplace_back()};
        json json = json::parse(finder::read_file(Material), nullptr, false);
        if (json.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: Invalid JSON.", Material)
        }
        json.get_to(Result);
        Result.Identifier = finder::relative(canonical(Material), Path).replace_extension().replace_extension().string();
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

    TArray Resolved{::ResolveInheritance(*this->ShaderSubsystem, Namespace, std::move(Materials))};
    TArray<LPbrMaterialTemplate> PbrMaterials;

    for (LPath& Material: finder::retrieve_files<finder::recursive_directory_iterator>(Path, "^(?:.*/)?(?:[^/]+\\.)?pbr\\.json$")) // TODO: Make this for windows...
    {
        LPbrMaterialTemplate& Result{PbrMaterials.emplace_back()};

        json json = json::parse(finder::read_file(Material), nullptr, false);
        if (json.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "[{}]: Invalid JSON.", Material)
        }
        json.get_to(Result);

        check(Material.has_parent_path())
        LPath Parent{Material.parent_path()};

        check(Result.Material.first.empty() || !Result.Material.second.empty())

        if (Result.Textures.empty())
        {
            auto TextureSizeToQuality{[](auto Q)
            {
                switch (Q)
                {
                case 8: return ETextureQualityBits::Cinematic;
                case 4: return ETextureQualityBits::Ultra;
                case 2: return ETextureQualityBits::Standard;
                case 1: return ETextureQualityBits::Mobile;
                default: break;
                }
                LOG_FATAL(LogMaterialSubsystem, "Unsupported texture quality [{}].", Q)
            }};

            std::array<std::size_t, 4> Qualities{ 8,4,2,1 };
            for (std::size_t Quality: Qualities)
            {
                //# Relative to the content dir. Empty if there is no such file.
                auto Target{[&Parent, &Path](LString const& File) -> LPath
                {
                    LPath const Absolute{finder::weakly_canonical(Parent/File)};
                    return is_regular_file(Absolute) ? finder::relative(Absolute, Path) : LPath{};
                }};
                LPath TargetAlbedo{Target(algo::sprintf("Albedo_{}k.png", Quality))};
                LPath TargetNormal{Target(algo::sprintf("Normal_{}k.png", Quality))};
                LPath TargetArm{Target(algo::sprintf("Arm_{}k.png", Quality))};
                LPath TargetEmissive{Target(algo::sprintf("Emissive_{}k.png", Quality))};

                if (!(TargetAlbedo.empty() && TargetNormal.empty() && TargetArm.empty() && TargetEmissive.empty()))
                {
                    Result.Textures.emplace_back(TextureSizeToQuality(Quality), LPbrMaterialTemplate::TextureBindings{
                        .Albedo = std::move(TargetAlbedo),
                        .Normal = std::move(TargetNormal),
                        .Arm = std::move(TargetArm),
                        .Emissive = std::move(TargetEmissive),
                        });
                }
            }
        }
        else
        {
            //# Relative to the content dir.
            auto ToContent{[&Parent, &Path](LPath& Texture)
            {
                if (!Texture.native().empty())
                {
                    Texture = finder::relative(finder::weakly_canonical(Parent/Texture), Path);
                }
            }};
            for (auto& TextureQuality: Result.Textures | std::views::values)
            {
                ToContent(TextureQuality.Albedo);
                ToContent(TextureQuality.Normal);
                ToContent(TextureQuality.Arm);
                ToContent(TextureQuality.Emissive);
            }
        }

        if (LPath Temp{finder::relative(canonical(Material), Path).replace_extension()};
            Temp.native().ends_with(LITERAL_TEXT("/pbr")))
        {
            Result.Identifier = Temp.parent_path().string();
        }
        else
        {
            Result.Identifier = Temp.replace_extension().string();
        }

        check(algo::count(PbrMaterials, Result.Identifier, &LPbrMaterialTemplate::Identifier) == 1)

        LOG_VERBOSE(LogMaterialSubsystem, "[{}/{}]: Fetched PBR material.", *Namespace, Result.Identifier)
    }

    check(!this->Templates.contains(Namespace))
    check(!this->PbrTemplates.contains(Namespace))

    this->Templates[Namespace] = std::move(Resolved);
    this->PbrTemplates[Namespace] = std::move(PbrMaterials);
    LOG_VERBOSE(LogMaterialSubsystem, "[{}]: Fetched [{}+{}] materials.", *Namespace
        , this->Templates.at(Namespace).size(), this->PbrTemplates.at(Namespace).size()
        )
}

void Jafg::JMaterialSubsystem::PrepareBindless()
{
    auto& Frontend{this->GetFrontend()};

    check(this->FreeBindlessPbrMaterialArraySlots.words().empty())
    this->FreeBindlessPbrMaterialArraySlots = algo::dynamic_bit_set{this->PbrInstanceLimit};
    check(this->FreeBindlessPbrMaterialArraySlots.bit_count() == this->PbrInstanceLimit)

    this->PbrInstanceBuffers = Frontend.Vk_CreateFrequentMappedBuffer(SSBO::BindlessPbrMaterial::buffer_create_info(this->PbrInstanceLimit));
    check(algo::all_of(this->PbrInstanceBuffers, [](auto const& Buffer)
    {
        return !!Buffer.data();
    }))

    TArray<vk::DescriptorSetLayout> FrequentLayouts;
    auto AppendFrequentLayout{[&](vk::DescriptorSetLayout Layout)
    {
        for (auto Frame{0uz}; Frame < Frontend.Vk_GetNumberOfFramesInFlight(); ++Frame)
        {
            FrequentLayouts.emplace_back(Layout);
        }
    }};
    AppendFrequentLayout(this->GetSharedDescriptorSetLayout<SSBO::BindlessPbrMaterial>());

    std::vector Sets{rhi::vk_allocate(Frontend.Vk_GetDevice(), vk::DescriptorSetAllocateInfo{
        .descriptorPool = Frontend.Vk_GetDescriptorPool(),
        .descriptorSetCount = static_cast<u32>(FrequentLayouts.size()),
        .pSetLayouts = FrequentLayouts.data(),
        })};

    auto Idx{0uz};
    auto RetrieveFrequentSet{[&](auto&& Target)
    {
        for (auto Frame{0uz}; Frame < Frontend.Vk_GetNumberOfFramesInFlight(); ++Frame)
        {
            check(Idx < Sets.size())
            Target[Frame] = std::move(Sets[Idx++]);
        }
    }};
    RetrieveFrequentSet(this->PbrInstanceDescriptorSets);

    check(algo::all_of(Sets, [](auto const& Set)
    {
        return !*Set;
    }))

    std::vector<vk::WriteDescriptorSet> Writes;
    Writes.reserve(Frontend.Vk_GetNumberOfFramesInFlight());
    for (auto Idx{0uz}; Idx < Frontend.Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        vk::DescriptorBufferInfo BufferInfo{SSBO::BindlessPbrMaterial::write_info(*this->PbrInstanceBuffers[Idx], 0, this->PbrInstanceLimit)};
        Writes.emplace_back(vk::WriteDescriptorSet{
            .dstSet = *this->PbrInstanceDescriptorSets[Idx],
            .dstBinding = SSBO::BindlessPbrMaterial::MaterialBinding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = SSBO::BindlessPbrMaterial::descriptor_type(),
            .pBufferInfo = &BufferInfo,
            });
    }
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
}
