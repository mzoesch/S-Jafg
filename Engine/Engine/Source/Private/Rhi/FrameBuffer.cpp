// Copyright mzoesch. All rights reserved.

#include "Rhi/FrameBuffer.h"
#include "Rhi/RhiVendorInclude.h"
#include "Rhi/Shader.h"
#include "System/EnginePath.h"
#include "Widgets/Viewport.h"

namespace
{

uint32 Vao = 0;
uint32 Vbo = 0;
uint32 Ebo = 0;
Jafg::LShader BufferShader = { };

void ConditionallyLoadShader()
{
    using namespace Jafg;

    static bool bLoaded = false;
    if (bLoaded)
    {
        return;
    }

    LOG_VERBOSE(LogRhi, "Loading buffer shader.")
    BufferShader.Load(LEnginePath(EEnginePaths::Shaders, "FrameBufferPaint"));
    BufferShader.Use();

    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);

    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);

    glGenBuffers(1, &Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);

    // Locations
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    // Texs
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    constexpr float Vertices[] =
    {
        // Locations      // Texs
        -1.0f,  1.0f,     0.0f, 1.0f, /* Top    Left  */
         1.0f,  1.0f,     1.0f, 1.0f, /* Top    Right */
        -1.0f, -1.0f,     0.0f, 0.0f, /* Bottom Left  */
         1.0f, -1.0f,     1.0f, 0.0f, /* Bottom Right */
    };
    const uint32 Indices[] = { 0, 1, 2, /*  ||  */ 1, 3, 2 };

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glUseProgram(0);

    bLoaded = true;

    return;
}

} /* ~Namespace Anonymous */

Jafg::LFrameBuffer::~LFrameBuffer()
{
    if (this->IsMeaningful())
    {
        this->Orphan();
    }

    return;
}

void Jafg::LFrameBuffer::Build(const LIntVector2& InSize)
{
    if (this->IsMeaningful())
    {
        LOG_WARNING(LogRhi, "Old framebuffer [{}] is implicitly orphaned.", this->Handle)
        this->Orphan();
    }

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &this->Color);
    glBindTexture(GL_TEXTURE_2D, this->Color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(InSize.X), static_cast<GLsizei>(InSize.Y), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &this->Handle);
    glBindFramebuffer(GL_FRAMEBUFFER, this->Handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Color, 0);

    glGenRenderbuffers(1, &this->Depth);
    glBindRenderbuffer(GL_RENDERBUFFER, this->Depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(InSize.X), static_cast<GLsizei>(InSize.Y));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->Depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_FATAL(LogRhi, "Framebuffer [{}] is not complete.", this->Handle)
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->bIsMeaningful = true;
    LOG_VERBOSE(LogRhi, "Built framebuffer [{}].", this->Handle)

    ConditionallyLoadShader();

    return;
}

void Jafg::LFrameBuffer::MakeDrawTarget()
{
    checkSlow( this->IsMeaningful() )
    glBindFramebuffer(GL_FRAMEBUFFER, this->Handle);

    return;
}

void Jafg::LFrameBuffer::ResetAndMakeDrawTarget()
{
    this->MakeDrawTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::MakeDefaultDrawTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Jafg::LFrameBuffer::ResetAndMakeDefaultDrawTarget()
{
    LFrameBuffer::MakeDefaultDrawTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::ReadToActive() const
{
    checkSlow( this->IsMeaningful() )
    glBindTexture(GL_TEXTURE_2D, this->Color);

    return;
}

void Jafg::LFrameBuffer::ReadTo(const uint32 InHandle) const
{
    checkSlow( InHandle >= GL_TEXTURE0 && InHandle <= GL_TEXTURE31 )
    glActiveTexture(InHandle);
    this->ReadToActive();

    return;
}

void Jafg::LFrameBuffer::PaintToViewport(const LViewport& InContext) const
{
    // Just don't fuck with specific custom viewport preferences.
    const bool CurrentDepthTest = glIsEnabled(GL_DEPTH_TEST);
    const bool CurrentBlend = glIsEnabled(GL_BLEND);

    BufferShader.Use();
    BufferShader.SetFloatUniform("OrthoZDepth", InContext.GetFrameOrthoZLayerDepth());
    glBindVertexArray(Vao);
    this->ReadTo(GL_TEXTURE0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    if (UNLIKELY(CurrentDepthTest)) { glEnable(GL_DEPTH_TEST); }
    if (LIKELY(CurrentBlend)) { glEnable(GL_BLEND); }

#if WITH_DEBUG_ZERO_UNBOUND
    glUseProgram(0);
    glBindVertexArray(0);
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    return;
}

void Jafg::LFrameBuffer::Orphan()
{
    if (this->bIsMeaningful == false)
    {
        LOG_WARNING(LogRhi, "Triggered orphaning of the non-meaningful frame buffer [{}].", this->Handle)
        return;
    }

    LOG_VERBOSE(LogRhi, "Orphaning frame buffer [{}].", this->Handle)

    // glDeleteFramebuffers(1, &this->Handle);
    // glDeleteTextures(1, &this->Color);
    // glDeleteRenderbuffers(1, &this->Depth);

#if WITH_DEBUG_ZERO_UNBOUND
    this->Handle = 0;
    this->Color = 0;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    this->bIsMeaningful = false;

    return;
}
