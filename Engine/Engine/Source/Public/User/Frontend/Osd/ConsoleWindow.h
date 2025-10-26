// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/FloatingWindow.h"
#include "User/ConsoleMessage.h"
#include "ConsoleWindow.generated.h"

namespace Jafg
{

class WScrollRegion;
class WVRegion;

DECLARE_JAFG_WIDGET()
class WConsoleWindow final : public WFloatingWindow
{
    GENERATED_CLASS_BODY()

protected:

    explicit WConsoleWindow(LCxxObjectInitializer const& ObjectInitializer);

    DEFAULT_OBJECT_CDR_CTOR(WConsoleWindow)

public:

    virtual void Construct() override;
    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason) override;

    virtual void Tick() override;

    ENGINE_API void AddToConsole(LConsoleMessage Message);

private:

    void LoadLogs(u64 Start);

    static TOptional<LVector2> LastWindowLocation;
    static TOptional<LVector2> LastWindowSize;

    u64 LoadedLogCount{ 0 };

    WScrollRegion* HistoryScrollRegion{ nullptr };
    WVRegion* HistoryRegion{ nullptr };
};

} /* ~Namespace Jafg */
