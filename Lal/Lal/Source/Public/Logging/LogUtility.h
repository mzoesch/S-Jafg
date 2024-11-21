// Copyright mzoesch. All rights reserved.

#pragma once

#define DO_LOG_SCOPED_TIME_TASK_MEASURER 1

/**
 * Quick and dirty way to measure the time taken by a control path. The result is being logged to the std out.
 *
 * @param Category  The category to log to.
 * @param Verbosity The verbosity level of the message. If the verbosity level of the category is lower than this,
 *                  the message will not be logged.
 * @param Name      The base name identifying the task that is being measured.
 */
#if DO_LOG_SCOPED_TIME_TASK_MEASURER
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)                                         \
        const ::Jafg::LPrivateLogTimeTaken  PRIVATE_JAFG_CORE_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Jafg::LPrivateLogTimeTaken<                                                                 \
                ::Jafg::ELogVerbosity:: Verbosity,                                                        \
                Category.GetCompileTimeVerbosity()                                                        \
            >                                                                                             \
            (Category, PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC, Name);
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format, ...)                             \
        const ::Jafg::LPrivateLogTimeTaken  PRIVATE_JAFG_CORE_JOIN_INNER_TWO(___TimeTaken, __COUNTER__) = \
            ::Jafg::LPrivateLogTimeTaken<                                                                 \
                ::Jafg::ELogVerbosity:: Verbosity,                                                        \
                Category.GetCompileTimeVerbosity()                                                        \
            >                                                                                             \
            (Category, PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC, std::format(                            \
                "" Format "", ##__VA_ARGS__                                                               \
            ));
#else /* DO_LOG_SCOPED_TIME_TASK_MEASURER */
    #define SCOPED_TIME_TAKEN_MEASURER(Category, Verbosity, Name)
    #define SCOPED_TIME_TAKEN_MEASURER_MsgF(Category, Verbosity, Format, ...)
#endif /* !DO_LOG_SCOPED_TIME_TASK_MEASURER */

namespace Jafg
{

template <ELogVerbosityType T, ELogVerbosityType CategoryVerbosity>
struct LPrivateLogTimeTaken
{
    typedef std::chrono::high_resolution_clock LHrc;

    LPrivateLogTimeTaken(
        Private::LLogCategory<CategoryVerbosity>& InCategory,
        LStringLegacy                             InFunction,
        const LStringLegacy&&                     InBaseMessage
    )
    : Category(&InCategory), Function(std::move(InFunction)), BaseMessage(InBaseMessage)
    {
        this->StartTime = LHrc::now();
    }

    ~LPrivateLogTimeTaken()
    {
        const double TimeTaken = std::chrono::duration<double>(LHrc::now() - this->StartTime).count();
        Private::LogMessage<T>(*this->Category, std::move(this->Function), std::format(
            "[{}] took {} seconds.",
            this->BaseMessage,
            TimeTaken
        ));
    }

private:

    Private::LLogCategory<CategoryVerbosity>* Category;
    LHrc::time_point StartTime;
    LStringLegacy    Function;
    LStringLegacy    BaseMessage;
};

} /* ~Namespace Jafg */
