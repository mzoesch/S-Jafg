// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Shader.h"

namespace Jafg
{

class LTexture2;
class LViewport;

//#
//# A simple shader helping class that renders a box on an orthographic projection.
//#
class ENGINE_API LBoxShaderContext
{
public:

    LBoxShaderContext() = default;
    PROHIBIT_COPY(LBoxShaderContext)
    DEFAULT_MOVE(LBoxShaderContext)
    ~LBoxShaderContext() { this->Free(); }

    //#
    //# Make the shader context meaningful.
    //#
    void Make();

    //#
    //# Free the shader context from the graphical device.
    //# No C++ memory will be freed through this method.
    //#
    void Free();

    void Draw(
        const LViewport& Context,
        const LVector2&  Size,
        const LVector2&  TopLeft,
        const LColor&    Color,
        const LTexture2* Texture = nullptr
    ) const;

    FORCEINLINE auto IsMeaningful() const -> bool { return this->bIsMeaningful; }

    void ReloadShader() const;

private:

    void FreeStayMeaningful() const;

    bool bIsMeaningful = false;

    mutable LShader Shader = {};
    mutable uint32  Vao    = 0x0u;
    mutable uint32  Vbo    = 0x0u;
    mutable uint32  Tex    = 0x0u;
    mutable uint32  Ebo    = 0x0u;
    mutable const LTexture2* LastFrameTexture  = nullptr;
};

} /* ~Namespace Jafg */
