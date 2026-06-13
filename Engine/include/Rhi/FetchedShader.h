// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LShaderEntrypoint final
{
    vk::ShaderStageFlagBits Stage;
    LString Name;
};

struct LFetchedShader final
{
    struct CompileTimeDefinition
    {
        LString Name;
        LString Value;
    };

    struct Layout
    {
        enum Type
        {
            eUnique,
            eShared,
        };

        enum UpdateFrequency
        {
            ePerFrame,
            eRarely,
        };
        inline static UpdateFrequency StringToUpdateFrequency(LString const& Str)
        {
            if (Str == "ePerFrame") { return UpdateFrequency::ePerFrame; }
            else if (Str == "eRarely") { return UpdateFrequency::eRarely; }
            else
            {
                LOG_FATAL(LogShaderSubsystem, "Invalid update frequency string [{}].", Str)
            }
        }

        struct Set
        {
            LString Identifier;
            vk::DescriptorType DescriptorType;
        };

        Type Type;

        //# Required for shared.
        std::optional<LString> Identifier;

        //# Required for unique.
        std::optional<UpdateFrequency> UpdateFrequency;
        std::optional<vk::ShaderStageFlags> Stage;
        std::optional<TArray<Set>> Sets;
    };

    LPath Path;
    LString Name;
    TArray<LString> Inherits;

    TArray<LString> IncludeDirectories;
    TArray<LShaderEntrypoint> Entrypoints;
    TArray<CompileTimeDefinition> CompileTimeDefinitions;

    std::optional<LPath> Src;
    std::optional<LPath> SrcPrefix;
    std::optional<LPath> Dst;
    std::optional<LPath> DstPrefix;

    std::optional<vk::PipelineInputAssemblyStateCreateInfo> PipelineInputAssemblyState;
    std::optional<vk::PipelineDepthStencilStateCreateInfo> PipelineDepthStencilState;

    std::optional<vk::SampleCountFlagBits> MsaaSamples;

    std::optional<LString> VertexInput;
    // Flatten this. with min vulkan guaranteed value.
    TArray<LString> PushConstants;
    TArray<Layout> Layouts;

    inline LPath GetSrc() const noexcept
    {
        if (this->Src.has_value() == false)
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No source path specified for this shader.", this->Path)
        }
        if (this->SrcPrefix.has_value())
        {
            return *this->SrcPrefix / *this->Src;
        }
        return *this->Src;
    }

    inline LPath GetDst() const noexcept
    {
        if (this->Dst.has_value() == false)
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No destination path specified for this shader.", this->Path)
        }
        if (this->DstPrefix.has_value())
        {
            return *this->DstPrefix / *this->Dst;
        }
        return *this->Dst;
    }
};

} /* ~Namespace Jafg */
