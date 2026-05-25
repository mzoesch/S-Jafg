// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg::UBO
{

//# The global jafg commonly used bindless texture array.
struct Bindless final
{
    //# Binding point for the bindless sampler.
    inline static constexpr u32 SamplerBinding{ 0 };
    //# Binding point for the bindless array.
    inline static constexpr u32 ArrayBinding{ 1 };

    //# Binding index of the identity texture for multiplication inside #ArrayBinding. Always valid.
    inline static constexpr u32 IdentityMulIdx{ 0 };

    enum struct Sampler : u8
    {
        LinearRepeatSamplerIdx = 0,
        LinearMirroredRepeatSamplerIdx = 1,
        LinearClampToEdgeSamplerIdx = 2,
        LinearClampToBorderSamplerIdx = 3,
        // LinearMirrorClampToEdgeSamplerIdx = ,
        NearestRepeatSamplerIdx = 4,
        NearestMirroredRepeatSamplerIdx = 5,
        NearestClampToEdgeSamplerIdx = 6,
        NearestClampToBorderSamplerIdx = 7,
        // NearestMirrorClampToEdgeSamplerIdx = ,
    };
    inline static constexpr u32 LinearRepeatSamplerIdx{std::to_underlying(Sampler::LinearRepeatSamplerIdx)};
    inline static constexpr u32 LinearMirroredRepeatSamplerIdx{std::to_underlying(Sampler::LinearMirroredRepeatSamplerIdx)};
    inline static constexpr u32 LinearClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearClampToEdgeSamplerIdx)};
    inline static constexpr u32 LinearClampToBorderSamplerIdx{std::to_underlying(Sampler::LinearClampToBorderSamplerIdx)};
    //inline static constexpr u32 LinearMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearMirrorClampToEdgeSamplerIdx)};
    inline static constexpr u32 NearestRepeatSamplerIdx{std::to_underlying(Sampler::NearestRepeatSamplerIdx)};
    inline static constexpr u32 NearestMirroredRepeatSamplerIdx{std::to_underlying(Sampler::NearestMirroredRepeatSamplerIdx)};
    inline static constexpr u32 NearestClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestClampToEdgeSamplerIdx)};
    inline static constexpr u32 NearestClampToBorderSamplerIdx{std::to_underlying(Sampler::NearestClampToBorderSamplerIdx)};
    //inline static constexpr u32 NearestMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestMirrorClampToEdgeSamplerIdx)};
    inline static constexpr u32 SamplerCount{std::to_underlying(Sampler::NearestClampToBorderSamplerIdx) + 1};

    inline static std::array<vk::DescriptorSetLayoutBinding, 2> GetBindings(u32 Capacity) noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = Bindless::SamplerBinding,
                .descriptorType = vk::DescriptorType::eSampler,
                .descriptorCount = SamplerCount,
                .stageFlags = Flags(),
                .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = Bindless::ArrayBinding,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = Capacity,
                .stageFlags = Flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
    inline static constexpr vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eFragment; }

    inline static vk::DescriptorSetLayoutBindingFlagsCreateInfo const& FlagsInfo() noexcept
    {
        static vk::DescriptorSetLayoutBindingFlagsCreateInfo FlagsInfo{
            .bindingCount = static_cast<u32>(BindingFlags().size()),
            .pBindingFlags = BindingFlags().data(),
            };

        return FlagsInfo;
    }
    inline static std::array<vk::DescriptorBindingFlags, 2> const& BindingFlags() noexcept
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
