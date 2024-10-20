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
    explicit LSimpleTestCase(std::string InName, std::string InCategory, std::string InFilename);
    FORCEINLINE LSimpleTestCase(const LSimpleTestCase& _) = default;
    FORCEINLINE LSimpleTestCase(LSimpleTestCase&& _)      = default;
    FORCEINLINE virtual ~LSimpleTestCase()                = default;

    TESTER_API auto GetName() const -> const std::string&;
    TESTER_API auto GetCategory() const -> const std::string&;
    TESTER_API auto GetFullName() const -> std::string;
    TESTER_API auto GetFilename() const -> const std::string&;

    FORCEINLINE auto HasErrors() const -> bool { return this->Errors.empty() == false; }
    FORCEINLINE auto GetErrors() const -> const std::vector<std::string>& { return this->Errors; }

    virtual void Run() = 0;

    TESTER_API void TestEqual(const LChar* What, const int32   A, const int32   B, const int32 LineNumber);
    TESTER_API void TestEqual(const LChar* What, const int64   A, const int64   B, const int32 LineNumber);
    TESTER_API void TestEqual(const LChar* What, const float   A, const float   B, const int32 LineNumber, const float  Tolerance = JAFG_FLOAT_NOT_SO_SMALL_NUMBER);
    TESTER_API void TestEqual(const LChar* What, const double  A, const double  B, const int32 LineNumber, const double Tolerance = JAFG_DOUBLE_NOT_SO_SMALL_NUMBER);

    template <typename T, typename U>
    FORCEINLINE void TestEqual(const LChar* What, const T& X, const U& Y, const int32 LineNumber)
    {
        this->IncreaseTestCount();

        if (X == Y)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    TESTER_API void   AddError(const LChar* What, const int32 LineNumber = -1);
    TESTER_API void   AddError(const LChar* What, const LChar* ExpectedValue, const int32 LineNumber = -1);
    TESTER_API void   IncreaseTestCount();
    TESTER_API uint32 GetNumberOfRunTests() const;

private:

    std::string Name;
    std::string Category;
    std::string Filename;

    uint32                   NumberOfRunTests = 0;
    std::vector<std::string> Errors           = { };
};

} /* Namespace Jafg::Tester */
