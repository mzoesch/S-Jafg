// Copyright mzoesch. All rights reserved.

#pragma once

#define DECLARE_AND_DEFINE_LOG_CATEGORY(CompiledCategoryName, CompileTimeVerbosity) \
    struct CompiledCategoryName final : public JafgLog::Private::LogCategory        \
    {                                                                               \
        CompiledCategoryName() : LogCategory() { }                                  \
                                                                                    \
        static const String& GetCategoryName()                                      \
        {                                                                           \
            static const String C = #CompiledCategoryName;                          \
            return C;                                                               \
        }                                                                           \
        static ELogVerbosity GetCompileTimeVerbosity()                              \
        {                                                                           \
            return CompileTimeVerbosity;                                            \
        }                                                                           \
    };
