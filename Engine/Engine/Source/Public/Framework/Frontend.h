// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/Surface.h"
#include "Widgets/Node.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

namespace Jafg
{

class LUserInput;
class LViewport;
class WUserWidget;

//#
//# The frontend is owned by the local ego and shares its lifetime.
//# The frontend is the main hub for all user interface elements. Create frontend subsystems to automatically
//# add multiple widgets of a type any viewport.
//#
class LFrontend final
{
public:

    LFrontend() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LFrontend)
    ~LFrontend() = default;

    void Initialize(LClassOuter* Outer);
    void Tick();
    void TearDown();

    ENGINE_API LEngine&   GetEngine() const noexceptcheck;
    ENGINE_API LLocalEgo& GetLocalEgo() const noexceptcheck;

    FORCEINLINE LSize GetSurfaceCount() const noexcept { return this->Surfaces.size(); }
    FORCEINLINE TArray<TUnique<LSurface>>& GetSurfaces() noexcept { return this->Surfaces; }
    FORCEINLINE TArray<TUnique<LSurface>> const& GetSurfaces() const noexcept { return this->Surfaces; }

    FORCEINLINE bool IsFocusedSurfaceValid() const { return this->FocusedSurface > INDEX_NONE; }
    FORCEINLINE auto GetFocusedSurface() -> LSurface* { return this->IsFocusedSurfaceValid() ? this->Surfaces[this->FocusedSurface].get() : nullptr; }
    FORCEINLINE auto GetFocusedSurfaceChecked() -> LSurface* { check( this->IsFocusedSurfaceValid() ) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE auto GetFocusedSurfaceAsserted() -> LSurface* { jassert( this->IsFocusedSurfaceValid() ) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE auto GetFocusedSurface() const -> const LSurface* { return this->IsFocusedSurfaceValid() ? this->Surfaces[this->FocusedSurface].get() : nullptr; }
    FORCEINLINE auto GetFocusedSurfaceChecked() const -> const LSurface* { check( this->IsFocusedSurfaceValid() ) return this->Surfaces[this->FocusedSurface].get(); }
    FORCEINLINE auto GetFocusedSurfaceAsserted() const -> const LSurface* { jassert( this->IsFocusedSurfaceValid() ) return this->Surfaces[this->FocusedSurface].get(); }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JFrontendSubsystem)

    ENGINE_API void AddWidget(LViewport* Context, WUserWidget* Widget);
    ENGINE_API void AddWidget(LSurface* Context, WUserWidget* Widget);
    ENGINE_API void RemoveWidget(WUserWidget* Widget);

    ENGINE_API WNode* GetTopLevelWidgetByClass(LViewport const* Context, TSubclassOf<WNode> Class) const { return Context->GetTopLevelWidgetByClass(Class); }
    ENGINE_API WNode* GetTopLevelWidgetByClass(LSurface const* Context, TSubclassOf<WNode> Class) const { return Context->GetViewport().GetTopLevelWidgetByClass(Class); }
    ENGINE_API WNode* GetTopLevelWidgetByClassChecked(LViewport* Context, TSubclassOf<WNode> Class) const { return Context->GetTopLevelWidgetByClassChecked(Class); }
    ENGINE_API WNode* GetTopLevelWidgetByClassChecked(LSurface* Context, TSubclassOf<WNode> Class) const { return Context->GetViewport().GetTopLevelWidgetByClassChecked(Class); }
    template<typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClass(LViewport* Context) const { return Context->GetTopLevelWidgetByClass<TNode>(Context); }
    template<typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClass(LSurface* Context) const { return Context->GetViewport().GetTopLevelWidgetByClass<TNode>(); }
    template<typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClassChecked(LViewport* Context) const { return Context->GetTopLevelWidgetByClassChecked<TNode>(); }
    template<typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClassChecked(LSurface* Context) const { return Context->GetViewport().GetTopLevelWidgetByClassChecked<TNode>(); }

    //# Checks first the currently focused surface, then all others. Returns the first valid result.
    ENGINE_API  WNode* GetFirstTopLevelWidgetByClass(TSubclassOf<WNode> Class) const;
    FORCEINLINE WNode* GetFirstTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const;
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetFirstTopLevelWidgetByClass() const { return StaticCast<TNode>(this->GetFirstTopLevelWidgetByClass(TNode::StaticClass())); }
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    FORCEINLINE TNode* GetFirstTopLevelWidgetByClassChecked() const { return StaticCastChecked<TNode>(this->GetFirstTopLevelWidgetByClassChecked(TNode::StaticClass())); }

    //#
    //# Change the visibility of a top level widget.
    //# @param InVisibility   The new visibility state of the widget.
    //# @param bAllowNotFound If true, nothing happens if the widget was not found. If false, the program will panic.
    //# @return True, if widget was found and made visible. False if widget was not found or was already visible.
    //#
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const LViewport* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const LSurface* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(const LViewport* Context, TSubclassOf<WNode>Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(const LSurface* Context, TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    //# Instead of searching inside a specific context, this method search algorithm works just like the #GetFirstTopLevelWidgetByClass method.
    template<typename TNode> requires std::is_base_of_v<WNode, TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(TSubclassOf<WNode>Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;

    ENGINE_API  bool FocusWidget(LViewport* Context, WNode* InNode);
    FORCEINLINE bool FocusWidgetChecked(LViewport* Context, WNode* InNode);
    FORCEINLINE bool FocusWidgetAsserted(LViewport* Context, WNode* InNode);
    FORCEINLINE bool FocusWidget(LSurface* Context, WNode* InNode) { check( Context ) return this->FocusWidget(&Context->GetViewport(), InNode); }
    FORCEINLINE bool FocusWidgetChecked(LSurface* Context, WNode* InNode) { check( Context ) return this->FocusWidgetChecked(&Context->GetViewport(), InNode); }
    FORCEINLINE bool FocusWidgetAsserted(LSurface* Context, WNode* InNode) { check( Context ) return this->FocusWidgetAsserted(&Context->GetViewport(), InNode); }

private:

    TUnique<LSurface> CreateNewSurface();

    i32 FocusedSurface{ 0 };
    TArray<TUnique<LSurface>> Surfaces;
    LSubsystemCollection Collection{ "Frontend" };
};

FORCEINLINE WNode* LFrontend::GetFirstTopLevelWidgetByClassChecked(TSubclassOf<WNode> Class) const
{
    WNode* Out = this->GetFirstTopLevelWidgetByClass(Class);
    check( Out )
    return Out;
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LViewport* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    return this->ChangeWidgetVisibility(Context, TNode::StaticClass(), InVisibility, bAllowNotFound);
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LSurface* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    return this->ChangeWidgetVisibility(Context, TNode::StaticClass(), InVisibility, bAllowNotFound);
}

template<typename TNode> requires std::is_base_of_v<WNode, TNode>
FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    return this->ChangeWidgetVisibility(TNode::StaticClass(), InVisibility, bAllowNotFound);
}

FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LSurface* Context, TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    check( Context )
    return this->ChangeWidgetVisibility(&Context->GetViewport(), Class, InVisibility, bAllowNotFound);
}

FORCEINLINE bool LFrontend::FocusWidgetChecked(LViewport* Context, WNode* InNode)
{
    const bool bOut = this->FocusWidget(Context, InNode);
    check( bOut )
    return bOut;
}

FORCEINLINE bool LFrontend::FocusWidgetAsserted(LViewport* Context, WNode* InNode)
{
    const bool bOut = this->FocusWidget(Context, InNode);
    jassert( bOut )
    return bOut;
}

} /* ~Namespace Jafg */
