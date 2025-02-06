// Copyright mzoesch. All rights reserved.

#include "Rhi/FrameBuffer.h"
#include "Rhi/RhiVendorInclude.h"

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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_FATAL(LogRhi, "Framebuffer [{}] is not complete.", this->Handle)
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->bIsMeaningful = true;
    LOG_VERBOSE(LogRhi, "Built framebuffer [{}].", this->Handle)

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
    checkSlow( InHandle > GL_TEXTURE0 && InHandle < GL_TEXTURE31 )
    glActiveTexture(InHandle);
    this->ReadToActive();

    return;
}

void Jafg::LFrameBuffer::PaintToViewport(const LViewport& InViewport, const bool bToDefaultBuffer) const
{
}

void Jafg::LFrameBuffer::Orphan()
{
    if (this->bIsMeaningful == false)
    {
        LOG_WARNING(LogRhi, "Triggered orphaning of the non-meaningful frame buffer [{}].", this->Handle)
        return;
    }

    LOG_VERBOSE(LogRhi, "Orphaning frame buffer [{}].", this->Handle)

    glDeleteFramebuffers(1, &this->Handle);
    glDeleteTextures(1, &this->Color);

#if WITH_DEBUG_ZERO_UNBOUND
    this->Handle = 0;
    this->Color = 0;
#endif /* WITH_DEBUG_ZERO_UNBOUND */

    this->bIsMeaningful = false;

    return;
}
