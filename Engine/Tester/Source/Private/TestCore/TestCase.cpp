// Copyright mzoesch. All rights reserved.

#include "TestCore/TestCase.h"
#include "TestCore/RunTests.h"

namespace Jafg::Tester
{

std::vector<::Jafg::Tester::LSimpleTestCase*>& GetAllSimpleTestCasesDuringStaticInitialization( void )
{
    static std::vector<::Jafg::Tester::LSimpleTestCase*> AllSimpleTestCases = { };
    return AllSimpleTestCases;
}

} /* Namespace Jafg::Tester */

Jafg::Tester::LSimpleTestCase::LSimpleTestCase(std::string InName, std::string InCategory, std::string InFilename)
{
    this->Name     = std::forward<std::string>(InName);
    this->Category = std::forward<std::string>(InCategory);
    this->Filename = std::forward<std::string>(InFilename);

    Jafg::Tester::GetAllSimpleTestCasesDuringStaticInitialization().push_back(this);

    return;
}

const std::string& Jafg::Tester::LSimpleTestCase::GetName() const
{
    return this->Name;
}

const std::string& Jafg::Tester::LSimpleTestCase::GetCategory() const
{
    return this->Category;
}

std::string Jafg::Tester::LSimpleTestCase::GetFullName() const
{
    return this->Category + "." + this->Name;
}

const std::string& Jafg::Tester::LSimpleTestCase::GetFilename() const
{
    return this->Filename;
}

void Jafg::Tester::LSimpleTestCase::TestEqual(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A == B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestEqual(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A == B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestEqual(
    const char* What,
    const float A,
    const float B,
    const int32 LineNumber,
    const float Tolerance /* = JAFG_FLOAT_NOT_SO_SMALL_NUMBER */
)
{
    this->IncreaseTestCount();

    if (Maths::IsNearlyEqual(A, B, Tolerance))
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestEqual(
    const char* What,
    const double A,
    const double B,
    const int32 LineNumber,
    const double Tolerance /* = JAFG_DOUBLE_NOT_SO_SMALL_NUMBER */
)
{
    this->IncreaseTestCount();

    if (Maths::IsNearlyEqual(A, B, Tolerance))
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNotEqual(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A != B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNotEqual(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A != B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNotEqual(
    const char* What,
    const float A,
    const float B,
    const int32 LineNumber,
    const float Tolerance /* = JAFG_DOUBLE_NOT_SO_SMALL_NUMBER */
)
{
    this->IncreaseTestCount();

    if (Maths::IsNearlyEqual(A, B, Tolerance) == false)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNotEqual(
    const char* What,
    const double A,
    const double B,
    const int32 LineNumber,
    const double Tolerance /* = JAFG_DOUBLE_NOT_SO_SMALL_NUMBER */
)
{
    this->IncreaseTestCount();

    if (Maths::IsNearlyEqual(A, B, Tolerance) == false)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLess(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A < B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLess(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A < B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLess(const char* What, const float A, const float B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A < B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLess(const char* What, const double A, const double B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A < B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreater(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A > B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreater(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A > B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreater(const char* What, const float A, const float B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A > B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreater(const char* What, const double A, const double B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A > B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLessOrEqual(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A <= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLessOrEqual(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A <= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLessOrEqual(const char* What, const float A, const float B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A <= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestLessOrEqual(const char* What, const double A, const double B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A <= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreaterOrEqual(const char* What, const int32 A, const int32 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A >= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreaterOrEqual(const char* What, const int64 A, const int64 B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A >= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreaterOrEqual(const char* What, const float A, const float B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A >= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestGreaterOrEqual(const char* What, const double A, const double B, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A >= B)
    {
        return;
    }

    const std::string EVal = std::format("{}", A);
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNull(const char* What, const void* A, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A == nullptr)
    {
        return;
    }

    const std::string EVal = "nullptr";
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestNotNull(const char* What, const void* A, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A != nullptr)
    {
        return;
    }

    const std::string EVal = "not nullptr";
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestTrue(const char* What, const bool A, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A)
    {
        return;
    }

    const std::string EVal = "true";
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::TestFalse(const char* What, const bool A, const int32 LineNumber)
{
    this->IncreaseTestCount();

    if (A == false)
    {
        return;
    }

    const std::string EVal = "false";
    this->AddError(What, EVal.c_str(), LineNumber);

    return;
}

void Jafg::Tester::LSimpleTestCase::AddError(const char* What, const int32 LineNumber /* = -1 */)
{
    this->Errors.emplace_back(std::format("FAULT {}. [Inside file {} at line {}.]", What, this->GetFilename(), LineNumber).c_str());
    return;
}

void Jafg::Tester::LSimpleTestCase::AddError(const char* What, const char* ExpectedValue, const int32 LineNumber /* = -1 */)
{
    this->Errors.emplace_back(std::format("FAULT {}. Expected: {{ {} }}. [Inside file {} at line {}.]", What, ExpectedValue, this->GetFilename(), LineNumber).c_str());
    return;
}

void Jafg::Tester::LSimpleTestCase::IncreaseTestCount()
{
    ++this->NumberOfRunTests;
}

uint32 Jafg::Tester::LSimpleTestCase::GetNumberOfRunTests() const
{
    return this->NumberOfRunTests;
}
