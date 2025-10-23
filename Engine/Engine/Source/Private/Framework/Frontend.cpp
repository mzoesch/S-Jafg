// Copyright mzoesch. All rights reserved.

#include "Framework/Frontend.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/Node.h"
#include "Widgets/Viewport.h"
#include "Core/LaunchProgress.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/UserWidget.h"
#include "Stats/Stats.h"

void Jafg::LFrontend::Initialize(LClassOuter* Outer)
{
    this->Surfaces.emplace_back(this->CreateNewSurface());
    this->Surfaces.back()->SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);

    this->FocusedSurface = this->Surfaces.size() - 1;
    check( this->IsFocusedSurfaceValid() )

    this->Collection.InitializeDeferred(Outer);
    this->Collection.InitializeSubsystems<JFrontendSubsystem>();

    return;
}

void Jafg::LFrontend::Tick(LUserInput* UserInput)
{
    STAT_CYCLE_FUNCTION()

    for (auto& Surface : this->Surfaces)
    {
        Surface->OnClear();
        Surface->BeginNewFrame();
        Surface->PollInputs();
        Surface->PollEvents();
        Surface->PollVirtualInputs();

        continue;
    }

    for (auto& Surface : this->Surfaces)
    {
        Surface->Tick();
    }

    if (this->IsFocusedSurfaceValid())
    {
        if (this->GetFocusedSurface()->GetInputMode() & EInputMode::InputSubSystem)
        {
            UserInput->DispatchInputDelegates();
        }
    }

    this->ForEachMutableSubsystem([](JFrontendSubsystem* E)
    {
        check( E )

        if (E->ShouldTick())
        {
            E->Tick();
        }

        return;
    });

    return;
}

void Jafg::LFrontend::TearDown()
{
    this->Collection.TearDownSubsystems();

    for (auto& Surface : this->Surfaces)
    {
        Surface->TearDown();
    }
    algo::orphan(&this->Surfaces);

    return;
}

Jafg::LEngine& Jafg::LFrontend::GetEngine() const noexceptcheck
{
    check( GEngine && "Absence of GEngine when an object of LFrontend exists is undefined behavior." )
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::LFrontend::GetLocalEgo() const noexceptcheck
{
    return this->GetEngine().GetLocalEgo();
}

Jafg::LUserInput& Jafg::LFrontend::GetUserInput() const noexceptcheck
{
    return this->GetEngine().GetLocalEgo().GetUserInput();
}

void Jafg::LFrontend::AddWidget(LViewport* Context, WUserWidget* Widget)
{
    check( Context )
    Context->AddWidget(Widget);
    return;
}

void Jafg::LFrontend::AddWidget(LSurface* Context, WUserWidget* Widget)
{
    check( Context )
    Context->GetViewport().AddWidget(Widget);
    return;
}

void Jafg::LFrontend::RemoveWidget(WUserWidget* Widget)
{
    for (auto& Surface : this->Surfaces)
    {
        if (Surface->GetViewport().TryRemoveWidget(Widget))
        {
            return;
        }

        continue;
    }

    panicMsgf( "Could not remove widget [{}] from any surface.", Widget->GetNameAsString() )

    return;
}

Jafg::WNode* Jafg::LFrontend::GetFirstTopLevelWidgetByClass(TSubclassOf<WNode> Class) const
{
    if (this->IsFocusedSurfaceValid())
    {
        if (WNode* Widget = this->GetFocusedSurface()->GetViewport().GetTopLevelWidgetByClass(Class); Widget)
        {
            return Widget;
        }
    }

    for (TArray<LSurface>::size_type Idx { 0 }; Idx < this->Surfaces.size(); ++Idx)
    {
        if (this->FocusedSurface != INDEX_NONE && static_cast<TArray<LSurface>::size_type>(this->FocusedSurface) == Idx)
        {
            continue;
        }

        LSurface const& Surface = *this->Surfaces[Idx];
        if (WNode* Widget{ Surface.GetViewport().GetTopLevelWidgetByClass(Class) }; Widget)
        {
            return Widget;
        }

        continue;
    }

    return nullptr;
}

bool Jafg::LFrontend::ChangeWidgetVisibility(const LViewport* Context, TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    WNode* Widget = this->GetTopLevelWidgetByClass(Context, Class);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf( "Could not find widget of class [{}] to change visibility.", Class->GetFullyQualifiedName() )
        }

        return false;
    }

    if (Widget->GetVisibility() == InVisibility)
    {
        return false;
    }

    Widget->SetVisibility(InVisibility);

    return true;
}

bool Jafg::LFrontend::ChangeWidgetVisibility(TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    WNode* Widget = this->GetFirstTopLevelWidgetByClass(Class);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf( "Could not find widget of class [{}] to change visibility.", Class->GetFullyQualifiedName() )
        }

        return false;
    }

    if (Widget->GetVisibility() == InVisibility)
    {
        return false;
    }

    Widget->SetVisibility(InVisibility);

    return true;
}

bool Jafg::LFrontend::FocusWidget(LViewport* Context, WNode* InNode)
{
    check( Context )
    return Context->FocusWidgetNode(InNode);
}

TUnique<Jafg::LSurface> Jafg::LFrontend::CreateNewSurface()
{
    TUnique Out{ std::make_unique<LSurface>() };

    if (LaunchProgress::Private::GProgressSurface && LaunchProgress::Private::bOwnerShipToken == false)
    {
        LaunchProgress::Private::bOwnerShipToken = true;
        Out = TUnique<LSurface>(LaunchProgress::Private::GProgressSurface);
        LaunchProgress::Private::GProgressSurface = nullptr;
    }
    else
    {
        Out->Initialize();
    }

    return Out;
}
