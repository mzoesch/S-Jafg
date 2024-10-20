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

Jafg::Tester::LSimpleTestCase::LSimpleTestCase(std::string InName, std::string InCategory)
{
    this->Name = std::move(InName);
    this->Category = std::forward<std::string>(InCategory);

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

void Jafg::Tester::LSimpleTestCase::TestEqual(const LChar* What, const int32 X, const int32 Y, const int32 LineNumber)
{
    if (X == Y)
    {
        return;
    }

    this->AddError(std::format("Equality test [{}] failed: {} != {} (at Line {}).", What, X, Y, LineNumber).c_str());

    return;
}

void Jafg::Tester::LSimpleTestCase::TestEqual(const LChar* What, const int64 X, const int64 Y, const int32 LineNumber)
{
    if (X == Y)
    {
        return;
    }

    this->AddError(std::format("Equality test [{}] failed: {} != {} (at Line {}).", What, X, Y, LineNumber).c_str());

    return;
}

void Jafg::Tester::LSimpleTestCase::AddError(const LChar* What)
{
    this->Errors.emplace_back(What);
    return;
}
