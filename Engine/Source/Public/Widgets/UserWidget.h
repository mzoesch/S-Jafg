// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
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

    //# Set the root for this user widget.
    void SetRoot(WNode* InRoot);
    virtual void RemoveFromParent(bool bDestroy = true) override;

    FORCEINLINE bool IsRootValid() const noexcept { return this->GetChildren().empty() == false; }
    FORCEINLINE WNode* GetRoot() const noexcept { if (this->GetChildren().empty()) { return nullptr; } return this->GetChildren()[0]->Content; }
    FORCEINLINE WNode* GetRootChecked() const noexcept { check(this->GetChildren().empty() == false) return this->GetChildren()[0]->Content; }
    FORCEINLINE WNode* GetRootAsserted() const noexcept { jassert(this->GetChildren().empty() == false) return this->GetChildren()[0]->Content; }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetRoot() const noexcept { return StaticCast<TNode>(this->GetRoot()); }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetRootChecked() const noexcept { return StaticCast<TNode>(this->GetRootChecked()); }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetRootAsserted() const noexcept { return StaticCast<TNode>(this->GetRootAsserted()); }

private:

    virtual LWidgetSlot* AddChild(WNode* InChild) override final { this->SetRoot(InChild); return this->GetChildren()[0]; }
    virtual LWidgetSlot* AddChildAt(const i32 InIndex, WNode* InChild) override final { this->SetRoot(InChild); return this->GetChildren()[0]; }

    bool bIsTopLevel{};
};

} /* ~Namespace Jafg */

#include "Widgets/Viewport.h"

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
            this->Class.GetViewport().AddWidget(&this->Class);
        }
    }
};

} /* ~Namespace Detail */

inline constexpr Detail::NewDeferredObjectFn<LWidgetDynamicInit, TWidgetStaticInit, Detail::TDeferredUserWidgetExec, WUserWidget> ConstructDeferredWidget{};
inline constexpr Detail::NewObjectFn<decltype(ConstructDeferredWidget), LWidgetDynamicInit, TWidgetStaticInit, WUserWidget> ConstructWidget{ConstructDeferredWidget};

template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
FORCEINLINE TWidget* LViewport::AddWidget()
{
    return ConstructWidget(TWidgetStaticInit<TWidget>{*this});
}

} /* ~Namespace Jafg */
