// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class RHI_API LShader
{
public:

    LShader() = default;

    // constructor reads and builds the shader
    LShader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use();
    void SetBoolUniform(const LSimpleString& Name, const bool Value) const;
    void SetIntUniform(const LSimpleString& Name, const int32 Value) const;
    void SetFloatUniform(const LSimpleString& Name, const float Value) const;

    FORCEINLINE auto GetId() const -> uint32 { return this->Id; }

private:

    uint32 Id;
};

} /* ~Namespace Jafg. */
