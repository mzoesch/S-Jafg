// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "DebugScreen.generated.h"

namespace Jafg
{

class WTextBlock;

DECLARE_JAFG_CLASS()
class ENGINE_API WDebugScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WDebugScreen)

public:

    // WUserWidget implementation
    virtual void Construct() override;
    virtual void Tick() override;
    // ~WUserWidget implementation

    void SlowTick();

private:

    WTextBlock* FpsSection = nullptr;
    WTextBlock* TimeStatsSection = nullptr;

    WTextBlock* LocalPawnLocationSection = nullptr;
    WTextBlock* LocalPawnFacingSection = nullptr;

    /** The time when we should invalidate the cache in seconds. */
    double ResetTime = 0.1;
    /** The current time in seconds when the cache will be invalidated. */
    double InvalidateCacheTime = 0.0;
};

} /* ~Namespace Jafg */
