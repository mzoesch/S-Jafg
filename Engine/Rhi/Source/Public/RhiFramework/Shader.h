// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include <glm/fwd.hpp>

namespace Jafg
{

class RHI_API LShader
{
public:

    LShader() = default;

    void Free();

    // constructor reads and builds the shader
    LShader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use() const;
    void SetBoolUniform(const LSimpleString& Name, const bool Value) const;
    void SetIntUniform(const LSimpleString& Name, const int32 Value) const;
    void SetUIntUniform(const LSimpleString& Name, const uint32 Value) const;
    void SetFloatUniform(const LSimpleString& Name, const float Value) const;
    void SetVector2Uniform(const LSimpleString& Name, const glm::vec2& Value) const;
    void SetVector4Uniform(const LSimpleString& Name, const glm::vec4& Value) const;
    void SetMatrix4Uniform(const LSimpleString& Name, const glm::mat4& Value) const;


    FORCEINLINE auto GetId() const -> uint32 { return this->Id; }

private:

    uint32 Id;
};

} /* ~Namespace Jafg. */
