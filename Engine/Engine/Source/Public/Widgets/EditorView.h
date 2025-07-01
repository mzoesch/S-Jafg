// Copyright mzoesch. All rights reserved.

#pragma once

#include "UserWidget.h"
#include "EditorView.generated.h"

namespace Jafg
{

struct LUserInputContext;

DECLARE_JAFG_WIDGET()
class ENGINE_API WEditorView final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditorView(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Tick() override;

    virtual LReply OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

private:

    void OnSecondaryDown();
    void OnSecondaryUp();

    TArray<LUserInputContext*> LastUsedContexts;
};

} /* ~Namespace Jafg */
