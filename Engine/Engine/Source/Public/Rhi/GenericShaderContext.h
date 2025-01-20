// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LShader;
class LViewport;

#define GENERIC_SHADER_DRAW_BODY(ArgsTy) \
    LGenericShaderContext::Draw(Context, InArgs); \
    ArgsTy& Args = *reinterpret_cast<ArgsTy*>(&InArgs);

struct LGenericShaderContextDrawArgs
{
};

class ENGINE_API LGenericShaderContext
{
public:

    LGenericShaderContext() = default;
    PROHIBIT_COPY(LGenericShaderContext)
    DEFAULT_MOVE(LGenericShaderContext)
    virtual ~LGenericShaderContext() { this->Free(); }

    //#
    //# Make the shader context meaningful.
    //#
    virtual void Make();

    //#
    //# Free the shader context from the graphical device.
    //# No C++ memory will be freed through this method.
    //#
    void Free();
    virtual void OnFree() { }

    void ReloadShader() { if (this->IsMeaningful()) { this->OnReload(); } else { this->Make(); } }
    virtual void OnReload() { }

    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const { checkSlow( this->IsMeaningful() ) }

    FORCEINLINE auto IsMeaningful() const -> bool { return this->bMeaningful; }

private:

    bool bMeaningful = false;
};

} /* ~Namespace Jafg */
