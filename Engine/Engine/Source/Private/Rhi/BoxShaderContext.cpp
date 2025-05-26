// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/BoxShaderContext.h"
#include "Widgets/Viewport.h"
#include "Rhi/RhiVendorInclude.h"
#include "System/EnginePath.h"
#include "Rhi/Texture2.h"

void Jafg::LBoxShaderContext::Make()
{
    if (this->bIsMeaningful)
    {
        panic( "Tried to override a meaningful shader context." )
        return;
    }

    this->ReloadShader(false);
    this->bIsMeaningful = true;

#if WITH_DEBUG_ZERO_UNBOUND
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LBoxShaderContext::Free()
{
    if (this->bIsMeaningful == false)
    {
        return;
    }

    this->FreeStayMeaningful();
    this->bIsMeaningful = false;

    return;
}

void Jafg::LBoxShaderContext::Draw(
    const LViewport& Context,
    const LVector2&  Size,
    const LVector2&  TopLeft,
    const LColor&    Color,
    const LTexture2* Texture /* = nullptr */
) const
{
    checkSlow( this->bIsMeaningful )

    checkCode
    (
        if (Size.X <= 0.0f || Size.Y <= 0.0f)
        {
            LOG_WARNING(LogTemporal, "Invalid: [{}x{}].", Size.X, Size.Y)
            return;
        }
    )

    // check( Size.X > 0.0f )
    // check( Size.Y > 0.0f )

    if (static_cast<float>(Context.GetDimensions().X) < TopLeft.X + Size.X)
    {
        LOG_ERROR(LogWidgetFramework, "Box X-Axis constraint: {} < {} + {}.", Context.GetDimensions().X, TopLeft.X, Size.X)
    }
    if (static_cast<float>(Context.GetDimensions().Y) < TopLeft.Y + Size.Y)
    {
        LOG_ERROR(LogWidgetFramework, "Box Y-Axis constraint: {} < {} + {}.", Context.GetDimensions().Y, TopLeft.Y, Size.Y)
    }

    if (this->LastFrameTexture != Texture)
    {
        this->FreeStayMeaningful();
        this->LastFrameTexture = Texture;
        this->ReloadShader(true);
    }
    else
    {
        this->Shader.Use();
        if (this->LastFrameTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->Tex);
        }
        glBindVertexArray(this->Vao);
    }

    const float Scale = Context.GetScaleFactor();
    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const LMatrix Projection = Maths::MakeOrthographicProjectionMatrix(
        LVector2(static_cast<float>(WindowDimensions.X), static_cast<float>(WindowDimensions.Y))
    );

    this->Shader.SetMatrixUniform("Projection", Projection);
    this->Shader.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Shader.SetIntUniform("BoxTint", *reinterpret_cast<const i32*>(&Color.Bits));

    if (this->LastFrameTexture)
    {
        float Vertices[] = {
            // Locations                                                // Texs
            TopLeft.X * Scale,            TopLeft.Y * Scale,            0.0f, 0.0f, /* Top    Left  */
            (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            1.0f, 0.0f, /* Top    Right */
            TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, 0.0f, 1.0f, /* Bottom Left  */
            (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, 1.0f, 1.0f, /* Bottom Right */
        };
        for (float& Vertex : Vertices) { Vertex = Maths::Floor(Vertex); }
        const u32 Indices[] = { 0, 1, 2, /*  ||  */ 1, 3, 2 };

        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_DYNAMIC_DRAW);
        glBindTexture(GL_TEXTURE_2D, this->Tex);
        glBindVertexArray(this->Vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        float Vertices[] =
        {
            TopLeft.X * Scale,            TopLeft.Y * Scale,            /* Top    Left  */
            (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
            TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, /* Bottom Left  */

            (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
            (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, /* Bottom Right */
            TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale  /* Bottom Left  */
        };
        for (float& Vertex : Vertices) { Vertex = Maths::Floor(Vertex); }

        glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

#if WITH_DEBUG_ZERO_UNBOUND
    glUseProgram(0);
    glBindVertexArray(0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LBoxShaderContext::ReloadShader(const bool bDeleteOld) const
{
    check( this->bIsMeaningful && bDeleteOld || bDeleteOld == false )

    if (bDeleteOld)
    {
        glDeleteVertexArrays(1, &this->Vao);
        glDeleteBuffers(1, &this->Vbo);
        if (this->LastFrameTexture) { glDeleteBuffers(1, &this->Ebo); }
    #if WITH_DEBUG_ZERO_UNBOUND
        this->Vao = 0;
        this->Vbo = 0;
        this->Ebo = 0;
    #endif /* WITH_DEBUG_ZERO_UNBOUND */

        this->Shader.Free();
    }

    if (this->LastFrameTexture)
    {
        this->Shader = LShader(LEnginePath(EEnginePaths::Shaders, "VisualTexBox"));
    }
    else
    {
        this->Shader = LShader(LEnginePath(EEnginePaths::Shaders, "VisualBox"));
    }

    this->Shader.Use();

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);
    if (this->LastFrameTexture) { glGenBuffers(1, &this->Ebo); }

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    if (this->LastFrameTexture) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo); }

    if (this->LastFrameTexture)
    {
        // Locations
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(0);
        // Texs
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenTextures(1, &this->Tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->Tex);
#if !PLATFORM_WASM
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
#endif /* !PLATFORM_WASM */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA /* out */,
            static_cast<GLsizei>(this->LastFrameTexture->GetFirstMipMap().Size.X),
            static_cast<GLsizei>(this->LastFrameTexture->GetFirstMipMap().Size.Y),
            // TODO: Maybe split atlas textures into separate textures? (Opaque, Translucent, etc.)
            0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, this->LastFrameTexture->GetFirstMipMap().Bulk.GetBulk());
    }
    else
    {
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(0);
    }

    return;
}

void Jafg::LBoxShaderContext::FreeStayMeaningful() const
{
    checkSlow( this->bIsMeaningful )

    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);
    if (this->LastFrameTexture) { glDeleteBuffers(1, &this->Ebo); }
#if WITH_DEBUG_ZERO_UNBOUND
    this->Vao = 0;
    this->Vbo = 0;
    this->Ebo = 0;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    if (this->LastFrameTexture)
    {
        glDeleteTextures(1, &this->Tex);
#if WITH_DEBUG_ZERO_UNBOUND
        this->Tex = 0;
#endif /* WITH_DEBUG_ZERO_UNBOUND */
    }

    this->Shader.Free();

    return;
}
