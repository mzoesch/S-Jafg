// Copyright mzoesch. All rights reserved.

#include "Rhi/EngineShader.h"
#include "Engine/Engine.h"

bool Jafg::LEngineShader::Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants /* = {} */)
{
    check( GEngine )
    check( this->IsValid() == false )

    InConstants.Append(this->GetDefaultConstants());
    checkCode
    (
        for (const LShaderCompileTimeConstant& Constant : InConstants)
        {
            i32 Count { 0 };
            InConstants.ForEach([&Count, &Constant](const LShaderCompileTimeConstant& InConstant) -> void
            {
                if (InConstant == Constant)
                {
                    ++Count;
                }

                return;
            });

            check( Count == 1 && "Shader compile time constants contains duplicates." )

            continue;
        }
    )

    this->bValid = true;
    this->Constants = std::move(InConstants);

    return GEngine->AddShader(InName, this);
}

void Jafg::LEngineShader::Recompile(const TArray<LShaderCompileTimeConstant>& InRemove, const TArray<LShaderCompileTimeConstant>& InAdd)
{
    for (const LShaderCompileTimeConstant& Constant : InRemove)
    {
        this->Constants.Remove(Constant);
    }

    for (const LShaderCompileTimeConstant& Constant : InAdd)
    {
        this->Constants.Add(Constant);
    }

    this->Program.Recompile(this->Constants);

    this->OnRecompile();

    return;
}

void Jafg::LEngineShader::OnFree()
{
    LOG_VERBOSE(LogRhi, "Freeing shader [{}] from graphical processing unit.", this->Program.GetId())

    this->Program.Free();
    this->bValid = false;

    if (GEngine)
    {
        GEngine->RemoveShader(this);
    }

    return;
}
