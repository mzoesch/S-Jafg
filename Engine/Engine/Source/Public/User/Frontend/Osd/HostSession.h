// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/Region.h"
#include "Storage/FetchedSave.h"
#include "Widgets/Button.h"
#include "Widgets/HRegion.h"
#include "HostSession.generated.h"

namespace Jafg
{

class WButton;
class WScrollRegion;
class WTabBar;
class WVRegion;
class WSwitcher;
class WEditableTextBox;
class WCommonMenuTabBar;
class WHostSessionScreen_New;
class WHostSessionScreen_Old;
class WHostSessionScreen_Old_Save;
class WHostSessionScreen_Old_Host;

//#
//# The main class screen for hosting a session.
//#
DECLARE_JAFG_WIDGET()
class WHostSessionScreen : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WHostSessionScreen)

public:

    ENGINE_API virtual void Construct() override;

    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    ENGINE_API void ShowOldScreen();
    ENGINE_API void ShowNewScreen();
    ENGINE_API void ShowOldScreenHost();

    FORCEINLINE WHostSessionScreen_Old* GetOldScreen() noexcept { return this->OldScreen; }
    FORCEINLINE WHostSessionScreen_Old_Host* GetOldScreenHost() noexcept { return this->OldScreenHost; }
    FORCEINLINE WHostSessionScreen_New* GetNewScreen() noexcept { return this->NewScreen; }

private:

    WSwitcher* Switcher { nullptr };
    WHostSessionScreen_Old* OldScreen { nullptr };
    WHostSessionScreen_Old_Host* OldScreenHost { nullptr };
    WHostSessionScreen_New* NewScreen { nullptr };
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WHostSessionScreen_New : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WHostSessionScreen_New(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    virtual bool AddData(const LWidgetNodeData* InData) override;

    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

    FORCEINLINE bool IsOwnerValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE WHostSessionScreen* GetOwner() noexcept { return this->Owner; }
    FORCEINLINE WHostSessionScreen* GetOwnerChecked() noexcept { check( this->Owner ) return this->Owner; }
    FORCEINLINE WHostSessionScreen* GetOwnerAsserted() noexcept { jassert( this->Owner ) return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwner() const noexcept { return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwnerChecked() const noexcept { check( this->Owner ) return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwnerAsserted() const noexcept { jassert( this->Owner ) return this->Owner; }

    FORCEINLINE bool IsInternalTabBarValid() const noexcept { return this->InternalTabBar != nullptr; }
    FORCEINLINE WCommonMenuTabBar* GetInternalTabBar() noexcept { return this->InternalTabBar; }
    FORCEINLINE WCommonMenuTabBar* GetInternalTabBarChecked() noexcept { check( this->InternalTabBar ) return this->InternalTabBar; }
    FORCEINLINE WCommonMenuTabBar* GetInternalTabBarAsserted() noexcept { jassert( this->InternalTabBar ) return this->InternalTabBar; }
    FORCEINLINE const WCommonMenuTabBar* GetInternalTabBar() const noexcept { return this->InternalTabBar; }
    FORCEINLINE const WCommonMenuTabBar* GetInternalTabBarChecked() const noexcept { check( this->InternalTabBar ) return this->InternalTabBar; }
    FORCEINLINE const WCommonMenuTabBar* GetInternalTabBarAsserted() const noexcept { jassert( this->InternalTabBar ) return this->InternalTabBar; }

private:

    TWidgetFactoryHRegion<WHRegion>* AddMenuButtons();

    void OnLoad_General(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Multiplayer(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_WorldGeneration(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Policies(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Advanced(WTabBar* TabBar, WNode* Button, WNode* Panel);

    WCommonMenuTabBar*  InternalTabBar { nullptr };
    WHostSessionScreen* Owner;
    WEditableTextBox* SessionName { nullptr };
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WHostSessionScreen_Old_Save : public WButton
{
    GENERATED_CLASS_BODY()

    friend WHostSessionScreen_Old;

protected:

    explicit WHostSessionScreen_Old_Save(const LObjectInitializer& ObjectInitializer);

public:

    void Reload();

    virtual void OnPrimaryRelease() override;

    FORCEINLINE bool IsSaveValid() const noexcept { return this->Save.IsValid(); }
    FORCEINLINE const LFetchedSave& GetSave() const noexcept { return this->Save; }

    FORCEINLINE bool IsOwnerValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE WHostSessionScreen_Old* GetOwner() noexcept { return this->Owner; }
    FORCEINLINE const WHostSessionScreen_Old* GetOwner() const noexcept { return this->Owner; }

protected:

    LFetchedSave Save;
    WHostSessionScreen_Old* Owner { nullptr };
    std::shared_ptr<LTexture2> OptionalHolder;
};

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class ENGINE_API WHostSessionScreen_Old : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WHostSessionScreen_Old(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    virtual bool AddData(const LWidgetNodeData* InData) override;

    FORCEINLINE bool IsOwnerValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE WHostSessionScreen* GetOwner() noexcept { return this->Owner; }
    FORCEINLINE WHostSessionScreen* GetOwnerChecked() noexcept { check( this->Owner ) return this->Owner; }
    FORCEINLINE WHostSessionScreen* GetOwnerAsserted() noexcept { jassert( this->Owner ) return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwner() const noexcept { return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwnerChecked() const noexcept { check( this->Owner ) return this->Owner; }
    FORCEINLINE const WHostSessionScreen* GetOwnerAsserted() const noexcept { jassert( this->Owner ) return this->Owner; }

    void RefetchSaves();

    CLASS_FIELD(Config)
    TSubclassOf<WHostSessionScreen_Old_Save> SaveNodeClass { LazyInit };

    void HighlightSave(WHostSessionScreen_Old_Save& Who);
    void HighlightNoSave(const bool bScrollUp = false);

private:

    void RefetchSavesImpl();

    i32 SelectedSaveIndex { INDEX_NONE };
    TArray<LFetchedSave> FetchedSaves;
    WHostSessionScreen* Owner { nullptr };
    WScrollRegion* SavesRegionContainer { nullptr };
    WVRegion* SavesRegion { nullptr };

    WButton* DeleteButton { nullptr };
    WButton* EditButton { nullptr };
    WButton* HostButton { nullptr };
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WHostSessionScreen_Old_Host : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WHostSessionScreen_Old_Host(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    virtual bool AddData(const LWidgetNodeData* InData) override;

    void UpdateToCachedSave();

    FORCEINLINE bool IsCachedSaveValid() const noexcept { return this->Save && this->Save->IsValid(); }
    FORCEINLINE void Reset() noexcept { this->Save = nullptr; }
    FORCEINLINE void SetSave(const LFetchedSave& Save) noexcept { this->Save = &Save; }

private:

    TWidgetFactoryHRegion<WHRegion>* AddMenuButtons();

    WTextBox* Header { nullptr };

    WHostSessionScreen* Owner { nullptr };
    const LFetchedSave* Save { nullptr };
};

} /* ~Namespace Jafg */
