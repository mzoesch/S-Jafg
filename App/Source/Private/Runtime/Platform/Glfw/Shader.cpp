// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Shader.h"
#include "Renderer.h"
#include <fstream>
#include <sstream>

Shader::Shader(String Path) : FilePath(std::move(Path)), RendererId(0)
{
    String VertexShader; String FragmentShader;
    ParseShader(VertexShader, FragmentShader);

    this->RendererId = CreateShader(VertexShader, FragmentShader);

    return;
}

Shader::~Shader()
{
    GL_CALL( glDeleteProgram(this->RendererId ) )
}

void Shader::Bind() const
{
    GL_CALL( glUseProgram(this->RendererId) )
}

void Shader::Unbind() const
{
    GL_CALL( glUseProgram(0) )
}

void Shader::SetUniform1I(const String& Name, const int32 V0) const
{
    GL_CALL( glUniform1i(this->GetUniformLocation(Name), V0) )
}

void Shader::SetUniform4F(const String& Name, const float V0, const float V1, const float V2, const float V3) const
{
    GL_CALL( glUniform4f(this->GetUniformLocation(Name), V0, V1, V2, V3) )
}

void Shader::SetUniformMat4F(const String& Name, const glm::mat4& Matrix) const
{
    GL_CALL( glUniformMatrix4fv(this->GetUniformLocation(Name), 1, GL_FALSE, &Matrix[0][0]) )
}

int32 Shader::GetUniformLocation(const String& Name) const
{
    if (this->UniformLocationCache.contains(Name))
    {
        return this->UniformLocationCache[Name];
    }

    GL_CALL( const int32 Location = glGetUniformLocation(this->RendererId, Name.c_str()) )

    if (Location == -1)
    {
        LOG_WARNING(LogCore, "Uniform '{}' not found in shader.", Name)
    }

    this->UniformLocationCache[Name] = Location;

    return Location;
}

uint32 Shader::CreateShader(const String& InVertexShader, const String& InFragmentShader)
{
    const uint32 Program = glCreateProgram();

    const uint32 VertexShader   = CompileShader(GL_VERTEX_SHADER, InVertexShader);
    const uint32 FragmentShader = CompileShader(GL_FRAGMENT_SHADER, InFragmentShader);

    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    glValidateProgram(Program);

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}

uint32 Shader::CompileShader(const uint32 Type, const String& Source)
{
    const uint32 Id  = glCreateShader(Type);
    const char*  Src = Source.c_str();

    glShaderSource(Id, 1, &Src, nullptr);
    glCompileShader(Id);

    int32 Result;
    glGetShaderiv(Id, GL_COMPILE_STATUS, &Result);

    if (Result == GL_FALSE)
    {
        int32 Length;
        glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &Length);

        char* Message = static_cast<char*>(alloca(Length * sizeof(char)));
        glGetShaderInfoLog(Id, Length, &Length, Message);

        jassert( false )

        /*
         * Do not remove this return for now as else we would let go of the char* Message (out of scope)
         * and therefore would not be able to see the error message in the debugger.
         */
        return Id;
    }

    return Id;
}

void Shader::ParseShader(String& OutVertexShader, String& OutFragmentShader) const
{
    std::ifstream Stream(this->FilePath);

    enum class ShaderType : int8 { None = -1, Vertex = 0, Fragment = 1, };

    String Line;
    std::stringstream Ss[2];
    ShaderType Type = ShaderType::None;
    while (getline(Stream, Line))
    {
        if (Line.find("#shader") != String::npos)
        {
            if (Line.find("Vertex") != String::npos)
            {
                Type = ShaderType::Vertex;
            }
            else if (Line.find("Fragment") != String::npos)
            {
                Type = ShaderType::Fragment;
            }
        }
        else
        {
            Ss[static_cast<int32>(Type)] << Line << '\n';
        }

        continue;
    }

    OutVertexShader   = Ss[0].str();
    OutFragmentShader = Ss[1].str();

    return;
}
