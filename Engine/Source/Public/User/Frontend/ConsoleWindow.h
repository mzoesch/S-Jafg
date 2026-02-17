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
class ENGINE_API WConsoleWindow final : public WFloatingWindow
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WIDGET_CONSTRUCTORS_BODY(WConsoleWindow)
    {
        this->SetShouldTick(true);
    }

public:

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;

    ENGINE_API void AddToConsole(LConsoleMessage Message);

private:

    u64 LoadLogs(u64 Start);

    bool StdInCommit(LString const& Text, ETextCommit CommitType);

    static TOptional<LVec2F> LastWindowLocation;
    static TOptional<LVec2F> LastWindowSize;

    u64 LoadedLogCount{};

    bool bHighlightOnConstruct{ true };

    WConsoleStdIn* StdIn{};
    WScrollRegion* HistoryScrollRegion{};
    WVRegion* HistoryRegion{};
};

} /* ~Namespace Jafg */
