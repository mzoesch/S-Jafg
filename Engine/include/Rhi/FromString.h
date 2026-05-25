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
    LOG_FATAL(LogVulkan, "[{}]: Unsupported sample count flag bits string.", String)
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
    LOG_FATAL(LogVulkan, "[{}]: Unsupported shader stage flag bits string.", String)
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
    if (String == "eStorageBuffer") { return vk::DescriptorType::eStorageBuffer; }
    LOG_FATAL(LogVulkan, "[{}]: Unsupported descriptor type string.", String)
}

template<>
inline vk::PrimitiveTopology Vk_FromString<vk::PrimitiveTopology>(LStringView String) noexcept
{
    if (String == "ePointList") { return vk::PrimitiveTopology::ePointList; }
    if (String == "eLineList") { return vk::PrimitiveTopology::eLineList; }
    if (String == "eLineStrip") { return vk::PrimitiveTopology::eLineStrip; }
    if (String == "eTriangleList") { return vk::PrimitiveTopology::eTriangleList; }
    if (String == "eTriangleStrip") { return vk::PrimitiveTopology::eTriangleStrip; }
    if (String == "eTriangleFan") { return vk::PrimitiveTopology::eTriangleFan; }
    if (String == "eLineListWithAdjacency") { return vk::PrimitiveTopology::eLineListWithAdjacency; }
    if (String == "eLineStripWithAdjacency") { return vk::PrimitiveTopology::eLineStripWithAdjacency; }
    if (String == "eTriangleListWithAdjacency") { return vk::PrimitiveTopology::eTriangleListWithAdjacency; }
    if (String == "eTriangleStripWithAdjacency") { return vk::PrimitiveTopology::eTriangleStripWithAdjacency; }
    if (String == "ePatchList") { return vk::PrimitiveTopology::ePatchList; }
    LOG_FATAL(LogVulkan, "[{}]: Unsupported primitive topology string.", String)
}

template<>
inline vk::CompareOp Vk_FromString<vk::CompareOp>(LStringView String) noexcept
{
    if (String == "eNever") { return vk::CompareOp::eNever; }
    if (String == "eLess") { return vk::CompareOp::eLess; }
    if (String == "eEqual") { return vk::CompareOp::eEqual; }
    if (String == "eLessOrEqual") { return vk::CompareOp::eLessOrEqual; }
    if (String == "eGreater") { return vk::CompareOp::eGreater; }
    if (String == "eNotEqual") { return vk::CompareOp::eNotEqual; }
    if (String == "eGreaterOrEqual") { return vk::CompareOp::eGreaterOrEqual; }
    if (String == "eAlways") { return vk::CompareOp::eAlways; }
    LOG_FATAL(LogVulkan, " [{}]: Unsupported compare op string.", String)
}

template<>
inline vk::StencilOp Vk_FromString<vk::StencilOp>(LStringView String) noexcept
{
    if (String == "eKeep") { return vk::StencilOp::eKeep; }
    if (String == "eZero") { return vk::StencilOp::eZero; }
    if (String == "eReplace") { return vk::StencilOp::eReplace; }
    if (String == "eIncrementAndClamp") { return vk::StencilOp::eDecrementAndClamp; }
    if (String == "eDecrementAndClamp") { return vk::StencilOp::eDecrementAndClamp; }
    if (String == "eInvert") { return vk::StencilOp::eInvert; }
    if (String == "eIncrementAndWrap") { return vk::StencilOp::eIncrementAndWrap; }
    if (String == "eDecrementAndWrap") { return vk::StencilOp::eDecrementAndWrap; }
    LOG_FATAL(LogVulkan, " [{}]: Unsupported stencil op.", String)
}

} /* ~Namespace Jafg */
