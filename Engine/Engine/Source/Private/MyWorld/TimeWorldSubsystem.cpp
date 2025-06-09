// Copyright mzoesch. All rights reserved.

#include "MyWorld/TimeWorldSubsystem.h"
#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Engine/Engine.h"

Jafg::JTimeWorldSubsystem::JTimeWorldSubsystem(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    if (GEngine)
    {
        this->DefaultOnly_RegisterCommands();
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, [this](void) -> void
        {
            this->DefaultOnly_RegisterCommands();
            return;
        });
    }

    return;
}

void Jafg::JTimeWorldSubsystem::OnGarbageDefault()
{
    Super::OnGarbageDefault();

    this->DefaultOnly_UnregisterCommands();

    return;
}

bool Jafg::JTimeWorldSubsystem::ShouldCreateSubsystem(const LObjectContext* InOuter) const
{
    if (Super::ShouldCreateSubsystem(InOuter) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(InOuter);
}

void Jafg::JTimeWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    return;
}

void Jafg::JTimeWorldSubsystem::Tick(const f32 DeltaTime)
{
    Super::Tick(DeltaTime);

    return;
}

void Jafg::JTimeWorldSubsystem::TearDown()
{
    Super::TearDown();

    return;
}

void Jafg::JTimeWorldSubsystem::SetDayTime(const ENamedDayTime::Type InNamedDayTime, const EDayTimeAddBehavior::Type InAddType)
{
    return;
}

void Jafg::JTimeWorldSubsystem::SetDayTime(const LDayTime InDayTime, const EDayTimeAddBehavior::Type InAddType)
{
    return;
}

void Jafg::JTimeWorldSubsystem::SetDayCycle(const LDayCycle InDayCycle)
{
    return;
}

void Jafg::JTimeWorldSubsystem::DefaultOnly_RegisterCommands()
{
    check( this->IsDefault() )
    check( GEngine )

    LOG_VERBOSE(LogTime, "Registering time related types and commands.")

    /* Type: DayTime */
    {
    }

    /* Command: Time */
    {
        check( this->CommandHandle_Time.IsValid() == false )
        this->CommandHandle_Time = GEngine->GetCommandLineInterface()->RegisterCommand({"Time", "Changes day and night related time variables.",
            LCommandParams()
            .Token(LCliType::Type<LCliQuery>("Time queries", "", LCliQuery::Input
            {
                "Clamp", "Add", "Next"
            }))
            .Token(LCliType::Type<LWorld>())
            .Token(LCliType::Type<u64>())
            .Exec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
            {
                check( InArgs.SubArgs.GetSize() == 3 )
                const EDayTimeAddBehavior::Type Behavior { InArgs[0].GetAs<EDayTimeAddBehavior::Type>() };
                LWorld* World { InArgs[1].GetAs<LWorld>() };
                const u64 Time { InArgs[2].GetAs<u64>() };

                JTimeWorldSubsystem* Subsystem { World->GetSubsystem<JTimeWorldSubsystem>() };
                if (Subsystem == nullptr)
                {
                    OutResponse->StdErr = LString::SprintF("Invalid subsystem at [{}]", InArgs[0].Name);
                    OutResponse->Rc = ECommandReturnCode::SemanticError;
                    return;
                }

                if (Time < Subsystem->GetMinDayTime())
                {
                    OutResponse->StdErr = LString::SprintF("Time is out of bounds [{} < {}]", Time, Subsystem->GetMinDayTime());
                    OutResponse->Rc = ECommandReturnCode::TypeError;
                    return;
                }
                if (Time > Subsystem->GetMaxDayTime())
                {
                    OutResponse->StdErr = LString::SprintF("Time is out of bounds [{} < {}]", Subsystem->GetMaxDayTime(), Time);
                    OutResponse->Rc = ECommandReturnCode::TypeError;
                    return;
                }

                Subsystem->SetDayTime(Time, Behavior);

                OutResponse->StdOut = LString::SprintF("Changed time to [{}]", Subsystem->GetDayTime());
                OutResponse->Rc = ECommandReturnCode::Success;
                return;
            }))
        });
        check( this->CommandHandle_Time.IsValid() )
    }

    return;
}

void Jafg::JTimeWorldSubsystem::DefaultOnly_UnregisterCommands()
{
    check( this->IsDefault() )

    LOG_VERBOSE(LogTime, "Unregistering time related commands.")

    return;
}
