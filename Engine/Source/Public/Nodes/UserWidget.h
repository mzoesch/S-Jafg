// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "UserWidget.generated.h"

namespace Jafg
{

class LViewport;
class WParent;

//# Initializer for the dynamic ctors for WUserWidgets.
typedef Detail::TCxxDynamicInit<LViewport, Detail::LViewport2OuterProj> LWidgetDynamicInit;
//# Initializer for the static ctors for WUserWidgets.
template<typename TCxxClass>
using TWidgetStaticInit = Detail::TCxxStaticInitBase<LViewport, TCxxClass, Detail::LViewport2OuterProj>;

//#
//# A user widget is a widget node that can be added to the local ego widget viewport.
//# A user widget can consist of multiple widget nodes and can be used to create complex
//# user interfaces.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WUserWidget : public WOverlay
{
    GENERATED_CLASS_BODY()
    friend LViewport;

protected:

    inline explicit WUserWidget(LWidgetDynamicInit const& Init)
        : Super{LNodeDynamicInit{.Outer=Init.Outer,.Class=Init.Class}}
    {
        this->SetAnchor(EAnchor::Fill);
        this->SetShouldTick(false);
    }
    template<typename TCxxClass>
    inline explicit WUserWidget(TWidgetStaticInit<TCxxClass> const& Init) noexcept
        : Super{TNodeStaticInit<TCxxClass>{.Outer=Init.Outer}}
    {
        this->SetAnchor(EAnchor::Fill);
        this->SetShouldTick(false);
    }

public:

    virtual void Destruct() override;

    //# Whether this is a top level widget inside the viewport.
    FORCEINLINE constexpr bool IsTopLevel() const noexcept { return this->bIsTopLevel; }

private:

    bool bIsTopLevel{};
};

} /* ~Namespace Jafg */

#include "Nodes/Viewport.h"

namespace Jafg
{

namespace Detail
{

template<typename TCxxClass> requires TIsCompleteType_v<TCxxClass> && std::is_base_of_v<JCxxClass, TCxxClass>
struct TDeferredUserWidgetExec : public TDeferredObjectExec<TCxxClass>
{
    inline ~TDeferredUserWidgetExec()
    {
        if (this->bReleased == false)
        {
            this->bReleased = true;
            this->Class.GetViewport()._AddWidget(&this->Class);
        }
    }
};

} /* ~Namespace Detail */

inline constexpr Detail::NewDeferredObjectFn<LWidgetDynamicInit, TWidgetStaticInit, Detail::TDeferredUserWidgetExec, WUserWidget> ConstructDeferredWidget{};
inline constexpr Detail::NewObjectFn<decltype(ConstructDeferredWidget), LWidgetDynamicInit, TWidgetStaticInit, WUserWidget> ConstructWidget{ConstructDeferredWidget};

} /* ~Namespace Jafg */
