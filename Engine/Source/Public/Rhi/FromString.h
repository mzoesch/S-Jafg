// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

template<typename T>
inline T Vk_FromString(LStringView String) noexcept = delete;

template<>
inline vk::Format Vk_FromString<vk::Format>(LStringView String) noexcept
{
    if (String == "eR8G8B8A8Srgb") { return vk::Format::eR8G8B8A8Srgb; }
    if (String == "eR8G8B8Srgb") { return vk::Format::eR8G8B8Srgb; }

    return vk::Format::eUndefined;
}

template<>
inline vk::SampleCountFlagBits Vk_FromString<vk::SampleCountFlagBits>(LStringView String) noexcept
{
    if (String == "e1") { return vk::SampleCountFlagBits::e1; }
    if (String == "e2") { return vk::SampleCountFlagBits::e2; }
    if (String == "e4") { return vk::SampleCountFlagBits::e4; }
    if (String == "e8") { return vk::SampleCountFlagBits::e8; }
    if (String == "e16") { return vk::SampleCountFlagBits::e16; }
    if (String == "e32") { return vk::SampleCountFlagBits::e32; }
    if (String == "e64") { return vk::SampleCountFlagBits::e64; }

    LOG_FATAL(LogVulkan, "Unsupported sample count string [{}] for conversion to sample count flag bits.", String)
}

template<>
inline vk::ShaderStageFlagBits Vk_FromString<vk::ShaderStageFlagBits>(LStringView String) noexcept
{
    if (String == "eVertex") { return vk::ShaderStageFlagBits::eVertex; }
    if (String == "eTessellationControl") { return vk::ShaderStageFlagBits::eTessellationControl; }
    if (String == "eTessellationEvaluation") { return vk::ShaderStageFlagBits::eTessellationEvaluation; }
    if (String == "eGeometry") { return vk::ShaderStageFlagBits::eGeometry; }
    if (String == "eFragment") { return vk::ShaderStageFlagBits::eFragment; }
    if (String == "eCompute") { return vk::ShaderStageFlagBits::eCompute; }

    LOG_FATAL(LogVulkan, "Unsupported shader stage string [{}] for conversion to shader stage flag bits.", String)
}

template<>
inline vk::ShaderStageFlags Vk_FromString(LStringView String) noexcept
{
    vk::ShaderStageFlags Flags{};
    if (String.contains("eVertex")) { Flags |= vk::ShaderStageFlagBits::eVertex; }
    if (String.contains("eFragment")) { Flags |= vk::ShaderStageFlagBits::eFragment; }
    return Flags;
}

template<>
inline vk::DescriptorType Vk_FromString<vk::DescriptorType>(LStringView String) noexcept
{
    if (String == "eSampler") { return vk::DescriptorType::eSampler; }
    if (String == "eSampledImage") { return vk::DescriptorType::eSampledImage; }
    if (String == "eUniformBuffer") { return vk::DescriptorType::eUniformBuffer; }

    LOG_FATAL(LogVulkan, "Unsupported descriptor type string [{}] for conversion to descriptor type.", String)
}

} /* ~Namespace Jafg */
