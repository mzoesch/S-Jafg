// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Shader.h"

namespace Jafg
{

class LEngine;
class LWorld;
class LEye;
class LViewport;

class ENGINE_API LEngineShader
{
public:

    friend LEngine;

    virtual ~LEngineShader() { this->Free(); }

    void Free() { if (this->bMeaningful) { this->bMeaningful = false; this->OnFree(); } }

    virtual u32 Make();
    virtual void   UpdateUniforms(const LViewport& Viewport, const LWorld& World, const LEye& Eye) = 0;
    virtual void   OnFree() { this->bMeaningful = false; }

    FORCEINLINE void Use() const { check( this->IsMeaningful() ) this->Program.Use(); }

    FORCEINLINE auto GetProgram()       noexcept ->       LShader& { return this->Program; }
    FORCEINLINE auto GetProgram() const noexcept -> const LShader& { return this->Program; }

    FORCEINLINE u32 GetUuid() const noexceptcheck { check( this->IsMeaningful() ) return this->Uuid; }
    FORCEINLINE bool   IsMeaningful() const noexcept { return this->bMeaningful; }

protected:

    //# Memory not managed by this class. Deallocate the GPU memory yourself.
    LShader Program;

private:

    u32 Uuid = 0;
    bool   bMeaningful = false;
};

} /* ~Namespace Jafg */
