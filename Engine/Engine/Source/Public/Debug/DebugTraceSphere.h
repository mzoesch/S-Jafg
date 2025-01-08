// Copyright mzoesch. All rights reserved.

#pragma once

#include "Debug/TemporalWorldObject.h"

namespace Jafg
{

struct LDebugTraceSphereVisualParams final
{
    int32  Segments;
    int32  Rings;
    LColor Color;
};

class LDebugTraceSphere final : public LTemporalWorldObject
{
public:

    LDebugTraceSphere() = delete;
    LDebugTraceSphere(const float InTimeToLive) = delete;
    LDebugTraceSphere(
        const float InTimeToLive,
        const LVector& InCenter,
        const float InRadius,
        const LDebugTraceSphereVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), Center(InCenter), Radius(InRadius), VisualParams(InVisualParams)
    {
        check( this->Radius > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small spheres." )
    }
    LDebugTraceSphere(
        const OneDrawCall InTimeToLive,
        const LVector& InCenter,
        const float InRadius,
        const LDebugTraceSphereVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), Center(InCenter), Radius(InRadius), VisualParams(InVisualParams)
    {
        check( this->Radius > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small spheres." )
    }
    PROHIBIT_COPY(LDebugTraceSphere)
    DEFAULT_MOVE(LDebugTraceSphere)
    virtual ~LDebugTraceSphere() override = default;

    void Draw(const LWorld& InContext) const override;

    FORCEINLINE auto GetCenter() const -> const LVector& { return this->Center; }
    FORCEINLINE auto GetRadius() const -> float { return this->Radius; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTraceSphereVisualParams& { return this->VisualParams; }

private:

    LVector Center;
    float Radius;
    LDebugTraceSphereVisualParams VisualParams;
};

} /* ~Namespace Jafg */
