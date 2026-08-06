// Copyright mzoesch. All rights reserved.

#include "Framework/TextureSubsystem.h"
#include "Rhi/TextureView.h"
#include "Engine/Engine.h"
#include "Framework/AssetDiscoverer.h"
#include "Framework/MaterialSubsystem.h"
#include "Stats/Stats.h"
#include "Rhi/FromString.h"

void Jafg::JTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->RefetchTextureViews();
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

Jafg::LTextureView const& Jafg::JTextureSubsystem::GetTextureView(LStringView Asset) const noexcept
{
    auto It{algo::find(this->TextureViews, Asset, &LTextureView::Name)};
    if (It == this->TextureViews.end())
    {
        LOG_FATAL(LogTextureSubsystem, "No such texture view [{}].", Asset)
    }
    return *It;
}

void Jafg::JTextureSubsystem::RefetchTextureViews()
{
    STAT_CYCLE_FUNCTION()
    algo::orphan(&this->TextureViews);
    LOG_VERBOSE(LogTextureSubsystem, "Refetching texture views.")

    for (auto& Assets{*this->GetEngine().GetSubsystemChecked<JAssetDiscoverer>()};
        auto const& Path: Assets.GetHeaders()
            | algo::views::filter([](auto const& Pair){ return Pair.second.Type == Detail::EAsset::Texture; })
            | algo::views::keys)
    {
        if constexpr (IS_COMPILED_LOG(LogTextureSubsystem, Trace))
        {
            this->TextureViews.emplace_back(Assets.PullAssetTextureView(Path));
        }
        else
        {
            this->TextureViews.emplace_back(Assets.PullAssetTextureView(Path));
        }
    }

    LOG_VERBOSE(LogTextureSubsystem, "Finished loading [{}] texture views.", this->TextureViews.size())
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromFile(LPath const& Path
    , LTexture2::HostInfo HostCreateInfo
    , LTexture2::DeviceInfo DeviceCreateInfo
    , ETexture2State State /* = ETexture2StateBits::Device */
    )
{
    if (auto const& It{this->Textures.find(Path)}; It != this->Textures.end())
    {
        LTexture2& Texture{*It->second};

        check(Texture.GetFormat() == vk::Format::eUndefined || Texture.GetFormat() == HostCreateInfo.Format)
        if (State & ETexture2StateBits::Host || ((State & ETexture2StateBits::Device) && Texture.IsOnDevice() == false))
        {
            if (Texture.IsOnHost() == false)
            {
                auto Result{Texture.LoadToHost(HostCreateInfo)};
                jassert(Result == LTexture2::EResult::Success)
            }
        }
        if (State & ETexture2StateBits::Device)
        {
            if (Texture.IsOnDevice() == false)
            {
                Texture.LoadToDevice(DeviceCreateInfo);
                if ((State & ETexture2StateBits::Host) == ETexture2StateBits::None)
                {
                    Texture.FreeFromHost();
                }
            }
        }

        return It->second;
    }

    this->Textures[Path] = std::make_shared<LTexture2>(Path, HostCreateInfo, DeviceCreateInfo, State);
    return this->Textures[Path];
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromAsset(LStringView Asset, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromTextureView(this->GetTextureView(Asset), State);
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromTextureView(LTextureView const& View, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromFile(View.Texture
        , LTexture2::HostInfo{.Format=View.Format}
        , LTexture2::DeviceInfo{.DesiredMipLevels=View.MipLevels,.Samples=View.MaxSampleCount}
        , State
        );
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
        .anisotropyEnable = vk::True, .maxAnisotropy = Frontend.Vk_GetPhysicalDevice().getProperties().limits.maxSamplerAnisotropy,
        .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f, // Increase for worse texture quality.
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = vk::BorderColor::eFloatOpaqueWhite,
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
        Texture->_SetBindlessIndex(static_cast<i64>(*R));
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

std::optional<std::size_t> Jafg::JTextureSubsystem::_Vk_AddTransientImageToGlobalBindlessArray(vk::ImageView const& ImageView)
{
    check(!!ImageView)

    u64 Idx{this->FreeBindlessTextureArraySlots.allocate()};
    if (Idx == algo::dynamic_bit_set::npos)
    {
        return {};
    }
    check(Idx < this->BindlessTextureArrayCapacity)

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

    return Idx;
}

void Jafg::JTextureSubsystem::_Vk_AllocateBindlessPool()
{
    auto& Frontend{this->GetFrontend()};

    LOG_VERBOSE(LogTextureSubsystem, "Allocating bindless texture array descriptor pool and descriptor set.")

    std::array Sizes{
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eSampler,
            .descriptorCount = UBO::Bindless::SamplerCount,
            },
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eSampledImage,
            .descriptorCount = this->BindlessTextureArrayCapacity,
            },
        };
    this->BindlessTextureArrayDescriptorPool = rhi::vk_build(Frontend.Vk_GetDevice(), vk::DescriptorPoolCreateInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
        .pPoolSizes = Sizes.data(),
        });

    vk::DescriptorSetVariableDescriptorCountAllocateInfo CountInfo{
        .descriptorSetCount = 1,
        .pDescriptorCounts = &this->BindlessTextureArrayCapacity,
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
    this->FreeBindlessTextureArraySlots = algo::dynamic_bit_set{this->BindlessTextureArrayCapacity};
    check(this->FreeBindlessTextureArraySlots.bit_count() == this->BindlessTextureArrayCapacity)

    TArray<LString> GuaranteedTextureIdentifiers{ "Textures/Jafg.IdentityMul" };
    if (!this->GuaranteedTextures.empty())
    {
        panic("Expected GuaranteedTextures to be empty.")
    }
    for (auto const& Gti: GuaranteedTextureIdentifiers)
    {
        this->GuaranteedTextures.emplace_back(this->FromAsset(Gti));
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
