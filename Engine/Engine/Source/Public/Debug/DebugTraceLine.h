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

    void Draw(const LWorld& InContext) const override;

    FORCEINLINE auto GetStart() const -> const LVector& { return this->Start; }
    FORCEINLINE auto GetEnd() const -> const LVector& { return this->End; }
    FORCEINLINE auto GetVisualParams() const -> const LDebugTraceLineVisualParams& { return this->VisualParams; }

private:

    static void DrawLine(const LWorld& InContext, const LVector& InStart, const LVector& InEnd, const LColor& InColor, const bool bUseCache);

    LVector Start;
    LVector End;
    LDebugTraceLineVisualParams VisualParams;
};

} /* ~Namespace Jafg */
