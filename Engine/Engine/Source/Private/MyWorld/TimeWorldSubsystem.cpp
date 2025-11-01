// Copyright mzoesch. All rights reserved.

#include "MyWorld/TimeWorldSubsystem.h"
#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "Framework/Skybox.h"

namespace
{

const TArray<LString> NamedDayTimeValues
{
    "Sunrise", "Morning", "Noon", "Dusk", "Night", "Midnight",
};

FORCEINLINE constexpr bool IsLeapYear(const u64 InYear)
{
    if (InYear % 4 != 0)
    {
        return false;
    }

    if (InYear % 100 != 0)
    {
        return true;
    }

    if (InYear % 400 != 0)
    {
        return false;
    }

    return true;
}

FORCEINLINE u64 DaysInMonth(const u64 InYear, const u64 InMonth)
{
    static constexpr u64 Days[]
    {
        31u,
        28u,
        31u,
        30u,
        31u,
        30u,
        31u,
        31u,
        30u,
        31u,
        30u,
        31u
    };

    if (InMonth == 2 && ::IsLeapYear(InYear))
    {
        return 29u;
    }

    return Days[InMonth - 1];
}

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
}

Jafg::LCommandArgsTypeRet<Jafg::LCliDayTime>::Type Jafg::LCommandArgsTypeRet<Jafg::LCliDayTime>::Dispatch(const LCommandArgs& Self, const JTimeWorldSubsystem* InSubsystem)
{
    check( InSubsystem )

    if (algo::contains(::NamedDayTimeValues, Self.Name))
    {
        return InSubsystem->GetDayTimeFromNamedTimes(StringToLex(Self.Name));
    }

    u64 Value;
    auto [ptr, ec]
    {
        std::from_chars
        (
            Self.Name.data(),
            Self.Name.end().base(),
            Value
        )
    };

    if (ec == std::errc{} && ptr == Self.Name.end().base())
    {
        return Value;
    }

    panicMsgf("Could not parse day time from [{}]", Self.Name);
}

Jafg::JTimeWorldSubsystem::JTimeWorldSubsystem(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
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

void Jafg::JTimeWorldSubsystem::OnGarbageDefault(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter)
{
    Super::OnGarbageDefault(Reason, PreviousOuter);

    this->DefaultOnly_UnregisterCliObjects();

    return;
}

bool Jafg::JTimeWorldSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(Outer);
}

void Jafg::JTimeWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    if (LWorld* World { this->GetWorld() }; World->IsSkyboxValid())
    {
        LSkybox& Skybox{ World->GetMutableSkybox() };

        LAstron Sun;
        Sun.Identifier.GenerateNew();
        Sun.Texture.LoadFromDisk({EEnginePaths::Textures, "Misc/Sun.png"}, ERawImageFormat::BGRA8);
        Sun.Texture.Upload();

        Sun.Scale = LVector{500.0f};
        this->SunAstronIdentifier = Sun.Identifier;

        Skybox.GetMutableAstra().emplace_back(std::move(Sun));
    }

    this->SetDayTime(ENamedDayTime::Sunrise, EDayTimeAddBehavior::Clamp);

    return;
}

void Jafg::JTimeWorldSubsystem::Tick(const f32 DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->IsDay())
    {
        this->Daytime += this->DayAcceleration * static_cast<LDaytime>(DeltaTime * LAL_S2MS_D);
    }
    else
    {
        this->Daytime += this->NightAcceleration * static_cast<LDaytime>(DeltaTime * LAL_S2MS_D);
    }

    this->OnTimeUpdated();

    return;
}

void Jafg::JTimeWorldSubsystem::SetDayTime(const ENamedDayTime::Type InNamedDayTime, const EDayTimeAddBehavior::Type InAddType)
{
    this->SetDayTime(this->GetDayTimeFromNamedTimes(InNamedDayTime), InAddType);
    return;
}

void Jafg::JTimeWorldSubsystem::SetDayTime(const LDaytime InDayTime, const EDayTimeAddBehavior::Type InAddType)
{
    const LDaytime PreviousDayTime { this->Daytime };

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
        while (this->Daytime >= this->MaxDaytime)
        {
            ++this->DayCycle;
            this->Daytime -= this->MaxDaytime;
            continue;
        }
        break;
    }
    case EDayTimeAddBehavior::Next:
    {
        jassert( this->Daytime >= this->MinDaytime && this->Daytime < this->MaxDaytime )
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

    if (PreviousDayTime != this->Daytime)
    {
        this->OnTimeUpdated();
    }

    return;
}

void Jafg::JTimeWorldSubsystem::SetDayCycle(const LDayCycle InDayCycle)
{
    this->DayCycle = InDayCycle;
    this->OnTimeUpdated();
    return;
}

Jafg::LDaytime Jafg::JTimeWorldSubsystem::GetDayTimeFromNamedTimes(const ENamedDayTime::Type InNamedDayTime) const
{
    /*
     * Hardcoded for now...
     */

    switch (InNamedDayTime)
    {
    case ENamedDayTime::Sunrise:
    {
        return 6 * LAL_H2MS_D; /* 6:00 */
    }
    case ENamedDayTime::Morning:
    {
        return 9 * LAL_H2MS_D; /* 9:00 */
    }
    case ENamedDayTime::Noon:
    {
        return 12 * LAL_H2MS_D; /* 12:00 */
    }
    case ENamedDayTime::Dusk:
    {
        return 18 * LAL_H2MS_D; /* 18:00 */
    }
    case ENamedDayTime::Night:
    {
        return 21 * LAL_H2MS_D; /* 21:00 */
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

Jafg::LAstron const* Jafg::JTimeWorldSubsystem::GetSunAstron() const
{
    if (const LWorld* World { this->GetWorld() }; World && World->IsSkyboxValid())
    {
        return algo::find_pointer(World->GetSkybox().GetAstra(), this->SunAstronIdentifier, &LAstron::Identifier);
    }

    return nullptr;
}

Jafg::LAstron* Jafg::JTimeWorldSubsystem::GetMutableSunAstron()
{
    if (LWorld* World { this->GetWorld() }; World && World->IsSkyboxValid())
    {
        return algo::find_pointer(World->GetMutableSkybox().GetMutableAstra(), this->SunAstronIdentifier, &LAstron::Identifier);
    }

    return nullptr;
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_HHMMSS) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D)) * LAL_MS2M_D)) };
    const u64 Seconds { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D) - (Minutes * LAL_M2MS_D)) * LAL_MS2S_D)) };

    return Lal::SprintF("{:02}:{:02}:{:02}", Hours, Minutes, Seconds);
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_HHMM) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D)) * LAL_MS2M_D)) };

    return Lal::SprintF("{:02}:{:02}", Hours, Minutes);
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_MMSS) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D)) * LAL_MS2M_D)) };
    const u64 Seconds { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D) - (Minutes * LAL_M2MS_D)) * LAL_MS2S_D)) };

    return Lal::SprintF("{:02}:{:02}", Minutes, Seconds);
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_HH) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };

    return Lal::SprintF("{:02}", Hours);
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_MM) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D)) * LAL_MS2M_D)) };

    return Lal::SprintF("{:02}", Minutes);
}

LString Jafg::JTimeWorldSubsystem::GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::E_SS) const
{
    const u64 EarthMs
    {
        static_cast<u64>(Maths::Round(this->GetDayTimeInPercentage_CurrentDayOnly() * (24.0 * LAL_H2MS_D)))
    };

    const u64 Hours   { static_cast<u64>(Maths::Round(EarthMs * LAL_MS2H_D)) };
    const u64 Minutes { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D)) * LAL_MS2M_D)) };
    const u64 Seconds { static_cast<u64>(Maths::Round((EarthMs - (Hours * LAL_H2MS_D) - (Minutes * LAL_M2MS_D)) * LAL_MS2S_D)) };

    return Lal::SprintF("{:02}", Seconds);
}

LString Jafg::JTimeWorldSubsystem::GetDayCycleAsItWouldBeOnEarth(JTimeWorldSubsystem::E_DDMMYYYY) const
{
    u64 DaysRemaining { this->DayCycle };

    /* Lol, there is no year zero, wtf. Why in the actual fuck would you wanna count from one. */
    u64 Year { 1 };
    while (true)
    {
        if
        (
            const u64 DaysInYear { ::IsLeapYear(Year) ? 366u : 365u };
            DaysRemaining >= DaysInYear
        )
        {
            DaysRemaining -= DaysInYear;
            ++Year;

            continue;
        }

        break;
    }

    i32 Month { 1 }; /* Start at one... */
    while (true)
    {
        if
        (
            const u64 DaysInCurrentMonth { ::DaysInMonth(Year, Month) };
            DaysRemaining >= DaysInCurrentMonth
        )
        {
            DaysRemaining -= DaysInCurrentMonth;
            ++Month;

            continue;
        }

        break;
    }

    /*
     * Plus one because days are not zero-based. Fucking idiots why did they think 500 years ago
     * that one-based stuff is a good idea?
     */
    const i32 Day { static_cast<i32>(DaysRemaining + 1) };

    return Lal::SprintF
    (
        "{:02}-{:02}-{:04}",
        Day,
        Month,
        Year
    );
}

void Jafg::JTimeWorldSubsystem::DefaultOnly_RegisterCliObjects()
{
    check( this->IsDefault() )
    check( GEngine )

    LOG_VERBOSE(LogTime, "Registering time related objects.")

    /* Type: DayTime */
    {
        check( this->TypeHandle_DayTime->IsValid() == false )
        this->TypeHandle_DayTime = GEngine->GetCommandLineInterface().RegisterType({"DayTime", "The time of the day.",
            {},
            [](const LCommandArgs& Args, i32* Cursor) -> bool
            {
                checkSlow( *Cursor < Args.GetArgCount() )
                if (Args[*Cursor].Name.empty())
                {
                    return false;
                }

                if (algo::contains(NamedDayTimeValues, Args[*Cursor].Name))
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
            [](const LCommandArgs& Args, const i32 Cursor, const u32 MaxSuggestions) -> TArray<LString>
            {
                const LCommandArgs* Target { nullptr };

                if (algo::is_valid_index(Args.SubArgs, Cursor))
                {
                    Target = &Args[Cursor];
                }

                TArray<LString> Out;

                for (const LString& Value : ::NamedDayTimeValues)
                {
                    if (Out.size() >= MaxSuggestions)
                    {
                        break;
                    }

                    if (Target)
                    {
                        if (Value.starts_with(Target->Name))
                        {
                            Out.emplace_back(Value);
                        }
                    }
                    else
                    {
                        Out.emplace_back(Value);
                    }

                    continue;
                }

                /*
                 * Also suggest here with the suffixes, d, h, m, s, ms
                 */

                return Out;
            }
        });

        check( this->TypeHandle_DayTime->IsValid() )
    }

    /* Command: Time */
    {
        check( this->CommandHandle_Time->IsValid() == false )
        this->CommandHandle_Time = GEngine->GetCommandLineInterface().RegisterCommand({"Time", "Changes day and night related time variables.",
            LCommandParams{}
            .Token(LCliType::Type<EDayTimeAddBehavior::Type>())
            .Token(LCliType::Type<LWorld>())
            .Token(LCliType::Type<LCliDayTime>())
            .Exec([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
            {
                check( InArgs.SubArgs.size() == 3 )
                const EDayTimeAddBehavior::Type Behavior { InArgs[0].GetAs<EDayTimeAddBehavior::Type>() };
                LWorld* World { InArgs[1].GetAs<LWorld>() };

                JTimeWorldSubsystem* Subsystem { World->GetSubsystem<JTimeWorldSubsystem>() };
                if (Subsystem == nullptr)
                {
                    OutResponse->StdErr = Lal::SprintF("Invalid subsystem at [{}]", InArgs[0].Name);
                    OutResponse->Rc = ECommandReturnCode::SemanticError;
                    return;
                }

                const u64 Time { InArgs[2].GetAs<LCliDayTime>(Subsystem) };

                if (Time < Subsystem->GetMinDayTime())
                {
                    OutResponse->StdErr = Lal::SprintF("Time is out of bounds [{} < {}]", Time, Subsystem->GetMinDayTime());
                    OutResponse->Rc = ECommandReturnCode::TypeError;
                    return;
                }
                if (Time > Subsystem->GetMaxDayTime())
                {
                    OutResponse->StdErr = Lal::SprintF("Time is out of bounds [{} < {}]", Subsystem->GetMaxDayTime(), Time);
                    OutResponse->Rc = ECommandReturnCode::TypeError;
                    return;
                }

                Subsystem->SetDayTime(Time, Behavior);

                OutResponse->StdOut = Lal::SprintF("Changed time to [{}]", Subsystem->GetDayTime());
                OutResponse->Rc = ECommandReturnCode::Success;

                return;
            }),
            LCommandParams()
            .Token(LCliType::Type<LCliString>("Get"))
            .Token(LCliType::Type<LWorld>())
            .Exec([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
            {
                check( InArgs.SubArgs.size() == 2 )
                check( InArgs[0].Name == "Get" )
                LWorld* World { InArgs[1].GetAs<LWorld>() };

                JTimeWorldSubsystem* Subsystem { World->GetSubsystem<JTimeWorldSubsystem>() };
                if (Subsystem == nullptr)
                {
                    OutResponse->StdErr = Lal::SprintF("Invalid subsystem at [{}]", InArgs[0].Name);
                    OutResponse->Rc = ECommandReturnCode::SemanticError;
                    return;
                }

                OutResponse->Rc = ECommandReturnCode::Success;
                OutResponse->StdOut = Lal::SprintF("It is {}.", Subsystem->GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::HHMMSS));

                return;
            })
        });

        check( this->CommandHandle_Time->IsValid() )
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

    if (ensure(this->TypeHandle_DayTime->IsValid()))
    {
        GEngine->GetCommandLineInterface().UnregisterType(this->TypeHandle_DayTime.get_ptr());
    }

    if (ensure(this->CommandHandle_Time->IsValid()))
    {
        GEngine->GetCommandLineInterface().UnregisterCommand(this->CommandHandle_Time.get_ptr());
    }

    return;
}

void Jafg::JTimeWorldSubsystem::OnTimeUpdated()
{
    if (!(this->GetWorld() && this->GetWorld()->IsSkyboxValid()))
    {
        return;
    }

    LSkybox& Skybox{ this->GetWorld()->GetMutableSkybox() };
    const bool bNight { this->IsNight() };

    if (bNight)
    {
        Skybox.SetBackgroundColor(Lal::LColor::Black);
    }
    else
    {
        Skybox.SetBackgroundColor({69, 177, 242});
    }

    if (auto* Sun{ this->GetMutableSunAstron() }; Sun)
    {
        if (bNight)
        {
            Sun->Load = 0.0f;
        }
        else
        {
            Sun->Load = 1.0f;

            const f64 DayTimePercentage
            {
                  static_cast<f64>(this->Daytime - this->StartOfTheDay)
                /
                  static_cast<f64>(this->EndOfTheDay - this->StartOfTheDay)
            };

            const f64 Angle { DayTimePercentage * LAL_PI_D };

            Sun->Direction = LVector
            {
                static_cast<f32>(Maths::Cos(Angle)),
                0.0f,
                static_cast<f32>(Maths::Sin(Angle))
            }.GetNormalized();
        }
    }

    if(auto* NightSky{ algo::find_pointer(Skybox.GetMutableMaps(), "Night", &LLoadedCubemap::Identifier) }; NightSky)
    {
        if (bNight)
        {
            NightSky->Load = 1.0f;
        }
        else
        {
            NightSky->Load = 0.0f;
        }
    }

    return;
}
