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

    void Use() const;

    template <typename T>
    void SetUniform(const LSimpleString& Name, const T Value) const UNSUPPORTED_TEMPLATED_SPECIALIZATION(T)

    void SetBoolUniform(const LSimpleString& Name, const bool Value) const;
    void SetIntUniform(const LSimpleString& Name, const int32 Value) const;
    void SetUIntUniform(const LSimpleString& Name, const uint32 Value) const;
    void SetFloatUniform(const LSimpleString& Name, const float Value) const;
    void SetMatrixUniform(const LSimpleString& Name, const LMatrixF& Value) const;

    FORCEINLINE auto GetId() const -> uint32 { return this->Id; }

private:

    void LoadShader(const LEnginePath& VertexPath, const LEnginePath& FragmentPath);

    uint32 Id;
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

} /* ~Namespace Jafg. */
