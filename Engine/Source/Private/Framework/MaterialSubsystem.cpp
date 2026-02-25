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

        if (Json::DoesObjectContainTypeCheckedKeys(MaterialJson, {{"Shader", Json::LKeyType::String}, {"Layouts", Json::LKeyType::Array}}, &MissingKey, &Error) == false)
        {
            Json::DefaultFail(Material.Path, MissingKey, Error);
        }

        Material.Shader = MaterialJson["Shader"].get<LString>();

        for (json LayoutArray = MaterialJson["Layouts"]; auto const& LayoutJson : LayoutArray)
        {
            if (LayoutJson.is_array() == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "Material [{}]: Layouts entry is not an array. Failed to load.", Material.Path)
            }

            for (auto const& LayoutMetaJson : LayoutJson)
            {
                if (Json::DoesObjectContainTypeCheckedKeys(LayoutMetaJson, {{"Type", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                {
                    Json::DefaultFail(Material.Path, MissingKey, Error);
                }

                if (LString SetType{LayoutMetaJson["Type"].get<LString>()}; SetType == "Unique")
                {
                    if (Json::DoesObjectContainTypeCheckedKeys(LayoutMetaJson, {{"Sets", Json::LKeyType::Array}}, &MissingKey, &Error) == false)
                    {
                        Json::DefaultFail(Material.Path, MissingKey, Error);
                    }

                    TArray<LFetchedMaterial::Layout::Set> Sets;
                    for (auto const& SetJson : LayoutMetaJson["Sets"])
                    {
                        if (Json::DoesObjectContainTypeCheckedKeys(SetJson, {{"Identifier", Json::LKeyType::String}
                            , {"Stage", Json::LKeyType::String}, {"DescriptorType", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                        {
                            Json::DefaultFail(Material.Path, MissingKey, Error);
                        }
                        Sets.emplace_back(LFetchedMaterial::Layout::Set{
                            .Identifier=SetJson["Identifier"].get<LString>(),
                            .Stage=Vk_StringToShaderStageFlagBits(SetJson["Stage"].get<LString>()),
                            .DescriptorType=Vk_StringToDescriptorType(SetJson["DescriptorType"].get<LString>())
                            });
                        continue;
                    }

                    Material.Layouts.emplace_back(LFetchedMaterial::Layout{.Type=LFetchedMaterial::Layout::Type::Unique,.Sets=std::move(Sets)});
                    check(SetType.contains("Identifier") == false)
                }
                else if (SetType == "Shared")
                {
                    if (Json::DoesObjectContainTypeCheckedKeys(LayoutMetaJson, {{"Identifier", Json::LKeyType::String}}, &MissingKey, &Error) == false)
                    {
                        Json::DefaultFail(Material.Path, MissingKey, Error);
                    }
                    Material.Layouts.emplace_back(LFetchedMaterial::Layout{.Type=LFetchedMaterial::Layout::Type::Shared,.Identifier=LayoutMetaJson["Identifier"].get<LString>()});
                    check(SetType.contains("Set") == false)
                }
                else
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Layout entry has invalid type [{}]. Failed to load.", Material.Path, SetType)
                }

                continue;
            }

            continue;
        }

        check(algo::contains(this->FetchedMaterials, Material.Path, &LFetchedMaterial::Path) == false)
        check(algo::contains(this->FetchedMaterials, Material.Name, &LFetchedMaterial::Name) == false)
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
    {
        auto It{this->Materials.find(Name)};
        if (It != this->Materials.end())
        {
            return It->second;
        }
    }

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    auto& ShaderSubsystem{*Frontend.GetSubsystemChecked<JShaderSubsystem>()};

    auto It{algo::find(this->FetchedMaterials, Name, &LFetchedMaterial::Name)};
    if (It == this->FetchedMaterials.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "No such material [{}].", Name)
    }
    auto& FetchedMaterial{*It};
    std::shared_ptr Material{std::make_shared<LMaterial>(LMaterial{.FetchedMaterial=It->Name})};

    auto& FetchedShader{ShaderSubsystem.GetFetchedShader(FetchedMaterial.Shader)};
    auto Factory{LDevicePipelineFactory{Frontend}};
    Factory.Shader(FetchedShader.GetDst(), FetchedShader.Entrypoints);

    if (FetchedShader.VertexInput.has_value() == false)
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: No vertex input specified in shader [{}]. Failed to create material.", FetchedMaterial.Path, FetchedShader.Path)
    }
    Factory.VertexInput(ShaderSubsystem.GetVertexInputStateCreateInfo(*FetchedShader.VertexInput));

    for (auto const& Layout : FetchedMaterial.Layouts)
    {
        if (Layout.Type == LFetchedMaterial::Layout::Unique)
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
        else if (Layout.Type == LFetchedMaterial::Layout::Shared)
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
        Factory.PushConstant(ShaderSubsystem.GetPushConstantInfo(PushConstant));
    }

    Material->Pipeline = Factory.Build();

    this->Materials[It->Name] = Material;
    return Material;
}

void Jafg::JMaterialSubsystem::SetCombinedImageSampler(LMaterialInstanceRef MaterialInstance, LStringView Where, LTexture2 const& Texture) const
{
    check(MaterialInstance.get() && MaterialInstance->Material.get())

    auto& Material{this->GetFetchedMaterial(MaterialInstance->Material->FetchedMaterial)};
    auto LayoutIdx{0uz};
    for (auto const& Layout : Material.Layouts)
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
                    .dstSet = *MaterialInstance->_UniqueDescriptorSets[LayoutIdx],
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

    return Instance;
}
