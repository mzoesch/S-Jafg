// Copyright mzoesch. All rights reserved.

#pragma once

#include "Debug/TemporalWorldObject.h"

namespace Jafg
{

struct LDebugTraceSphereVisualParams final
{
    i32    Segments;
    i32    Rings;
    LColor Color;
    /* Depending on the hardware, the max value may vary (but usually it's not more than 10). */
    u8     Thickness = 1;
};

class LDebugTraceSphere final : public LTemporalWorldObject
{
public:

    LDebugTraceSphere() = delete;
    LDebugTraceSphere(const float InTimeToLive) = delete;
    LDebugTraceSphere(
        const float InTimeToLive,
        const LVector& InCenter,
        const float InRadius = 0.5f,
        const LDebugTraceSphereVisualParams& InVisualParams = { 10, 10, LColor::Red }
    )
        : LTemporalWorldObject(InTimeToLive), Center(InCenter), Radius(InRadius), VisualParams(InVisualParams)
    {
        check( this->Radius > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small spheres." )
    }
    LDebugTraceSphere(
        const OneDrawCall InTimeToLive,
        const LVector& InCenter,
        const float InRadius = 0.5f,
        const LDebugTraceSphereVisualParams& InVisualParams = { 10, 10, LColor::Red }
    )
        : LTemporalWorldObject(InTimeToLive), Center(InCenter), Radius(InRadius), VisualParams(InVisualParams)
    {
        check( this->Radius > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small spheres." )
    }
    PROHIBIT_COPY(LDebugTraceSphere)
    DEFAULT_MOVE(LDebugTraceSphere)
    virtual ~LDebugTraceSphere() override = default;

    virtual void Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const override;

    FORCEINLINE auto GetCenter() const -> const LVector& { return this->Center; }
    FORCEINLINE auto GetRadius() const -> float { return this->Radius; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTraceSphereVisualParams& { return this->VisualParams; }

private:

    LVector Center;
    float Radius;
    LDebugTraceSphereVisualParams VisualParams;
};

} /* ~Namespace Jafg */
