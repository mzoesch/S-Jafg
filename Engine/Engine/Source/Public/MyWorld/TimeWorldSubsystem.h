// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/TickableWorldSubsystem.h"
#include "Core/Uuid.h"
#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Cli/CliHandles.h"
#include "Cli/CommandLineInterface.h"
#include "TimeWorldSubsystem.generated.h"

namespace Jafg
{

class JTimeWorldSubsystem;
struct LCliDayTime;
struct LAstron;

//#
//# The daytime of a MyWorld.
//# The daytime is the time for one single day. At the end of the day, it resets to zero.
//# The daytime is measured in (world) milliseconds. Meaning this does not equal to the real and physically defined
//# millisecond.
//# Generally speaking, the time of the day is as followed defined (for worlds that use the default time system - this
//# logic will not uphold for all worlds):
//# - Starts at     00:00:00:000    with a value of:              0.
//# - Ends   at     23:59:59:999    with a value of:    8.64e+7 - 1.
//# Therefore some common values are:
//# - 00:00:00:000  =>  0
//# - 06:00:00:000  =>  2,16e+7
//# - 12:00:00:000  =>  4,32e+7
//# - 18:00:00:000  =>  6,48e+7
//# - 23:59:59:999  =>  8.64e+7 - 1
//#
typedef u64 LDaytime;

//#
//# The cycle of a day in MyWorld.
//# One cycle is one full day (@see LDayTime). This integer represents the number of cycles that have passed since
//# the world has been created.
//# This cycle times the #JTimeWorldSubsystem::MaxDayTime and added with the #JTimeWorldSubsystem::DayTime is the
//# most accurate time that can be represented in a MyWorld.
//#
typedef u64 LDayCycle;

//#
//# When calling the #JTimeWorldSubsystem::SetDayTime method, this enum defines how the time value should be treated.
//#
namespace EDayTimeAddBehavior
{

enum Type
{
    //#
    //# Clamp the provided daytime to the worlds min and max daytime. Then override the current daytime value.
    //#
    Clamp,

    //#
    //# Add the provided daytime to the current daytime value. If the resulting value is larger than the
    //# world's max daytime, then the day cycle is incremented and the daytime is set to the resulting value
    //# minus the world's max daytime. This process is repeated until the resulting value is smaller than
    //# the world's max daytime.
    //#
    Add,

    //#
    //# Sets the daytime to the provided value, but will not go back in time. If the provided value is smaller
    //# than the current daytime, then the current daytime is set and the day cycle counter is incremented by one.
    //# The value has to be in the bounds of the world's min and max daytime, otherwise it will panic.
    //#
    Next,
};

} /* ~Namespace EDayTimeAdd */

template<>
struct LCommandArgsTypeRet<EDayTimeAddBehavior::Type> final
{
    typedef EDayTimeAddBehavior::Type Type;
};
template <>
FORCEINLINE LCommandArgsTypeRet<EDayTimeAddBehavior::Type>::Type LCommandArgs::GetAs<EDayTimeAddBehavior::Type>() const;
template <>
FORCEINLINE LCliType LCliType::Type<EDayTimeAddBehavior::Type>()
{
    return LCliType::Type<LCliQuery>("<Mutates>", LString{}, LCliQuery::Input
    {
        "Clamp", "Add", "Next"
    });
}

//#
//# Named day times for worlds that follow a daylight cycle.
//# If a world does not follow a daylight cycle, then the behavior of setting the time through
//# these values is undefined.
//#
namespace ENamedDayTime
{

enum Type
{
    Sunrise,
    Morning,
    Noon,
    Dusk,
    Night,
    Midnight,
};

} /* ~Namespace ENamedDayTime */
ENamedDayTime::Type StringToLex(const LString& InString);

struct LCliDayTime final
{
    UTILITY_STRUCT(LCliDayTime)
};
template <>
struct LCommandArgsTypeRet<LCliDayTime> final
{
    UTILITY_STRUCT(LCommandArgsTypeRet)
    typedef LDaytime Type;

    ENGINE_API static Type Dispatch(const LCommandArgs& Self, const JTimeWorldSubsystem* InSubsystem);
    typedef decltype(&Dispatch) Dispatcher;
};
template <> FORCEINLINE LCliType LCliType::Type<LCliDayTime>() { return LCliType::Type("DayTime"); }

//#
//# Controls various time related stuff for worlds.
//# Note that this does not include anything with delta time or engine time - this ONLY is responsible
//# for "time" from a simulation standpoint.
//#
//# The default daytime is considered from 04:50 o'clock to 21:53 o'clock, the other "half" is considered as night.
//# The day is therefore 17:03 hours (1'023 minutes) long, the night is 06:57 hours (417 minutes) long.
//# The default day acceleration is:     25.575.
//# The default night acceleration is:   20.85.
//# Therefore the day is 40 minutes long (in the real world) and the night is 20 minutes long; combining to a total
//# of 50 (real world) minutes.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API JTimeWorldSubsystem final : public JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    explicit JTimeWorldSubsystem(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(JTimeWorldSubsystem)

    virtual void OnGarbageDefault(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter) override;

    // JTickableWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(LClassOuter const* Outer) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void Tick(const f32 DeltaTime) override;
    // ~JTickableWorldSubsystem implementation

public:

    enum E_DDMMYYYY { DDMMYYYY };

    enum E_HHMMSS { HHMMSS };
    enum E_HHMM { HHMM };
    enum E_MMSS { MMSS };
    enum E_HH { HH };
    enum E_MM { MM };
    enum E_SS { SS };

    //# The minimal time in this world.
    FORCEINLINE LDaytime GetMinDayTime() const noexcept { return this->MinDaytime; }
    FORCEINLINE void SetMinDayTime(const LDaytime InMinTime) noexcept { this->MinDaytime = InMinTime; }
    //# The maximal time in this world.
    FORCEINLINE LDaytime GetMaxDayTime() const noexcept { return this->MaxDaytime; }
    FORCEINLINE void SetMaxDayTime(const LDaytime InMaxTime) noexcept { this->MaxDaytime = InMaxTime; }

    void SetDayTime(const ENamedDayTime::Type InNamedDayTime, const EDayTimeAddBehavior::Type InAddType = EDayTimeAddBehavior::Clamp);
    void SetDayTime(const LDaytime InDayTime, const EDayTimeAddBehavior::Type InAddType = EDayTimeAddBehavior::Clamp);
    void SetDayCycle(const LDayCycle InDayCycle);

    LDaytime GetDayTimeFromNamedTimes(const ENamedDayTime::Type InNamedDayTime) const;

    FORCEINLINE LDaytime  GetDayTime() const noexcept { return this->Daytime; }
    FORCEINLINE LDayCycle GetDayCycle() const noexcept { return this->DayCycle; }

    FORCEINLINE void SetDayAcceleration(const f64 InDayAcceleration) noexcept { this->DayAcceleration = InDayAcceleration; }
    FORCEINLINE f64  GetDayAcceleration() const noexcept { return this->DayAcceleration; }
    FORCEINLINE void SetNightAcceleration(const f64 InNightAcceleration) noexcept { this->NightAcceleration = InNightAcceleration; }
    FORCEINLINE f64  GetNightAcceleration() const noexcept { return this->NightAcceleration; }

    FORCEINLINE bool  IsSunAstronIdentifierValid() const noexcept { return this->SunAstronIdentifier.IsValid(); }
    FORCEINLINE const LUuid&   GetSunAstronIdentifier() const noexcept { return this->SunAstronIdentifier; }
                const LAstron* GetSunAstron() const;
    FORCEINLINE const LAstron* GetSunAstronChecked() const { const LAstron* Out { this->GetSunAstron() }; check( Out ) return Out; }
    FORCEINLINE const LAstron* GetSunAstronAsserted() const { const LAstron* Out { this->GetSunAstron() }; jassert( Out ) return Out; }
                      LAstron* GetMutableSunAstron();
    FORCEINLINE       LAstron* GetMutableSunAstronChecked() { LAstron* Out { this->GetMutableSunAstron() }; check( Out ) return Out; }
    FORCEINLINE       LAstron* GetMutableSunAstronAsserted() { LAstron* Out { this->GetMutableSunAstron() }; jassert( Out ) return Out; }

    FORCEINLINE LDaytime GetDayTimeSinceStart() const noexcept;
    FORCEINLINE u64      GetDayTimeSinceStartInSeconds() const noexcept;
    FORCEINLINE f64      GetDayTimeInPercentage() const noexcept;
    FORCEINLINE f64      GetDayTimeInPercentage_CurrentDayOnly() const noexcept;

    FORCEINLINE bool IsDay() const noexcept;
    FORCEINLINE bool IsNight() const noexcept;

    //#
    //# Get the time as it would be on Earth (meaning from 00:00 to 23:59).
    //# The output will be the same PERCENTAGE of the earth-day compared to the world-day.
    //# If the world-day is only 12 milliseconds long then this would return at the time of 00:00:00:006
    //# 12:00:00:000 o'clock.
    //#
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_HHMMSS) const;
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_HHMM) const;
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_MMSS) const;
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_HH) const;
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_MM) const;
    LString GetInterpolatedTimeAsItWouldBeOnEarth(E_SS) const;

    //#
    //# The day of the month with the year, as it would be on Earth.
    //#
    LString GetDayCycleAsItWouldBeOnEarth(E_DDMMYYYY) const;

private:

    void DefaultOnly_RegisterCliObjects();
    void DefaultOnly_UnregisterCliObjects();

    void OnTimeUpdated();

    CLASS_FIELD()
    TCdrIgnore<LCliCommandHandle> CommandHandle_Time;

    CLASS_FIELD()
    TCdrIgnore<LCliTypeHandle> TypeHandle_DayTime;

    //# The actual daytime.
    LDaytime Daytime { 0 };

    //# Inclusive daytime.
    LDaytime MinDaytime { 0 };
    //# Exclusive daytime.
    LDaytime MaxDaytime { static_cast<LDaytime>(24.0 * LAL_H2MS_D) };
    //# How many daylight cycles have passed.
    LDayCycle DayCycle { 0 };

    //# Default is: 04:50 o'clock
    LDaytime StartOfTheDay { (4 * static_cast<LDaytime>(LAL_H2M) + 50) * static_cast<LDaytime>(LAL_M2MS) };
    //# Default is: 21:53 o'clock
    LDaytime EndOfTheDay { (21 * static_cast<LDaytime>(LAL_H2M) + 53) * static_cast<LDaytime>(LAL_M2MS) };

    f64 DayAcceleration { 25.575 };
    f64 NightAcceleration { 20.85 };

    LUuid SunAstronIdentifier;
};

template <>
FORCEINLINE LCommandArgsTypeRet<EDayTimeAddBehavior::Type>::Type LCommandArgs::GetAs<EDayTimeAddBehavior::Type>() const
{
    if (this->Name == "Clamp")
    {
        return EDayTimeAddBehavior::Clamp;
    }
    if (this->Name == "Add")
    {
        return EDayTimeAddBehavior::Add;
    }
    if (this->Name == "Next")
    {
        return EDayTimeAddBehavior::Next;
    }

    jassertNoEntry()
    return EDayTimeAddBehavior::Clamp;
}

FORCEINLINE LDaytime JTimeWorldSubsystem::GetDayTimeSinceStart() const noexcept
{
    return (this->DayCycle * this->MaxDaytime) + this->Daytime;
}

FORCEINLINE u64 JTimeWorldSubsystem::GetDayTimeSinceStartInSeconds() const noexcept
{
    return static_cast<u64>(static_cast<f64>(this->GetDayTimeSinceStart()) * LAL_MS2S_D);
}

FORCEINLINE f64 JTimeWorldSubsystem::GetDayTimeInPercentage() const noexcept
{
    return static_cast<f64>(this->GetDayTimeSinceStart()) / static_cast<f64>(this->MaxDaytime - this->MinDaytime);
}

FORCEINLINE f64 JTimeWorldSubsystem::GetDayTimeInPercentage_CurrentDayOnly() const noexcept
{
    return static_cast<f64>(this->Daytime) / static_cast<f64>(this->MaxDaytime - this->MinDaytime);
}

FORCEINLINE bool JTimeWorldSubsystem::IsDay() const noexcept
{
    return this->Daytime >= this->StartOfTheDay && this->Daytime < this->EndOfTheDay;
}

FORCEINLINE bool JTimeWorldSubsystem::IsNight() const noexcept
{
    return this->IsDay() == false;
}

} /* ~Namespace Jafg */
