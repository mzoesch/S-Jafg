// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg::UBO
{

struct BindlessTextureArray
{
    inline static constexpr u32 SamplerBinding{ 0 };
    inline static constexpr u32 ArrayBinding{ 1 };

    enum Sampler : u32
    {
        LinearRepeatSamplerIdx = 0,
        LinearMirroredRepeatSamplerIdx,
        LinearClampToEdgeSamplerIdx,
        LinearClampToBorderSamplerIdx,
        // LinearMirrorClampToEdgeSamplerIdx,
        NearestRepeatSamplerIdx,
        NearestMirroredRepeatSamplerIdx,
        NearestClampToEdgeSamplerIdx,
        NearestClampToBorderSamplerIdx,
        // NearestMirrorClampToEdgeSamplerIdx,
    };
    inline static constexpr u32 SamplerCount{NearestClampToBorderSamplerIdx + 1};

    inline static constexpr u32 IdentityMulIdx{ 0 };
    static std::array<vk::DescriptorSetLayoutBinding, 2> GetBindings(u32 Capacity) noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = BindlessTextureArray::SamplerBinding,
                .descriptorType = vk::DescriptorType::eSampler,
                .descriptorCount = SamplerCount,
                .stageFlags = Flags(),
                .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = BindlessTextureArray::ArrayBinding,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = Capacity,
                .stageFlags = Flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
    inline static constexpr vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eFragment; }

    static vk::DescriptorSetLayoutBindingFlagsCreateInfo const& FlagsInfo() noexcept
    {
        static vk::DescriptorSetLayoutBindingFlagsCreateInfo FlagsInfo{
            .bindingCount = static_cast<u32>(BindingFlags().size()),
            .pBindingFlags = BindingFlags().data(),
            };

        return FlagsInfo;
    }

    static std::array<vk::DescriptorBindingFlags, 2> const& BindingFlags() noexcept
    {
        static std::array BindingFlags{
            vk::DescriptorBindingFlags{},
            vk::DescriptorBindingFlags{
                  vk::DescriptorBindingFlagBits::ePartiallyBound
                | vk::DescriptorBindingFlagBits::eVariableDescriptorCount
                | vk::DescriptorBindingFlagBits::eUpdateAfterBind
                },
            };

        return BindingFlags;
    }
};

} /* ~Namespace Jafg */
