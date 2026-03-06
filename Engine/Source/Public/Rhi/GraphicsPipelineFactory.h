// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Frontend.h"
#include "Rhi/GraphicsPipeline.h"
#include "Rhi/FetchedShader.h"
#include "Rhi/VertexInput.h"
#include "Rhi/Layout.h"
#include "Rhi/PushConstants.h"
#include "Rhi/Material.h"

namespace Jafg
{

struct LDevicePipelineFactory
{
    explicit LDevicePipelineFactory(LFrontend const& InFrontend) noexcept : Frontend{ InFrontend } {}

    PROHIBIT_REALLOC_OF_ANY_FORM(LDevicePipelineFactory)

    decltype(auto) Shader(this auto&& Self, LPath const& Path, TArray<LShaderEntrypoint> InEntrypoints)
    {
        check(Self.ShaderEntrypoints.contains(Path) == false)
        Self.ShaderEntrypoints[Path] = std::move(InEntrypoints);
        auto& Entrypoints{Self.ShaderEntrypoints[Path]};

        const auto Code{Finder::ReadFileAsBinary(Path)};
        Self.ShaderModules.emplace_back(vk::raii::ShaderModule{
            Self.Frontend.Vk_GetDevice(),
            vk::ShaderModuleCreateInfo{
                .codeSize = Code.size() * sizeof(u8),
                .pCode = reinterpret_cast<u32 const*>(Code.data())
                }
            });
        auto ShaderModuleHandle{*Self.ShaderModules.back()};

        vk::ShaderStageFlags Stages{};
        algo::for_each(Entrypoints, [&Stages](auto const& Entrypoint)
        {
            Stages |= Entrypoint.Stage;
        });

        if (Stages & vk::ShaderStageFlagBits::eVertex)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eVertex,
                .module = ShaderModuleHandle,
                .pName = algo::find(Entrypoints, vk::ShaderStageFlagBits::eVertex, &LShaderEntrypoint::Stage)->Name.c_str(),
                });
        }

        if (Stages & vk::ShaderStageFlagBits::eFragment)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eFragment,
                .module = ShaderModuleHandle,
                .pName = algo::find(Entrypoints, vk::ShaderStageFlagBits::eFragment, &LShaderEntrypoint::Stage)->Name.c_str(),
                });
        }

        check( (Stages & ~(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)) == vk::ShaderStageFlags{}
            && "These stage flags are not yet supported." )

        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceVertexInput TDeviceVertexInput>
    decltype(auto) VertexInput(this auto&& Self) noexcept
    {
        check(Self.VertexInputInfo.has_value() == false)
        Self.VertexInputInfo = {
            .vertexBindingDescriptionCount = static_cast<u32>(TDeviceVertexInput::BindingDescriptions().size()),
            .pVertexBindingDescriptions = TDeviceVertexInput::BindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(TDeviceVertexInput::AttributeDescriptions().size()),
            .pVertexAttributeDescriptions = TDeviceVertexInput::AttributeDescriptions().data(),
            };

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) VertexInput(this auto&& Self, vk::PipelineVertexInputStateCreateInfo&& Info)
    {
        check(Self.VertexInputInfo.has_value() == false)
        Self.VertexInputInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) InputAssembly(this auto&& Self, vk::PipelineInputAssemblyStateCreateInfo&& Info) noexcept
    {
        Self.InputAssemblyInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ViewportState(this auto&& Self, vk::PipelineViewportStateCreateInfo&& Info) noexcept
    {
        Self.ViewportStateInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) Rasterization(this auto&& Self, vk::PipelineRasterizationStateCreateInfo&& Info) noexcept
    {
        Self.RasterizationInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) MultisamplingShading(this auto&& Self, vk::Bool32 Enable) noexcept
    {
        Self.MultisamplingShadingEnable = Enable;
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) DepthStencil(this auto&& Self, vk::PipelineDepthStencilStateCreateInfo&& Info) noexcept
    {
        Self.DepthStencilInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ColorBlending(this auto&& Self, vk::LogicOp Op) noexcept
    {
        Self.ColorBlendLogicOpEnable = vk::True;
        Self.ColorBlendLogicalOp = Op;
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ColorBlendAttachment(this auto&& Self, vk::PipelineColorBlendAttachmentState&& Info) noexcept
    {
        Self.ColorBlendAttachment = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) DynamicStates(this auto&& Self, std::array<vk::DynamicState, 2>&& States) noexcept
    {
        Self.DynamicStates = std::move(States);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) SharedLayout(this auto&& Self, vk::DescriptorSetLayout SharedDescriptorSetLayout) noexcept
    {
        Self.SharedDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), SharedDescriptorSetLayout);
        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceLayout TDeviceLayout>
    decltype(auto) UniqueLayout(this auto&& Self) noexcept
    {
        Self.UniqueDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), vk::raii::DescriptorSetLayout{
            Self.Frontend.Vk_GetDevice(),
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(TDeviceLayout::Bindings().size()),
                .pBindings = TDeviceLayout::Bindings().data(),
                }
            });

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) UniqueLayout(this auto&& Self, vk::DescriptorSetLayoutCreateInfo const& Layout) noexcept
    {
        Self.UniqueDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), vk::raii::DescriptorSetLayout{
            Self.Frontend.Vk_GetDevice(),
            Layout
            });
        return std::forward<decltype(Self)>(Self);
    }

    template<CPushConstant TPushConstant>
    decltype(auto) PushConstant(this auto&& Self) noexcept
    {
        Self.PushConstantRange.emplace_back(vk::PushConstantRange{
            .stageFlags = TPushConstant::Flags(),
            .offset = static_cast<u32>(Self.PushConstantRange.size()),
            .size = sizeof(TPushConstant),
            });
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) PushConstant(this auto&& Self, Detail::LPushConstantInfo const& Info) noexcept
    {
        Self.PushConstantRange .emplace_back(vk::PushConstantRange{
            .stageFlags = Info.StageFlags,
            .offset = static_cast<u32>(Self.PushConstantRange.size()),
            .size = Info.Size
            });
        return std::forward<decltype(Self)>(Self);
    }

    ENGINE_API LGraphicsDevicePipeline Build();

    inline u32 GetCurrentNumberOfLayouts() const noexcept
    {
        return static_cast<u32>(this->SharedDescriptorSetLayouts.size() + this->UniqueDescriptorSetLayouts.size());
    }

    template<typename T>
    struct TDescriptorSetLayout
    {
        u32 Binding;
        T DescriptorSetLayout;
    };

    LFrontend const& Frontend;
    TArray<vk::raii::ShaderModule> ShaderModules;
    TArray<vk::PipelineShaderStageCreateInfo> Shaders;
    std::unordered_map<LPath, TArray<LShaderEntrypoint>> ShaderEntrypoints;
    std::optional<vk::PipelineVertexInputStateCreateInfo> VertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False
        };
    vk::PipelineViewportStateCreateInfo ViewportStateInfo{
        .viewportCount = 1,
        .scissorCount = 1,
        };
    vk::PipelineRasterizationStateCreateInfo RasterizationInfo{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .depthBiasSlopeFactor = 1.0f,
        .lineWidth = 1.0f
        };
    vk::Bool32 MultisamplingShadingEnable{ vk::False };
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo{
        .depthTestEnable       = vk::True,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False
        };

    vk::PipelineColorBlendStateCreateInfo const* PipelineColorBlendStateCreateInfo{};

    std::array<vk::DynamicState, 3> DynamicStateInfo{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::ePolygonModeEXT,
        };
    TArray<TDescriptorSetLayout<vk::DescriptorSetLayout>> SharedDescriptorSetLayouts;
    TArray<TDescriptorSetLayout<vk::raii::DescriptorSetLayout>> UniqueDescriptorSetLayouts;
    vk::raii::PipelineLayout PipelineLayout{ nullptr };
    TArray<vk::PushConstantRange> PushConstantRange;
};

} /* ~Namespace Jafg */
