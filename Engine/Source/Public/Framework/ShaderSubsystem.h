// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "ShaderSubsystem.generated.h"

namespace Jafg
{

struct LShaderCompilationRequest
{
    LPath In;
    LPath Out;
    LString Target{ "spirv" };
    LString Profile{ "spirv_1_5" };
    TArray<LString> IncludeDirectories;
    TArray<LString> EntryPoints;
};

DECLARE_JAFG_CLASS()
class ENGINE_API JShaderSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JShaderSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    void RecompileChangedShaders();
    void RecompileAllShaders() { checkNoEntry() }

    //# @return System response.
    i32 RecompileShader(LShaderCompilationRequest const& Request);

    TArray<LString> GetDefaultShaderIncludeDirectories() const;

private:

    LShaderCompilationRequest DefaultRequestTemplate;
};

} /* ~Namespace Jafg */
