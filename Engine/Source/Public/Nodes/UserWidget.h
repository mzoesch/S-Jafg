// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "UserWidget.generated.h"

namespace Jafg
{

class LViewport;
class WParent;

//#
//# A user widget is a visual node that can be added to #Viewport of a surface as a top level node.
//# A user widget can consist of many widgets and nodes and can be used to create complex
//# user interfaces. User widgets are there to only orchestrate the overall flow of UI events and
//# serve as the API for the logic it contains. Therefore, they have no painting capabilities for themselves.
//# If you see yourself digging through the children of a user widget, then this action should be implemented
//# as a method of the user widget itself.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WUserWidget : public WOverlay
{
    GENERATED_CLASS_BODY()
    friend LViewport;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WUserWidget)
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

template<typename TNode> requires TIsCompleteType_v<TNode> && std::is_base_of_v<WUserWidget, TNode>
struct TDeferredUserWidgetExec : public TDeferredObjectExec<TNode>
{
    inline ~TDeferredUserWidgetExec()
    {
        if (this->bReleased == false)
        {
            this->bReleased = true;
            checkCode
            (
                if (this->Factory.get())
                {
                    check(this->Factory->_IsReleased() == false && this->Factory->_IsDecommissioned() == false)
                    this->Factory->_Release();
                }
            )
            this->Class.GetViewport()._AddWidget(&this->Class);
        }
    }

    TUnique<LNodeFactoryBase> Factory;
    typename TNode::LFactory& Style()
    {
        check(this->Factory.get() == nullptr)
        this->Factory = TUnique<typename TNode::LFactory>{new typename TNode::LFactory{this->Class}};
        return *static_cast<typename TNode::LFactory*>(&*this->Factory);
    }
};

} /* ~Namespace Detail */

inline constexpr Detail::NewDeferredObjectFn<LNodeDynamicInit, TNodeStaticInit, Detail::TDeferredUserWidgetExec, WUserWidget> ConstructDeferredWidget{};
inline constexpr Detail::NewObjectFn<decltype(ConstructDeferredWidget), LNodeDynamicInit, TNodeStaticInit, WUserWidget> ConstructWidget{ConstructDeferredWidget};

} /* ~Namespace Jafg */
