// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "RhiFramework/Shader.h"
#include <filesystem>
#include <glad/glad.h> /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#include <fstream>
#include <sstream>
#include <iostream>
#if PLATFORM_WINDOWS
#include <Windows.h>
#endif /* PLATFORM_WINDOWS */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

void Jafg::LShader::Free()
{
    glDeleteProgram(Id);
}

Jafg::LShader::LShader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::string ExecPath(PLATFORM_MAX_PATH, '\0');
        GetModuleFileNameA(nullptr, ExecPath.data(), static_cast<DWORD>(ExecPath.size()));
        ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));

        std::cout << "Opening vertex shader file: " << ExecPath + '/' + vertexPath << '\n';
        std::cout << "Opening fragment shader file: " << ExecPath + '/' + fragmentPath << '\n';

        // open files
        vShaderFile.open(ExecPath + '/' + vertexPath);
        fShaderFile.open(ExecPath + '/' + fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
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
            << E.what() << '\n'
            << std::filesystem::current_path() << '\n';
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

void Jafg::LShader::SetVector2Uniform(const LSimpleString& Name, const glm::vec2& Value) const
{
    glUniform2fv(glGetUniformLocation(this->Id, Name.ToC()), 1, glm::value_ptr(Value));
}

void Jafg::LShader::SetVector4Uniform(const LSimpleString& Name, const glm::vec4& Value) const
{
    glUniform4fv(glGetUniformLocation(this->Id, Name.ToC()), 1, glm::value_ptr(Value));
}

void Jafg::LShader::SetMatrix4Uniform(const LSimpleString& Name, const glm::mat4& Value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->Id, Name.ToC()), 1, GL_FALSE, glm::value_ptr(Value));
}

void Jafg::LShader::SetMatrix4Uniform(const LSimpleString& Name, const LMatrixF& Value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->Id, Name.ToC()), 1, GL_FALSE, Value.GetData());
}
