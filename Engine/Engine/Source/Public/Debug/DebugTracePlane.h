// Copyright mzoesch. All rights reserved.

#pragma once

#include "Debug/TemporalWorldObject.h"

namespace Jafg
{

struct LDebugTracePlaneVisualParams final
{
    LColor Color;
    /* Depending on the hardware, the max value may vary (but usually it's not more than 10). */
    uint8 Thickness = 1;
};

class LDebugTracePlane final : public LTemporalWorldObject
{
public:

    LDebugTracePlane() = delete;
    LDebugTracePlane(const float InTimeToLive) = delete;
    LDebugTracePlane(
        const float InTimeToLive,
        const LVector& InP1, const LVector& InP2, const LVector& InP3, const LVector& InP4,
        const LDebugTracePlaneVisualParams& InVisualParams
    ) : LTemporalWorldObject(InTimeToLive), P1(InP1), P2(InP2), P3(InP3), P4(InP4), VisualParams(InVisualParams)
    {
    }
    LDebugTracePlane(
        const OneDrawCall InTimeToLive,
        const LVector& InP1, const LVector& InP2, const LVector& InP3, const LVector& InP4,
        const LDebugTracePlaneVisualParams& InVisualParams
        ) : LTemporalWorldObject(InTimeToLive), P1(InP1), P2(InP2), P3(InP3), P4(InP4), VisualParams(InVisualParams)
    {
    }
    PROHIBIT_COPY(LDebugTracePlane)
    DEFAULT_MOVE(LDebugTracePlane)
    virtual ~LDebugTracePlane() override = default;

    virtual void Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const override;

    FORCEINLINE auto GetP1() const -> const LVector& { return this->P1; }
    FORCEINLINE auto GetP2() const -> const LVector& { return this->P2; }
    FORCEINLINE auto GetP3() const -> const LVector& { return this->P3; }
    FORCEINLINE auto GetP4() const -> const LVector& { return this->P4; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTracePlaneVisualParams& { return this->VisualParams; }

private:

    LVector P1; // Clockwise
    LVector P2;
    LVector P3;
    LVector P4;
    LDebugTracePlaneVisualParams VisualParams;
};

} /* ~Namespace Jafg */
