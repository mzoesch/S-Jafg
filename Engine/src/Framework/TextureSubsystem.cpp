// Copyright mzoesch. All rights reserved.

#include "Framework/TextureSubsystem.h"
#include "Rhi/TextureView.h"
#include "Engine/Engine.h"
#include "Framework/MaterialSubsystem.h"
#include "Rhi/FromString.h"

namespace
{

struct UnresolvedTextureView final
{
    LPath Path;
    LString Identifier;

    bool bRecursive{};
    std::variant<std::monostate, LPath, LString> What;
    vk::Format Format{ vk::Format::eUndefined };
    std::optional<u32> MipLevels;
    std::optional<vk::SampleCountFlagBits> MaxSampleCount;

    NODISCARD FORCEINLINE constexpr bool IsFullyResolved() const noexcept
    {
        return (std::holds_alternative<LPath>(this->What) && !std::get<LPath>(this->What).empty())
            || (std::holds_alternative<LString>(this->What) && !std::get<LString>(this->What).empty());
    }
};

SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(::UnresolvedTextureView)
{
    if (j.contains("file"))
    {
        j.at("file").get_to(field.What.emplace<LPath>());
    }
    if (j.contains("regex"))
    {
        j.at("regex").get_to(field.What.emplace<LString>());
    }

    if (j.contains("recursive"))
    {
        j.at("recursive").get_to(field.bRecursive);
    }

    if (j.contains("format"))
    {
        LString FormatStr;
        j.at("format").get_to(FormatStr);
        field.Format = serde::from_string<vk::Format>(FormatStr);
    }
    else
    {
        LOG_FATAL(LogTextureSubsystem, "[format]: No such field.")
    }

    if (j.contains("mipLevels"))
    {
        j.at("mipLevels").get_to(field.MipLevels.emplace());
    }
    if (j.contains("maxSampleCount"))
    {
        LString SampleCountStr;
        j.at("maxSampleCount").get_to(SampleCountStr);
        field.MaxSampleCount = serde::from_string<vk::SampleCountFlagBits>(SampleCountStr);
    }
}

} /* ~Namespace <Anonymous> */

void Jafg::JTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->GetMutableEngine().OnForeignPluginLoaded.EmplaceStrong([this](LLoadedPlugin& Plugin)
    {
        if (Plugin.GetFetched().bHasContent)
        {
            this->RetrieveTextureViewsFrom(Plugin.GetContent());
        }

        return false;
    });

    this->RetrieveTextureViewsFrom(*JafgInfo);
}

void Jafg::JTextureSubsystem::PurgeUnused() noexcept
{
    std::erase_if(this->Textures, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused texture [{}].", E.first)
            return true;
        }

        return false;
    });

    // TODO: Do we want to rehash??
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromFile(LStringView Key, LPath const& Path
    , LTexture2::HostInfo HostCreateInfo
    , LTexture2::DeviceInfo DeviceCreateInfo
    , ETexture2State State /* = ETexture2StateBits::Device */
    )
{
    if (auto It{this->Textures.find(Key)}; It != this->Textures.end())
    {
        check(It->second->GetHostCreateInfo().Format == HostCreateInfo.Format)
        check(It->second->GetDeviceCreateInfo().DesiredMipLevels == DeviceCreateInfo.DesiredMipLevels)
        check(It->second->GetDeviceCreateInfo().Samples == DeviceCreateInfo.Samples)
        It->second->BringToState(State);
        return It->second;
    }

    auto Result{std::make_shared<LTexture2>(Path, HostCreateInfo, DeviceCreateInfo)};
    this->Textures[LString{Key}] = Result;
    Result->BringToState(State);
    return Result;
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromTextureView(LStringView Namespace, LStringView Name, ETexture2State State /* = ETexture2StateBits::Device */)
{
    auto const& View{this->GetTextureView(Namespace, Name)};
    return this->FromFile(this->GetKey(Namespace, Name), View.Texture
        , LTexture2::HostInfo{.Format=View.Format}
        , LTexture2::DeviceInfo{.DesiredMipLevels=View.MipLevels, .Samples=View.MaxSampleCount}
        , State
        );
}

Jafg::LTextureCube2Ref Jafg::JTextureSubsystem::CubeFromFile(LPath const& Path)
{
    if (auto const& It{this->CubeTextures.find(Path)}; It != this->CubeTextures.end())
    {
        check(!!It->second->GetDeviceHandle().get_allocation())
        return It->second;
    }

    this->CubeTextures[Path] = std::make_shared<LTextureCube2>(LTextureCube2::CubeMap{std::move(Path)});
    return this->CubeTextures[Path];
}

void Jafg::JTextureSubsystem::_Vk_UpdateSamplers()
{
    LOG_VERBOSE(LogVulkan, "Updating Vulkan samplers.")

    auto& Frontend{this->GetFrontend()};

    std::array<vk::DescriptorImageInfo, UBO::Bindless::SamplerCount> DescriptorImageInfos;

    vk::SamplerCreateInfo CreateInfo{
        .magFilter = vk::Filter::eLinear, .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat, .addressModeV = vk::SamplerAddressMode::eRepeat, .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        //# TODO: Make user pref.
        .anisotropyEnable = vk::True, .maxAnisotropy = Frontend.Vk_GetPhysicalDevice().getProperties().limits.maxSamplerAnisotropy,
        .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f, // Increase for worse texture quality.
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = vk::BorderColor::eFloatTransparentBlack, /* TODO: Chagned this from eFloatOpaqueWhite. Was this ok? */
        };
    this->Vk_DefaultSamplers[UBO::Bindless::LinearRepeatSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearMirroredRepeatSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToEdgeSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToBorderSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToBorderSamplerIdx],
        };

    // CreateInfo.addressModeU = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeV = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeW = vk::SamplerAddressMode::eMirrorClampToEdge;
    // this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx] = vk::raii::Sampler{Frontend.Vk_GetDevice(), CreateInfo};
    // DescriptorImageInfos[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
    //     .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx],
    //     };

    CreateInfo.magFilter = vk::Filter::eNearest;
    CreateInfo.minFilter = vk::Filter::eNearest;
    CreateInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;

    CreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestRepeatSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestMirroredRepeatSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToEdgeSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToBorderSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToBorderSamplerIdx],
        };

    // CreateInfo.addressModeU = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeV = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeW = vk::SamplerAddressMode::eMirrorClampToEdge;
    // this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = rhi::vk_build(Frontend.Vk_GetDevice(), CreateInfo);
    // DescriptorImageInfos[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
    //     .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx],
    //     };

    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = this->Vk_GetBindlessTextureArrayDescriptorSet(),
            .dstBinding = UBO::Bindless::SamplerBinding,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<u32>(DescriptorImageInfos.size()),
            .descriptorType = vk::DescriptorType::eSampler,
            .pImageInfo = DescriptorImageInfos.data(),
            },
        };
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
}

void Jafg::JTextureSubsystem::AddTextureToGlobalBindlessArray(LTexture2* Texture)
{
    check(Texture)
    check(Texture->IsOnDevice())
    check(!Texture->IsBindless())

    if (auto R{this->_Vk_AddTransientImageToGlobalBindlessArray(Texture->GetImageView())})
    {
        Texture->_SetBindlessIndex(*R);
    }
    else
    {
        Texture->_SetBindlessIndex(INDEX_NONE);
    }
    if (!Texture->IsBindless())
    {
        LOG_FATAL(LogVulkan, "[{}]: Failed to make texture bindless. Out of binding points.", Texture->GetPath())
    }
    LOG_VERBOSE(LogVulkan, "[{}]: Binding resource to global bindless texture array slot [{}].", Texture->GetPath(), Texture->GetBindlessIndex())
}

std::optional<rhi::resource_index_t> Jafg::JTextureSubsystem::_Vk_AddTransientImageToGlobalBindlessArray(vk::ImageView const& ImageView)
{
    check(!!ImageView)

    u64 Idx{this->FreeBindlessTextureArraySlots.allocate()};
    if (Idx == algo::dynamic_bit_set::npos)
    {
        return {};
    }
    check(Idx < BindlessTextureArrayCapacity) // Unlikely.

    vk::DescriptorImageInfo ImageInfo{
        .imageView = ImageView,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = *this->BindlessTextureArrayDescriptorSet,
        .dstBinding = UBO::Bindless::ArrayBinding,
        .dstArrayElement = static_cast<u32>(Idx),
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eSampledImage,
        .pImageInfo = &ImageInfo,
        }};
    this->GetFrontend().Vk_GetDevice().updateDescriptorSets(Writes, {});

    return static_cast<rhi::resource_index_t>(Idx);
}

void Jafg::JTextureSubsystem::_Vk_AllocateBindlessPool()
{
    auto& Frontend{this->GetFrontend()};

    LOG_VERBOSE(LogTextureSubsystem, "Allocating bindless texture array descriptor pool and descriptor set with capacity [{}].", BindlessTextureArrayCapacity)
    check(BindlessTextureArrayCapacity > 0)

    std::array Sizes{
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eSampler,
            .descriptorCount = UBO::Bindless::SamplerCount,
            },
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eSampledImage,
            .descriptorCount = BindlessTextureArrayCapacity,
            },
        };
    this->BindlessTextureArrayDescriptorPool = rhi::vk_build(Frontend.Vk_GetDevice(), vk::DescriptorPoolCreateInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
        .pPoolSizes = Sizes.data(),
        });

    u32 Count{BindlessTextureArrayCapacity};
    vk::DescriptorSetVariableDescriptorCountAllocateInfo CountInfo{
        .descriptorSetCount = 1,
        .pDescriptorCounts = &Count,
        };
    std::vector Sets{rhi::vk_allocate(Frontend.Vk_GetDevice(), vk::DescriptorSetAllocateInfo{
        .pNext = &CountInfo,
        .descriptorPool = this->BindlessTextureArrayDescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &*Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetSharedDescriptorSetLayouts().at(UBO::Bindless::name()),
        })};
    check(Sets.size() == 1)
    this->BindlessTextureArrayDescriptorSet = std::move(Sets[0]);

    check(this->FreeBindlessTextureArraySlots.words().empty())
    this->FreeBindlessTextureArraySlots = algo::dynamic_bit_set{BindlessTextureArrayCapacity};
    check(this->FreeBindlessTextureArraySlots.bit_count() == BindlessTextureArrayCapacity)

    TArray<std::pair<LString, LString>> GuaranteedTextureIdentifiers{ {*JafgInfo, "Textures/IdentityMul"} };
    if (!this->GuaranteedTextures.empty())
    {
        panic("Expected GuaranteedTextures to be empty.")
    }
    for (auto const& [Namespace, Name]: GuaranteedTextureIdentifiers)
    {
        this->GuaranteedTextures.emplace_back(this->FromTextureView(Namespace, Name));
    }
    for (auto& Gt: this->GuaranteedTextures)
    {
        this->AddTextureToGlobalBindlessArray(&*Gt);
    }
    std::size_t Cursor{};
    for (auto& Gt: this->GuaranteedTextures)
    {
        check(Gt->IsBindless())
        if (Gt->GetBindlessIndex() != static_cast<i64>(Cursor))
        {
            LOG_FATAL(LogVulkan, "[{}]: Expected guaranteed texture to be at bindless array slot [{}], but it is at slot [{}].", Gt->GetPath(), Cursor, Gt->GetBindlessIndex())
        }
        ++Cursor;
    }
}

void Jafg::JTextureSubsystem::RetrieveTextureViewsFrom(LPath const& Path)
{
    STAT_CUSTOM_ZONE_FMT("RetrieveTextureViewsFrom-{}", Path.string())

    LString Key{canonical(Path).string()};

    LOG_VERBOSE(LogTextureSubsystem, "[{}]: Refetching texture views.", Key)

    TArray<UnresolvedTextureView> Views;
    for (LPath& TextureView: finder::retrieve_files<finder::recursive_directory_iterator>(Path, "^(?:.*/)?(?:[^/]+\\.)?view\\.json$"))
    {
        UnresolvedTextureView& Result{Views.emplace_back()};
        json json = json::parse(finder::read_file(TextureView), nullptr, false);
        if (json.is_discarded())
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: Invalid JSON.", TextureView)
        }
        json.get_to(Result);

        if (LPath Temp{finder::relative(canonical(TextureView), Path).replace_extension()};
            Temp.native().ends_with(LITERAL_TEXT("/view")))
        {
            Result.Identifier = Temp.parent_path().generic_string();
        }
        else
        {
            Result.Identifier = Temp.replace_extension().generic_string();
        }
        Result.Path = std::move(TextureView);
        check(algo::count(Views, Result.Identifier, &UnresolvedTextureView::Identifier) == 1)
    }

    for (auto& View: Views)
    {
        if (!View.IsFullyResolved())
        {
            View.What = LPath{View.Identifier}.stem().concat(".png");
        }

        if (!View.IsFullyResolved())
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: Texture view is not fully resolved.", View.Path)
        }
    }

    TArray<LTextureView> Resolved;
    for (auto& View: Views)
    {
        if (std::holds_alternative<LPath>(View.What))
        {
            if constexpr (LogTextureSubsystem.CompilesFor<ELogVerbosity::Warning>)
            {
                if (View.bRecursive)
                {
                    LOG_WARNING(LogTextureSubsystem
                        , "[{}]: Has recursive field set but does not use regex."
                        , View.Path
                        )
                }
            }
            Resolved.emplace_back(LTextureView{
                .Path = View.Path,
                .Identifier = std::move(View.Identifier),
                .Texture = finder::relative(finder::weakly_canonical(View.Path.parent_path() / std::get<LPath>(View.What)), Path),
                .Format = View.Format,
                .MipLevels = View.MipLevels,
                .MaxSampleCount = View.MaxSampleCount,
                });
        }
        else if (std::holds_alternative<LString>(View.What))
        {
            if (View.bRecursive)
            {
                for (LPath& Texture: finder::retrieve_files<finder::recursive_directory_iterator>(
                    View.Path.parent_path(),
                    std::get<LString>(View.What) // TODO: Is this dangerous??? Yes. How can we protect?
                    ))
                {
                    Resolved.emplace_back(LTextureView{
                        .Path = View.Path,
                        .Identifier = finder::relative(Texture, Path).replace_extension().string(),
                        .Texture = std::move(Texture),
                        .Format = View.Format,
                        .MipLevels = View.MipLevels,
                        .MaxSampleCount = View.MaxSampleCount,
                        });
                }
            }
            else
            {
                for (LPath& Texture: finder::retrieve_files<finder::directory_iterator>(
                    View.Path.parent_path(),
                    std::get<LString>(View.What)
                    ))
                {
                    Resolved.emplace_back(LTextureView{
                        .Path = View.Path,
                        .Identifier = finder::relative(Texture, Path).replace_extension().string(),
                        .Texture = std::move(Texture),
                        .Format = View.Format,
                        .MipLevels = View.MipLevels,
                        .MaxSampleCount = View.MaxSampleCount,
                        });
                }
            }
        }
        else
        {
            std::unreachable();
        }
    }

    check(!this->TextureViews.contains(Key))
    this->TextureViews[Key] = std::move(Resolved);
    LOG_VERBOSE(LogTextureSubsystem, "[{}]: Fetched [{}] texture views.", Key
        , this->TextureViews.at(Key).size()
        )
}
