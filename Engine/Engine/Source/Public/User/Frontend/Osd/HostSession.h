// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Widgets/Region.h"
#include "Storage/FetchedSave.h"
#include "Widgets/Button.h"
#include "HostSession.generated.h"

namespace Jafg
{

class WButton;
class WScrollRegion;
class WTabBar;
class WVRegion;
class WSwitcher;
class WCommonMenuTabBar;
class WHostSessionScreen_New;
class WHostSessionScreen_Old;
class WHostSessionScreen_Old_Save;

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

    ENGINE_API void ShowOldScreen();
    ENGINE_API void ShowNewScreen();

private:

    WSwitcher* Switcher { nullptr };
    WHostSessionScreen_Old* OldScreen { nullptr };
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

    void OnLoad_General(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Multiplayer(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_WorldGeneration(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Policies(WTabBar* TabBar, WNode* Button, WNode* Panel);
    void OnLoad_Advanced(WTabBar* TabBar, WNode* Button, WNode* Panel);

    WCommonMenuTabBar * InternalTabBar { nullptr };
    WHostSessionScreen* Owner;
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

    FORCEINLINE bool IsSaveValid() const noexcept { return this->Save.IsValid(); }
    FORCEINLINE const LFetchedSave& GetSave() const noexcept { return this->Save; }

    FORCEINLINE bool IsOwnerValid() const noexcept { return this->Owner != nullptr; }
    FORCEINLINE WHostSessionScreen_Old* GetOwner() noexcept { return this->Owner; }
    FORCEINLINE const WHostSessionScreen_Old* GetOwner() const noexcept { return this->Owner; }

protected:

    LFetchedSave Save;
    WHostSessionScreen_Old* Owner;
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

private:

    void RefetchSavesImpl();

    i32 SelectedSaveIndex { INDEX_NONE };
    TArray<LFetchedSave> FetchedSaves;
    WHostSessionScreen* Owner;
    WScrollRegion* SavesRegionContainer { nullptr };
    WVRegion* SavesRegion { nullptr };

    WButton* DeleteButton { nullptr };
    WButton* EditButton { nullptr };
    WButton* HostButton { nullptr };
};

} /* ~Namespace Jafg */
