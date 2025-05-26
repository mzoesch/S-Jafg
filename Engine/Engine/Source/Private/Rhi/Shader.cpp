// Copyright mzoesch. All rights reserved.

#include "Rhi/Shader.h"
#include "Rhi/RhiVendorInclude.h"
#include "System/EnginePath.h"
#include "System/Finder.h"

Jafg::LShader::LShader(const LEnginePath& Path)
{
    this->Load(Path);
}

Jafg::LShader::LShader(const LEnginePath& Path, const TArray<LShaderCompileTimeConstant>& InConstants)
{
    this->Load(Path, InConstants);
}

Jafg::LShader::~LShader()
{
    if (this->bLoaded)
    {
        this->Free();
    }

    return;
}

void Jafg::LShader::Free()
{
    if (this->bLoaded)
    {
        glDeleteProgram(this->Id);
    #if WITH_DEBUG_ZERO_UNBOUND
        this->Id = 0x0u;
    #endif /* WITH_DEBUG_ZERO_UNBOUND */
        this->bLoaded = false;
    }
    else
    {
        LOG_ERROR(LogRhi, "Tried to free a shader that was not loaded.")
    }

    return;
}

void Jafg::LShader::Load(const LEnginePath& Path)
{
    const TArray<LShaderCompileTimeConstant> Dummy;
    this->Load(Path, Dummy);

    return;
}

void Jafg::LShader::Load(const LEnginePath& Path, const TArray<LShaderCompileTimeConstant>& InConstants)
{
    if (this->bLoaded)
    {
        LOG_ERROR(LogRhi, "Tried to load or overwrite a shader that was already loaded.")
        this->Free();
    }

    LEnginePath VertexPath = Path;
    VertexPath.AddExtension(".vert");
    LEnginePath FragmentPath = Path;
    FragmentPath.AddExtension(".frag");

    this->LoadShader(VertexPath, FragmentPath, InConstants);

    return;
}

void Jafg::LShader::Use() const
{
    check( this->bLoaded )

    glUseProgram(this->Id);

    return;
}

#if WITH_DEBUG_ZERO_UNBOUND
void Jafg::LShader::Unuse() const
{
    glUseProgram(0);
}
#endif /* WITH_DEBUG_ZERO_UNBOUND */

void Jafg::LShader::SetBoolUniform(const LString& Name, const bool Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform1i(glGetUniformLocation(this->Id, Name.ToPtr()), static_cast<int>(Value));

    return;
}

void Jafg::LShader::SetIntUniform(const LString& Name, const i32 Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform1i(glGetUniformLocation(this->Id, Name.ToPtr()), Value);

    return;
}

void Jafg::LShader::SetUIntUniform(const LString& Name, const u32 Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform1ui(glGetUniformLocation(this->Id, Name.ToPtr()), Value);

    return;
}

void Jafg::LShader::SetFloatUniform(const LString& Name, const f32 Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform1f(glGetUniformLocation(this->Id, Name.ToPtr()), Value);

    return;
}

void Jafg::LShader::SetVec2Uniform(const LString& Name, const LVector2& Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform2f(glGetUniformLocation(this->Id, Name.ToPtr()), Value.X, Value.Y);

    return;
}

void Jafg::LShader::SetVec3Uniform(const LString& Name, const LVector3& Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform3f(glGetUniformLocation(this->Id, Name.ToPtr()), Value.X, Value.Y, Value.Z);

    return;
}

void Jafg::LShader::SetVec4Uniform(const LString& Name, const LVector4& Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniform4f(glGetUniformLocation(this->Id, Name.ToPtr()), Value.X, Value.Y, Value.Z, Value.W);

    return;
}

void Jafg::LShader::SetMatrixUniform(const LString& Name, const LMatrixF& Value) const
{
    checkCode
    (
        if (glGetUniformLocation(this->Id, Name.ToPtr()) < 0)
        {
            LOG_WARNING(LogRhi, "Invalid uniform: [{}].", Name)
        }
    )

    glUniformMatrix4fv(glGetUniformLocation(this->Id, Name.ToPtr()), 1, GL_FALSE, Value.GetData());

    return;
}

void Jafg::LShader::SetColorUniform(const LString& Name, const LColor& Value) const
{
    this->SetIntUniform(Name, *reinterpret_cast<const i32*>(&Value.Bits));
}

void Jafg::LShader::SetColorVec3Uniform(const LString& Name, const LColor& Value) const
{
    this->SetVec3Uniform(Name, Value.ToVector3());
}

void Jafg::LShader::SetColorVec4Uniform(const LString& Name, const LColor& Value) const
{
    this->SetVec4Uniform(Name, Value.ToVector4());
}

void Jafg::LShader::LoadShader(const LEnginePath& VertexPath, const LEnginePath& FragmentPath, const TArray<LShaderCompileTimeConstant>& InConstants)
{
    LString UncompiledVertex   = Finder::ReadFile(VertexPath);
    LString UncompiledFragment = Finder::ReadFile(FragmentPath);

    i32 AddConstantsIdxFragment = INDEX_NONE;
    i32 AddConstantsIdxVertex   = INDEX_NONE;

    const i32 HashVersionFragment = UncompiledFragment.FindFirst("#version");
    const i32 HashVersionVertex   = UncompiledVertex.FindFirst("#version");
    jassert( HashVersionFragment != INDEX_NONE )
    jassert( HashVersionVertex   != INDEX_NONE )

    for (i32 i = HashVersionFragment; i < UncompiledFragment.GetSize(); ++i)
    {
        if (UncompiledFragment[i] == '\n')
        {
            AddConstantsIdxFragment = i + 1;
            break;
        }

        continue;
    }
    jassert( AddConstantsIdxFragment != INDEX_NONE )

    for (i32 i = HashVersionVertex; i < UncompiledVertex.GetSize(); ++i)
    {
        if (UncompiledVertex[i] == '\n')
        {
            AddConstantsIdxVertex = i + 1;
            break;
        }

        continue;
    }
    jassert( AddConstantsIdxVertex != INDEX_NONE )

    LString ConstantsAsStr;
    for (const auto& [Name, Value] : InConstants)
    {
        ConstantsAsStr += LString::SprintF("#define {} {}\n", Name, Value);
    }

    UncompiledFragment.AppendAt(AddConstantsIdxFragment, ConstantsAsStr);
    UncompiledVertex.AppendAt(AddConstantsIdxVertex, ConstantsAsStr);

    const char* UncompiledVertexC   = UncompiledVertex.ToPtr();
    const char* UncompiledFragmentC = UncompiledFragment.ToPtr();

    i32 Success;
    char InfoLog[512];

    const u32 Vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vertex, 1, &UncompiledVertexC, nullptr);
    glCompileShader(Vertex);

    glGetShaderiv(Vertex, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        glGetShaderInfoLog(Vertex, 512, nullptr, InfoLog);
        panicMsgf( "Error compiling vertex shader.\n{}", InfoLog )
        return;
    }

    const u32 Fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Fragment, 1, &UncompiledFragmentC, nullptr);
    glCompileShader(Fragment);
    glGetShaderiv(Fragment, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        glGetShaderInfoLog(Fragment, 512, nullptr, InfoLog);
        panicMsgf( "Error compiling fragment shader.\n{}", InfoLog )
        return;
    }

    this->Id = glCreateProgram();
    glAttachShader(this->Id, Vertex);
    glAttachShader(this->Id, Fragment);
    glLinkProgram(this->Id);
    glGetProgramiv(this->Id, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(this->Id, 512, nullptr, InfoLog);
        panicMsgf( "Error linking shader program.\n{}", InfoLog )
        return;
    }

    glValidateProgram(this->Id);

    glDeleteShader(Vertex);
    glDeleteShader(Fragment);

    this->bLoaded = true;

    return;
}
