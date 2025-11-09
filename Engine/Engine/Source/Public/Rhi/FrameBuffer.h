// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class LViewport;

//#
//# Buffer for a part of a frame.
//#
class LFrameBuffer final
{
public:

    LFrameBuffer() = default;
    ~LFrameBuffer() {}

    FORCEINLINE LFrameBuffer(const LFrameBuffer&) noexcept = delete;
    FORCEINLINE LFrameBuffer(LFrameBuffer&& Other) noexcept;
    FORCEINLINE LFrameBuffer& operator=(const LFrameBuffer&) noexcept = delete;
    FORCEINLINE LFrameBuffer& operator=(LFrameBuffer&& Other) noexcept;

    FORCEINLINE bool IsValid() const { return this->bValid; }

    void Build(const LIntVector2& InSize) {}

    void MakeDrawTarget() {}
    void MakeDrawTargetAndReset() {}
    void MakeDrawTargetAndReset(const Lal::LLinearColor& InColor) {}

    static void MakeDefaultDrawTarget() {}
    static void MakeDefaultDrawTargetAndReset() {}
    static void MakeDefaultDrawTargetAndReset(const Lal::LLinearColor& InColor) {}

    /*ENGINE_API*/ void ReadToActive() const {}
    /*ENGINE_API*/ void ReadTo(const u32 InHandle) const {}

    // Based of current active rhi context!
    /*ENGINE_API*/ void PaintToViewport(const LViewport& InContext) const {}

private:

    void Orphan() {}

    bool bValid { false };
    u32 Handle { 0 };
    u32 Depth { 0 };
    u32 Color { 0 };
};

FORCEINLINE LFrameBuffer::LFrameBuffer(LFrameBuffer&& Other) noexcept
    : bValid(Other.bValid), Handle(Other.Handle), Depth(Other.Depth), Color(Other.Color)
{
    Other.bValid = false;
    return;
}

FORCEINLINE LFrameBuffer& LFrameBuffer::operator=(LFrameBuffer&& Other) noexcept
{
    this->bValid = Other.bValid;
    this->Handle = Other.Handle;
    this->Depth = Other.Depth;
    this->Color = Other.Color;

    Other.bValid = false;
    return *this;
}

} /* ~Namespace Jafg */
