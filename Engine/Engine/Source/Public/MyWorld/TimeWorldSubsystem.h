// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/TickableWorldSubsystem.h"
#include "Cli/CliPrimitives.h"
#include "Cli/CliExtended.h"
#include "Cli/CliHandles.h"
#include "Cli/CommandLineInterface.h"
#include "TimeWorldSubsystem.generated.h"

namespace Jafg
{

class JTimeWorldSubsystem;
struct LCliDayTime;

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
typedef u64 LDayTime;

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
    typedef LDayTime Type;

    ENGINE_API static Type Dispatch(const LCommandArgs& Self, const JTimeWorldSubsystem* InSubsystem);
    typedef decltype(&Dispatch) Dispatcher;
};
template <> FORCEINLINE LCliType LCliType::Type<LCliDayTime>() { return LCliType::Type("DayTime"); }

//#
//# Controls various time related stuff for worlds.
//# Note that this does not include anything with delta time or engine time - this ONLY is responsible
//# for "time" from a simulation standpoint.
//#
DECLARE_JAFG_CLASS()
class JTimeWorldSubsystem : public JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    explicit JTimeWorldSubsystem(const LObjectInitializer& ObjectInitializer);
    virtual void OnGarbageDefault() override;

    // JTickableWorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void Tick(const f32 DeltaTime) override;
    virtual void TearDown() override;
    // ~JTickableWorldSubsystem implementation

public:

    //# The minimal time in this world.
    FORCEINLINE LDayTime GetMinDayTime() const noexcept { return this->MinDayTime; }
    FORCEINLINE void SetMinDayTime(const LDayTime InMinTime) noexcept { this->MinDayTime = InMinTime; }
    //# The maximal time in this world.
    FORCEINLINE LDayTime GetMaxDayTime() const noexcept { return this->MaxDayTime; }
    FORCEINLINE void SetMaxDayTime(const LDayTime InMaxTime) noexcept { this->MaxDayTime = InMaxTime; }

    ENGINE_API void SetDayTime(const ENamedDayTime::Type InNamedDayTime, const EDayTimeAddBehavior::Type InAddType = EDayTimeAddBehavior::Clamp);
    ENGINE_API void SetDayTime(const LDayTime InDayTime, const EDayTimeAddBehavior::Type InAddType = EDayTimeAddBehavior::Clamp);
    ENGINE_API void SetDayCycle(const LDayCycle InDayCycle);

    ENGINE_API LDayTime GetDayTimeFromNamedTimes(const ENamedDayTime::Type InNamedDayTime) const;

    FORCEINLINE LDayTime  GetDayTime() const noexcept { return this->Daytime; }
    FORCEINLINE LDayCycle GetDayCycle() const noexcept { return this->DayCycle; }

    FORCEINLINE LDayTime GetDayTimeSinceStart() const noexcept;
    FORCEINLINE u64      GetDayTimeSinceStartInSeconds() const noexcept;
    FORCEINLINE f64      GetPastDayTimeInPercentage() const noexcept;
    FORCEINLINE f64      GetPastDayTimeInPercentage_CurrentDayOnly() const noexcept;

    //#
    //# Get the time as it would be on Earth (meaning from 00:00 to 23:59).
    //# The output will be the same PERCENTAGE of the earth-day compared to the world-day.
    //# If the world-day is only 12 milliseconds long then this would return at the time of 00:00:00:006
    //# 12:00:00:000 o'clock.
    //#
    ENGINE_API LString GetInterpolatedTimeAsItWouldBeOnEarth() const;

    //#
    //# The day of the month with the year, as it would be on Earth.
    //#
    ENGINE_API LString GetDayCycleAsItWouldBeOnEarth() const;

private:

    void DefaultOnly_RegisterCliObjects();
    void DefaultOnly_UnregisterCliObjects();

    CLASS_FIELD(DefaultOnly)
    LCliCommandHandle CommandHandle_Time;

    CLASS_FIELD(DefaultOnly)
    LCliTypeHandle TypeHandle_DayTime;

    //# The actual daytime.
    LDayTime Daytime { 0 };

    //# Inclusive daytime.
    LDayTime MinDayTime { 0 };
    //# Exclusive daytime.
    LDayTime MaxDayTime { static_cast<LDayTime>(24.0 * JAFG_H2MS_D) };
    //# How many daylight cycles have passed.
    LDayCycle DayCycle { 0 };
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

FORCEINLINE LDayTime JTimeWorldSubsystem::GetDayTimeSinceStart() const noexcept
{
    return (this->DayCycle * this->MaxDayTime) + this->Daytime;
}

FORCEINLINE u64 JTimeWorldSubsystem::GetDayTimeSinceStartInSeconds() const noexcept
{
    return static_cast<u64>(static_cast<f64>(this->GetDayTimeSinceStart()) * JAFG_MS2S_D);
}

FORCEINLINE f64 JTimeWorldSubsystem::GetPastDayTimeInPercentage() const noexcept
{
    return static_cast<f64>(this->GetDayTimeSinceStart()) / static_cast<f64>(this->MaxDayTime - this->MinDayTime);
}

FORCEINLINE f64 JTimeWorldSubsystem::GetPastDayTimeInPercentage_CurrentDayOnly() const noexcept
{
    return static_cast<f64>(this->Daytime) / static_cast<f64>(this->MaxDayTime - this->MinDayTime);
}

} /* ~Namespace Jafg */
