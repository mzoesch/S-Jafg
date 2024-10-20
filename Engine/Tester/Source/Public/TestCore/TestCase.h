// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg::Tester
{

/**
 * Base class for all simple test cases.
 */
struct LSimpleTestCase
{
    LSimpleTestCase()                                     = delete;
    explicit LSimpleTestCase(std::string InName, std::string InCategory);
    FORCEINLINE LSimpleTestCase(const LSimpleTestCase& _) = default;
    FORCEINLINE LSimpleTestCase(LSimpleTestCase&& _)      = default;
    FORCEINLINE virtual ~LSimpleTestCase()                = default;

    TESTER_API auto GetName() const -> const std::string&;
    TESTER_API auto GetCategory() const -> const std::string&;
    TESTER_API auto GetFullName() const -> std::string;

    FORCEINLINE auto HasErrors() const -> bool { return this->Errors.empty() == false; }
    FORCEINLINE auto GetErrors() const -> const std::vector<std::string>& { return this->Errors; }

    virtual void Run() = 0;

    TESTER_API void TestEqual(const LChar* What, const int32 X, const int32 Y, const int32 LineNumber);
    TESTER_API void TestEqual(const LChar* What, const int64 X, const int64 Y, const int32 LineNumber);

    TESTER_API void AddError(const LChar* What);


private:

    std::string Name;
    std::string Category;

    std::vector<std::string> Errors;
};

} /* Namespace Jafg::Tester */
