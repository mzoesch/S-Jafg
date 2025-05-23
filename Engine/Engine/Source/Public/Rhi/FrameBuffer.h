// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

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
    ~LFrameBuffer();

    FORCEINLINE bool IsMeaningful() const { return this->bIsMeaningful; }

    void Build(const LIntVector2& InSize);

    void MakeDrawTarget();
    void ResetAndMakeDrawTarget();
    void ResetAndMakeDrawTarget(const LLinearColor& InColor);

    static void MakeDefaultDrawTarget();
    static void ResetAndMakeDefaultDrawTarget();
    static void ResetAndMakeDefaultDrawTarget(const LLinearColor& InColor);

    ENGINE_API void ReadToActive() const;
    ENGINE_API void ReadTo(const u32 InHandle) const;

    // Based of current active rhi context!
    ENGINE_API void PaintToViewport(const LViewport& InContext) const;

private:

    void Orphan();

    bool bIsMeaningful = false;
    u32 Handle = 0;
    u32 Depth = 0;
    u32 Color = 0;
};

} /* ~Namespace Jafg */
