// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(SpScQueueOperations, "Lal.Containers")
{
    using namespace Jafg;

    TMpmcQueue<i32> Queue;
    CHECK_TRUE(     "Queue with zero size.", Queue.IsEmpty()        )
    // CHECK_NULL(     "Queue with zero size.", Queue.Peek()           )
    CHECK_FALSE(    "Queue with zero size.", Queue.Pop()            )
    CHECK_EQUALS(   "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS(   "Queue with zero size.", Queue.Head, Queue.Tail )
    CHECK_NULL(     "Queue with zero size.", Queue.Head             )

    Queue.Enqueue(5);
    CHECK_FALSE(  "Queue with one item.", Queue.IsEmpty()        )
    CHECK_EQUALS( "Queue with one item.", Queue.GetSizeSlow(),  1 )
    // CHECK_EQUALS( "Queue with one item.", *Queue.Peek(),       5 )
    CHECK_EQUALS( "Queue with one item.", Queue.Head, Queue.Tail )

    i32 A = 0;
    CHECK_TRUE(   "Queue with one item.", Queue.Dequeue(&A)       )
    CHECK_EQUALS( "Queue with zero size.", A,                   5 )
    CHECK_TRUE(   "Queue with zero size.", Queue.IsEmpty()        )
    // CHECK_NULL(   "Queue with zero size.", Queue.Peek()           )
    CHECK_FALSE(  "Queue with zero size.", Queue.Pop()            )
    CHECK_EQUALS( "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS( "Queue with zero size.", Queue.Head, Queue.Tail )

    Queue.Enqueue(1); Queue.Enqueue(2); Queue.Enqueue(3); Queue.Enqueue(4); Queue.Enqueue(5);
    CHECK_FALSE(     "Queue with five items.", Queue.IsEmpty()        )
    CHECK_EQUALS(    "Queue with five items.", Queue.GetSizeSlow(),  5 )
    // CHECK_EQUALS(    "Queue with five items.", *Queue.Peek(),       1 )
    CHECK_NOT_EQUALS("Queue with five items.", Queue.Head, Queue.Tail )
    CHECK_TRUE(      "Queue with five items.", Queue.Dequeue(&A)      )
    CHECK_EQUALS(    "Queue with four items.", A,                   1 )
    CHECK_EQUALS(    "Queue with four items.", Queue.GetSizeSlow(),  4 )
    // CHECK_EQUALS(    "Queue with four items.", *Queue.Peek(),       2 )
    CHECK_NOT_EQUALS("Queue with four items.", Queue.Head, Queue.Tail )

    Queue.Empty();
    CHECK_EQUALS( "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS( "Queue with zero size.", Queue.Head, Queue.Tail )
    CHECK_NULL(   "Queue with zero size.", Queue.Head             )
    // CHECK_NULL(   "Queue with zero size.", Queue.Peek()           )

    return;
}

TEST_CASE(SimpleQueueOperations, "Lal.Containers")
{
    using namespace Jafg;

    TQueue<i32> Queue;
    CHECK_TRUE(     "Queue with zero size.", Queue.IsEmpty()        )
    // CHECK_NULL(     "Queue with zero size.", Queue.Peek()           )
    CHECK_FALSE(    "Queue with zero size.", Queue.Pop()            )
    CHECK_EQUALS(   "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS(   "Queue with zero size.", Queue.Head, Queue.Tail )
    CHECK_NULL(     "Queue with zero size.", Queue.Head             )

    Queue.Enqueue(5);
    CHECK_FALSE(    "Queue with one item.", Queue.IsEmpty()        )
    CHECK_EQUALS(   "Queue with one item.", Queue.GetSizeSlow(),  1 )
    // CHECK_EQUALS(   "Queue with one item.", *Queue.Peek(),       5 )
    CHECK_EQUALS(   "Queue with one item.", Queue.Head, Queue.Tail )

    i32 A = 0;
    CHECK_TRUE(     "Queue with one item.", Queue.Dequeue(&A)       )
    CHECK_EQUALS(   "Queue with zero size.", A,                   5 )
    CHECK_TRUE(     "Queue with zero size.", Queue.IsEmpty()        )
    // CHECK_NULL(     "Queue with zero size.", Queue.Peek()           )
    CHECK_FALSE(    "Queue with zero size.", Queue.Pop()            )
    CHECK_EQUALS(   "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS(   "Queue with zero size.", Queue.Head, Queue.Tail )
    CHECK_NULL(     "Queue with zero size.", Queue.Head             )

    Queue.Enqueue(1); Queue.Enqueue(2); Queue.Enqueue(3); Queue.Enqueue(4); Queue.Enqueue(5);
    CHECK_FALSE(      "Queue with five items.", Queue.IsEmpty()        )
    CHECK_EQUALS(     "Queue with five items.", Queue.GetSizeSlow(),  5 )
    // CHECK_EQUALS(     "Queue with five items.", *Queue.Peek(),       1 )
    CHECK_NOT_EQUALS( "Queue with five items.", Queue.Head, Queue.Tail )
    CHECK_NOT_NULL(   "Queue with five items.", Queue.Head             )
    CHECK_TRUE(       "Queue with five items.", Queue.Dequeue(&A)      )
    CHECK_EQUALS(     "Queue with four items.", A,                   1 )
    CHECK_EQUALS(     "Queue with four items.", Queue.GetSizeSlow(),  4 )
    // CHECK_EQUALS(     "Queue with four items.", *Queue.Peek(),       2 )
    CHECK_NOT_EQUALS( "Queue with four items.", Queue.Head, Queue.Tail )

    Queue.Empty();
    CHECK_EQUALS(     "Queue with zero size.", Queue.GetSizeSlow(),  0 )
    CHECK_EQUALS(     "Queue with zero size.", Queue.Head, Queue.Tail )
    CHECK_NULL(       "Queue with zero size.", Queue.Head             )
    // CHECK_NULL(       "Queue with zero size.", Queue.Peek()           )

    return;
}
