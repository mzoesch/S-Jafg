// Copyright mzoesch. All rights reserved.

#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Engine/Engine.h"
#include "Framework/ShaderSubsystem.h"
#include "User/LocalEgo.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"

void Jafg::JMaterialSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<JTextureSubsystem>(this);
    Collection.InitializeDependency<JShaderSubsystem>(this);

    this->TextureSubsystem = Collection.GetSubsystemChecked<JTextureSubsystem>();
    this->ShaderSubsystem = Collection.GetSubsystemChecked<JShaderSubsystem>();

    this->ReloadMaterials();

    return;
}

void Jafg::JMaterialSubsystem::ReloadMaterials()
{
    STAT_CYCLE_FUNCTION()
    LOG_VERBOSE(LogMaterialSubsystem, "Reloading materials.")

    algo::orphan(&this->FetchedMaterials);

    LString MissingKey; Json::EError Error;
    for (auto MaterialFiles{Finder::FindFilesRecursively("Content/Materials", true, ".*\\.mat.json")}; auto const& MaterialFile : MaterialFiles)
    {
        LFetchedMaterial Material{.Path=MaterialFile,.Name=MaterialFile.stem().stem().string()};

        json MaterialJson = json::parse(Finder::ReadFile(Material.Path), nullptr, false);
        if (MaterialJson.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "Material [{}] is not valid json. Failed to load.", Material.Path)
        }

        if (Json::DoesObjectContainTypeCheckedKeys(MaterialJson, {{"Shader", Json::LKeyType::String}}, &MissingKey, &Error) == false)
        {
            Json::DefaultFail(Material.Path, MissingKey, Error);
        }

        Material.Shader = MaterialJson["Shader"].get<LString>();

        if (MaterialJson.contains("Properties"))
        {
            if (MaterialJson["Properties"].is_object() == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "Material [{}]: Properties entry is not an object. Failed to load.", Material.Path)
            }
            for (auto const& Property : MaterialJson["Properties"].items())
            {
                if (Property.value().is_string() == false)
                {
                    LOG_FATAL(LogMaterialSubsystem, "Material [{}]: Property value for key [{}] is not a string. Failed to load.", Material.Path, Property.key())
                }
                Material.Properties.emplace_back(LFetchedMaterial::Property{Property.key(), Property.value().get<LString>()});
            }
        }

        check(algo::contains(this->FetchedMaterials, Material.Path, &LFetchedMaterial::Path) == false)
        check(algo::contains(this->FetchedMaterials, Material.Name, &LFetchedMaterial::Name) == false)

        if constexpr (IS_COMPILED_LOG(LogMaterialSubsystem, Trace))
        LOG_TRACE(LogMaterialSubsystem, "[{}]: Shader [{}], Properties [{}]."
            , Material.Path
            , Material.Shader
            , algo::join(Material.Properties, [](auto const& P){ return P.Key + "=" + P.Value; })
            )

        this->FetchedMaterials.emplace_back(std::move(Material));
        continue;
    }

    LOG_VERBOSE(LogMaterialSubsystem, "Finished loading [{}] materials.", this->Materials.size())
    return;
}

Jafg::LFetchedMaterial const& Jafg::JMaterialSubsystem::GetFetchedMaterial(LString const& Name) const noexcept
{
    auto It{algo::find(this->FetchedMaterials, Name, &LFetchedMaterial::Name)};
    if (It == this->FetchedMaterials.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "No such material [{}].", Name)
    }
    return *It;
}

Jafg::LMaterialRef Jafg::JMaterialSubsystem::GetMaterial(LString const& Name) noexcept
{
    check(this->ShaderSubsystem && this->TextureSubsystem)

    {
        auto It{this->Materials.find(Name)};
        if (It != this->Materials.end())
        {
            return It->second;
        }
    }

    auto& Frontend{this->GetLocalEgo().GetFrontend()};

    auto It{algo::find(this->FetchedMaterials, Name, &LFetchedMaterial::Name)};
    if (It == this->FetchedMaterials.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "No such material [{}].", Name)
    }
    auto& FetchedMaterial{*It};
    std::shared_ptr Material{std::make_shared<LMaterial>(LMaterial{.FetchedMaterial=It->Name})};

    auto& FetchedShader{this->ShaderSubsystem->GetFetchedShader(FetchedMaterial.Shader)};
    auto Factory{LDevicePipelineFactory{Frontend}};
    Factory.Shader(FetchedShader.GetDst(), FetchedShader.Entrypoints);

    if (FetchedShader.VertexInput.has_value() == false)
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: No vertex input specified in shader [{}]. Failed to create material.", FetchedMaterial.Path, FetchedShader.Path)
    }
    Factory.VertexInput(this->ShaderSubsystem->GetVertexInputStateCreateInfo(*FetchedShader.VertexInput));

    for (auto const& Layout : FetchedShader.Layouts)
    {
        if (Layout.Type == LFetchedShader::Layout::Unique)
        {
            TArray<vk::DescriptorSetLayoutBinding> Bindings; Bindings.reserve(Layout.Sets->size());
            check(Layout.Sets.has_value())
            for (auto Idx{0uz}; Idx < Layout.Sets->size(); ++Idx)
            {
                auto const& Set{(*Layout.Sets)[Idx]};
                Bindings.emplace_back(vk::DescriptorSetLayoutBinding{
                    .binding = static_cast<u32>(Idx),
                    .descriptorType = Set.DescriptorType,
                    .descriptorCount = 1,
                    .stageFlags = Set.Stage,
                    .pImmutableSamplers = nullptr
                    });
            }
            Factory.UniqueLayout({
                .bindingCount = static_cast<u32>(Bindings.size()),
                .pBindings = Bindings.data(),
                });
        }
        else if (Layout.Type == LFetchedShader::Layout::Shared)
        {
            check(Layout.Identifier.has_value())
            if (Layout.Identifier == "PerspectiveCamera")
            {
                Factory.SharedLayout(Frontend.Vk_GetPerspectiveCameraDescriptorSetLayout());
            }
            else
            {
                checkNoEntry()
            }
        }
        else
        {
            unreachable()
        }
    }

    for (auto const& PushConstant : FetchedShader.PushConstants)
    {
        Factory.PushConstant(this->ShaderSubsystem->GetPushConstantInfo(PushConstant));
    }

    Material->Pipeline = Factory.Build();

    this->Materials[It->Name] = Material;
    return Material;
}

void Jafg::JMaterialSubsystem::SetMaterialInstanceField(LMaterialInstance& Instance, LString const& Key, LString const& Value, LFetchedShader const& FetchedShader) const noexcept
{
    LOG_VERBOSE(LogMaterialSubsystem, "Setting material instance field [{}] to [{}].", Key, Value)

    check(this->ShaderSubsystem)
    check(Instance.Material.get())
    checkCode
    (
        if (this->GetFetchedMaterial(Instance.Material->FetchedMaterial).Shader != FetchedShader.Name)
        {
            LOG_FATAL(LogMaterialSubsystem
                , "Fetched shader [{}] does not match the shader [{}] specified in the material [{}]. Failed to set material instance field [{}] to [{}]."
                , FetchedShader.Name, this->GetFetchedMaterial(Instance.Material->FetchedMaterial).Shader, Instance.Material->FetchedMaterial
                , Key, Value
                )
        }
    )

    for (auto const& Layout : FetchedShader.Layouts)
    {
        if (Layout.Sets.has_value() == false)
        {
            check(Layout.Identifier.has_value() && Layout.Type == LFetchedShader::Layout::Shared)
            continue;
        }

        for (auto const& Set : *Layout.Sets)
        {
            if (Set.Identifier != Key)
            {
                continue;
            }

            switch (Set.DescriptorType)
            {
            case vk::DescriptorType::eCombinedImageSampler:
            {
                this->SetCombinedImageSampler(Instance, Key, *this->TextureSubsystem->FromTextureViewIdentifier(Value));
                return;
            }
            default:
            {
                LOG_FATAL(LogMaterialSubsystem
                    , "[{}]: Unsupported descriptor type [{}] for set identifier [{}]., Failed to set value [{}]."
                    , Instance.Material->FetchedMaterial, vk::to_string(Set.DescriptorType), Key, Value
                    )
            }
            }

            return;
        }
    }

    LOG_FATAL(LogMaterialSubsystem
        , "[{}]: No such set in any layout [{}]. Failed to set value [{}]."
        , Instance.Material->FetchedMaterial, Key, Value
        )
}

void Jafg::JMaterialSubsystem::SetCombinedImageSampler(LMaterialInstance& Instance, LStringView Where, LTexture2 const& Texture) const
{
    check(Instance.Material.get())
    check(this->ShaderSubsystem)

    auto& Material{this->GetFetchedMaterial(Instance.Material->FetchedMaterial)};
    auto& Shader{this->ShaderSubsystem->GetFetchedShader(Material.Shader)};
    auto LayoutIdx{0uz};
    for (auto const& Layout : Shader.Layouts)
    {
        if (Layout.Sets.has_value() == false)
        {
            continue;
        }

        for (auto SetIdx{0uz}; SetIdx < Layout.Sets->size(); ++SetIdx)
        {
            auto& Set{(*Layout.Sets)[SetIdx]};
            if (Set.Identifier != Where)
            {
                continue;
            }
            if (Set.DescriptorType != vk::DescriptorType::eCombinedImageSampler)
            {
                LOG_FATAL(LogMaterialSubsystem
                    , "Material [{}] has a set with identifier [{}], but it is not a combined image sampler. Failed to set combined image sampler."
                    , Material.Name, Where
                    )
            }

            auto& Frontend{this->GetFrontend()};

            vk::DescriptorImageInfo ImageInfo{
                .sampler = Frontend.Vk_GetDefaultSampler(),
                .imageView = Texture.GetImageView(),
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
                };

            std::array Writes{
                vk::WriteDescriptorSet{
                    .dstSet = *Instance._UniqueDescriptorSets[LayoutIdx],
                    .dstBinding = static_cast<u32>(SetIdx),
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                    .pImageInfo = &ImageInfo,
                    },
                };
            Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

            return;
        }

        ++LayoutIdx;
        continue;
    }

    LOG_FATAL(LogMaterialSubsystem
        , "Material [{}] does not have a combined image sampler set with identifier [{}]. Failed to set combined image sampler."
        , Material.Name, Where
        )
}

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::GetInstance(LMaterialRef Material)
{
    auto& Frontend{this->GetLocalEgo().GetFrontend()};

    auto Instance{std::make_shared<LMaterialInstance>(LMaterialInstance{.Material=Material})};
    if (Material->Pipeline._UniqueDescriptorSetLayout.empty() == false)
    {
        TArray<vk::DescriptorSetLayout> LayoutsToAllocate(Material->Pipeline._UniqueDescriptorSetLayout.size());
        algo::transform(Material->Pipeline._UniqueDescriptorSetLayout, LayoutsToAllocate.begin(), [](auto& Layout){ return *Layout; });
        Instance->_UniqueDescriptorSets = Frontend.Vk_GetDevice().allocateDescriptorSets({
            .descriptorPool = Frontend.Vk_GetDescriptorPool(),
            .descriptorSetCount = static_cast<u32>(LayoutsToAllocate.size()),
            .pSetLayouts = LayoutsToAllocate.data(),
        });
    }

    auto& FetchedMaterial{this->GetFetchedMaterial(Material->FetchedMaterial)};
    auto& Shader{this->ShaderSubsystem->GetFetchedShader(FetchedMaterial.Shader)};
    for (auto const& [Key, Value] : FetchedMaterial.Properties)
    {
        this->SetMaterialInstanceField(*Instance, Key, Value, Shader);
    }

    return Instance;
}
