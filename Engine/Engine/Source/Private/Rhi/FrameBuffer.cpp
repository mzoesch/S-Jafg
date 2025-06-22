// Copyright mzoesch. All rights reserved.

#include "Rhi/FrameBuffer.h"
#include "Rhi/RhiVendorInclude.h"
#include "Rhi/Shader.h"
#include "System/EnginePath.h"
#include "Widgets/Viewport.h"

namespace
{

u32 Vao { 0 };
u32 Vbo { 0 };
u32 Ebo { 0 };
Jafg::LShader BufferShader;

void LoadConditionally()
{
    using namespace Jafg;

    static bool bLoaded { false };
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), reinterpret_cast<void*>(2 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    constexpr f32 Vertices[]
    {
        -1.0f,  1.0f,     0.0f, 1.0f, /* Top    Left  */
         1.0f,  1.0f,     1.0f, 1.0f, /* Top    Right */
        -1.0f, -1.0f,     0.0f, 0.0f, /* Bottom Left  */
         1.0f, -1.0f,     1.0f, 0.0f, /* Bottom Right */
    };
    constexpr u32 Indices[] { 0, 1, 2, /*  ||  */ 1, 3, 2 };

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    bLoaded = true;

    return;
}

} /* ~Namespace Anonymous */

Jafg::LFrameBuffer::~LFrameBuffer()
{
    if (this->IsValid())
    {
        this->Orphan();
    }

    return;
}

void Jafg::LFrameBuffer::Build(const LIntVector2& InSize)
{
    if (this->IsValid())
    {
        LOG_WARNING(LogRhi, "Old framebuffer [{}] is implicitly orphaned.", this->Handle)
        this->Orphan();
    }

    GLint CurFb { 0 };
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurFb);

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

    glBindFramebuffer(GL_FRAMEBUFFER, CurFb);

    this->bValid = true;
    LOG_VERBOSE(LogRhi, "Built framebuffer [{}].", this->Handle)

    LoadConditionally();

    return;
}

void Jafg::LFrameBuffer::MakeDrawTarget()
{
    checkSlow( this->IsValid() )
    glBindFramebuffer(GL_FRAMEBUFFER, this->Handle);

    return;
}

void Jafg::LFrameBuffer::MakeDrawTargetAndReset()
{
    this->MakeDrawTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::MakeDrawTargetAndReset(const LLinearColor& InColor)
{
    this->MakeDrawTarget();
    glClearColor(InColor.R, InColor.G, InColor.B, InColor.A);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::MakeDefaultDrawTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Jafg::LFrameBuffer::MakeDefaultDrawTargetAndReset()
{
    LFrameBuffer::MakeDefaultDrawTarget();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::MakeDefaultDrawTargetAndReset(const LLinearColor& InColor)
{
    LFrameBuffer::MakeDefaultDrawTarget();
    glClearColor(InColor.R, InColor.G, InColor.B, InColor.A);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void Jafg::LFrameBuffer::ReadToActive() const
{
    checkSlow( this->IsValid() )
    glBindTexture(GL_TEXTURE_2D, this->Color);

    return;
}

void Jafg::LFrameBuffer::ReadTo(const u32 InHandle) const
{
    checkSlow( InHandle >= GL_TEXTURE0 && InHandle <= GL_TEXTURE31 )
    glActiveTexture(InHandle);
    this->ReadToActive();

    return;
}

void Jafg::LFrameBuffer::PaintToViewport(const LViewport& InContext) const
{
    /* Just don't fuck with specific custom viewport preferences. */
    const bool CurrentDepthTest { static_cast<bool>(glIsEnabled(GL_DEPTH_TEST)) };
    const bool CurrentBlend { static_cast<bool>(glIsEnabled(GL_BLEND)) };

    BufferShader.Use();
    BufferShader.SetFloatUniform("OrthoZDepth", InContext.GetFrameOrthoZLayerDepth());

    glBindVertexArray(Vao);

    this->ReadToActive();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    if (CurrentDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    if (CurrentBlend)
    {
        glEnable(GL_BLEND);
    }

    glBindVertexArray(0);

    return;
}

void Jafg::LFrameBuffer::Orphan()
{
    if (this->bValid == false)
    {
        LOG_WARNING(LogRhi, "Triggered orphaning of the non-meaningful frame buffer [{}].", this->Handle)
        return;
    }

    LOG_VERBOSE(LogRhi, "Orphaning frame buffer [{}].", this->Handle)

    glDeleteFramebuffers(1, &this->Handle);
    glDeleteTextures(1, &this->Color);
    glDeleteRenderbuffers(1, &this->Depth);

    this->bValid = false;

    return;
}
