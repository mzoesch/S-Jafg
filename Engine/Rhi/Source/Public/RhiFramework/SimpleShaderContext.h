// Copyright mzoesch. All rights reserved.

#pragma once

#include "RhiFramework/Shader.h"

namespace Jafg
{

class LViewport;

/**
 * A simple shader helping class that renders a box on an orthographic projection.
 */
class RHI_API LBoxShaderContext
{
public:

    LBoxShaderContext() = default;
    PROHIBIT_COPY(LBoxShaderContext)
    DEFAULT_MOVE(LBoxShaderContext)
    ~LBoxShaderContext() { this->Free(); }

    /**
     * Make the shader context meaningful.
     */
    void Make();

    /**
     * Free the shader context from the graphical device.
     * No C++ memory will be freed through this method.
     */
    void Free();

    /**
     * Draws the box.
     */
    void Draw(const LViewport& Context, const LVector2& Size, const LVector2& TopLeft) const;

    FORCEINLINE auto IsMeaningful() const -> bool { return this->bIsMeaningful; }

private:

    bool    bIsMeaningful = false;
    LShader Shader;
    uint32  Vao   = 0x0u;
    uint32  Vbo   = 0x0u;
};

} /* ~Namespace Jafg. */
