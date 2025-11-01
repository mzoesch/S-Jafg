// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/FloatingWindow.h"
#include "User/ConsoleMessage.h"
#include "Widgets/EditableTextBoxForward.h"
#include "ConsoleWindow.generated.h"

namespace Jafg
{

class WScrollRegion;
class WVRegion;
class WConsoleStdIn;

DECLARE_JAFG_WIDGET()
class WConsoleWindow final : public WFloatingWindow
{
    GENERATED_CLASS_BODY()

protected:

    explicit WConsoleWindow(LCxxObjectInitializer const& ObjectInitializer);

    DEFAULT_OBJECT_CDR_CTOR(WConsoleWindow)

public:

    virtual void Construct() override;
    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter) override;

    virtual void Tick() override;

    ENGINE_API void AddToConsole(LConsoleMessage Message);

private:

    u64 LoadLogs(u64 Start);

    bool StdInCommit(LString const& Text, ETextCommit::Type CommitType);

    static TOptional<LVector2> LastWindowLocation;
    static TOptional<LVector2> LastWindowSize;

    u64 LoadedLogCount{ 0 };

    bool bHighlightOnConstruct{ true };

    WConsoleStdIn* StdIn{ nullptr };
    WScrollRegion* HistoryScrollRegion{ nullptr };
    WVRegion* HistoryRegion{ nullptr };
};

} /* ~Namespace Jafg */
