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

    DEFAULT_NODE_CONSTRUCTORS_BODY(WEditorView)
    {
        this->SetAnchor(EAnchor::Fill);
        this->SetVisibility(ENodeVisibility::Visible);
    }

public:

    virtual void Tick() override;

    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

private:

    void OnSecondaryDown();
    void OnSecondaryUp();

    TArray<LUserInputContext*> LastUsedContexts;
};

} /* ~Namespace Jafg */
