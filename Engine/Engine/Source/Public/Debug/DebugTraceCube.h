// Copyright mzoesch. All rights reserved.

#pragma once

#include "Debug/TemporalWorldObject.h"

namespace Jafg
{

struct LDebugTraceCubeVisualParams final
{
    LColor Color;
    /* Depending on the hardware, the max value may vary (but usually it's not more than 10). */
    u8 Thickness = 1;
};

class LDebugTraceCube final : public LTemporalWorldObject
{
public:

    LDebugTraceCube() = delete;
    LDebugTraceCube(const float InTimeToLive) = delete;
    LDebugTraceCube(
        const float InTimeToLive,
        const LVector& InBottomNearLeft,
        const LVector& InRelTopFarRight,
        const LDebugTraceCubeVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), BottomNearLeft(InBottomNearLeft), TopRelFarRight(InRelTopFarRight), VisualParams(InVisualParams)
    {
    }
    LDebugTraceCube(
        const OneDrawCall InTimeToLive,
        const LVector& InBottomNearLeft,
        const LVector& InTopFarRight,
        const LDebugTraceCubeVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), BottomNearLeft(InBottomNearLeft), TopRelFarRight(InTopFarRight), VisualParams(InVisualParams)
    {
    }
    PROHIBIT_COPY(LDebugTraceCube)
    DEFAULT_MOVE(LDebugTraceCube)
    virtual ~LDebugTraceCube() override = default;

    virtual void Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const override;

    FORCEINLINE auto GetBottomNearLeft() const -> const LVector& { return this->BottomNearLeft; }
    FORCEINLINE auto GetRelTopFarRight() const -> const LVector& { return this->TopRelFarRight; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTraceCubeVisualParams& { return this->VisualParams; }

private:

    LVector BottomNearLeft;
    LVector TopRelFarRight;
    LDebugTraceCubeVisualParams VisualParams;
};

} /* ~Namespace Jafg */
