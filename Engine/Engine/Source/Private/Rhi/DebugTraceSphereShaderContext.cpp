// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/DebugTraceSphereShaderContext.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/Viewport.h"

namespace
{

/**
 * Parametric equation of a sphere centered at the origin:
 *     x = r * sin(phi) * cos(theta)        Where phi is the polar angle (0 <= phi <= PI) and
 *     y = r * sin(phi) * sin(theta)              theta is the azimuthal angle (0 <= theta < 2PI).
 *     z = r * cos(phi)
 */
void MakeSphereVertices(const float InRadius, const int32 InSlices, const int32 InStacks, Jafg::TdhArray<Jafg::LVector>& OutLocations)
{
    using namespace Jafg;

    for (int32 i = 0; i <= InStacks; ++i)
    {
        const float Phi = static_cast<float>(i) * JAFG_PI / static_cast<float>(InStacks);
        for (int32 j = 0; j <= InSlices; ++j)
        {
            const float Theta = static_cast<float>(j) * 2.0f * JAFG_PI / static_cast<float>(InSlices);

            // Longitude
            OutLocations.Emplace(
                InRadius * Maths::Sin(Phi) * Maths::Cos(Theta),
                InRadius * Maths::Sin(Phi) * Maths::Sin(Theta),
                InRadius * Maths::Cos(Phi)
            );

            // Latitude
            if (i > 0)
            {
                OutLocations.Emplace(
                    InRadius * Maths::Sin(Phi - JAFG_PI / static_cast<float>(InStacks)) * Maths::Cos(Theta),
                    InRadius * Maths::Sin(Phi - JAFG_PI / static_cast<float>(InStacks)) * Maths::Sin(Theta),
                    InRadius * Maths::Cos(Phi - JAFG_PI / static_cast<float>(InStacks))
                );
            }

            continue;
        }

        continue;
    }

    return;
}

} /* ~Anonymous Namespace */

void Jafg::LDebugTraceSphereShaderContext::Make()
{
    LGenericShaderContext::Make();

    this->Program = LShader(LEnginePath(EEnginePaths::Shaders, "DebugSphere"));
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

void Jafg::LDebugTraceSphereShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LDebugTraceSphereShaderContextDrawArgs)

    TdhArray<LVector> SphereVertices;
    MakeSphereVertices(Args.Radius, Args.Slices, Args.Stacks, SphereVertices);

    this->Program.Use();
    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(sizeof(LVector) * SphereVertices.GetSize()),
        SphereVertices.GetData(),
        GL_DYNAMIC_DRAW
    );

    const TMatrix Projection = Maths::MakePerspectiveProjectionMatrix(
        Maths::ToRadians(Args.DegYFov),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        0.1f, 2000.0f
    );
    LMatrix Model; Model.InlineTranslate(Args.Center);

    this->Program.SetMatrixUniform("Model", Model);
    this->Program.SetColorUniform("Color", Args.Color);
    this->Program.SetMatrixUniform("View", Args.ViewMatrix);
    this->Program.SetMatrixUniform("Projection", Projection);

    glLineWidth(Args.Thickness);
    glDrawArrays(GL_LINES, 0, SphereVertices.GetSize());
    glLineWidth(1); /* Reset */

#if WITH_DEBUG_ZERO_UNBOUND
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LDebugTraceSphereShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    glDeleteBuffers(1, &this->Vbo);
    glDeleteVertexArrays(1, &this->Vao);
    this->Program.Free();

    return;
}
