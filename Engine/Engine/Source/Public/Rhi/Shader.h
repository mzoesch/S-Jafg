// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/SystemForward.h"

namespace Jafg
{

class ENGINE_API LShader
{
public:

    LShader() = default;

    void Free();

    explicit LShader(const LEnginePath& Path);

    //#
    //# When using this method, you have to make sure to delete the previous shader from the graphic processing unit.
    //# Memory leaks will occur if you don't.
    //#
    void Load(const LEnginePath& Path);

    void Use() const;

    template <typename T>
    void SetUniform(const LSimpleString& Name, const T Value) const UNSUPPORTED_TEMPLATED_SPECIALIZATION(T)

    void SetBoolUniform(const LSimpleString& Name, const bool Value) const;
    void SetIntUniform(const LSimpleString& Name, const int32 Value) const;
    void SetUIntUniform(const LSimpleString& Name, const uint32 Value) const;
    void SetFloatUniform(const LSimpleString& Name, const float Value) const;

    void SetVec3Uniform(const LSimpleString& Name, const LVector3& Value) const;
    void SetVec4Uniform(const LSimpleString& Name, const LVector4& Value) const;
    void SetMatrixUniform(const LSimpleString& Name, const LMatrixF& Value) const;

    //# Emits an int32 from a LColor.
    void SetColorUniform(const LSimpleString& Name, const LColor& Value) const;
    //# Emits an vec3 from LColor (without the alpha channel).
    void SetColorVec3Uniform(const LSimpleString& Name, const LColor& Value) const;
    void SetColorVec4Uniform(const LSimpleString& Name, const LColor& Value) const;

    FORCEINLINE auto GetId() const -> uint32 { return this->Id; }

private:

    void LoadShader(const LEnginePath& VertexPath, const LEnginePath& FragmentPath);

    uint32 Id = NULL;
};

template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const bool Value) const { this->SetBoolUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const int32 Value) const { this->SetIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const uint32 Value) const { this->SetUIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const float Value) const { this->SetFloatUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const LMatrixF& Value) const { this->SetMatrixUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LSimpleString& Name, const LColor& Value) const { this->SetColorUniform(Name, Value); }

} /* ~Namespace Jafg */
