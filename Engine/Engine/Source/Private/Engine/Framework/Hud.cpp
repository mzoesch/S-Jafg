// Copyright mzoesch. All rights reserved.

#include "Engine/Framework/Hud.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "Subsystems/HudSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/Viewport.h"
#include "Core/LaunchProgress.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/UserWidget.h"

void Jafg::LHud::Initialize(LObjectContext* InOuter)
{
    this->CachedOuter = InOuter;
    GCurrentWidgetContextState = this->CachedOuter;

    this->Surfaces.Emplace(this->CreateNewSurface());

    for (LSurface& Surface : this->Surfaces)
    {
        // TODO Move to LLevel or something as an option.
        Surface.SetInputMode(EInputMode::InputSubSystem, HideMouseCursor);
    }

    this->FocusedSurface = this->Surfaces.GetSize() - 1;
    check( this->IsFocusedSurfaceValid() )

    this->Collection.DeferredInitialize(this->CachedOuter);
    this->Collection.InitializeSubsystems(JHudSubsystem::StaticClass());

    return;
}

void Jafg::LHud::Tick(LUserInput* UserInput)
{
    for (LSurface& Surface : this->Surfaces)
    {
        Surface.OnClear();
        Surface.BeginNewFrame();
        Surface.PollInputs();
        Surface.PollEvents();
        Surface.PollVirtualInputs();

        continue;
    }

    for (LSurface& Surface : this->Surfaces)
    {
        Surface.Tick();
    }

    for (LSurface& Surface : this->Surfaces)
    {
        if (Surface.GetInputMode() & EInputMode::InputSubSystem)
        {
            UserInput->DispatchInputDelegates();
        }

        continue;
    }

    this->Collection.ForEachSubsystem( [] (JSubsystem* Subsystem)
    {
        if (JHudSubsystem* HudSubsystem = DynamicCast<JHudSubsystem>(Subsystem); HudSubsystem)
        {
            if (HudSubsystem->ShouldTick())
            {
                HudSubsystem->Tick(Application::GetDeltaTimeAsFloat());
            }

            return;
        }

        panicMsgf( "Could not cast predicated hud subsystem [{}] to JHudSubsystem.", Subsystem->GetFullName() )

        return;
    });

    return;
}

void Jafg::LHud::TearDown()
{
    check( this->CachedOuter )

    GCurrentWidgetContextState = nullptr;
    this->Collection.TearDownSubsystems();

    for (LSurface& Surface : this->Surfaces)
    {
        Surface.TearDown();
    }
    this->Surfaces.Empty();
    this->CachedOuter = nullptr;

    return;
}

Jafg::LLocalEgo* Jafg::LHud::GetLocalEgo() const
{
    check( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo* Jafg::LHud::GetLocalEgoChecked() const
{
    check( GEngine )
    return GEngine->GetLocalEgoChecked();
}

Jafg::LUserInput* Jafg::LHud::GetUserInput() const
{
    check( GEngine )
    return GEngine->GetLocalEgo()->GetUserInput();
}

Jafg::LUserInput* Jafg::LHud::GetUserInputChecked() const
{
    check( GEngine && GEngine->GetLocalEgoChecked() )
    return GEngine->GetLocalEgoChecked()->GetUserInput();
}

void Jafg::LHud::AddWidget(LViewport* Context, WUserWidget* Widget)
{
    check( Context )
    Context->AddWidget(Widget);
    return;
}

void Jafg::LHud::AddWidget(LSurface* Context, WUserWidget* Widget)
{
    check( Context )
    Context->GetViewport().AddWidget(Widget);
    return;
}

void Jafg::LHud::RemoveWidget(WUserWidget* Widget)
{
    for (LSurface& Surface : this->Surfaces)
    {
        if (Surface.GetViewport().TryRemoveWidget(Widget))
        {
            return;
        }

        continue;
    }

    panicMsgf( "Could not remove widget [{}] from any surface.", Widget->GetFullName() )

    return;
}

Jafg::WWidgetNode* Jafg::LHud::GetFirstTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    if (this->IsFocusedSurfaceValid())
    {
        if (WWidgetNode* Widget = this->GetFocusedSurface()->GetViewport().GetTopLevelWidgetByClass(WidgetClass); Widget)
        {
            return Widget;
        }
    }

    for (int32 i = 0; i < this->Surfaces.GetSize(); ++i)
    {
        if (this->FocusedSurface == i)
        {
            continue;
        }

        const LSurface& Surface = this->Surfaces[i];
        if (WWidgetNode* Widget = Surface.GetViewport().GetTopLevelWidgetByClass(WidgetClass); Widget)
        {
            return Widget;
        }

        continue;
    }

    return nullptr;
}

bool Jafg::LHud::ChangeWidgetVisibility(const LViewport* Context, const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    WWidgetNode* Widget = this->GetTopLevelWidgetByClass(Context, WidgetClass);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf( "Could not find widget of class [{}] to change visibility.", WidgetClass->GetSpacedClassName() )
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

bool Jafg::LHud::ChangeWidgetVisibility(const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
{
    WWidgetNode* Widget = this->GetFirstTopLevelWidgetByClass(WidgetClass);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf( "Could not find widget of class [{}] to change visibility.", WidgetClass->GetSpacedClassName() )
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

bool Jafg::LHud::FocusWidget(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    return Context->FocusWidgetNode(InNode);
}

bool Jafg::LHud::FocusWidgetChecked(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    const bool bOut = this->FocusWidget(Context, InNode);
    check( bOut )
    return bOut;
}

bool Jafg::LHud::FocusWidgetAsserted(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    const bool bOut = this->FocusWidget(Context, InNode);
    jassert( bOut )
    return bOut;
}

Jafg::LSurface Jafg::LHud::CreateNewSurface()
{
    LSurface Out;

    if (LaunchProgress::Private::GProgressSurface && LaunchProgress::Private::bOwnerShipToken == false)
    {
        LaunchProgress::Private::bOwnerShipToken = true;
        Out = std::move(*LaunchProgress::Private::GProgressSurface);
        LaunchProgress::Private::GProgressSurface = nullptr;
    }
    else
    {
        Out.Initialize();
    }

    return std::move(Out);
}
