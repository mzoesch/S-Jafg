// Copyright mzoesch. All rights reserved.

#pragma once

#include "Debug/TemporalWorldObject.h"
#include "Engine/World.h"

namespace Jafg
{

class LDebugTracePlane;

struct LDebugTraceLineVisualParams final
{
    LColor Color;
    /* Depending on the hardware, the max value may vary (but usually it's not more than 10). */
    uint8 Thickness = 1;
};

class LDebugTraceLine final : public LTemporalWorldObject
{
    friend LDebugTracePlane;

public:

    LDebugTraceLine() = delete;
    LDebugTraceLine(const float InTimeToLive) = delete;
    LDebugTraceLine(
        const float InTimeToLive,
        const LVector& InStart,
        const LVector& InEnd,
        const LDebugTraceLineVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), Start(InStart), End(InEnd), VisualParams(InVisualParams)
    {
        check( (this->Start - this->End).Magnitude() > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small distances." )
    }
    LDebugTraceLine(
        const OneDrawCall InTimeToLive,
        const LVector& InStart,
        const LVector& InEnd,
        const LDebugTraceLineVisualParams& InVisualParams
    )
        : LTemporalWorldObject(InTimeToLive), Start(InStart), End(InEnd), VisualParams(InVisualParams)
    {
        check( (this->Start - this->End).Magnitude() > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small distances." )
    }
    PROHIBIT_COPY(LDebugTraceLine)
    DEFAULT_MOVE(LDebugTraceLine)
    virtual ~LDebugTraceLine() override = default;

    virtual void Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const override;

    FORCEINLINE auto GetStart() const -> const LVector& { return this->Start; }
    FORCEINLINE auto GetEnd() const -> const LVector& { return this->End; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTraceLineVisualParams& { return this->VisualParams; }

private:

    static void DrawLine(
        const LViewport& InViewport, const LEye& InEye,
        const LVector& InStart, const LVector& InEnd,
        const LColor& InColor, const uint8 Thickness
    );
    static void DrawLineCachedValues(const LVector& InStart, const LVector& InEnd, const LColor& InColor, const uint8 Thickness);

    LVector Start;
    LVector End;
    LDebugTraceLineVisualParams VisualParams;
};

} /* ~Namespace Jafg */
