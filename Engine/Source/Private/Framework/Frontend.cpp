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

void Jafg::LFrontendBase::Tick()
{
    STAT_CYCLE_FUNCTION()

    for (auto const& Surface : this->Surfaces)
    {
        Surface->BeginNewFrame();
    }

    for (auto const& Surface : this->Surfaces)
    {
        Surface->Tick();
    }

    if (this->IsFocusedSurfaceValid())
    {
        if (auto* Fs{this->GetFocusedSurface()}; Fs->GetInputMode() & EInputMode::InputSubSystem)
        {
            Fs->GetUserInput().DispatchInputDelegates(*Fs);
        }
    }

    this->ForEachMutableSubsystem([](JFrontendSubsystem* E)
    {
        if (E->ShouldTick())
        {
            E->Tick();
        }
    });

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

Jafg::LEngine& Jafg::LFrontendBase::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine when an object of LFrontendBase exists is undefined behavior.")
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::LFrontendBase::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine when an object of LFrontendBase exists is undefined behavior.")
    return GEngine->GetLocalEgo();
}

void Jafg::LFrontendBase::AddSurface(TUnique<LSurface> Surface, ENewSurfaceBehavior Behavior) noexcept
{
    check( Tasks::IsOnMasterThread() )

    this->Surfaces.emplace_back(std::move(Surface));

    if (Behavior == ENewSurfaceBehavior::Focus)
    {
        this->FocusedSurface = this->Surfaces.size() - 1;
        check( this->IsFocusedSurfaceValid() )
    }
    else if (Behavior == ENewSurfaceBehavior::FocusIfNoneFocused)
    {
        if (this->IsFocusedSurfaceValid() == false)
        {
            this->FocusedSurface = this->Surfaces.size() - 1;
            check( this->IsFocusedSurfaceValid() )
        }
    }
    else if (Behavior == ENewSurfaceBehavior::FocusIfNonePresent)
    {
        if (this->Surfaces.size() == 1)
        {
            this->FocusedSurface = this->Surfaces.size() - 1;
            check( this->IsFocusedSurfaceValid() )
        }
    }

    return;
}

void Jafg::LFrontendBase::AddWidget(LViewport* Viewport, WUserWidget* Widget)
{
    check(Viewport)
    Viewport->AddWidget(Widget);
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

    panicMsgf("Could not remove widget [{}] from any surface.", Widget->GetNameAsString())

    return;
}

Jafg::WNode* Jafg::LFrontendBase::GetFirstTopLevelWidgetByClass(TSubclassOf<WNode> Class) const
{
    if (this->IsFocusedSurfaceValid())
    {
        if (WNode* Widget{this->GetFocusedSurface()->GetViewport().GetTopLevelWidgetByClass(Class)}; Widget)
        {
            return Widget;
        }
    }

    for (auto Idx{0uz}; Idx < this->Surfaces.size(); ++Idx)
    {
        if (this->FocusedSurface != INDEX_NONE && static_cast<TArray<LSurface>::size_type>(this->FocusedSurface) == Idx)
        {
            continue;
        }

        LSurface const& Surface{*this->Surfaces[Idx]};
        if (WNode* Widget{Surface.GetViewport().GetTopLevelWidgetByClass(Class)}; Widget)
        {
            return Widget;
        }

        continue;
    }

    return nullptr;
}

bool Jafg::LFrontendBase::ChangeWidgetVisibility(const LViewport* Context, TSubclassOf<WNode> Class, ENodeVisibility Visibility, const bool bAllowNotFound) const
{
    WNode* Widget = this->GetTopLevelWidgetByClass(Context, Class);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf("Could not find widget of class [{}] to change visibility.", Class->GetFullyQualifiedName())
        }
        return false;
    }

    if (Widget->GetVisibility() == Visibility)
    {
        return false;
    }

    Widget->SetVisibility(Visibility);
    return true;
}

bool Jafg::LFrontendBase::ChangeWidgetVisibility(TSubclassOf<WNode> Class, ENodeVisibility Visibility, const bool bAllowNotFound) const
{
    WNode* Widget = this->GetFirstTopLevelWidgetByClass(Class);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf("Could not find widget of class [{}] to change visibility.", Class->GetFullyQualifiedName())
        }
        return false;
    }

    if (Widget->GetVisibility() == Visibility)
    {
        return false;
    }

    Widget->SetVisibility(Visibility);
    return true;
}

bool Jafg::LFrontendBase::FocusWidget(LViewport* Context, WNode* InNode)
{
    check(Context)
    return Context->FocusWidgetNode(InNode);
}
