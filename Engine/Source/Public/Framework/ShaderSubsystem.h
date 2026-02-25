// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/FetchedShader.h"
#include "Rhi/VkAl.h"
#include "ShaderSubsystem.generated.h"

namespace Jafg
{

struct LShaderCompilationRequest
{
    LString Target{ "spirv" };
    LString Profile{ "spirv_1_5" };
    LFetchedShader const& FetchedShader;
};

DECLARE_JAFG_CLASS()
class ENGINE_API JShaderSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JShaderSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    inline bool HasFetchedShader(LStringView Name) const noexcept { return algo::contains(this->FetchedShaders, Name, &LFetchedShader::Name); }
    inline LFetchedShader const& GetFetchedShader(LStringView Name) const noexcept
    {
        auto It{algo::find(this->FetchedShaders, Name, &LFetchedShader::Name)};
        if (It == this->FetchedShaders.end())
        {
            LOG_FATAL(LogShaderSubsystem, "No such shader [{}].", Name)
        }
        return *It;
    }
    void RefetchShaders();

    void RecompileChangedShaders();
    void RecompileAllShaders() { checkNoEntry() }

    //# @return System response.
    i32 RecompileShader(LShaderCompilationRequest const& Request);

    template<typename TVertexInput> requires CDeviceVertexInput<TVertexInput>
    inline void AddVertexProvider() noexcept { Detail::AddVertexProvider<TVertexInput>(); }
    vk::PipelineVertexInputStateCreateInfo GetVertexInputStateCreateInfo(LString const& Name) const;

    template<typename TPushConstant> requires CPushConstant<TPushConstant>
    inline void AddPushConstantProvider() noexcept { Detail::AddPushConstantProvider<TPushConstant>(); }
    Detail::LPushConstantInfo GetPushConstantInfo(LString const& Name) const;

private:

    TArray<LFetchedShader> FetchedShaders;
};

} /* ~Namespace Jafg */
