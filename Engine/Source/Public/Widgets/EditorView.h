// Copyright mzoesch. All rights reserved.

#pragma once

#include "UserWidget.h"
#include "EditorView.generated.h"

namespace Jafg
{

struct LUserInputContext;

DECLARE_JAFG_WIDGET()
class WEditorView final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditorView(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WEditorView)

public:

    virtual void Tick() override;

    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

private:

    void OnSecondaryDown();
    void OnSecondaryUp();

    TCdrEmptyArray<TArray<LUserInputContext*>> LastUsedContexts;
};

} /* ~Namespace Jafg */
