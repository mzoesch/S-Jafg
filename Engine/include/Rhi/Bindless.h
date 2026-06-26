// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg::UBO
{

struct Bindless : rhi::ubo_template<Bindless, vk::ShaderStageFlagBits::eFragment>
{
    //# Binding point for the bindless samplers.
    static constexpr u32 SamplerBinding{ 0 };
    //# Binding point for the bindless array.
    static constexpr u32 ArrayBinding{ 1 };

    //# Binding index of the identity texture for multiplication inside #ArrayBinding. Always valid.
    static constexpr u32 IdentityMulIdx{ 0 };

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
    static constexpr u32 LinearRepeatSamplerIdx{std::to_underlying(Sampler::LinearRepeatSamplerIdx)};
    static constexpr u32 LinearMirroredRepeatSamplerIdx{std::to_underlying(Sampler::LinearMirroredRepeatSamplerIdx)};
    static constexpr u32 LinearClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearClampToEdgeSamplerIdx)};
    static constexpr u32 LinearClampToBorderSamplerIdx{std::to_underlying(Sampler::LinearClampToBorderSamplerIdx)};
    // static constexpr u32 LinearMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearMirrorClampToEdgeSamplerIdx)};
    static constexpr u32 NearestRepeatSamplerIdx{std::to_underlying(Sampler::NearestRepeatSamplerIdx)};
    static constexpr u32 NearestMirroredRepeatSamplerIdx{std::to_underlying(Sampler::NearestMirroredRepeatSamplerIdx)};
    static constexpr u32 NearestClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestClampToEdgeSamplerIdx)};
    static constexpr u32 NearestClampToBorderSamplerIdx{std::to_underlying(Sampler::NearestClampToBorderSamplerIdx)};
    // static constexpr u32 NearestMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestMirrorClampToEdgeSamplerIdx)};
    static constexpr u32 SamplerCount{std::to_underlying(Sampler::NearestClampToBorderSamplerIdx) + 1};

    static std::array<vk::DescriptorBindingFlags, 2> const& descriptor_binding_flags() noexcept
    {
        static std::array Flags{
            vk::DescriptorBindingFlags{},
            vk::DescriptorBindingFlags{
                  vk::DescriptorBindingFlagBits::ePartiallyBound
                | vk::DescriptorBindingFlagBits::eVariableDescriptorCount
                | vk::DescriptorBindingFlagBits::eUpdateAfterBind
                },
            };
        return Flags;
    }
    static std::array<vk::DescriptorSetLayoutBinding, 2> bindings(u32 Capacity) noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = SamplerBinding,
                .descriptorType = vk::DescriptorType::eSampler,
                .descriptorCount = SamplerCount,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = ArrayBinding,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = Capacity,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
    static vk::DescriptorSetLayoutCreateFlags descriptor_set_layout_flags() noexcept
    {
        return vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
    }
};
static_assert(rhi::ubo<Bindless>);

} /* ~Namespace Jafg::UBO */
