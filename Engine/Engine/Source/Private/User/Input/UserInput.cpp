// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/InputAction.h"
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
    const LLocalEgo* LocalEgo = this->GetCheckedLocalEgo();
    if (!LocalEgo->DoesPossess() || !LocalEgo->HasPrimarySurface())
    {
        return;
    }
    check( LocalEgo->GetPossessed()->GetWorld() )

    LWorld*   TargetWorld    = LocalEgo->GetPossessed()->GetWorld(); check( TargetWorld )
    LSurface* PrimarySurface = LocalEgo->GetPrimarySurface(); check( PrimarySurface )

    const TdhArray<LRawInput> TriggeredKeys = this->GetTriggeredKeys();
    const TdhArray<LRawInput> OngoingKeys   = this->GetOngoingKeys();
    const TdhArray<LRawInput> CompletedKeys = this->GetCompletedKeys();

    for (LUserInputContext* Context : this->ActiveContexts)
    {
        for (LInputMappedAction& MappedAction : Context->GetMappedActions())
        {
            if (MappedAction.Trigger == EInputActionTrigger::Triggered)
            {
                this->DispatchInputDelegatesForAction(TriggeredKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EInputActionTrigger::Ongoing)
            {
                this->DispatchInputDelegatesForAction(OngoingKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EInputActionTrigger::Completed)
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

Jafg::LUserInputContext* Jafg::LUserInput::GetContextByName(const LSimpleString& InName)
{
    for (LUserInputContext* Context : this->RegisteredContexts)
    {
        if (Context->GetUniqueIdentifier() == InName)
        {
            return Context;
        }

        continue;
    }

    return nullptr;
}

Jafg::LUserInputContext* Jafg::LUserInput::GetCheckedContextByName(const LSimpleString& InName)
{
#if DO_CHECKS
    LUserInputContext* Context = this->GetContextByName(InName);
    check( Context )
    return Context;
#else /* DO_CHECKS */
    return this->GetContextByName(InName);
#endif /* !DO_CHECKS */
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

void Jafg::LUserInput::DispatchInputDelegatesForAction(const TdhArray<LRawInput>& InRawInputs, const LInputMappedAction* InAction)
{
    LInputActionValue Value = InAction->Action->Category;

    for (const LRawInput& RawInput : InRawInputs)
    {
        for (const LInputActionMappedKey& MappedKey : InAction->Action->MappedKeys)
        {
            if (MappedKey.Key == RawInput.Key)
            {
                LVector Magnitude = LVector(RawInput.Value, 0.0f, 0.0f);

                for (LInputActionMappedKeyModifier* Modifier : MappedKey.Modifiers)
                {
                    Magnitude = Modifier->ApplyModifier(Magnitude);
                }

                Value += Magnitude;

                continue;
            }

            if (MappedKey.Key == EKeys::MouseXY)
            {
                if (RawInput.Key == EKeys::MouseX)
                {
                    Value += LVector(RawInput.Value, 0.0f, 0.0f);
                }
                else if (RawInput.Key == EKeys::MouseY)
                {
                    Value += LVector(0.0f, RawInput.Value, 0.0f);
                }
            }

            continue;
        }

        continue;
    }

    if (Value.IsNonZero())
    {
        check( InAction->Callback )
        (*InAction->Callback)(Value);
    }

    return;
}
