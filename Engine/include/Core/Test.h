// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#if !JAFG_WITH_TESTS
    #error "JAFG_WITH_TESTS is not enabled."
#endif /* !JAFG_WITH_TESTS */

#define DETAIL_JAFG_TEST_MAKE_TRIVIAL(Class, File, Line, Category, Pretty) \
    struct Class final: ::Jafg::LTrivialTestCase                           \
    {                                                                      \
        Class(): LTrivialTestCase{File, Line, Category, Pretty} {}         \
        virtual void Run() override;                                       \
    };                                                                     \
    static Class JAFG_JOIN_INNER_THREE(Class, _, Instance);                \
    void Class::Run()

#define TRIVIAL_TEST_CASE(Category, Class)  \
    DETAIL_JAFG_TEST_MAKE_TRIVIAL(          \
        JAFG_JOIN_OUTER_FIVE(               \
            Class, _,                       \
            __LINE__, _,                    \
            __COUNTER__                     \
            ),                              \
        __FILE__,                           \
        static_cast<std::size_t>(__LINE__), \
        Category,                           \
        #Class                              \
        )

#ifndef TEST_EQUAL
    #define TEST_EQUAL(A, ...) this->TestEqual("EQ: "#A" == "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_EQUAL */
#ifndef TEST_UNEQUAL
    #define TEST_UNEQUAL(A, ...) this->TestUnequal("NEQ: "#A" != "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_UNEQUAL */
#ifndef TEST_LESS
    #define TEST_LESS(A, ...) this->TestLess("LT: "#A" < "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_LESS */
#ifndef TEST_LESS_EQUAL
    #define TEST_LESS_EQUAL(A, ...) this->TestLessEqual("LTEQ: "#A" <= "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_LESS_EQUAL */
#ifndef TEST_GREATER
    #define TEST_GREATER(A, ...) this->TestGreater("GT: "#A" > "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_GREATER */
#ifndef TEST_GREATER_EQUAL
    #define TEST_GREATER_EQUAL(A, ...) this->TestGreaterEqual("GTEQ: "#A" >= "#__VA_ARGS__"", __LINE__, A, __VA_ARGS__);
#endif /* !TEST_GREATER_EQUAL */
#ifndef TEST_NULL
    #define TEST_NULL(...) this->TestNull("NULL: "#__VA_ARGS__" == nullptr", __LINE__, __VA_ARGS__);
#endif /* !TEST_NULL */
#ifndef TEST_NOT_NULL
    #define TEST_NOT_NULL(...) this->TestNotNull("NOT NULL: "#__VA_ARGS__" != nullptr", __LINE__, __VA_ARGS__);
#endif /* !TEST_NOT_NULL */
#ifndef TEST_TRUE
    #define TEST_TRUE(...) this->TestTrue("TRUE: "#__VA_ARGS__" == true", __LINE__, __VA_ARGS__);
#endif /* !TEST_TRUE */
#ifndef TEST_FALSE
    #define TEST_FALSE(...) this->TestFalse("FALSE: "#__VA_ARGS__" == false", __LINE__, __VA_ARGS__);
#endif /* !TEST_FALSE */

namespace Jafg
{

struct LTestCase
{
};

struct LTrivialTestCase: LTestCase
{
    LTrivialTestCase() noexcept = delete;
    inline LTrivialTestCase(LString File, std::size_t Line, LString Category, LString Name) noexcept;

    virtual ~LTrivialTestCase() noexcept = default;

    NODISCARD constexpr LString const& File() const noexcept { return this->m_File; }
    NODISCARD constexpr std::size_t Line() const noexcept { return this->m_Line; }
    NODISCARD constexpr LString const& Category() const noexcept { return this->m_Category; }
    NODISCARD constexpr LString const& Name() const noexcept { return this->m_Name; }
    NODISCARD constexpr LString FullName() const noexcept { return algo::sprintf("{}::{}", this->Category(), this->Name()); }

    NODISCARD constexpr std::size_t Total() const noexcept { return this->m_Total; }
    NODISCARD constexpr std::size_t Passed() const noexcept { return this->m_Total - this->m_Errors.size(); }
    NODISCARD constexpr bool Errors() const noexcept { return !this->m_Errors.empty(); }
    NODISCARD constexpr auto const& HumanReadableErrors() const noexcept { return this->m_Errors; }

    //# Your test logic.
    virtual void Run() noexcept = 0;

    void TestEqual(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_eq_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A == B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }

    void TestUnequal(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_uneq_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A != B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }

    void TestLess(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_eq_lt_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A < B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }
    void TestLessEqual(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_eq_lteq_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A <= B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }

    void TestGreater(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_eq_gt_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A > B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }

    void TestGreaterEqual(LString What, std::size_t Line, auto&& A, auto&& B) requires algo::is_weak_eq_gteq_v<decltype(A), decltype(B)>
    {
        ++this->m_Total;
        if (!(A >= B))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A), std::forward<decltype(B)>(B));
        }
    }

    void TestNull(LString What, std::size_t Line, auto&& A) requires algo::is_weak_eq_v<decltype(A), std::nullptr_t>
    {
        ++this->m_Total;
        if (!(A == nullptr))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A));
        }
    }

    void TestNotNull(LString What, std::size_t Line, auto&& A) requires algo::is_weak_uneq_v<decltype(A), std::nullptr_t>
    {
        ++this->m_Total;
        if (!(A != nullptr))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A));
        }
    }

    void TestTrue(LString What, std::size_t Line, auto&& A) requires algo::is_weak_eq_v<decltype(A), bool>
    {
        ++this->m_Total;
        if (!(A == true))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A));
        }
    }

    void TestFalse(LString What, std::size_t Line, auto&& A) requires algo::is_weak_eq_v<decltype(A), bool>
    {
        ++this->m_Total;
        if (!(A == false))
        {
            this->AppendError(What, Line, std::forward<decltype(A)>(A));
        }
    }

private:

    void AppendError(LString What, std::size_t Line, auto&& A, auto&& B) noexcept
    {
        if constexpr (requires{ algo::sprintf("{}{}", A, B); }
            && std::is_default_constructible_v<std::formatter<decltype(A), char>>
            && std::is_default_constructible_v<std::formatter<decltype(B), char>>
            )
        {
            this->m_Errors.emplace_back(algo::sprintf("FAULT {} [{}::{}] ({} > {})", What, this->File(), Line, A, B));
        }
        else
        {
            this->m_Errors.emplace_back(algo::sprintf("FAULT {} [{}::{}]", What, this->File(), Line));
        }
    }

    void AppendError(LString What, std::size_t Line, auto&& A) noexcept
    {
        if constexpr (requires{ algo::sprintf("{}", A); })
        {
            this->m_Errors.emplace_back(algo::sprintf("FAULT {} [{}::{}] ({})", What, this->File(), Line, A));
        }
        else
        {
            this->m_Errors.emplace_back(algo::sprintf("FAULT {} [{}::{}]", What, this->File(), Line));
        }
    }

    LString m_File;
    std::size_t m_Line{};
    LString m_Category;
    LString m_Name;

    std::size_t m_Total{};
    TArray<LString> m_Errors;
};

namespace Detail
{

ENGINE_API extern TArray<TReference<LTrivialTestCase>> TrivialTestCases;

} /* ~Namespace Detail */

inline LTrivialTestCase::LTrivialTestCase(LString File, std::size_t Line, LString Category, LString Name) noexcept
    : m_File{std::move(File)}, m_Line{Line}, m_Category{std::move(Category)}, m_Name{std::move(Name)}
{
    Detail::TrivialTestCases.emplace_back(*this);
}

enum struct ETestCategoryBits
{
    Identity = 0x0,
    Trivial = 0x1 << 0,
};
ENUM_STRUCT_FLAGS(ETestCategoryBits, ETestCategoryFlags)

struct LTestRunInstance final
{
    friend LTestCase;

    //# Not a steady clock. But who cares. This is just for short running tests.
    typedef std::chrono::high_resolution_clock clock;

    ENGINE_API LTestRunInstance(ETestCategoryFlags Flags);
    ~LTestRunInstance() noexcept = default;

    NODISCARD std::optional<LString> ExitReason() const noexcept
    {
        if (this->Errors())
        {
            return algo::sprintf("Test Framework failed ({} tests, {} passed) in {}ms."
                , this->Total(), this->Passed(), std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - this->Start).count());
        }
        return {};
    }

    NODISCARD constexpr std::size_t Total() const noexcept { return this->m_Total; }
    NODISCARD constexpr std::size_t Passed() const noexcept { return this->m_Passed; }
    NODISCARD constexpr bool Errors() const { return this->Total() != this->Passed(); }

private:

    clock::time_point Start{clock::now()};
    std::size_t m_Total{};
    std::size_t m_Passed{};
};

} /* ~Namespace Jafg */
