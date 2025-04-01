// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/Surface.h"
#include "Widgets/WidgetNode.h"
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

    void Initialize(LObjectContext* InOuter);
    void Tick(LUserInput* UserInput);
    void TearDown();

    ENGINE_API LLocalEgo*  GetLocalEgo() const;
    ENGINE_API LLocalEgo*  GetLocalEgoChecked() const;
    ENGINE_API LUserInput* GetUserInput() const;
    ENGINE_API LUserInput* GetUserInputChecked() const;

    FORCEINLINE i32 GetSurfaceCount() const { return this->Surfaces.GetSize(); }
    FORCEINLINE auto  GetSurfaces() -> TArray<LSurface>& { return this->Surfaces; }
    FORCEINLINE auto  GetSurfaces() const -> const TArray<LSurface>& { return this->Surfaces; }

    FORCEINLINE bool IsFocusedSurfaceValid() const { return this->FocusedSurface > INDEX_NONE; }
    FORCEINLINE auto GetFocusedSurface() -> LSurface* { return this->IsFocusedSurfaceValid() ? &this->Surfaces[this->FocusedSurface] : nullptr; }
    FORCEINLINE auto GetFocusedSurfaceChecked() -> LSurface* { check( this->IsFocusedSurfaceValid() ) return &this->Surfaces[this->FocusedSurface]; }
    FORCEINLINE auto GetFocusedSurfaceAsserted() -> LSurface* { jassert( this->IsFocusedSurfaceValid() ) return &this->Surfaces[this->FocusedSurface]; }
    FORCEINLINE auto GetFocusedSurface() const -> const LSurface* { return this->IsFocusedSurfaceValid() ? &this->Surfaces[this->FocusedSurface] : nullptr; }
    FORCEINLINE auto GetFocusedSurfaceChecked() const -> const LSurface* { check( this->IsFocusedSurfaceValid() ) return &this->Surfaces[this->FocusedSurface]; }
    FORCEINLINE auto GetFocusedSurfaceAsserted() const -> const LSurface* { jassert( this->IsFocusedSurfaceValid() ) return &this->Surfaces[this->FocusedSurface]; }

    ENGINE_API void AddWidget(LViewport* Context, WUserWidget* Widget);
    ENGINE_API void AddWidget(LSurface* Context, WUserWidget* Widget);
    ENGINE_API void RemoveWidget(WUserWidget* Widget);

    ENGINE_API WWidgetNode* GetTopLevelWidgetByClass(const LViewport* Context, const LObjectClass* WidgetClass) const { return Context->GetTopLevelWidgetByClass(WidgetClass); }
    ENGINE_API WWidgetNode* GetTopLevelWidgetByClass(const LSurface* Context, const LObjectClass* WidgetClass) const { return Context->GetViewport().GetTopLevelWidgetByClass(WidgetClass); }
    ENGINE_API WWidgetNode* GetTopLevelWidgetByClassChecked(LViewport* Context, const LObjectClass* WidgetClass) const { return Context->GetTopLevelWidgetByClassChecked(WidgetClass); }
    ENGINE_API WWidgetNode* GetTopLevelWidgetByClassChecked(LSurface* Context, const LObjectClass* WidgetClass) const { return Context->GetViewport().GetTopLevelWidgetByClassChecked(WidgetClass); }
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClass(LViewport* Context) const { return Context->GetTopLevelWidgetByClass<TNode>(Context); }
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClass(LSurface* Context) const { return Context->GetViewport().GetTopLevelWidgetByClass<TNode>(); }
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClassChecked(LViewport* Context) const { return Context->GetTopLevelWidgetByClassChecked<TNode>(); }
    template <typename TNode> FORCEINLINE TNode* GetTopLevelWidgetByClassChecked(LSurface* Context) const { return Context->GetViewport().GetTopLevelWidgetByClassChecked<TNode>(); }

    //# Checks first the currently focused surface, then all others. Returns the first valid result.
    ENGINE_API  WWidgetNode* GetFirstTopLevelWidgetByClass(const LObjectClass* WidgetClass) const;
    FORCEINLINE WWidgetNode* GetFirstTopLevelWidgetByClassChecked(const LObjectClass* WidgetClass) const;
    template <typename TNode> FORCEINLINE TNode* GetFirstTopLevelWidgetByClass() const;
    template <typename TNode> FORCEINLINE TNode* GetFirstTopLevelWidgetByClassChecked() const;

    //#
    //# Change the visibility of a top level widget.
    //# @param InVisibility   The new visibility state of the widget.
    //# @param bAllowNotFound If true, nothing happens if the widget was not found. If false, the program will panic.
    //# @return True, if widget was found and made visible. False if widget was not found or was already visible.
    //#
    template <typename TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const LViewport* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    template <typename TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const LSurface* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(const LViewport* Context, const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(const LSurface* Context, const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    //# Instead of searching inside a specific context, this method search algorithm works just like the #GetFirstTopLevelWidgetByClass method.
    template <typename TNode>
    UNUSED FORCEINLINE bool ChangeWidgetVisibility(const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;
    UNUSED ENGINE_API  bool ChangeWidgetVisibility(const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound = false) const;

    ENGINE_API  bool FocusWidget(LViewport* Context, const WWidgetNode* InNode);
    ENGINE_API  bool FocusWidgetChecked(LViewport* Context, const WWidgetNode* InNode);
    ENGINE_API  bool FocusWidgetAsserted(LViewport* Context, const WWidgetNode* InNode);
    FORCEINLINE bool FocusWidget(LSurface* Context, const WWidgetNode* InNode) { return this->FocusWidget(&Context->GetViewport(), InNode); }
    FORCEINLINE bool FocusWidgetChecked(LSurface* Context, const WWidgetNode* InNode) { return this->FocusWidgetChecked(&Context->GetViewport(), InNode); }
    FORCEINLINE bool FocusWidgetAsserted(LSurface* Context, const WWidgetNode* InNode) { return this->FocusWidgetAsserted(&Context->GetViewport(), InNode); }

private:

    LSurface CreateNewSurface();

    TArray<LSurface>   Surfaces;
    i32                FocusedSurface = 0;
    LObjectContext*      CachedOuter = nullptr;
    LSubsystemCollection Collection;
};

FORCEINLINE WWidgetNode* LFrontend::GetFirstTopLevelWidgetByClassChecked(const LObjectClass* WidgetClass) const
{
    WWidgetNode* Out = this->GetFirstTopLevelWidgetByClass(WidgetClass);
    check( Out )
    return Out;
}

template <typename TNode>
FORCEINLINE TNode* LFrontend::GetFirstTopLevelWidgetByClass() const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode>(this->GetFirstTopLevelWidgetByClass(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE TNode* LFrontend::GetFirstTopLevelWidgetByClassChecked() const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return CheckedStaticCast<TNode>(this->GetFirstTopLevelWidgetByClassChecked(TNode::StaticClass()));
}

template <typename TNode>
FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LViewport* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return this->ChangeWidgetVisibility(Context, TNode::StaticClass(), InVisibility, bAllowNotFound);
}

template <typename TNode>
FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LSurface* Context, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return this->ChangeWidgetVisibility(Context, TNode::StaticClass(), InVisibility, bAllowNotFound);
}

template <typename TNode>
bool LFrontend::ChangeWidgetVisibility(const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    static_assert(std::derived_from<TNode, WWidgetNode>, "TNode must derive from WWidgetNode.");
    return this->ChangeWidgetVisibility(TNode::StaticClass(), InVisibility, bAllowNotFound);
}

FORCEINLINE bool LFrontend::ChangeWidgetVisibility(const LSurface* Context, const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    check( Context )
    return this->ChangeWidgetVisibility(&Context->GetViewport(), WidgetClass, InVisibility, bAllowNotFound);
}

} /* ~Namespace Jafg */
