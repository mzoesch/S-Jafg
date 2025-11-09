// Copyright mzoesch. All rights reserved.

#include "Rhi/DebugTraceCubeShaderContext.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"
#include "Framework/Eye.h"

#if 0

void Jafg::LDebugTraceCubeShaderContext::Make()
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

    glBindVertexArray(0);

    return;
}

void Jafg::LDebugTraceCubeShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LDebugTraceCubeShaderContextDrawArgs)

    const float Vertices[/* Lines */12 * /* Vertices */2 * /* Domains */3] =
    {
        // Bottom Near Left                                                                                                                             Top Near Left
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,                                                                            Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Top Near Left                                                                                                                                Top Near Right
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,                                                    Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Top Near Right                                                                                                                               Bottom Near Right
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,                            Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,
        // Bottom Near Right                                                                                                                            Bottom Near Left
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,                                                    Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,

        // Bottom Far Left                                                                                                                              Top Far Left
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,                                                    Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Top Far Left                                                                                                                                 Top Far Right
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,                            Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Top Far Right                                                                                                                                Bottom Far Right
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,    Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,
        // Bottom Far Right                                                                                                                             Bottom Far Left
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,                            Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,

        // Bottom Near Left                                                                                                                             Bottom Far Left
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,                                                                            Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,
        // Top Near Left                                                                                                                                Top Far Left
        Args.BottomNearLeft.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,                                                    Args.BottomNearLeft.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Top Near Right                                                                                                                               Top Far Right
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,                            Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z + Args.RelTopFarRight.Z,
        // Bottom Near Right                                                                                                                            Bottom Far Right
        Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y, Args.BottomNearLeft.Z,                                                    Args.BottomNearLeft.X + Args.RelTopFarRight.X, Args.BottomNearLeft.Y + Args.RelTopFarRight.Y, Args.BottomNearLeft.Z,
    };

    this->Program.Use();
    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (/* Lines */12 * /* Vertices */2 * /* Domains */3), Vertices, GL_DYNAMIC_DRAW);

    const LMatrix Projection = Maths::MakePerspectiveProjectionMatrix(
        Maths::ToRadians(Args.Eye->GetDegYFov()),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        Args.Eye->GetNearFrustum(), Args.Eye->GetFarFrustum()
    );

    this->Program.SetColorUniform("Color", Args.Color);
    this->Program.SetMatrixUniform("View", Args.Eye->GetViewMatrix());
    this->Program.SetMatrixUniform("Projection", Projection);

    glLineWidth(Args.Thickness);
    glDrawArrays(GL_LINES, 0, /* Lines */12 * /* Vertices */2);
    glLineWidth(1); /* Reset */

    glBindVertexArray(0);

    return;
}

void Jafg::LDebugTraceCubeShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    glDeleteBuffers(1, &this->Vbo);
    glDeleteVertexArrays(1, &this->Vao);
    this->Program.Free();

    return;
}

#endif
