// Copyright mzoesch. All rights reserved.

#include "Engine/Framework/Frontend.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "Subsystems/FrontendSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/Viewport.h"
#include "Core/LaunchProgress.h"
#include "Engine/Engine.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "Widgets/UserWidget.h"

void Jafg::LFrontend::Initialize(LObjectContext* InOuter)
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
    this->Collection.InitializeSubsystems(JFrontendSubsystem::StaticClass());

    return;
}

void Jafg::LFrontend::Tick(LUserInput* UserInput)
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

    if (this->IsFocusedSurfaceValid())
    {
        if (this->GetFocusedSurface()->GetInputMode() & EInputMode::InputSubSystem)
        {
            UserInput->DispatchInputDelegates();
        }
    }

    this->Collection.ForEachSubsystem<JFrontendSubsystem>([](JFrontendSubsystem* Subsystem)
    {
        if (Subsystem->ShouldTick())
        {
            Subsystem->Tick(Application::GetDeltaTimeAsFloat());
        }

        return;
    });

    return;
}

void Jafg::LFrontend::TearDown()
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

Jafg::LLocalEgo* Jafg::LFrontend::GetLocalEgo() const
{
    check( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo* Jafg::LFrontend::GetLocalEgoChecked() const
{
    check( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LUserInput* Jafg::LFrontend::GetUserInput() const
{
    check( GEngine )
    return GEngine->GetLocalEgo()->GetUserInput();
}

Jafg::LUserInput* Jafg::LFrontend::GetUserInputChecked() const
{
    check( GEngine )
    return GEngine->GetLocalEgo()->GetUserInput();
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

Jafg::WWidgetNode* Jafg::LFrontend::GetFirstTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
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

bool Jafg::LFrontend::ChangeWidgetVisibility(const LViewport* Context, const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
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

bool Jafg::LFrontend::ChangeWidgetVisibility(const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound) const
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

bool Jafg::LFrontend::FocusWidget(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    return Context->FocusWidgetNode(InNode);
}

bool Jafg::LFrontend::FocusWidgetChecked(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    const bool bOut = this->FocusWidget(Context, InNode);
    check( bOut )
    return bOut;
}

bool Jafg::LFrontend::FocusWidgetAsserted(LViewport* Context, const WWidgetNode* InNode)
{
    check( Context )
    const bool bOut = this->FocusWidget(Context, InNode);
    jassert( bOut )
    return bOut;
}

Jafg::LSurface Jafg::LFrontend::CreateNewSurface()
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
