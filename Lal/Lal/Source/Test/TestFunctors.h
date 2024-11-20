// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleFunctorOperations, "Lal.Core")
{
    using namespace Jafg;

    TFunction<bool(const int32 A)> Lambda;
    CHECK_TRUE(   "Lambda is null.",         Lambda == nullptr )
    CHECK_FALSE(  "Lambda is not null.",        Lambda.IsSet() )

    Lambda = [] (const int32 A) -> bool { return A == 0; };
    CHECK_FALSE( "Lambda is set.",         Lambda == nullptr )
    CHECK_TRUE(  "Lambda is set.",            Lambda.IsSet() )

    Lambda.CheckForValidCall();
    CHECK_TRUE(  "Lambda is valid.",          Lambda(0) )
    CHECK_TRUE(  "Lambda is valid.",     Lambda.Call(0) )
    CHECK_FALSE( "Lambda is valid.",          Lambda(1) )
    CHECK_FALSE( "Lambda is valid.",     Lambda.Call(1) )

    Lambda.Reset();
    CHECK_TRUE(  "Lambda reset.",     Lambda == nullptr )
    CHECK_FALSE( "Lambda reset.",        Lambda.IsSet() )

    {
        Lambda = [] (const int32 A) -> bool { return A == 1; };
    }
    CHECK_FALSE( "Scoped lambda.",     Lambda == nullptr )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.IsSet() )
    CHECK_FALSE( "Scoped lambda.",             Lambda(0) )
    CHECK_FALSE( "Scoped lambda.",        Lambda.Call(0) )
    CHECK_TRUE(  "Scoped lambda.",             Lambda(1) )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.Call(1) )

    Lambda = nullptr;
    CHECK_TRUE(   "Lambda set nullptr.",     Lambda == nullptr )
    CHECK_FALSE(  "Lambda set nullptr.",        Lambda.IsSet() )

    Lambda = [] (const int32 A) -> bool { return A == 2; };

    TFunction<bool(const int32 A)> OtherLambda = nullptr;
    CHECK_TRUE(   "Lambda declared with null.", OtherLambda == nullptr )
    CHECK_FALSE(  "Lambda declared with null.",    OtherLambda.IsSet() )

    OtherLambda = Lambda;
    CHECK_FALSE( "Lambda assigned.",       Lambda == nullptr )
    CHECK_TRUE(  "Lambda assigned.",          Lambda.IsSet() )
    CHECK_FALSE( "Lambda assigned.",    OtherLambda == nullptr )
    CHECK_TRUE(  "Lambda assigned.",       OtherLambda.IsSet() )

    OtherLambda.Reset();
    CHECK_TRUE(  "Lambda reset.",     OtherLambda == nullptr )
    CHECK_FALSE( "Lambda reset.",        OtherLambda.IsSet() )

    OtherLambda = std::move(Lambda);
    CHECK_TRUE(   "Lambda moved.",      Lambda == nullptr )
    CHECK_FALSE(  "Lambda moved.",      Lambda != nullptr )
    CHECK_FALSE(  "Lambda moved.",         Lambda.IsSet() )
    CHECK_FALSE(  "Lambda moved.", OtherLambda == nullptr )
    CHECK_TRUE(   "Lambda moved.", OtherLambda != nullptr )
    CHECK_TRUE(   "Lambda moved.",    OtherLambda.IsSet() )

    return;
}

TEST_CASE(VoidFunctorOperations, "Lal.Core")
{
    using namespace Jafg;

    TFunction<void()> Lambda;
    CHECK_TRUE(   "Lambda is null.",         Lambda == nullptr )
    CHECK_FALSE(  "Lambda is not null.",        Lambda.IsSet() )

    Lambda = [] () -> void { return; };
    CHECK_FALSE( "Lambda is set.",         Lambda == nullptr )
    CHECK_TRUE(  "Lambda is set.",            Lambda.IsSet() )

    TFunction<void()> OtherLambda = nullptr;
    CHECK_TRUE(   "Lambda declared with null.", OtherLambda == nullptr )

    OtherLambda = std::move(Lambda);
    CHECK_TRUE(   "Lambda moved.",      Lambda == nullptr )
    CHECK_FALSE(  "Lambda moved.",      Lambda != nullptr )
    CHECK_FALSE(  "Lambda moved.",         Lambda.IsSet() )
    CHECK_FALSE(  "Lambda moved.", OtherLambda == nullptr )
    CHECK_TRUE(   "Lambda moved.", OtherLambda != nullptr )
    CHECK_TRUE(   "Lambda moved.",    OtherLambda.IsSet() )

    return;
}

TEST_CASE(SimpleUniqueFunctorOperations, "Lal.Core")
{
    using namespace Jafg;

    TUniqueFunction<bool(const int32 A)> Lambda;
    CHECK_TRUE(   "Lambda is null.",         Lambda == nullptr )
    CHECK_FALSE(  "Lambda is not null.",        Lambda.IsSet() )

    Lambda = [] (const int32 A) -> bool { return A == 0; };
    CHECK_FALSE( "Lambda is set.",         Lambda == nullptr )
    CHECK_TRUE(  "Lambda is set.",            Lambda.IsSet() )

    Lambda.CheckForValidCall();
    CHECK_TRUE(  "Lambda is valid.",          Lambda(0) )
    CHECK_TRUE(  "Lambda is valid.",     Lambda.Call(0) )
    CHECK_FALSE( "Lambda is valid.",          Lambda(1) )
    CHECK_FALSE( "Lambda is valid.",     Lambda.Call(1) )

    Lambda.Reset();
    CHECK_TRUE(  "Lambda reset.",     Lambda == nullptr )
    CHECK_FALSE( "Lambda reset.",        Lambda.IsSet() )

    {
        Lambda = [] (const int32 A) -> bool { return A == 1; };
    }
    CHECK_FALSE( "Scoped lambda.",     Lambda == nullptr )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.IsSet() )
    CHECK_FALSE( "Scoped lambda.",             Lambda(0) )
    CHECK_FALSE( "Scoped lambda.",        Lambda.Call(0) )
    CHECK_TRUE(  "Scoped lambda.",             Lambda(1) )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.Call(1) )

    TUniqueFunction<bool(const int32 A)> OtherLambda = nullptr;
    CHECK_TRUE(   "Lambda declared with null.", OtherLambda == nullptr )
    CHECK_FALSE(  "Lambda declared with null.",    OtherLambda.IsSet() )

    // This must not compile.
    /* OtherLambda = Lambda; */
    // This is okay.
    OtherLambda = std::move(Lambda);
    CHECK_TRUE(   "Lambda moved.",      Lambda == nullptr )
    CHECK_FALSE(  "Lambda moved.",      Lambda != nullptr )
    CHECK_FALSE(  "Lambda moved.",         Lambda.IsSet() )
    CHECK_FALSE(  "Lambda moved.", OtherLambda == nullptr )
    CHECK_TRUE(   "Lambda moved.", OtherLambda != nullptr )
    CHECK_TRUE(   "Lambda moved.",    OtherLambda.IsSet() )

    std::function<void()> A = [] () -> void { return; };

    return;
}

namespace Jafg::Testing::Functors
{

class MyFunctorClass final
{
public:

    int32 Member = 0;

    bool MyNamedMemberConst(const int32 A)    const { return A == this->Member; }
    bool MyNamedMemberNonConst(const int32 A, const int32 B, const int32 C)
    {
        Member += A;
        Member += B;
        Member += C;

        return true;
    }
};

inline bool MyNamedFunction(const int32 A) { return A == 0; }

} /* ~Namespace Jafg::Testing::Functors */

TEST_CASE(NamedFunctorOperations, "Lal.Core")
{
    using namespace Jafg;
    using namespace Jafg::Testing::Functors;

    MyFunctorClass MyFunctorObject;
    MyFunctorObject.Member = 10;

    TFunction<bool(const int32 A)> NamedFunction = nullptr;
    CHECK_TRUE(  "Function is null.",     NamedFunction == nullptr )
    CHECK_FALSE( "Function is not null.",    NamedFunction.IsSet() )

    NamedFunction = &Jafg::Testing::Functors::MyNamedFunction;
    CHECK_FALSE( "Function is set.",     NamedFunction == nullptr )
    CHECK_TRUE(  "Function is set.",        NamedFunction.IsSet() )
    CHECK_TRUE(  "Function execution.",          NamedFunction(0) )
    CHECK_FALSE( "Function execution.",          NamedFunction(1) )

    NamedFunction.BindMember(&MyFunctorClass::MyNamedMemberConst, &MyFunctorObject);
    CHECK_TRUE(  "Member function is set.",      NamedFunction.IsSet() )
    CHECK_FALSE( "Member function execution.",        NamedFunction(9) )
    CHECK_TRUE(  "Member function execution.",       NamedFunction(10) )
    CHECK_FALSE( "Member function execution.",       NamedFunction(11) )

    TFunction<bool(const int32 A, const int32 B, const int32 C)> NamedTripleFunction = nullptr;
    CHECK_TRUE( "Function is null.",     NamedTripleFunction == nullptr )
    CHECK_FALSE("Function is null.",        NamedTripleFunction.IsSet() )

    NamedTripleFunction.BindMember(&MyFunctorClass::MyNamedMemberNonConst, &MyFunctorObject);
    CHECK_TRUE(  "Member function is set.",           NamedTripleFunction.IsSet() )
    CHECK_TRUE(  "Member function execution.",       NamedTripleFunction(1, 2, 3) )
    CHECK_EQUALS("Member function execution.",         MyFunctorObject.Member, 16 )
    CHECK_TRUE(  "Member function execution.",  NamedTripleFunction.Call(1, 2, 3) )
    CHECK_EQUALS("Member function execution.",         MyFunctorObject.Member, 22 )

    return;
}
