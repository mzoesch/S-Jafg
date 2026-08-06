// Copyright mzoesch. All rights reserved.

#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Platform/Surface.h"
#include "Framework/FrontendSubsystem.h"
#include "Framework/SubsystemCollection.h"
#include "Core/LaunchProgress.h"
#include "Engine/Engine.h"
#include "User/Input/UserInput.h"
#include "Stats/Stats.h"

void Jafg::LFrontendBase::Initialize(LClassOuter* Outer)
{
    this->GetCollection()->InitializeDeferred(Outer);
    this->GetCollection()->InitializeSubsystems<JFrontendSubsystem>();
}

void Jafg::LFrontendBase::Tick()
{
    STAT_CYCLE_FUNCTION()

    algo::for_each(this->Surfaces, [](auto&& Surface){ Surface->BeginNewFrame(); });
    this->AsFrontend()->PollPlatformEvents();
    algo::for_each(this->Surfaces, [](auto&& Surface){ Surface->Poll(); });
    algo::for_each(this->Surfaces, [](auto&& Surface){ Surface->Tick(); });

    this->ForEachMutableSubsystem([](JFrontendSubsystem* E)
    {
        if (E->ShouldTick())
        {
            E->Tick();
        }
    });
}

void Jafg::LFrontendBase::TearDown()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogFrontend, "Tearing down frontend and all its surfaces.")

    this->Collection.TearDownSubsystems();
    algo::orphan(&this->Surfaces);
}

Jafg::LEngine const& Jafg::LFrontendBase::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine when an object of LFrontendBase exists is undefined behavior.")
    return *GEngine;
}

Jafg::LEngine& Jafg::LFrontendBase::GetMutableEngine() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine when an object of LFrontendBase exists is undefined behavior.")
    return *Detail::GMutableEngine;
}

Jafg::LLocalEgo const& Jafg::LFrontendBase::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine when an object of LFrontendBase exists is undefined behavior.")
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo& Jafg::LFrontendBase::GetMutableLocalEgo() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine when an object of LFrontendBase exists is undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo();
}

Jafg::LSurface& Jafg::LFrontendBase::AddSurface(TUnique<LSurface> Surface, ENewSurfaceBehavior Behavior) noexcept
{
    check( Tasks::IsOnMasterThread() )

    auto& Result{*this->Surfaces.emplace_back(std::move(Surface))};

    if (Behavior == ENewSurfaceBehavior::Focus)
    {
        this->FocusedSurface = static_cast<i32>(this->Surfaces.size()) - 1;
        check( this->IsFocusedSurfaceValid() )
    }
    else if (Behavior == ENewSurfaceBehavior::FocusIfNoneFocused)
    {
        if (this->IsFocusedSurfaceValid() == false)
        {
            this->FocusedSurface = static_cast<i32>(this->Surfaces.size()) - 1;
            check( this->IsFocusedSurfaceValid() )
        }
    }
    else if (Behavior == ENewSurfaceBehavior::FocusIfNonePresent)
    {
        if (this->Surfaces.size() == 1)
        {
            this->FocusedSurface = static_cast<i32>(this->Surfaces.size()) - 1;
            check( this->IsFocusedSurfaceValid() )
        }
    }

    return Result;
}

void Jafg::LFrontendBase::RemoveSurface(LSurface& Surface, bool bExitIfLastSurface /* = true */) noexcept
{
    auto It{algo::find_if(this->Surfaces, [&Surface](auto const& Ptr){ return Ptr.get() == &Surface; })};
    check(It != this->Surfaces.end())
    std::size_t Idx{static_cast<std::size_t>(std::distance(this->Surfaces.begin(), It))};

    this->Surfaces.erase(It);
    if (this->FocusedSurface == static_cast<i32>(Idx))
    {
        this->FocusedSurface = INDEX_NONE;
    }
    else if (this->FocusedSurface > static_cast<i32>(Idx))
    {
        --this->FocusedSurface;
    }

    if (bExitIfLastSurface && this->Surfaces.empty())
    {
        App::RequestEngineExit("All surfaces ended.");
    }
}
