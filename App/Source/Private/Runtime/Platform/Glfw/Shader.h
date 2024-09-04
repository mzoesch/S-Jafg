// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "glm.hpp"

class Shader
{
public:

    explicit Shader(String Path);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1I(const String& Name, const int32 V0) const;
    void SetUniform4F(const String& Name, const float V0, const float V1, const float V2, const float V3) const;
    void SetUniformMat4F(const String& Name, const glm::mat4& Matrix) const;

private:

    int32 GetUniformLocation(const String& Name) const;

    uint32 CreateShader(const String& InVertexShader, const String& InFragmentShader);
    uint32 CompileShader(const uint32 Type, const String& Source);
    void   ParseShader(String& OutVertexShader, String& OutFragmentShader) const;

    String FilePath;
    uint32 RendererId;
    mutable std::unordered_map<String, int32> UniformLocationCache;
};
