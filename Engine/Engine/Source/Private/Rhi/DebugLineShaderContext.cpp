// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/DebugLineShaderContext.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

void Jafg::LDebugTraceLineShaderContext::Make()
{
    LGenericShaderContext::Make();

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "DebugTrace"));
    this->Program.Use();

    glGenVertexArrays(1, &this->Vao);
    glBindVertexArray(this->Vao);

    glGenBuffers(1, &this->Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

#if WITH_DEBUG_ZERO_UNBOUND
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LDebugTraceLineShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LDebugTraceLineShaderContextDrawArgs)

    float Vertices[6] = { };
    ::memcpy(Vertices, Args.Start.GetData(), Args.Start.GetDataByteSize());
    ::memcpy(Vertices + 3, Args.End.GetData(), Args.End.GetDataByteSize());

    this->Program.Use();
    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, Vertices, GL_DYNAMIC_DRAW);

    const TMatrix Projection = Maths::MakePerspectiveProjectionMatrix(
        Maths::ToRadians(Args.DegYFov),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        0.1f, 2000.0f
    );
    LMatrix Model; Model.InlineTranslate(LVector::Zero());

    this->Program.SetColorUniform("Color", Args.Color);
    this->Program.SetMatrixUniform("View", Args.ViewMatrix);
    this->Program.SetMatrixUniform("Projection", Projection);

    glDrawArrays(GL_LINES, 0, 2);

#if WITH_DEBUG_ZERO_UNBOUND
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LDebugTraceLineShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    glDeleteBuffers(1, &this->Vbo);
    glDeleteVertexArrays(1, &this->Vao);
    this->Program.Free();

    return;
}
