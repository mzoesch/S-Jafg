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
#include "Platform/PlatformMisc.h"

void Jafg::LFrontendBase::Initialize(LClassOuter* Outer)
{
    check( this->Surfaces.empty() )

    LSurfaceCreateInfo Info{
#if !IN_SHIPPING
        /* For development purposes, we want a smaller window as it does not cover so much space. */
        .DesiredDimensionsPx = { 855, 475 },
#endif /* !IN_SHIPPING */
        .HumanReadableName = "Jafg - @mzoesch",
        };

    this->Surfaces.emplace_back(std::make_unique<LSurface>(Info));

    this->FocusedSurface = this->Surfaces.size() - 1;
    check( this->IsFocusedSurfaceValid() )

    return;
}

void Jafg::LFrontendBase::Tick()
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
        if (auto* Fs{ this->GetFocusedSurface() }; Fs->GetInputMode() & EInputMode::InputSubSystem)
        {
            Fs->GetUserInput().DispatchInputDelegates(*Fs);
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

void Jafg::LFrontendBase::OnUpdate()
{
    for (auto& Surface : this->Surfaces)
    {
        Surface->OnUpdate();
    }

    return;
}

void Jafg::LFrontendBase::TearDown()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogFrontEnd, "Tearing down frontend and all its surfaces.")

    this->Collection.TearDownSubsystems();
    algo::orphan(&this->Surfaces);

    return;
}

Jafg::LEngine& Jafg::LFrontendBase::GetEngine() const noexceptcheck
{
    check( GEngine && "Absence of GEngine when an object of LFrontendBase exists is undefined behavior." )
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::LFrontendBase::GetLocalEgo() const noexceptcheck
{
    return this->GetEngine().GetLocalEgo();
}

void Jafg::LFrontendBase::AddWidget(LViewport* Context, WUserWidget* Widget)
{
    check( Context )
    Context->AddWidget(Widget);
    return;
}

void Jafg::LFrontendBase::AddWidget(LSurface* Context, WUserWidget* Widget)
{
    check( Context )
    Context->GetViewport().AddWidget(Widget);
    return;
}

void Jafg::LFrontendBase::RemoveWidget(WUserWidget* Widget)
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

Jafg::WNode* Jafg::LFrontendBase::GetFirstTopLevelWidgetByClass(TSubclassOf<WNode> Class) const
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

bool Jafg::LFrontendBase::ChangeWidgetVisibility(const LViewport* Context, TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
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

bool Jafg::LFrontendBase::ChangeWidgetVisibility(TSubclassOf<WNode> Class, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
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

bool Jafg::LFrontendBase::FocusWidget(LViewport* Context, WNode* InNode)
{
    check( Context )
    return Context->FocusWidgetNode(InNode);
}
