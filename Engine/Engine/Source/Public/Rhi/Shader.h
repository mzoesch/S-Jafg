// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/SystemForward.h"
#include "System/EnginePath.h"

namespace Jafg
{

//#
//# A compile time constant that can be dynamically set before compiling a shader program.
//#
struct LShaderCompileTimeConstant
{
    //#
    //# The name of the constant.
    //#
    LString Name;

    //#
    //# An optional value for the constant. Leave empty for just defining the name to the program.
    //#
    LString Value;

    FORCEINLINE bool operator==(const LShaderCompileTimeConstant& Rhs) const noexcept
    {
        return this->Name == Rhs.Name;
    }
    FORCEINLINE bool operator!=(const LShaderCompileTimeConstant& Rhs) const noexcept
    {
        return !(*this == Rhs);
    }

    FORCEINLINE bool operator==(const LString& Rhs) const noexcept
    {
        return this->Name == Rhs;
    }
    FORCEINLINE bool operator!=(const LString& Rhs) const noexcept
    {
        return !(*this == Rhs);
    }
};

//#
//# A shader with a direct handle to the shader on the graphical processing unit.
//#
class LShader final
{
public:

    ENGINE_API LShader() = default;
    ENGINE_API explicit LShader(const LEnginePath& Path);
    ENGINE_API explicit LShader(const LEnginePath& Path, const TArray<LShaderCompileTimeConstant>& InConstants);
    ENGINE_API explicit LShader(LString&& UncompiledVertex, LString&& UncompiledFragment, const TArray<LShaderCompileTimeConstant>& InConstants);

    PROHIBIT_COPY(LShader)
    FORCEINLINE LShader(LShader&& InShader) noexcept { *this = std::move(InShader); }
    FORCEINLINE LShader& operator=(LShader&& InShader) noexcept;

    ENGINE_API~LShader();

    ENGINE_API void Free();
    ENGINE_API void Load(const LEnginePath& Path);
    ENGINE_API void Load(const LEnginePath& Path, const TArray<LShaderCompileTimeConstant>& InConstants);
    ENGINE_API void Load(const TArray<LShaderCompileTimeConstant>& InConstants);
    ENGINE_API void Load(LString&& UncompiledVertex, LString&& UncompiledFragment, const TArray<LShaderCompileTimeConstant>& InConstants = {});
    ENGINE_API void Recompile(const TArray<LShaderCompileTimeConstant>& InConstants);
    ENGINE_API void Use() const;

    template <typename T>
    inline     void SetUniform(const LString& Name, const T Value) const UNSUPPORTED_TEMPLATED_SPECIALIZATION(T)

    ENGINE_API void SetBoolUniform(const LString& Name, const bool Value) const;
    ENGINE_API void SetIntUniform(const LString& Name, const i32 Value) const;
    ENGINE_API void SetUIntUniform(const LString& Name, const u32 Value) const;
    ENGINE_API void SetFloatUniform(const LString& Name, const f32 Value) const;

    ENGINE_API void SetVec2Uniform(const LString& Name, const LVector2& Value) const;
    ENGINE_API void SetVec3Uniform(const LString& Name, const LVector3& Value) const;
    ENGINE_API void SetVec4Uniform(const LString& Name, const LVector4& Value) const;
    ENGINE_API void SetMatrixUniform(const LString& Name, const LMatrixF& Value) const;

    //# Emits an i32 from a LColor.
    ENGINE_API void SetColorUniform(const LString& Name, const Lal::LColor& Value) const;
    //# Emits a vec3 from LColor (without the alpha channel).
    ENGINE_API void SetColorVec3Uniform(const LString& Name, const Lal::LColor& Value) const;
    ENGINE_API void SetColorVec4Uniform(const LString& Name, const Lal::LColor& Value) const;

    FORCEINLINE const LEnginePath& GetCachedPath() const noexcept { return this->CachedPath; }

    FORCEINLINE u32 GetId() const noexcept { check( this->bLoaded ) return this->Id; }
    FORCEINLINE operator u32() const noexcept { check( this->bLoaded ) return this->Id; }

private:

    void LoadImpl(LString&& UncompiledVertex, LString&& UncompiledFragment, const TArray<LShaderCompileTimeConstant>& InConstants);

    bool bLoaded { false };
    LEnginePath CachedPath;
    u32 Id = NULL;
};

FORCEINLINE LShader& LShader::operator=(LShader&& InShader) noexcept
{
    if (this->bLoaded)
    {
        LOG_ERROR(LogRhi, "Tried to load or overwrite a shader that was already loaded.")
        this->Free();
    }

    this->bLoaded = InShader.bLoaded;
    this->CachedPath = std::move(InShader.CachedPath);
    this->Id = InShader.Id;
    InShader.bLoaded = false;

    return *this;
}

template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const bool Value) const { this->SetBoolUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const i32 Value) const { this->SetIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const u32 Value) const { this->SetUIntUniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const f32 Value) const { this->SetFloatUniform(Name, Value); }

template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LVector2& Value) const { this->SetVec2Uniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LVector3& Value) const { this->SetVec3Uniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LVector4& Value) const { this->SetVec4Uniform(Name, Value); }
template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const LMatrixF& Value) const { this->SetMatrixUniform(Name, Value); }

template <>
FORCEINLINE void LShader::SetUniform(const LString& Name, const Lal::LColor& Value) const { this->SetColorUniform(Name, Value); }

} /* ~Namespace Jafg */
