// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "Button.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET(EClassFlags::Abstract)
class ENGINE_API WButton : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WButton(const LObjectInitializer& ObjectInitializer);

public:


    // WWidgetNode implementation
    virtual void Construct() override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LKeyEvent& InKeyEvent) override;
    // ~WWidgetNode implementation

    virtual void OnPrimaryPress()     { }
    virtual void OnPrimaryUnpress()   { }
    virtual void OnSecondaryPress()   { }
    virtual void OnSecondaryUnpress() { }

    FORCEINLINE void SetNormalBrush(const LRegionBrush& InBrush) { this->NormalBrush = InBrush; }
    FORCEINLINE auto GetNormalBrush() const -> const LRegionBrush& { return this->NormalBrush; }
    FORCEINLINE void SetHoverBrush(const LRegionBrush& InBrush) { this->HoverBrush = InBrush; }
    FORCEINLINE auto GetHoverBrush() const -> const LRegionBrush& { return this->HoverBrush; }
    FORCEINLINE void SetPressBrush(const LRegionBrush& InBrush) { this->PressBrush = InBrush; }
    FORCEINLINE auto GetPressBrush() const -> const LRegionBrush& { return this->PressBrush; }

    FORCEINLINE void SetLetUiReactToEvents(const bool bInLetUiReactToEvents) { this->bLetUiReactToEvents = bInLetUiReactToEvents; }
    FORCEINLINE auto GetLetUiReactToEvents() const -> bool { return this->bLetUiReactToEvents; }

protected:

    bool bLetUiReactToEvents = true;
    LRegionBrush NormalBrush = { LColor::Black };
    LRegionBrush HoverBrush  = { LColor::Gray };
    LRegionBrush PressBrush  = { LColor::White };
};

} /* ~Namespace Jafg */
