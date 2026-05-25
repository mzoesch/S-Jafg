// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #if JAFG_IN_SHIPPING
        #define JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER                    0
    #else /* JAFG_IN_SHIPPING */
        #define JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER                    1
    #endif /* !JAFG_IN_SHIPPING */
#endif /* !JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER */

//#
//# Quick and dirty way to measure the time taken by a control path. The result is being logged.
//#
//# @param Category  The category to log to.
//# @param Verbosity The verbosity level of the message. If the verbosity level of the category is lower than this,
//#                  the message will not be logged.
//# @param Name      The base name identifying the task that is being measured.
//#
#if JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)                          \
        const ::Jafg::LPrivateLogTimeTaken  JAFG_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Jafg::LPrivateLogTimeTaken<                                                   \
                ::Jafg::ELogVerbosity:: Verbosity,                                          \
                Category.GetCompileTimeVerbosity()                                         \
                >                                                                          \
            (Category, PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC, Name);
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format,...)               \
        const ::Jafg::LPrivateLogTimeTaken  JAFG_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Jafg::LPrivateLogTimeTaken<                                                   \
                ::Jafg::ELogVerbosity:: Verbosity,                                          \
                Category.GetCompileTimeVerbosity()                                         \
            >                                                                              \
            (Category, PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC, std::format(              \
                "" Format "" __VA_OPT__(,) __VA_ARGS__                                                \
            ));
#else /* DO_LOG_SCOPED_TIME_TASK_MEASURER */
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format,...)
#endif /* !DO_LOG_SCOPED_TIME_TASK_MEASURER */

namespace Jafg
{

template<ELogVerbosity::Type Verbosity, ELogVerbosity::Type CategoryVerbosity>
struct LPrivateLogTimeTaken final
{
    LPrivateLogTimeTaken(
        LLogCategory<CategoryVerbosity>& InCategory,
        LString InFunction,
        LString InBaseMessage
    )
        : Category(&InCategory), StartTime(std::chrono::high_resolution_clock::now()), Function(std::move(InFunction)), BaseMessage(InBaseMessage)
    {
    }

    ~LPrivateLogTimeTaken()
    {
        const f64 TimeTaken{ std::chrono::duration<f64>(std::chrono::high_resolution_clock::now() - this->StartTime).count() };

        ::Jafg::LogMessage<Verbosity, CategoryVerbosity>(
           "{}[{}] - {}: {} took {} seconds. " JAFG_LOG_COLOR_END,
           GetColorForVerbosity(Verbosity),
           this->Category->GetCategory(),
           this->Function,
           this->BaseMessage,
           TimeTaken
           );
    }

private:

    LLogCategory<CategoryVerbosity>* Category;
    std::chrono::high_resolution_clock::time_point StartTime;
    LString Function;
    LString BaseMessage;
};

} /* ~Namespace Jafg */
