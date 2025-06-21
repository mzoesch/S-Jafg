// Copyright mzoesch. All rights reserved.

#include "MyWorld/TimeWorldSubsystem.h"
#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

namespace
{

const Jafg::TArray<Jafg::LString> NamedDayTimeValues
{
    "Sunrise", "Morning", "Noon", "Dusk", "Night", "Midnight",
};

} /* ~Namespace <Anonymous> */

Jafg::ENamedDayTime::Type Jafg::StringToLex(const LString& InString)
{
    if (InString == "Sunrise")
    {
        return ENamedDayTime::Sunrise;
    }

    if (InString == "Morning")
    {
        return ENamedDayTime::Morning;
    }

    if (InString == "Noon")
    {
        return ENamedDayTime::Noon;
    }

    if (InString == "Dusk")
    {
        return ENamedDayTime::Dusk;
    }

    if (InString == "Night")
    {
        return ENamedDayTime::Night;
    }

    if (InString == "Midnight")
    {
        return ENamedDayTime::Midnight;
    }

    panicMsgf("Could not parse named day time from [{}].", InString);
    return ENamedDayTime::Sunrise;
}

Jafg::LCommandArgsTypeRet<Jafg::LCliDayTime>::Type Jafg::LCommandArgsTypeRet<Jafg::LCliDayTime>::Dispatch(const LCommandArgs& Self, const JTimeWorldSubsystem* InSubsystem)
{
    check( InSubsystem )

    if (::NamedDayTimeValues.Contains(Self.Name))
    {
        return InSubsystem->GetDayTimeFromNamedTimes(StringToLex(Self.Name));
    }

    u64 Value;
    auto [ptr, ec]
    {
        std::from_chars
        (
            Self.Name.GetBegin(),
            Self.Name.GetEnd(),
            Value
        )
    };

    if (ec == std::errc{} && ptr == Self.Name.GetEnd())
    {
        return Value;
    }

    panicMsgf("Could not parse day time from [{}]", Self.Name);
    return 0;
}

Jafg::JTimeWorldSubsystem::JTimeWorldSubsystem(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    if (GEngine)
    {
        this->DefaultOnly_RegisterCliObjects();
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, [this](void) -> void
        {
            this->DefaultOnly_RegisterCliObjects();
            return;
        });
    }

    return;
}

void Jafg::JTimeWorldSubsystem::OnGarbageDefault()
{
    Super::OnGarbageDefault();

    this->DefaultOnly_UnregisterCliObjects();

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

    if (LWorld* World { this->GetWorld() }; World->IsSkyboxValid())
    {
        LSkybox& Skybox { World->GetSkybox() };

        LAstron Sun;
        Sun.Texture.LoadFromDisk({EEnginePaths::Textures, "Misc/Sun.png"}, ERawImageFormat::BGRA8);
        Sun.Texture.Upload();
        Sun.Direction = LVector{20.0f, 0.0f, 30.0f};
        Sun.Direction.Normalize();
        Sun.Scale = LVector{500.0f};

        Skybox.GetMutableAstra().Emplace(std::move(Sun));
    }

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
    this->SetDayTime(this->GetDayTimeFromNamedTimes(InNamedDayTime), InAddType);
    return;
}

void Jafg::JTimeWorldSubsystem::SetDayTime(const LDayTime InDayTime, const EDayTimeAddBehavior::Type InAddType)
{
    switch (InAddType)
    {
    case EDayTimeAddBehavior::Clamp:
    {
        this->Daytime = Maths::Clamp(InDayTime, this->GetMinDayTime(), this->GetMaxDayTime());
        break;
    }
    case EDayTimeAddBehavior::Add:
    {
        this->Daytime += InDayTime;
        while (this->Daytime >= this->MaxDayTime)
        {
            ++this->DayCycle;
            this->Daytime -= this->MaxDayTime;
            continue;
        }
        break;
    }
    case EDayTimeAddBehavior::Next:
    {
        jassert( this->Daytime >= this->MinDayTime && this->Daytime < this->MaxDayTime )
        if (InDayTime < this->Daytime)
        {
            this->DayCycle += 1;
            this->Daytime = InDayTime;
        }
        else
        {
            this->Daytime = InDayTime;
        }
        break;
    }
    default:
    {
        checkNoEntry()
        break;
    }
    }

    return;
}

void Jafg::JTimeWorldSubsystem::SetDayCycle(const LDayCycle InDayCycle)
{
    this->DayCycle = InDayCycle;
    return;
}

Jafg::LDayTime Jafg::JTimeWorldSubsystem::GetDayTimeFromNamedTimes(const ENamedDayTime::Type InNamedDayTime) const
{
    /*
     * Hardcoded for now...
     */

    switch (InNamedDayTime)
    {
    case ENamedDayTime::Sunrise:
    {
        return 6 * JAFG_H2MS_D; /* 6:00 */
    }
    case ENamedDayTime::Morning:
    {
        return 9 * JAFG_H2MS_D; /* 9:00 */
    }
    case ENamedDayTime::Noon:
    {
        return 12 * JAFG_H2MS_D; /* 12:00 */
    }
    case ENamedDayTime::Dusk:
    {
        return 18 * JAFG_H2MS_D; /* 18:00 */
    }
    case ENamedDayTime::Night:
    {
        return 21 * JAFG_H2MS_D; /* 21:00 */
    }
    case ENamedDayTime::Midnight:
    {
        return 0; /* 24:00 */
    }
    default:
    {
        checkNoEntry()
        return 0; /* Default to zero if something goes wrong. */
    }
    }
}

Jafg::LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth() const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetPastDayTimeInPercentage_CurrentDayOnly() * (24.0 * JAFG_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * JAFG_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * JAFG_H2MS_D)) * JAFG_MS2M_D)) };
    const u64 Seconds { static_cast<u64>(Maths::Round((EarthMs - (Hours * JAFG_H2MS_D) - (Minutes * JAFG_M2MS_D)) * JAFG_MS2S_D)) };

    return LString::SprintF("{:02}:{:02}:{:02}", Hours, Minutes, Seconds);
}

void Jafg::JTimeWorldSubsystem::DefaultOnly_RegisterCliObjects()
{
    check( this->IsDefault() )
    check( GEngine )

    LOG_VERBOSE(LogTime, "Registering time related objects.")

    /* Type: DayTime */
    {
        this->TypeHandle_DayTime = GEngine->GetCommandLineInterface()->RegisterType({"DayTime", "The time of the day.",
            nullptr,
            [](const LCommandArgs& Args, i32* Cursor) -> bool
            {
                checkSlow( *Cursor < Args.GetArgCount() )
                if (Args[*Cursor].Name.IsEmpty())
                {
                    return false;
                }

                if (::NamedDayTimeValues.Contains(Args[*Cursor].Name))
                {
                    ++*Cursor;
                    return true;
                }

                /*
                 * Add things like 1d, 1h, 1m, 1s, 1ms
                 */

                if (GEngine == nullptr)
                {
                    return false;
                }

                return LCliType::Type<u64>().CanParse(Args, Cursor);
            },
            nullptr,
            [](const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions) -> TArray<LString>
            {
                const LCommandArgs* Target { nullptr };

                if (Args.SubArgs.IsValidIndex(Cursor))
                {
                    Target = &Args[Cursor];
                }

                TArray<LString> Out;

                for (const LString& Value : ::NamedDayTimeValues)
                {
                    if (Out.GetSize() >= MaxSuggestions)
                    {
                        break;
                    }

                    if (Target)
                    {
                        if (Value.StartsWith(Target->Name))
                        {
                            Out.Emplace(Value);
                        }
                    }
                    else
                    {
                        Out.Emplace(Value);
                    }

                    continue;
                }

                /*
                 * Also suggest here with the suffixes, d, h, m, s, ms
                 */

                return Out;
            }
        });

        check( this->TypeHandle_DayTime.IsValid() )
    }

    /* Command: Time */
    {
        check( this->CommandHandle_Time.IsValid() == false )
        this->CommandHandle_Time = GEngine->GetCommandLineInterface()->RegisterCommand({"Time", "Changes day and night related time variables.",
            LCommandParams()
            .Token(LCliType::Type<EDayTimeAddBehavior::Type>())
            .Token(LCliType::Type<LWorld>())
            .Token(LCliType::Type<LCliDayTime>())
            .Exec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
            {
                check( InArgs.SubArgs.GetSize() == 3 )
                const EDayTimeAddBehavior::Type Behavior { InArgs[0].GetAs<EDayTimeAddBehavior::Type>() };
                LWorld* World { InArgs[1].GetAs<LWorld>() };

                JTimeWorldSubsystem* Subsystem { World->GetSubsystem<JTimeWorldSubsystem>() };
                if (Subsystem == nullptr)
                {
                    OutResponse->StdErr = LString::SprintF("Invalid subsystem at [{}]", InArgs[0].Name);
                    OutResponse->Rc = ECommandReturnCode::SemanticError;
                    return;
                }

                const u64 Time { InArgs[2].GetAs<LCliDayTime>(Subsystem) };

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
            })),
            LCommandParams()
            .Token(LCliType::Type<LCliString>("Get"))
            .Token(LCliType::Type<LWorld>())
            .Exec([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
            {
                check( InArgs.SubArgs.GetSize() == 2 )
                check( InArgs[0].Name == "Get" )
                LWorld* World { InArgs[1].GetAs<LWorld>() };

                JTimeWorldSubsystem* Subsystem { World->GetSubsystem<JTimeWorldSubsystem>() };
                if (Subsystem == nullptr)
                {
                    OutResponse->StdErr = LString::SprintF("Invalid subsystem at [{}]", InArgs[0].Name);
                    OutResponse->Rc = ECommandReturnCode::SemanticError;
                    return;
                }

                OutResponse->Rc = ECommandReturnCode::Success;
                OutResponse->StdOut = LString::SprintF("It is {}.", Subsystem->GetInterpolatedTimeAsItWouldBeOnEarth());

                return;
            })
        });
        check( this->CommandHandle_Time.IsValid() )
    }

    return;
}

void Jafg::JTimeWorldSubsystem::DefaultOnly_UnregisterCliObjects()
{
    check( this->IsDefault() )

    if (GEngine == nullptr)
    {
        LOG_VERBOSE(LogTime, "Engine is invalid. Failed to unregister time related objects.")
        return;
    }

    LOG_VERBOSE(LogTime, "Unregistering time related objects.")

    if (ensure(this->TypeHandle_DayTime.IsValid()))
    {
        GEngine->GetCommandLineInterface()->UnregisterType(&this->TypeHandle_DayTime);
    }

    if (ensure(this->CommandHandle_Time.IsValid()))
    {
        GEngine->GetCommandLineInterface()->UnregisterCommand(&this->CommandHandle_Time);
    }

    return;
}
