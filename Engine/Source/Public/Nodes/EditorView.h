// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
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
        this->SetVisibility(ENodeVisibility::Visible);
        this->Anchor = EAnchor::Fill;
    }

public:

    virtual void Tick() override;

    virtual LReply OnKeyDown(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyDownNoFocus(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUpNoFocus(LNodeKeyEventInfo const& Data, LKeyEvent const& Event) override;

private:

    void OnSecondaryDown();
    void OnSecondaryUp();

    TArray<LUserInputContext*> LastUsedContexts;
};

} /* ~Namespace Jafg */
