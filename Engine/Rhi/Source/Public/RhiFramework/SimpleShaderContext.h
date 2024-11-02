// Copyright mzoesch. All rights reserved.

#pragma once

#include "RhiFramework/Shader.h"

namespace Jafg
{

class RHI_API LSimpleShaderContext
{
public:

    LSimpleShaderContext() = default;

    FORCEINLINE auto GetShader() -> LShader& { return this->Shader; }
    FORCEINLINE auto GetShader() const -> const LShader& { return this->Shader; }
    FORCEINLINE auto GetVao() -> uint32& { return this->Vao; }
    FORCEINLINE auto GetVao() const -> const uint32& { return this->Vao; }
    FORCEINLINE auto GetVbo() -> uint32& { return this->Vbo; }
    FORCEINLINE auto GetVbo() const -> const uint32& { return this->Vbo; }
    FORCEINLINE auto GetShaderPtr() -> LShader* { return &this->Shader; }
    FORCEINLINE auto GetShaderPtr() const -> const LShader* { return &this->Shader; }
    FORCEINLINE auto GetVaoPtr() -> uint32* { return &this->Vao; }
    FORCEINLINE auto GetVaoPtr() const -> const uint32* { return &this->Vao; }
    FORCEINLINE auto GetVboPtr() -> uint32* { return &this->Vbo; }
    FORCEINLINE auto GetVboPtr() const -> const uint32* { return &this->Vbo; }

private:

    LShader Shader;
    uint32 Vao   = 0x0u;
    uint32 Vbo   = 0x0u;
};

} /* ~Namespace Jafg. */
