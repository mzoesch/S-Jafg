// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Rhi/Texture2.h"
#include "Cli/CliHandles.h"
#include "DebugScreen.generated.h"

namespace Jafg
{

class WTextBox;

} /* ~Namespace Jafg */

namespace Jgc
{

DECLARE_JAFG_WIDGET()
class WDebugScreen final : public Jafg::WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WDebugScreen(Jafg::LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WDebugScreen)

public:

    virtual void BeginLifeCDR() override;
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnGarbageDefault(Jafg::ECxxRecordTearDownReason::Type Reason, Jafg::LClassOuter& PreviousOuter) override;

    void SlowTick();

private:

    void RegisterCliObjects();
    void UnregisterCliObjects();

    CDR_NULL_PTR(Jafg::WTextBox*) FpsSection{ nullptr };
    CDR_NULL_PTR(Jafg::WTextBox*) TimeStatsSection{ nullptr };

    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnLocationSection{ nullptr };
    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnFacingSection{ nullptr };
    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnChunkSection{ nullptr };
    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnVoxelSection{ nullptr };

    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnTargetVoxelSectionDestroy{ nullptr };
    CDR_NULL_PTR(Jafg::WTextBox*) LocalPawnTargetVoxelSectionCreate{ nullptr };

    CDR_NULL_PTR(Jafg::WTextBox*) MyWorldTimeSection{ nullptr };

    //# The time when we should invalidate the cache in seconds.
    f32 ResetTime{ 0.1f };
    //# The current time in seconds when the cache will be invalidated.
    f32 InvalidateCacheTime{ 0.0f };

    TCdrIgnore<Jafg::LCliVariableHandle> VariableHandle_ShowChunkBordersInDebugScreen;
};

} /* ~Namespace Jgc */
