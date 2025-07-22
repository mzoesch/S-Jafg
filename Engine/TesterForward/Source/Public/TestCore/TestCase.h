// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>
#include <vector>
#include <format>
#include <concepts>

#if !WITH_TESTS
    #error "Test header file included, but tests are not active."
#endif /* !WITH_TESTS */

namespace Jafg
{

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

namespace Tester
{

struct LSimpleTestCase;

TESTERFORWARD_API const std::vector<LSimpleTestCase*>& GetRegisteredTestCases();

namespace Private
{

TESTERFORWARD_API std::vector<LSimpleTestCase*>& GetMutableRegisteredTestCases();

} /* ~Namespace Private */

//#
//# Base class for all simple test cases.
//#
struct LSimpleTestCase
{
    constexpr LSimpleTestCase() = delete;
    LSimpleTestCase(std::string_view&& InName, std::string_view&& InCategory, std::string_view&& InFilename)
        : Name(std::move(InName)), Category(std::move(InCategory)), Filename(std::move(InFilename))
    {
        Private::GetMutableRegisteredTestCases().emplace_back(this);
        return;
    }

    virtual ~LSimpleTestCase() = default;

    constexpr const std::string_view& GetName() const noexcept { return this->Name; }
    constexpr const std::string_view& GetCategory() const noexcept { return this->Category; }
    constexpr const std::string_view& GetFilename() const noexcept { return this->Filename; }
    constexpr std::string GetFullName() const noexcept
    {
        return std::format(
            "{}.{}",
            this->Category,
            this->Name
            );
    }

    void   IncreaseTestCount() noexcept { ++this->TestsRun; return; }
    void   IncreaseTestCountBy(const size_t InCount) noexcept { this->TestsRun += InCount; return; }
    size_t GetNumberOfTestsRun() const noexcept { return this->TestsRun; }

    void AddError(const std::string_view& What, const size_t LineNumber = -1)
    {
        this->Errors.emplace_back(std::format(
            "FAULT {}. [Inside file {} at line {}.]",
            What,
            this->GetFilename(), LineNumber
            ));

        return;
    }

    void AddError(const std::string_view& What, const std::string_view& ExpectedValue, const size_t LineNumber = -1)
    {
        this->Errors.emplace_back(std::format(
            "FAULT {}. Expected: {{ {} }}. [Inside file {} at line {}.]",
            What,
            ExpectedValue,
            this->GetFilename(), LineNumber
            ));

        return;
    }

    template <typename T>
    void AddError(const std::string_view& What, const T& ExpectedValue, const size_t LineNumber = -1)
    {
        this->Errors.emplace_back(std::format(
            "FAULT {}. Expected: {{ {} }}. [Inside file {} at line {}.]",
            What,
            ExpectedValue,
            this->GetFilename(), LineNumber
            ));

        return;
    }

    void AddError(const std::string_view& What, const std::string_view& ExpectedValue, const std::string_view& RealValue, const size_t LineNumber = -1)
    {
        this->Errors.emplace_back(std::format(
            "FAULT {}. Expected: {{ {} }}. Got: {{ {} }}. [Inside file {} at line {}.]",
            What,
            ExpectedValue, RealValue,
            this->GetFilename(), LineNumber
            ));

        return;
    }

    template <typename T, typename U>
    void AddError(const std::string_view& What, const T& ExpectedValue, const U& RealValue, const size_t LineNumber = -1)
    {
        this->Errors.emplace_back(std::format(
            "FAULT {}. Expected: {{ {} }}. Got: {{ {} }}. [Inside file {} at line {}.]",
            What,
            ExpectedValue, RealValue,
            this->GetFilename(), LineNumber
            ));

        return;
    }

    constexpr bool HasErrors() const noexcept { return this->Errors.empty() == false; }
    constexpr const std::vector<std::string>& GetErrors() const noexcept { return this->Errors; }

    //#
    //# Override this method to run all tests of this test case.
    //#
    virtual void Run() = 0;

    template <std::integral T>
    constexpr void TestEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A == B)
        {
            return;
        }

        this->AddError(What, A, B, LineNumber);

        return;
    }

    template <std::floating_point T>
    constexpr void TestEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber, const T Tolerance = static_cast<T>(1.e-8))
    {
        this->IncreaseTestCount();

        const T Diff { A - B };
        const T Abs { (Diff < static_cast<T>(0)) ? -Diff : Diff };

        if (Abs < Tolerance)
        {
            return;
        }

        this->AddError(What, A, B, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestEqual(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A == B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <std::integral T>
    constexpr void TestNotEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A != B)
        {
            return;
        }

        this->AddError(What, A, B, LineNumber);

        return;
    }

    template <std::floating_point T>
    constexpr void TestNotEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber, const T Tolerance = static_cast<T>(1.e-8))
    {
        this->IncreaseTestCount();

        const T Diff { A - B };
        const T Abs { (Diff < static_cast<T>(0)) ? -Diff : Diff };

        if ((Abs < Tolerance) == false)
        {
            return;
        }

        this->AddError(What, A, B, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestNotEqual(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A != B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <Numeric T>
    constexpr void TestLess(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A < B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestLess(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A < B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <Numeric T>
    constexpr void TestGreater(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A > B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestGreater(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A > B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <Numeric T>
    constexpr void TestLessOrEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A <= B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestLessOrEqual(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A <= B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <Numeric T>
    constexpr void TestGreaterOrEqual(const std::string_view& What, const T A, const T B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A >= B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    template <typename T, typename U> requires (
           ((!std::is_integral_v<T>) || std::is_same_v<T, char>)
        && ((!std::is_integral_v<U>) || std::is_same_v<U, char>)
        && !std::is_floating_point_v<T>
        && !std::is_floating_point_v<U>
        )
    constexpr void TestGreaterOrEqual(const std::string_view& What, const T& A, const U& B, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A >= B)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    constexpr void TestNull(const std::string_view& What, const void* A, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A == nullptr)
        {
            return;
        }

        this->AddError(What, "nullptr", LineNumber);

        return;
    }

    constexpr void TestNotNull(const std::string_view& What, const void* A, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A != nullptr)
        {
            return;
        }

        this->AddError(What, "not nullptr", LineNumber);

        return;
    }

    constexpr void TestTrue(const std::string_view& What, const bool A, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

    constexpr void TestFalse(const std::string_view& What, const bool A, const size_t LineNumber)
    {
        this->IncreaseTestCount();

        if (A == false)
        {
            return;
        }

        this->AddError(What, LineNumber);

        return;
    }

private:

    std::string_view Name;
    std::string_view Category;
    std::string_view Filename;

    size_t TestsRun { 0 };
    std::vector<std::string> Errors;
};

} /* ~Namespace Tester */

} /* ~Namespace Jafg */
