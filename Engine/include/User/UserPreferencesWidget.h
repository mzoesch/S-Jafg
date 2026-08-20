// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "User/PreferenceValue.h"
#include "UserPreferencesWidget.generated.h"

namespace Jafg
{

class WTextButton;
class WSwitcher;

DECLARE_JAFG_WIDGET()
class ENGINE_API WUserPreferencesWidget : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WUserPreferencesWidget)
    {
        this->SetShouldTick(true);
    }

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Preferences", "Icons/Jafg.Preferences")

    virtual void Construct() override;
    virtual void Tick() override;

    TArray<LPreferenceValueChangeRequest> Changes;

private:

    void Rebuild(std::size_t Index);

    WTextButton* ApplyButton{};
    WTextButton* DiscardButton{};
};

} /* ~Namespace Jafg */
