// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
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

        LPath Path;
        std::vector<LString> IncludeDirs;
        std::vector<CompileTimeDefinition> CompileTimeDefinitions;
        std::inplace_vector<LPath, 2> SourceFiles;

        static LPath GetDestination(LPath const& SourceFile) noexcept;
        static LPath GetReflectionFile(LPath const& SourceFile) noexcept;
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

SERDE_JSON_TYPE_NON_INTRUSIVE_ONLY_DESERIALIZE(JShaderSubsystem::Shader2::CompileTimeDefinition, Identifier, Value)
inline void from_json(json const& j, JShaderSubsystem::Shader2& Shader) noexcept
{
    j.at("IncludeDirs").get_to(Shader.IncludeDirs);
    for (auto& IncludeDir: Shader.IncludeDirs)
    {
        IncludeDir = (finder::content_dir()/IncludeDir).string();
    }
    j.at("SourceFiles").get_to(Shader.SourceFiles);
    for (auto& SourceFile: Shader.SourceFiles)
    {
        SourceFile = finder::content_dir()/SourceFile;
    }
    if (j.contains("CompileTimeDefinitions"))
    {
        j.at("CompileTimeDefinitions").get_to(Shader.CompileTimeDefinitions);
    }
}

} /* ~Namespace Jafg */
