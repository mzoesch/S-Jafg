// Copyright mzoesch. All rights reserved.

#pragma once

#include "GenericShaderContext.h"

namespace Jafg
{

struct LChunkShaderDrawArgs : public LGenericShaderContextDrawArgs
{
    float   DegYFov    = 0.0f;
    LMatrix ViewMatrix = LMatrix(Matrix::Identity);
};

class RHI_API LChunkShaderContext final : public LGenericShaderContext
{
public:

    virtual void Make() override;
    virtual void OnFree() override;
    virtual void Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const override;

    FORCEINLINE LShader* GetProgram() { return this->Program; }
    FORCEINLINE const LShader* GetProgram() const { return this->Program; }

    FORCEINLINE uint32 GetTexture() const { return this->Texture; }

private:

    LShader* Program = nullptr;
    uint32   Texture = 0;
};

} /* ~Namespace Jafg */
