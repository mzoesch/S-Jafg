// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SimpleDelegateOperations, "Lal.Delegates")
{
    MAKE_DELEGATE_SIGNATURE(RetValSig, bool, const int32 A)

    RetValSig RetValDel;
    CHECK_FALSE( "Delegate declaration.", RetValDel.IsBound() )

    RetValDel.Bind( [] (const int32 A) -> bool { return A == 0; });
    CHECK_TRUE(  "Delegate binding.",               RetValDel.IsBound() )
    CHECK_TRUE(  "Delegate execution.",            RetValDel.Execute(0) )
    CHECK_FALSE( "Delegate execution.",            RetValDel.Execute(1) )

    DECLARE_DELEGATE(VoidValSig, VoidValDel, void, const int32 A)
    // This must not compile.
    /* RetValDel.ExecuteIfBound(0); */
    // This is okay;
    /* VoidValDel.ExecuteIfBound(0); */
    CHECK_FALSE( "Delegate declaration.", VoidValDel.IsBound() )

    int32 MyInt = 0;
    VoidValDel.Bind( [&MyInt] (const int32 A) -> void { MyInt = A; });
    CHECK_TRUE(  "Delegate binding.",                       VoidValDel.IsBound() )
    CHECK_FALSE( "Delegate execution.",                               MyInt == 1 )

    VoidValDel.Execute(1);
    CHECK_TRUE(  "Delegate execution.",                               MyInt == 1 )

    VoidValDel.Execute(2);
    CHECK_TRUE(  "Delegate execution.",                               MyInt == 2 )
    CHECK_TRUE(  "Conditional delegate execution.", VoidValDel.ExecuteIfBound(3) )
    CHECK_TRUE(  "Conditional delegate execution.",                   MyInt == 3 )

    VoidValDel.Unbind();
    CHECK_FALSE( "Delegate unbinding.",                     VoidValDel.IsBound() )
    CHECK_FALSE( "Delegate execution.",                               MyInt == 4 )
    CHECK_FALSE( "Conditional delegate execution.", VoidValDel.ExecuteIfBound(4) )
    CHECK_FALSE( "Conditional delegate execution.",                   MyInt == 4 )
    CHECK_TRUE(  "Conditional delegate execution.",                   MyInt == 3 )

    return;
}

DECLARE_INLINE_DELEGATE(MyInlineDelSig, MyInlineDel, void, const int32 A)

TEST_CASE(InlineDelegateOperations, "Lal.Delegates")
{
    using namespace Jafg;

    CHECK_FALSE(  "Delegate declaration.",           MyInlineDel.IsBound() )
    CHECK_FALSE(  "Delegate declaration.",   MyInlineDel.ExecuteIfBound(0) )

    int32 MyInt = 0;
    MyInlineDel.Bind( [&MyInt] (const int32 A) -> void { MyInt = A; return; });
    CHECK_TRUE(   "Inline delegate execution.",         MyInlineDel.IsBound() )
    CHECK_TRUE(   "Inline delegate execution.",                    MyInt == 0 )
    CHECK_FALSE(  "Inline delegate execution.",                    MyInt == 1 )
    CHECK_TRUE(   "Inline delegate execution.", MyInlineDel.ExecuteIfBound(1) )
    CHECK_TRUE(   "Inline delegate execution.",                    MyInt == 1 )

    return;
}

TEST_CASE(SimpleMulticastDelegateOperations, "Lal.Delegates")
{
    using namespace Jafg;

    int32 MyInt = 0;
    DECLARE_MULTICAST_DELEGATE(MyMulticastDelSig, MyMulticastDel, const int32 A)
    CHECK_FALSE( "Delegate declaration.",       MyMulticastDel.HasAny() )

    MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    CHECK_TRUE(   "Delegate binding.",         MyMulticastDel.HasAny() )
    CHECK_EQUALS( "Delegate binding.",                        MyInt, 0 )

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 1 )
    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 2 )

    MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    CHECK_TRUE(   "Delegate binding.",         MyMulticastDel.HasAny() )
    CHECK_EQUALS( "Delegate binding.",                        MyInt, 2 )

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 4 )
    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 6 )

    MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    CHECK_TRUE(   "Delegate binding.",         MyMulticastDel.HasAny() )
    CHECK_EQUALS( "Delegate binding.",                        MyInt, 6 )

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 9 )

    MyMulticastDel.UnbindAll();
    CHECK_FALSE( "Delegate unbinding.",       MyMulticastDel.HasAny() )
    CHECK_EQUALS( "Delegate unbinding.",                     MyInt, 9 )

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 9 )

    return;
}

TEST_CASE(HandleMulticastDelegateOperations, "Lal.Delegates")
{
    using namespace Jafg;

    int32 MyInt = 0;
    DECLARE_MULTICAST_DELEGATE(MyMulticastDelSig, MyMulticastDel, const int32 A)
    CHECK_FALSE( "Delegate declaration.",       MyMulticastDel.HasAny() )

    LDelegateHandle Handle = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    CHECK_TRUE(   "Delegate binding.",         MyMulticastDel.HasAny() )
    CHECK_EQUALS( "Delegate binding.",                        MyInt, 0 )

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 1 )
    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 2 )

    CHECK_TRUE(   "Delegate removal.",        MyMulticastDel.IsStillBound(Handle) )
    CHECK_TRUE(   "Delegate removal.",              MyMulticastDel.Remove(Handle) )
    CHECK_FALSE(  "Delegate removal.",        MyMulticastDel.IsStillBound(Handle) )
    CHECK_FALSE(  "Delegate removal.",              MyMulticastDel.Remove(Handle) )
    CHECK_FALSE(  "Delegate removal.",                    MyMulticastDel.HasAny() )
    CHECK_TRUE(   "Delegate removal.",                                 MyInt == 2 )
    CHECK_FALSE(  "Delegate removal.",                           Handle.IsValid() )
#if IN_DEBUG
    CHECK_NULL(   "Delegate removal.",                         Handle.GetHandle() )
#endif /* IN_DEBUG */

    /* Enforcing dynamic array to grow. */
    LDelegateHandle H01 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H02 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H03 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H04 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H05 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H06 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H07 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H08 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H09 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H10 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
    LDelegateHandle H11 = MyMulticastDel.Add( [&MyInt] (const int32 A) -> void { MyInt += A; return; });
#if IN_DEBUG
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H01.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H02.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H03.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H04.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H05.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H06.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H07.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H08.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H09.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H10.GetHandle() )
    CHECK_NOT_NULL( "Delegate buffer grow.",                  H11.GetHandle() )
#endif /* IN_DEBUG */

    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 13 )
    MyMulticastDel.Broadcast(1);
    CHECK_EQUALS( "After delegate broadcast.",     MyInt, 24 )

    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H01) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H02) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H03) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H04) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H05) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H06) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H07) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H08) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H09) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H10) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H11) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H01) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H02) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H03) )
#if IN_DEBUG
    CHECK_NULL(  "Delegate removal.",                               H01.GetHandle() )
    CHECK_NULL(  "Delegate removal.",                               H02.GetHandle() )
    CHECK_NULL(  "Delegate removal.",                               H03.GetHandle() )
#endif /* IN_DEBUG */
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H01) )
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H02) )
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H03) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H04) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H05) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H06) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H07) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H08) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H09) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H10) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H11) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H07) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H08) )
    CHECK_TRUE(  "Delegate removal.",                    MyMulticastDel.Remove(H09) )
#if IN_DEBUG
    CHECK_NOT_NULL( "Delegate removal.",                            H04.GetHandle() )
    CHECK_NOT_NULL( "Delegate removal.",                            H05.GetHandle() )
    CHECK_NOT_NULL( "Delegate removal.",                            H06.GetHandle() )
    CHECK_NULL(     "Delegate removal.",                            H07.GetHandle() )
    CHECK_NULL(     "Delegate removal.",                            H08.GetHandle() )
    CHECK_NULL(     "Delegate removal.",                            H09.GetHandle() )
#endif /* IN_DEBUG */
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H04) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H05) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H06) )
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H07) )
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H08) )
    CHECK_FALSE( "Delegate removal.",              MyMulticastDel.IsStillBound(H09) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H10) )
    CHECK_TRUE(  "Delegate removal.",              MyMulticastDel.IsStillBound(H11) )
    CHECK_EQUALS( "Delegate removal.",                MyMulticastDel.UnbindAll(), 5 )
    CHECK_FALSE(  "Delegate removal.",                      MyMulticastDel.HasAny() )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H01) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H02) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H03) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H04) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H05) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H06) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H07) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H08) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H09) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H10) )
    CHECK_FALSE(  "Delegate removal.",             MyMulticastDel.IsStillBound(H11) )

    return;
}

namespace Jafg::Testing::Delegates
{

class MyDelegateClass final
{
public:

    int32 Member = 0;

    bool MyNamedMemberConst(const int32 A)    const { return A == this->Member; }
    bool MyNamedMemberNonConst(const int32 A, const int32 B, const int32 C)
    {
        this->Member += A;
        this->Member += B;
        this->Member += C;

        return true;
    }
};

inline bool MyNamedFunction(const int32 A) { return A == 0; }

} /* ~Namespace Jafg::Testing::Delegates */

TEST_CASE(NamedDelegateOperations, "Lal.Delegates")
{
    using namespace Jafg;
    using namespace Jafg::Testing::Delegates;

    DECLARE_DELEGATE(MyNamedDelSig, MyNamedDel, bool, const int32 A)
    CHECK_FALSE( "Delegate declaration.", MyNamedDel.IsBound() )

    MyNamedDel.Bind( &MyNamedFunction );
    CHECK_TRUE(  "Delegate binding.",               MyNamedDel.IsBound() )
    CHECK_TRUE(  "Delegate execution.",            MyNamedDel.Execute(0) )
    CHECK_FALSE( "Delegate execution.",            MyNamedDel.Execute(1) )

    MyNamedDel.Unbind();
    CHECK_FALSE( "Delegate unbinding.",             MyNamedDel.IsBound() )

    MyDelegateClass MyDelegateObject;
    MyDelegateObject.Member = 10;
    MyNamedDel.BindMember(&MyDelegateClass::MyNamedMemberConst, &MyDelegateObject);
    CHECK_TRUE(  "Delegate member execution.",             MyNamedDel.IsBound() )
    CHECK_TRUE(  "Delegate member execution.",           MyNamedDel.Execute(10) )
    CHECK_FALSE( "Delegate member execution.",           MyNamedDel.Execute(11) )

    DECLARE_DELEGATE(MyTripleDelSig, MyTripleDel, bool, const int32 A, const int32 B, const int32 C)
    CHECK_FALSE( "Delegate declaration.", MyTripleDel.IsBound() )

    MyTripleDel.BindMember(&MyDelegateClass::MyNamedMemberNonConst, &MyDelegateObject);
    CHECK_TRUE(   "Delegate member execution.",                  MyTripleDel.IsBound() )
    CHECK_EQUALS( "Delegate member execution.",            MyDelegateObject.Member, 10 )
    CHECK_TRUE(   "Delegate member execution.",           MyTripleDel.Execute(1, 2, 3) )
    CHECK_EQUALS( "Delegate member execution.",            MyDelegateObject.Member, 16 )
    CHECK_TRUE(   "Delegate member execution.",           MyTripleDel.Execute(1, 2, 3) )
    CHECK_EQUALS( "Delegate member execution.",            MyDelegateObject.Member, 22 )

    return;
}

TEST_CASE(NamedMulticastDelegateOperations, "Lal.Delegates")
{
    using namespace Jafg;
    using namespace Jafg::Testing::Delegates;

    DECLARE_MULTICAST_DELEGATE(MyNamedMulticastDelSig, MyNamedMulticastDel, const int32 A)
    CHECK_FALSE( "Delegate declaration.",       MyNamedMulticastDel.HasAny() )

    MyNamedMulticastDel.Add( &MyNamedFunction );
    CHECK_TRUE(   "Delegate binding.",         MyNamedMulticastDel.HasAny() )

    MyNamedMulticastDel.Broadcast(0);
    CHECK_TRUE(   "After delegate broadcast.", MyNamedMulticastDel.HasAny() )

    MyNamedMulticastDel.UnbindAll();
    CHECK_FALSE( "Delegate unbinding.",        MyNamedMulticastDel.HasAny() )

    DECLARE_MULTICAST_DELEGATE(MyNamedTripleMulticastDelSig, MyNamedTripleMulticastDel, const int32 A, const int32 B, const int32 C)
    CHECK_FALSE( "Delegate declaration.",       MyNamedTripleMulticastDel.HasAny() )

    MyDelegateClass MyDelegateObject;
    LDelegateHandle H1 = MyNamedTripleMulticastDel.AddMember(&MyDelegateClass::MyNamedMemberNonConst, &MyDelegateObject);
    CHECK_TRUE(   "Delegate binding.",         MyNamedTripleMulticastDel.HasAny() )

    LDelegateHandle H2 = MyNamedTripleMulticastDel.AddMember(&MyDelegateClass::MyNamedMemberNonConst, &MyDelegateObject);
    LDelegateHandle H3 = MyNamedTripleMulticastDel.AddMember(&MyDelegateClass::MyNamedMemberNonConst, &MyDelegateObject);
#if IN_DEBUG
    CHECK_NOT_NULL( "Delegate binding.",                  H1.GetHandle() )
    CHECK_NOT_NULL( "Delegate binding.",                  H2.GetHandle() )
    CHECK_NOT_NULL( "Delegate binding.",                  H3.GetHandle() )
#endif /* IN_DEBUG */
    CHECK_TRUE(     "Delegate binding.",                    H1.IsValid() )
    CHECK_TRUE(     "Delegate binding.",                    H2.IsValid() )
    CHECK_TRUE(     "Delegate binding.",                    H3.IsValid() )

    MyNamedTripleMulticastDel.Broadcast(1, 2, 3);
    CHECK_EQUALS( "After delegate broadcast.",       MyDelegateObject.Member, 18 )
    MyNamedTripleMulticastDel.Broadcast(1, 2, 3);
    CHECK_EQUALS( "After delegate broadcast.",       MyDelegateObject.Member, 36 )

    CHECK_TRUE(  "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H1) )
    CHECK_TRUE(  "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H2) )
    CHECK_TRUE(  "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H3) )
    CHECK_TRUE(  "Delegate removal.",                    MyNamedTripleMulticastDel.Remove(H2) )
    CHECK_TRUE(  "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H1) )
    CHECK_FALSE( "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H2) )
    CHECK_TRUE(  "Delegate removal.",              MyNamedTripleMulticastDel.IsStillBound(H3) )
    CHECK_FALSE( "Delegate removal.",                                            H2.IsValid() )

    MyNamedTripleMulticastDel.Broadcast(1, 2, 3);
    CHECK_EQUALS( "After delegate broadcast.",       MyDelegateObject.Member, 48 )

    MyNamedTripleMulticastDel.UnbindAll();
    CHECK_FALSE( "Delegate unbinding.",       MyNamedTripleMulticastDel.HasAny() )

    MyNamedTripleMulticastDel.Broadcast(1, 2, 3);
    CHECK_EQUALS( "After delegate broadcast.",       MyDelegateObject.Member, 48 )

    return;
}
