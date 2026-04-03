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

    virtual LReply OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;
    virtual LReply OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event) override;

private:

    void OnSecondaryDown();
    void OnSecondaryUp();

    TArray<LUserInputContext*> LastUsedContexts;
};

} /* ~Namespace Jafg */
