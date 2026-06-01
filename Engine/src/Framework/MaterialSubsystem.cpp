// Copyright mzoesch. All rights reserved.

#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Engine/Engine.h"
#include "Framework/ShaderSubsystem.h"
#include "User/LocalEgo.h"
#include "Stats/Stats.h"
#include "Rhi/GraphicsPipelineFactory.h"

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

    check(this->ShaderSubsystem)

    // TODO: We need to track changes and then change device information accordingly/rebuild pipelines, etc.
    check(this->FetchedMaterials.empty())

    LString MissingKey; Json::EError Error;
    for (auto MaterialFiles{Finder::FindFilesRecursively("Content/Materials", true, ".*\\.json")}; auto const& MaterialFile : MaterialFiles)
    {
        json MaterialJson = json::parse(Finder::ReadFile(MaterialFile), nullptr, false);
        if (MaterialJson.is_discarded())
        {
            LOG_FATAL(LogMaterialSubsystem, "Material [{}] is not valid json. Failed to load.", MaterialFile)
        }

        if (Json::DoesObjectContainTypeCheckedKeys(MaterialJson, {{"Shader", Json::LKeyType::String}}, &MissingKey, &Error) == false)
        {
            Json::DefaultFail(MaterialFile, MissingKey, Error);
        }

        std::unique_ptr Material{std::make_unique<LFetchedMaterial>(
              MaterialFile
            , MaterialFile.stem().string()
            , this->ShaderSubsystem->GetFetchedShader(MaterialJson["Shader"].get<LString>())
            )};

        if (MaterialJson.contains("Type"))
        {
            if (MaterialJson["Type"].is_string() == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Type entry is not a string. Failed to load.", Material->Path)
            }
            if (LString Type{MaterialJson["Type"].get<LString>()}; Type == "Solid")
            {
                Material->Type = LFetchedMaterial::Solid;
            }
            else if (Type == "Translucent")
            {
                Material->Type = LFetchedMaterial::Translucent;
            }
            else
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: No such material type [{}]. Failed to load.", Material->Path, Type)
            }
        }

        if (MaterialJson.contains("Properties"))
        {
            if (MaterialJson["Properties"].is_object() == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: Properties entry is not an object. Failed to load.", Material->Path)
            }
            for (auto const& Property : MaterialJson["Properties"].items())
            {
                if (Property.value().is_string() == false)
                {
                    LOG_FATAL(LogMaterialSubsystem, "[{}]: Property value for key [{}] is not a string. Failed to load.", Material->Path, Property.key())
                }
                Material->Properties.emplace_back(LFetchedMaterial::Property{Property.key(), Property.value().get<LString>()});
            }
        }

        check(algo::contains(this->FetchedMaterials, Material->Name, [](std::unique_ptr<LFetchedMaterial> const& M){ return M->Name; }) == false)

        if constexpr (IS_COMPILED_LOG(LogMaterialSubsystem, Trace))
        LOG_TRACE(LogMaterialSubsystem, "[{}]: Shader [{}], Properties [{}]."
            , Material->Path
            , Material->FetchedShader.Name
            , algo::join(Material->Properties, [](auto const& P){ return P.Key + "=" + P.Value; })
            )

        this->FetchedMaterials.emplace_back(std::move(Material));
        continue;
    }

    LOG_VERBOSE(LogMaterialSubsystem, "Finished loading [{}] materials.", this->Materials.size())
    return;
}

void Jafg::JMaterialSubsystem::PurgeUnused()
{
    std::erase_if(this->MaterialInstances, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused material instance [{}].", E.first)
            return true;
        }

        return false;
    });
    std::erase_if(this->Materials, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused material [{}].", E.first)
            return true;
        }

        return false;
    });

    return;
}

Jafg::LFetchedMaterial const& Jafg::JMaterialSubsystem::GetFetchedMaterial(LString const& Name) const noexcept
{
    auto It{algo::find(this->FetchedMaterials, Name, [](std::unique_ptr<LFetchedMaterial> const& M){ return M->Name; })};
    if (It == this->FetchedMaterials.end())
    {
        LOG_FATAL(LogMaterialSubsystem, "No such material [{}].", Name)
    }
    return **It;
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
    auto const& FetchedMaterial{this->GetFetchedMaterial(Name)};
    std::shared_ptr Material{std::make_shared<LMaterial>(LMaterial{.FetchedMaterial=FetchedMaterial})};

    auto& FetchedShader{FetchedMaterial.FetchedShader};
    auto Factory{LDevicePipelineFactory{Frontend}};
    Factory.Shader(FetchedShader.GetDst(), FetchedShader.Entrypoints);

    if (FetchedShader.VertexInput.has_value())
    {
        Factory.VertexInput(this->ShaderSubsystem->GetVertexInputStateCreateInfo(*FetchedShader.VertexInput));
    }

    if (FetchedShader.PipelineInputAssemblyState.has_value() == false)
    {
        LOG_FATAL(LogMaterialSubsystem
            , "[{}]: No pipeline input assembly state specified for this shader. Failed to create material."
            , FetchedMaterial.Path
            )
    }
    Factory.InputAssemblyInfo = *FetchedShader.PipelineInputAssemblyState;
    if (FetchedShader.PipelineDepthStencilState.has_value() == false)
    {
        LOG_FATAL(LogMaterialSubsystem
            , "[{}]: No pipeline depth stencil state specified for this shader. Failed to create material."
            , FetchedMaterial.Path
            )
    }
    Factory.DepthStencilInfo = *FetchedShader.PipelineDepthStencilState;

    switch (FetchedMaterial.Type)
    {
    case LFetchedMaterial::Solid:
    {
        check(Factory.PipelineColorBlendStateCreateInfo == nullptr)
        Factory.PipelineColorBlendStateCreateInfo = &this->GetSolidColorBlending();
        break;
    }
    case LFetchedMaterial::Translucent:
    {
        check(Factory.PipelineColorBlendStateCreateInfo == nullptr)
        Factory.PipelineColorBlendStateCreateInfo = &this->GetTranslucentBlending();
        break;
    }
    default:
    {
        LOG_FATAL(LogMaterialSubsystem, "[{}]: Missing implementation for material type [{}].", FetchedMaterial.Path, std::to_string(FetchedMaterial.Type))
    }
    }

    for (auto const& Layout : FetchedShader.Layouts)
    {
        if (Layout.Type == LFetchedShader::Layout::eUnique)
        {
            TArray<vk::DescriptorSetLayoutBinding> Bindings; Bindings.reserve(Layout.Sets->size());
            check(Layout.Stage.has_value())
            check(Layout.Sets.has_value())
            for (auto Idx{0uz}; Idx < Layout.Sets->size(); ++Idx)
            {
                auto const& Set{(*Layout.Sets)[Idx]};
                Bindings.emplace_back(vk::DescriptorSetLayoutBinding{
                    .binding = static_cast<u32>(Idx),
                    .descriptorType = Set.DescriptorType,
                    .descriptorCount = 1,
                    .stageFlags = *Layout.Stage,
                    .pImmutableSamplers = nullptr
                    });
            }
            Factory.UniqueLayout({
                .bindingCount = static_cast<u32>(Bindings.size()),
                .pBindings = Bindings.data(),
                });
        }
        else if (Layout.Type == LFetchedShader::Layout::eShared)
        {
            check(Layout.Identifier.has_value())
            auto& SharedLayouts{Frontend.Vk_GetDescriptorSetLayouts()};
            if (auto It2{SharedLayouts.find(*Layout.Identifier)}; It2 == SharedLayouts.end())
            {
                LOG_FATAL(LogMaterialSubsystem, "[{}]: No such shared layout [{}].", FetchedMaterial.Path, *Layout.Identifier)
            }
            else
            {
                Factory.SharedLayout(*It2->second);
            }
        }
        else
        {
            std::unreachable();
        }
    }

    for (auto const& PushConstant : FetchedShader.PushConstants)
    {
        Factory.PushConstant(this->ShaderSubsystem->GetPushConstantInfo(PushConstant));
    }

    Material->Pipeline = Factory.Build();

    this->Materials.emplace(FetchedMaterial.Name, Material);
    return Material;
}

Jafg::LMaterialInstanceRef Jafg::JMaterialSubsystem::GetInstance(LMaterialRef Material)
{
    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    auto& FetchedMaterial{Material->FetchedMaterial};
    auto& FetchedShader{FetchedMaterial.FetchedShader};

    auto Instance{std::make_shared<LMaterialInstance>(LMaterialInstance{.Material = Material})};

    auto UniqueIdx{0uz};
    TArray<vk::DescriptorSetLayout> LayoutsToAllocate;
    for (auto Idx{0uz}; Idx < FetchedShader.Layouts.size(); ++Idx)
    {
        auto& Layout{FetchedShader.Layouts[Idx]};
        if (Layout.Type == LFetchedShader::Layout::eShared)
        {
            continue;
        }

        if (Layout.UpdateFrequency == LFetchedShader::Layout::ePerFrame)
        {
            check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)
            for (auto FramesInFlight{0uz}; FramesInFlight < Frontend.Vk_GetNumberOfFramesInFlight(); ++FramesInFlight)
            {
                LayoutsToAllocate.emplace_back(*Material->Pipeline._UniqueDescriptorSetLayout[UniqueIdx]);
            }
        }
        else if (Layout.UpdateFrequency == LFetchedShader::Layout::eRarely)
        {
            LayoutsToAllocate.emplace_back(*Material->Pipeline._UniqueDescriptorSetLayout[UniqueIdx]);
        }
        else
        {
            std::unreachable();
        }

        ++UniqueIdx;
        continue;
    }
    check(UniqueIdx == Material->Pipeline._UniqueDescriptorSetLayout.size())

    if (LayoutsToAllocate.empty() == false)
    {
        auto SetIdx{0uz};
        auto DescriptorSets{Frontend.Vk_GetDevice().allocateDescriptorSets({
            .descriptorPool = Frontend.Vk_GetDescriptorPool(),
            .descriptorSetCount = static_cast<u32>(LayoutsToAllocate.size()),
            .pSetLayouts = LayoutsToAllocate.data(),
            })};

        for (auto Idx{0uz}; Idx < FetchedShader.Layouts.size(); ++Idx)
        {
            auto& Layout{FetchedShader.Layouts[Idx]};
            if (Layout.Type == LFetchedShader::Layout::eShared)
            {
                continue;
            }
            if (Layout.UpdateFrequency == LFetchedShader::Layout::ePerFrame)
            {
                check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)
                for (auto FramesInFlight{0uz}; FramesInFlight < Frontend.Vk_GetNumberOfFramesInFlight(); ++FramesInFlight)
                {
                    Instance->FrequentDescriptorSets[FramesInFlight].emplace_back(static_cast<u32>(Idx), std::move(DescriptorSets[SetIdx++]));
                }
            }
            else if (Layout.UpdateFrequency == LFetchedShader::Layout::eRarely)
            {
                Instance->InfrequentDescriptorSets.emplace_back(static_cast<u32>(Idx), std::move(DescriptorSets[SetIdx++]));
            }
            else
            {
                std::unreachable();
            }

            continue;
        }

        check(SetIdx == DescriptorSets.size())
    }

    for (auto const& [Key, Value] : FetchedMaterial.Properties)
    {
        this->SetMaterialInstanceField(*Instance, this->GetBinding(*Instance, Key), Value);
    }

    return Instance;
}

Jafg::JMaterialSubsystem::LBinding Jafg::JMaterialSubsystem::GetBinding(LMaterialInstance& Instance, LStringView Key)
{
    check(this->ShaderSubsystem)
    check(Instance.Material.get())

    auto& FetchedShader{Instance.Material->FetchedMaterial.FetchedShader};
    for (auto LayoutIdx{0uz}; LayoutIdx < FetchedShader.Layouts.size(); ++LayoutIdx)
    {
        auto& Layout{FetchedShader.Layouts[LayoutIdx]};
        if (Layout.Sets.has_value() == false)
        {
            check(Layout.Identifier.has_value() && Layout.Type == LFetchedShader::Layout::eShared)
            continue;
        }
        for (auto SetIdx{0uz}; SetIdx < Layout.Sets->size(); ++SetIdx)
        {
            auto& Set{(*Layout.Sets)[SetIdx]};
            if (Set.Identifier != Key)
            {
                continue;
            }
            return {
                .Type = Set.DescriptorType,
                .Layout = static_cast<u32>(LayoutIdx),
                .Set = static_cast<u32>(SetIdx),
                };
        }
    }

    LOG_FATAL(LogMaterialSubsystem
        , "[{}]: No such set in any layout [{}]."
        , Instance.Material->FetchedMaterial.Name, Key
        )
}

void Jafg::JMaterialSubsystem::SetMaterialInstanceField(LMaterialInstance& Instance, LBinding Where, LString const& Value) const noexcept
{
    switch (Where.Type)
    {
    case vk::DescriptorType::eSampler:
    {
        if (Value == "Jafg.LinearRepeatSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetLinearSamplerRepeat());
        }
        else if (Value == "Jafg.LinearMirroredRepeatSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetLinearSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.LinearClampToEdgeSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetLinearSamplerClampToEdge());
        }
        else if (Value == "Jafg.LinearClampToBorderSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetLinearSamplerClampToBorder());
        }
        else if (Value == "Jafg.NearestRepeatSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetNearestSamplerRepeat());
        }
        else if (Value == "Jafg.NearestMirroredRepeatSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetNearestSamplerMirroredRepeat());
        }
        else if (Value == "Jafg.NearestClampToEdgeSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetNearestSamplerClampToEdge());
        }
        else if (Value == "Jafg.NearestClampToBorderSampler")
        {
            this->SetSampler(Instance, Where, this->GetLocalEgo().GetFrontend().Vk_GetNearestSamplerClampToBorder());
        }
        else
        {
            LOG_FATAL(LogMaterialSubsystem
                , "[{}]: No such sampler identifier [{}]. Failed to set value [{}] for set identifier [{}]."
                , Instance.Material->FetchedMaterial.Name, Value, Value, Where.Set
                )
        }
        break;
    }
    case vk::DescriptorType::eSampledImage:
    {
        this->SetSampledImage(Instance, Where, *this->TextureSubsystem->FromTextureViewIdentifier(Value));
        break;
    }
    default:
    {
        LOG_FATAL(LogMaterialSubsystem
            , "[{}]: Unsupported descriptor type [{}] for set identifier [{}]. Failed to set value [{}]."
            , Instance.Material->FetchedMaterial.Name, vk::to_string(Where.Type), Where.Set, Value
            )
    }
    }
}

void Jafg::JMaterialSubsystem::SetSampler(LMaterialInstance& Instance, LBinding Where, vk::Sampler const& Sampler) const
{
    auto& Frontend{this->GetFrontend()};

    auto It{algo::find(Instance.InfrequentDescriptorSets, Where.Layout, [](auto const& E){ return E.first; })};
    check(It != Instance.InfrequentDescriptorSets.end())

    vk::DescriptorImageInfo ImageInfo{
        .sampler = Sampler,
        };
    check(ImageInfo.imageView == nullptr && ImageInfo.imageLayout == vk::ImageLayout::eUndefined)
    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = *It->second,
            .dstBinding = Where.Set,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampler,
            .pImageInfo = &ImageInfo,
            },
        };
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    return;
}

void Jafg::JMaterialSubsystem::SetSampledImage(LMaterialInstance& Instance, LBinding Where, LTexture2 const& Texture) const
{
    auto& Frontend{this->GetFrontend()};

    auto It{algo::find(Instance.InfrequentDescriptorSets, Where.Layout, [](auto const& E){ return E.first; })};
    check(It != Instance.InfrequentDescriptorSets.end())

    vk::DescriptorImageInfo ImageInfo{
        .imageView = Texture.GetImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    check(ImageInfo.sampler == nullptr)
    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = *It->second,
            .dstBinding = Where.Set,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &ImageInfo,
            },
        };
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    return;
}
