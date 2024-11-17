// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/Shader.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include "RhiVendorInclude.h"
#include "User/UserPreferences.h"
#include "System/EnginePath.h"

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
    const JUserPreferences& Preferences = *GetMutableDefault<JUserPreferences>();

    LEnginePath VertexPath = Path;
    VertexPath.AddExtension(".vert");
    LEnginePath FragmentPath = Path;
    FragmentPath.AddExtension(".frag");

    this->LoadShader(
        VertexPath.ResolveAbsolutePath(Preferences).GetPath().ToC(),
        FragmentPath.ResolveAbsolutePath(Preferences).GetPath().ToC()
    );

    return;
}

void Jafg::LShader::Use() const
{
    glUseProgram(Id);
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

void Jafg::LShader::LoadShader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::string ExecPath(PLATFORM_MAX_PATH, '\0');
        GetModuleFileNameA(nullptr, ExecPath.data(), static_cast<DWORD>(ExecPath.size()));
        ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));

        LOG_TRACE(LogPlatform, "Opening vertex shader file: [{}].", vertexPath)
        LOG_TRACE(LogPlatform, "Opening fragment shader file: [{}].", fragmentPath)

        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streamss
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure& E)
    {
        std::cout << "Error reading shader source files.\n"
            << "Vertex path: " << vertexPath << '\n'
            << "Fragment path: " << fragmentPath << '\n'
            << E.what() << '\n';
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Error compiling vertex shader!\n" << infoLog << '\n';
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Error compiling fragment shader!\n" << infoLog << '\n';
    }

    // shader program
    Id = glCreateProgram();
    glAttachShader(Id, vertex);
    glAttachShader(Id, fragment);
    glLinkProgram(Id);
    // print linking errors if any
    glGetProgramiv(Id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(Id, 512, NULL, infoLog);
        std::cout << "Error linking shader program!\n" << infoLog << '\n';
    }

    // delete the shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
