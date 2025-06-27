// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #if IN_SHIPPING
        #define LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER                    0
    #else /* IN_SHIPPING */
        #define LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER                    1
    #endif /* !IN_SHIPPING */
#endif /* !LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER */

//#
//# Quick and dirty way to measure the time taken by a control path. The result is being logged.
//#
//# @param Category  The category to log to.
//# @param Verbosity The verbosity level of the message. If the verbosity level of the category is lower than this,
//#                  the message will not be logged.
//# @param Name      The base name identifying the task that is being measured.
//#
#if LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)                                         \
        const ::Lal::LPrivateLogTimeTaken  PRIVATE_JAFG_CORE_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Lal::LPrivateLogTimeTaken<                                                                 \
                ::Lal::ELogVerbosity:: Verbosity,                                                        \
                Category.GetCompileTimeVerbosity()                                                        \
            >                                                                                             \
            (Category, PRIVATE_LAL_LOG_TRACE_STR_CUR_CLASS_FUNC, Name);
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format, ...)                             \
        const ::Lal::LPrivateLogTimeTaken  PRIVATE_JAFG_CORE_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Lal::LPrivateLogTimeTaken<                                                                 \
                ::Lal::ELogVerbosity:: Verbosity,                                                        \
                Category.GetCompileTimeVerbosity()                                                        \
            >                                                                                             \
            (Category, PRIVATE_LAL_LOG_TRACE_STR_CUR_CLASS_FUNC, std::format(                            \
                "" Format "", ##__VA_ARGS__                                                               \
            ));
#else /* DO_LOG_SCOPED_TIME_TASK_MEASURER */
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format, ...)
#endif /* !DO_LOG_SCOPED_TIME_TASK_MEASURER */

namespace Lal
{

template <ELogVerbosity::Type Verbosity, ELogVerbosity::Type CategoryVerbosity>
struct LPrivateLogTimeTaken final
{
    typedef std::chrono::high_resolution_clock LHrc;

    LPrivateLogTimeTaken(
        LLogCategory<CategoryVerbosity>& InCategory,
        LStringLegacy                    InFunction,
        const LStringLegacy&&            InBaseMessage
    )
        : Category(&InCategory), StartTime(LHrc::now()), Function(std::move(InFunction)), BaseMessage(InBaseMessage)
    {
    }

    ~LPrivateLogTimeTaken()
    {
        const f64 TimeTaken { std::chrono::duration<f64>(LHrc::now() - this->StartTime).count() };

        ::Lal::LogMessage<Verbosity, CategoryVerbosity>(
           "{}[{}] - {}: {} took {} seconds. " LAL_LOG_COLOR_END,
           GetColorForVerbosityView(Verbosity),
           this->Category->GetCategory(),
           this->Function,
           this->BaseMessage,
           TimeTaken
           );
    }

private:

    LLogCategory<CategoryVerbosity>* Category;
    LHrc::time_point StartTime;
    LStringLegacy    Function;
    LStringLegacy    BaseMessage;
};

} /* ~Namespace Lal */
