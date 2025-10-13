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
class WDebugScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WDebugScreen(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WDebugScreen)

public:

    virtual void BeginLifeCDR() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnGarbageDefault(ECxxRecordTearDownReason::Type Reason) override;

    void SlowTick();

private:

    void RegisterCliObjects();
    void UnregisterCliObjects();

    CDR_NULL_PTR(WTextBox*) FpsSection{ nullptr };
    CDR_NULL_PTR(WTextBox*) TimeStatsSection{ nullptr };

    CDR_NULL_PTR(WTextBox*) LocalPawnLocationSection{ nullptr };
    CDR_NULL_PTR(WTextBox*) LocalPawnFacingSection{ nullptr };
    CDR_NULL_PTR(WTextBox*) LocalPawnChunkSection{ nullptr };
    CDR_NULL_PTR(WTextBox*) LocalPawnVoxelSection{ nullptr };

    CDR_NULL_PTR(WTextBox*) LocalPawnTargetVoxelSectionDestroy{ nullptr };
    CDR_NULL_PTR(WTextBox*) LocalPawnTargetVoxelSectionCreate{ nullptr };

    CDR_NULL_PTR(WTextBox*) MyWorldTimeSection{ nullptr };

    //# The time when we should invalidate the cache in seconds.
    f32 ResetTime { 0.1f };
    //# The current time in seconds when the cache will be invalidated.
    f32 InvalidateCacheTime { 0.0f };

    TCdrExpectDefault<LTexture2> TestTexture;

    CLASS_FIELD()
    TCdrIgnore<LCliVariableHandle> VariableHandle_ShowChunkBordersInDebugScreen;
};

} /* ~Namespace Jafg */
