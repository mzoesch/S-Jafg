// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

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

class RHI_API LGenericShaderContext
{
public:

    LGenericShaderContext() = default;
    PROHIBIT_COPY(LGenericShaderContext)
    DEFAULT_MOVE(LGenericShaderContext)
    virtual ~LGenericShaderContext() { this->Free(); }

    /**
     * Make the shader context meaningful.
     */
    virtual void Make();

    /**
     * Free the shader context from the graphical device.
     * No C++ memory will be freed through this method.
     */
    void Free();
    virtual void OnFree() { }

    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const { }

    FORCEINLINE auto IsMeaningful() const -> bool { return this->bMeaningful; }

private:

    bool bMeaningful = false;
};

} /* ~Namespace Jafg */
