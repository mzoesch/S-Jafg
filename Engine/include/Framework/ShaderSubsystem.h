// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Rhi/Objects.h"
#include "Rhi/ReflectedShader.h"
#include "ShaderSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JShaderSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JShaderSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    struct Shader2 final
    {
        struct CompileTimeDefinition final
        {
            LString Identifier;
            LString Value;
        };

        LPath Namespace;
        LPath Path;
        std::vector<LString> IncludeDirs;
        std::vector<CompileTimeDefinition> CompileTimeDefinitions;
        std::inplace_vector<LPath, 2> SourceFiles;

        static LPath GetDestination(LPath const& SourceFile) noexcept;
        static LPath GetReflectionFile(LPath const& SourceFile) noexcept;
        //# The source file and everything it includes, written by slangc in make syntax. @see -depfile
        static LPath GetDependencyFile(LPath const& SourceFile) noexcept;
    };

    NODISCARD bool HasShader(LStringView Name) const noexcept
    {
        return algo::contains(this->ReflectedShaders, Name, &rhi::reflected_shader::Identifier);
    }
    NODISCARD rhi::reflected_shader const& GetShader(LStringView Name) const noexcept
    {
        auto It{algo::find(this->ReflectedShaders, Name, &rhi::reflected_shader::Identifier)};
        if (It == this->ReflectedShaders.end())
        {
            LOG_FATAL(LogShaderSubsystem, "[{}]: No such shader.", Name)
        }
        return *It;
    }

    template<rhi::vertex_input TVertexInput>
    inline void AddVertexProvider() noexcept { LVertexInputRegistrator<TVertexInput>{}; }
    LVertexInputProvider const& GetVertexInput(LStringView Identifier) const noexcept;

    template<rhi::pc TPushConstant>
    inline void AddPushConstantProvider() noexcept { LPushConstantRegistrator<TPushConstant>{}; }
    LPushConstantProvider const& GetPushConstant(LStringView Identifier) const noexcept;

    template<rhi::detail::object TBufferObject>
    inline void AddBufferObjectProvider() noexcept { LBufferObjectRegistrator<TBufferObject>{}; }
    LBufferObjectProvider const& GetBufferObject(LStringView Identifier) const noexcept;

private:

    std::size_t RecompileShaderConditionally(Shader2 const& Shader, bool bForce = false);

    //#
    //# Once populated, shaders will never change their address again. This is guaranteed by jafg.
    //#
    //# TODO: Can't we solve with std::hive?
    //#
    TArray<rhi::reflected_shader> ReflectedShaders;
};

} /* ~Namespace Jafg */
