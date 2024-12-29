// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleFunctorOperations, "Lal.Core")
{
    using namespace Jafg;

    TFunction<bool(const int32 A)> Lambda;
    CHECK_TRUE(   "Lambda is null.",         Lambda == nullptr )
    CHECK_FALSE(  "Lambda is not null.",        Lambda.IsBound() )

    Lambda = [] (const int32 A) -> bool { return A == 0; };
    CHECK_FALSE( "Lambda is set.",         Lambda == nullptr )
    CHECK_TRUE(  "Lambda is set.",            Lambda.IsBound() )

    Lambda.CheckValidCall();
    CHECK_TRUE(  "Lambda is valid.",          Lambda(0) )
    CHECK_TRUE(  "Lambda is valid.",     Lambda.Invoke(0) )
    CHECK_FALSE( "Lambda is valid.",          Lambda(1) )
    CHECK_FALSE( "Lambda is valid.",     Lambda.Invoke(1) )

    Lambda.Reset();
    CHECK_TRUE(  "Lambda reset.",     Lambda == nullptr )
    CHECK_FALSE( "Lambda reset.",        Lambda.IsBound() )

    {
        Lambda = [] (const int32 A) -> bool { return A == 1; };
    }
    CHECK_FALSE( "Scoped lambda.",     Lambda == nullptr )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.IsBound() )
    CHECK_FALSE( "Scoped lambda.",             Lambda(0) )
    CHECK_FALSE( "Scoped lambda.",        Lambda.Invoke(0) )
    CHECK_TRUE(  "Scoped lambda.",             Lambda(1) )
    CHECK_TRUE(  "Scoped lambda.",        Lambda.Invoke(1) )

    Lambda = nullptr;
    CHECK_TRUE(   "Lambda set nullptr.",     Lambda == nullptr )
    CHECK_FALSE(  "Lambda set nullptr.",        Lambda.IsBound() )

    Lambda = [] (const int32 A) -> bool { return A == 2; };

    TFunction<bool(const int32 A)> OtherLambda = nullptr;
    CHECK_TRUE(   "Lambda declared with null.", OtherLambda == nullptr )
    CHECK_FALSE(  "Lambda declared with null.",    OtherLambda.IsBound() )

    OtherLambda = std::move(Lambda);
    CHECK_TRUE(   "Lambda moved.",      Lambda == nullptr )
    CHECK_FALSE(  "Lambda moved.",      Lambda != nullptr )
    CHECK_FALSE(  "Lambda moved.",         Lambda.IsBound() )
    CHECK_FALSE(  "Lambda moved.", OtherLambda == nullptr )
    CHECK_TRUE(   "Lambda moved.", OtherLambda != nullptr )
    CHECK_TRUE(   "Lambda moved.",    OtherLambda.IsBound() )

    return;
}

TEST_CASE(VoidFunctorOperations, "Lal.Core")
{
    using namespace Jafg;

    TFunction<void()> Lambda;
    CHECK_TRUE(   "Lambda is null.",         Lambda == nullptr )
    CHECK_FALSE(  "Lambda is not null.",        Lambda.IsBound() )

    Lambda = [] () -> void { return; };
    CHECK_FALSE( "Lambda is set.",         Lambda == nullptr )
    CHECK_TRUE(  "Lambda is set.",            Lambda.IsBound() )

    TFunction<void()> OtherLambda = nullptr;
    CHECK_TRUE(   "Lambda declared with null.", OtherLambda == nullptr )

    OtherLambda = std::move(Lambda);
    CHECK_TRUE(   "Lambda moved.",      Lambda == nullptr )
    CHECK_FALSE(  "Lambda moved.",      Lambda != nullptr )
    CHECK_FALSE(  "Lambda moved.",         Lambda.IsBound() )
    CHECK_FALSE(  "Lambda moved.", OtherLambda == nullptr )
    CHECK_TRUE(   "Lambda moved.", OtherLambda != nullptr )
    CHECK_TRUE(   "Lambda moved.",    OtherLambda.IsBound() )

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
    CHECK_FALSE( "Function is not null.",    NamedFunction.IsBound() )

    NamedFunction = &Jafg::Testing::Functors::MyNamedFunction;
    CHECK_FALSE( "Function is set.",     NamedFunction == nullptr )
    CHECK_TRUE(  "Function is set.",        NamedFunction.IsBound() )
    CHECK_TRUE(  "Function execution.",          NamedFunction(0) )
    CHECK_FALSE( "Function execution.",          NamedFunction(1) )

    NamedFunction.BindMember(&MyFunctorObject, &MyFunctorClass::MyNamedMemberConst);
    CHECK_TRUE(  "Member function is set.",      NamedFunction.IsBound() )
    CHECK_FALSE( "Member function execution.",        NamedFunction(9) )
    CHECK_TRUE(  "Member function execution.",       NamedFunction(10) )
    CHECK_FALSE( "Member function execution.",       NamedFunction(11) )

    TFunction<bool(const int32 A, const int32 B, const int32 C)> NamedTripleFunction = nullptr;
    CHECK_TRUE( "Function is null.",     NamedTripleFunction == nullptr )
    CHECK_FALSE("Function is null.",        NamedTripleFunction.IsBound() )

    NamedTripleFunction.BindMember(&MyFunctorObject, &MyFunctorClass::MyNamedMemberNonConst);
    CHECK_TRUE(  "Member function is set.",           NamedTripleFunction.IsBound() )
    CHECK_TRUE(  "Member function execution.",       NamedTripleFunction(1, 2, 3) )
    CHECK_EQUALS("Member function execution.",         MyFunctorObject.Member, 16 )
    CHECK_TRUE(  "Member function execution.",  NamedTripleFunction.Invoke(1, 2, 3) )
    CHECK_EQUALS("Member function execution.",         MyFunctorObject.Member, 22 )

    return;
}
