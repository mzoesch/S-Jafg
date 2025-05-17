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
//# It should be used as a hup to store shaders so that others can easily access and use them.
//#
class ENGINE_API LEngineShader
{
    friend LEngine;

public:

    LEngineShader() = default;
    virtual ~LEngineShader() { this->Free(); }

    FORCEINLINE void Free() { if (this->bMeaningful) { this->bMeaningful = false; this->OnFree(); } }

    FORCEINLINE bool IsMeaningful() const noexcept { return this->bMeaningful; }

    virtual     bool Make(const LName InName);
    FORCEINLINE bool MakeChecked(const LName InName);
    FORCEINLINE bool MakeAsserted(const LName InName);

    FORCEINLINE void Use() const { check( this->IsMeaningful() ) this->Program.Use(); }

    virtual void UpdateWorldUniforms(const LViewport& Context, const LWorld& World, const LEye& Eye) { }
    virtual void UpdateViewportUniforms(const LViewport& Context) { }

    FORCEINLINE auto GetProgram()       noexcept ->       LShader& { return this->Program; }
    FORCEINLINE auto GetProgram() const noexcept -> const LShader& { return this->Program; }

protected:

    //# Will free the underlying #Program.
    virtual void OnFree();

    LShader Program;

private:

    bool bMeaningful { false };
};

FORCEINLINE bool LEngineShader::MakeChecked(const LName InName)
{
    const bool bOut = this->Make(InName);
    check( bOut );
    return bOut;
}

FORCEINLINE bool LEngineShader::MakeAsserted(const LName InName)
{
    const bool bOut = this->Make(InName);
    jassert( bOut );
    return bOut;
}

} /* ~Namespace Jafg */
