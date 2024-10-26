// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "WidgetNode.generated.h"

namespace Jafg
{

class LSurface;
struct LWidgetConstructor;

namespace EWidgetVisibility
{

enum Type : uint8
{
    /** Visible, takes up space in the widget layout and hit-testable. */
    Visible,

    /** Not visible, takes up space in the widget layout and is not hit-testable. */
    Hidden,

    /** Not visible, does not take up space in the widget layout and is not hit-testable. */
    Collapsed,

    /** Visible, takes up space in the widget layout and is not hit-testable. */
    TransitiveHitTestInvisible,

    /** Visible, takes up space in the widget layout and is itself not hit-testable, but all children are. */
    IntransitiveHitTestInvisible,
};

} /* ~Namespace EWidgetVisibility */

DECLARE_JAFG_CLASS()
class RHI_API WWidgetNode : public ::Jafg::Private::JObjectBase
{
    GENERATED_CLASS_BODY()

    friend LWidgetConstructor;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetNode)

public:

    virtual void Construct();
    virtual void Tick();
    virtual void Destruct();

    virtual void Draw(LSurface* Context) const;

    /** Weather this widget is allowed to tick this frame. */
    FORCEINLINE auto ShouldNowTick() const -> bool
    {
        return
               ( this->bDisableTick == false )
            && (
                   this->Visibility == EWidgetVisibility::Visible
                || this->Visibility == EWidgetVisibility::TransitiveHitTestInvisible
                || this->Visibility == EWidgetVisibility::IntransitiveHitTestInvisible
            );
    }

    FORCEINLINE auto GetVisibility() const -> EWidgetVisibility::Type { return this->Visibility; }
    FORCEINLINE auto SetVisibility(const EWidgetVisibility::Type InVisibility) -> void { this->Visibility = InVisibility; }

private:

    bool                    bDisableTick = false;
    EWidgetVisibility::Type Visibility   = EWidgetVisibility::Visible;
};

} /* ~Namespace Jafg */
