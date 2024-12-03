// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/Shader.h"
#include "Rhi/RhiVendorInclude.h"
#include "System/EnginePath.h"
#include "System/Finder.h"

void Jafg::LShader::Free()
{
    glDeleteProgram(this->Id);
#if WITH_DEBUG_ZERO_UNBOUND
    this->Id = 0x0u;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

Jafg::LShader::LShader(const char* vertexPath, const char* fragmentPath)
{
    LoadShader(vertexPath, fragmentPath);
}

Jafg::LShader::LShader(const LEnginePath& Path)
{
    LEnginePath VertexPath = Path;
    VertexPath.AddExtension(".vert");
    LEnginePath FragmentPath = Path;
    FragmentPath.AddExtension(".frag");

    this->LoadShader(VertexPath, FragmentPath);

    return;
}

void Jafg::LShader::Use() const
{
    glUseProgram(this->Id);
}

void Jafg::LShader::SetBoolUniform(const LSimpleString& Name, const bool Value) const
{
    glUniform1i(glGetUniformLocation(this->Id, Name.ToC()), static_cast<int>(Value));
}

void Jafg::LShader::SetIntUniform(const LSimpleString& Name, const int32 Value) const
{
    glUniform1i(glGetUniformLocation(this->Id, Name.ToC()), Value);
}

void Jafg::LShader::SetUIntUniform(const LSimpleString& Name, const uint32 Value) const
{
    glUniform1ui(glGetUniformLocation(this->Id, Name.ToC()), Value);
}

void Jafg::LShader::SetFloatUniform(const LSimpleString& Name, const float Value) const
{
    glUniform1f(glGetUniformLocation(this->Id, Name.ToC()), Value);
}

void Jafg::LShader::SetMatrixUniform(const LSimpleString& Name, const LMatrixF& Value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->Id, Name.ToC()), 1, GL_FALSE, Value.GetData());
}

void Jafg::LShader::LoadShader(const LEnginePath& VertexPath, const LEnginePath& FragmentPath)
{
    const LStringLegacy UncompiledVertex   = Finder::ReadFile(VertexPath);
    const LStringLegacy UncompiledFragment = Finder::ReadFile(FragmentPath);
    const char* UncompiledVertexC   = UncompiledVertex.c_str();
    const char* UncompiledFragmentC = UncompiledFragment.c_str();

    int32 Success;
    char InfoLog[512];

    const uint32 Vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vertex, 1, &UncompiledVertexC, nullptr);
    glCompileShader(Vertex);

    glGetShaderiv(Vertex, GL_COMPILE_STATUS, &Success);
    if (!Success)
    {
        glGetShaderInfoLog(Vertex, 512, nullptr, InfoLog);
        panicMsgf( "Error compiling vertex shader.\n{}", InfoLog )
        return;
    }

    const uint32 Fragment = glCreateShader(GL_FRAGMENT_SHADER);
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

    glDeleteShader(Vertex);
    glDeleteShader(Fragment);

    return;
}
