// Copyright mzoesch. All rights reserved.

#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Platform/Surface.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Core/LaunchProgress.h"
#include "Engine/Engine.h"
#include "User/Input/UserInput.h"
#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"

void Jafg::LFrontendBase::Initialize(LClassOuter* Outer)
{
    this->GetCollection()->InitializeDeferred(Outer);
    this->GetCollection()->InitializeSubsystems<JFrontendSubsystem>();

    this->GuaranteedTextures.emplace_back(this->GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Jafg.IdentityMul"));
}

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
        if (auto* Fs{this->GetFocusedSurface()}; Fs->GetInputMode() & EInputModeBits::InputSubsystem)
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

    algo::orphan(&this->GuaranteedTextures);
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
