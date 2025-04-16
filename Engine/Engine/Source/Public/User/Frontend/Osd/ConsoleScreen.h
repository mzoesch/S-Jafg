// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/EditableTextBlockForward.h"
#include "ConsoleScreen.generated.h"

namespace Jafg
{

class WEditableTextBlock;

namespace EConsoleScreenState
{

enum Type : u8
{
    Show,
    Hide,
};

} /* ~Namespace EConsoleScreenState */

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class ENGINE_API WConsoleScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WConsoleScreen)

public:

    virtual void Construct() override;

    void SetConsoleFrontendState(const EConsoleScreenState::Type InState);
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    void AddToHistory(const LString& InText);
    FORCEINLINE i32  GetMaxHistorySize() const { return GetDefault<WConsoleScreen>()->MaxHistorySize; }
    FORCEINLINE auto GetHistory() const -> const TArray<LString>& { return GetDefault<WConsoleScreen>()->History; }

private:

    void HideConsoleScreenWithSideEffects();
    void ShowConsoleScreenWithSideEffects();

    void OnTextCommit(const LString& InText, const ETextCommit::Type InCommitType);

    WEditableTextBlock* EditableTextBlock = nullptr;

    CLASS_FIELD(Config, DefaultOnly)
    i32 MaxHistorySize { 50 };

    CLASS_FIELD(Config, DefaultOnly)
    TArray<LString> History;
};

} /* ~Namespace Jafg */
