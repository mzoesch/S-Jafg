// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Name.h"
#include "Rhi/Shader.h"

namespace Jafg
{

class LEngine;
class LWorld;
class LEye;
class LViewport;

//#
//# The engine shader class is meant for shaders that are commonly used by the engine or clients of it.
//# The engine stores them as a hup so that others can easily access and use them without
//# allocating their own instances.
//#
class LEngineShader
{
    friend LEngine;

public:

    LEngineShader() = default;
    virtual ~LEngineShader() { this->Free(); }

    FORCEINLINE bool IsValid() const noexcept { return this->bValid; }
    FORCEINLINE void Free() { if (this->bValid) { this->bValid = false; this->OnFree(); } }

    virtual TArray<LShaderCompileTimeConstant> GetDefaultConstants() { return { }; }

    ENGINE_API  virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants = {});
    FORCEINLINE bool MakeChecked(const LName InName, TArray<LShaderCompileTimeConstant>&&  InConstants = {});
    FORCEINLINE bool MakeAsserted(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants = {});

    ENGINE_API void Recompile(const TArray<LShaderCompileTimeConstant>& InRemove, const TArray<LShaderCompileTimeConstant>& InAdd);
    virtual    void OnRecompile() { }

    FORCEINLINE void Use() const { check( this->IsValid() ) this->Program.Use(); }

    virtual void UpdateWorldUniforms(const LViewport& Context, const LWorld& World, const LEye& Eye) { }
    virtual void UpdateViewportUniforms(const LViewport& Context) { }

    FORCEINLINE LShader& GetProgram() noexcept { return this->Program; }
    FORCEINLINE const LShader& GetProgram() const noexcept { return this->Program; }

    FORCEINLINE const TArray<LShaderCompileTimeConstant>& GetCachedConstants() const noexcept { return this->Constants; }

protected:

    //# Will free the underlying #Program.
    virtual void OnFree();

    LShader Program;
    TArray<LShaderCompileTimeConstant> Constants;

private:

    bool bValid { false };
};

FORCEINLINE bool LEngineShader::MakeChecked(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants /* = {} */)
{
    const bool bOut = this->Make(InName, std::move(InConstants));
    check( bOut );
    return bOut;
}

FORCEINLINE bool LEngineShader::MakeAsserted(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants /* = {} */)
{
    const bool bOut = this->Make(InName, std::move(InConstants));
    jassert( bOut );
    return bOut;
}

} /* ~Namespace Jafg */
