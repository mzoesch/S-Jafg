// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Rhi/Texture2.h"
#include "Cli/CliHandles.h"
#include "DebugScreen.generated.h"

namespace Jafg
{

class WTextBox;

DECLARE_JAFG_WIDGET()
class ENGINE_API WDebugScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WDebugScreen(const LObjectInitializer& ObjectInitializer);

public:

    virtual void BeginLifeDefault() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnGarbageDefault() override;

    void SlowTick();

private:

    void RegisterCliObjects();
    void UnregisterCliObjects();

    WTextBox* FpsSection { nullptr };
    WTextBox* TimeStatsSection { nullptr };

    WTextBox* LocalPawnLocationSection { nullptr };
    WTextBox* LocalPawnFacingSection { nullptr };
    WTextBox* LocalPawnChunkSection { nullptr };
    WTextBox* LocalPawnVoxelSection { nullptr };

    WTextBox* LocalPawnTargetVoxelSectionDestroy { nullptr };
    WTextBox* LocalPawnTargetVoxelSectionCreate { nullptr };

    WTextBox* MyWorldTimeSection { nullptr };

    //# The time when we should invalidate the cache in seconds.
    f32 ResetTime { 0.1f };
    //# The current time in seconds when the cache will be invalidated.
    f32 InvalidateCacheTime { 0.0f };

    LTexture2 TestTexture;

    CLASS_FIELD(DefaultOnly)
    LCliVariableHandle VariableHandle_ShowChunkBordersInDebugScreen;
};

} /* ~Namespace Jafg */
