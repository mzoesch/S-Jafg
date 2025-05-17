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

#if WITH_DEBUG_ZERO_UNBOUND
    //# Debugging only.
    void Unuse() const;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    template <typename T>
    void SetUniform(const LString& Name, const T Value) const UNSUPPORTED_TEMPLATED_SPECIALIZATION(T)

    void SetBoolUniform(const LString& Name, const bool Value) const;
    void SetIntUniform(const LString& Name, const i32 Value) const;
    void SetUIntUniform(const LString& Name, const u32 Value) const;
    void SetFloatUniform(const LString& Name, const f32 Value) const;

    void SetVec3Uniform(const LString& Name, const LVector3& Value) const;
    void SetVec4Uniform(const LString& Name, const LVector4& Value) const;
    void SetMatrixUniform(const LString& Name, const LMatrixF& Value) const;

    //# Emits an i32 from a LColor.
    void SetColorUniform(const LString& Name, const LColor& Value) const;
    //# Emits a vec3 from LColor (without the alpha channel).
    void SetColorVec3Uniform(const LString& Name, const LColor& Value) const;
    void SetColorVec4Uniform(const LString& Name, const LColor& Value) const;

    FORCEINLINE u32 GetId() const { return this->Id; }

private:

    void LoadShader(const LEnginePath& VertexPath, const LEnginePath& FragmentPath);

    u32 Id = NULL;
};

template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const bool Value) const { this->SetBoolUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const i32 Value) const { this->SetIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const u32 Value) const { this->SetUIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const f32 Value) const { this->SetFloatUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LMatrixF& Value) const { this->SetMatrixUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LColor& Value) const { this->SetColorUniform(Name, Value); }

} /* ~Namespace Jafg */
