// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/UserInput.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInputAction.h"
#include "User/Input/InputActionValue.h"

void Jafg::LUserInput::BeginNewFrame()
{
    LSurface* Context = this->GetLocalEgo()->GetPrimarySurface();
    check( Context )

    Context->GetCurrentlyPressedKeys().SwapBuffers(Context->GetLastFramePressedKeys());
    Context->GetCurrentlyPressedKeys().Reset(Context->GetCurrentlyPressedKeys().GetSize());

    return;
}

bool Jafg::LUserInput::IsNewDown(const LKey Key) const
{
    const LSurface* Context = this->GetLocalEgo()->GetPrimarySurface();
    check( Context )
    return Context->GetCurrentlyPressedKeys().Contains(Key) && (Context->GetLastFramePressedKeys().Contains(Key) == false);
}

void Jafg::LUserInput::DispatchInputDelegates()
{
    LLocalEgo* LocalEgo = this->GetCheckedLocalEgo();
    if (!LocalEgo->DoesPossess() || !LocalEgo->HasPrimarySurface())
    {
        return;
    }
    check( LocalEgo->GetPossessed()->GetWorld() )
    LWorld* TargetWorld = LocalEgo->GetPossessed()->GetWorld();
    LSurface* PrimarySurface = LocalEgo->GetPrimarySurface();

    const TdhArray<LRawInput> TriggeredKeys = this->GetTriggeredKeys();
    const TdhArray<LRawInput> OngoingKeys   = this->GetOngoingKeys();
    const TdhArray<LRawInput> CompletedKeys = this->GetCompletedKeys();

    // if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Q))
    // {
    //     LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(DOWN, Application::GetDeltaTimeAsFloat());
    // }
    //
    // if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::E))
    // {
    //     LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(UP, Application::GetDeltaTimeAsFloat());
    // }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Escape))
    {
        if (this->IsNewDown(EKeys::Escape))
        {
            TargetWorld->bShowMouse = !TargetWorld->bShowMouse;
            TargetWorld->FirstTimeMouseScroll = true;
            LocalEgo->GetPrimarySurface()->SetInputMode(TargetWorld->bShowMouse);
        }
    }

    {
        const LRawInput* const MouseX = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseX);
        const LRawInput* const MouseY = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseY);

        if (PrimarySurface->GetCurrentlyPressedKeys().IsEmpty() == false)
        {
            PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH();
        }

        if (MouseX != nullptr && MouseY != nullptr)
        {
            TargetWorld->MouseCallback(MouseX->Value, MouseY->Value);
        }
        else if (MouseX != nullptr)
        {
            TargetWorld->MouseCallback(MouseX->Value, 0.0f);
        }
        else if (MouseY != nullptr)
        {
            TargetWorld->MouseCallback(0.0f, MouseY->Value);
        }
    }

    {
        const LRawInput* const MouseWheelAxis = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseWheelAxis);
        if (MouseWheelAxis != nullptr)
        {
            TargetWorld->ScrollCallback(MouseWheelAxis->Value);
        }
    }

    for (LUserInputContext* Context : this->ActiveContexts)
    {
        for (LUserInputMappedAction& MappedAction : Context->GetMappedActions())
        {
            if (MappedAction.Trigger == EUserInputActionTrigger::Triggered)
            {
                this->DispatchInputDelegatesForAction(TriggeredKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EUserInputActionTrigger::Ongoing)
            {
                this->DispatchInputDelegatesForAction(OngoingKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EUserInputActionTrigger::Completed)
            {
                this->DispatchInputDelegatesForAction(CompletedKeys, &MappedAction);
            }

            continue;
        }

        continue;
    }

    return;
}

Jafg::LLocalEgo* Jafg::LUserInput::GetLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetCheckedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetCheckedLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetPanickedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetPanickedLocalEgo();
}

void Jafg::LUserInput::RegisterContext(LUserInputContext&& Context, const bool bMakeActive /* = false */)
{
    LUserInputContext* ContextPtr = new LUserInputContext(std::move(Context));
    this->RegisteredContexts.Add(ContextPtr);

    if (bMakeActive)
    {
        this->ActiveContexts.Add(ContextPtr);
    }

    return;
}

Jafg::TdhArray<Jafg::LRawInput> Jafg::LUserInput::GetTriggeredKeys() const
{
    TdhArray<LRawInput> TriggeredKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys())
    {
        if (this->GetLocalEgo()->GetPrimarySurface()->GetLastFramePressedKeys().Contains(Key) == false)
        {
            TriggeredKeys.Emplace(Key);
        }

        continue;
    }

    return TriggeredKeys;
}

Jafg::TdhArray<Jafg::LRawInput>& Jafg::LUserInput::GetOngoingKeys() const
{
    return this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys();
}

Jafg::TdhArray<Jafg::LRawInput> Jafg::LUserInput::GetCompletedKeys() const
{
    TdhArray<LRawInput> CompletedKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetPrimarySurface()->GetLastFramePressedKeys())
    {
        if (this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys().Contains(Key) == false)
        {
            CompletedKeys.Emplace(Key);
        }

        continue;
    }

    return CompletedKeys;
}

void Jafg::LUserInput::GetContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const
{
    for (LUserInputContext* Context : this->RegisteredContexts)
    {
        if (Context->GetUniqueIdentifier() == InName)
        {
            OutContext = Context;
            return;
        }

        continue;
    }

    OutContext = nullptr;
    return;
}

void Jafg::LUserInput::GetCheckedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const
{
    this->GetContextByName(InName, OutContext);
    check( OutContext )
    return;
}

void Jafg::LUserInput::DispatchInputDelegatesForAction(const TdhArray<LRawInput>& InRawInputs, LUserInputMappedAction* InAction)
{
    LInputActionValue Value = InAction->Action->Category;

    for (const LRawInput& RawInput : InRawInputs)
    {
        for (const LInputActionMappedKey& MappedKey : InAction->Action->MappedKeys)
        {
            if (MappedKey.Key == RawInput.Key)
            {
                LVector Magnitude = LVector::UnitX();

                for (LInputActionMappedKeyModifier* Modifier : MappedKey.Modifiers)
                {
                    Magnitude = Modifier->ApplyModifier(Magnitude);
                }

                Value += Magnitude;

                continue;
            }

            continue;
        }

        continue;
    }

    if (Value.IsNonZero())
    {
        InAction->Callback(Value);
    }

    return;
}
