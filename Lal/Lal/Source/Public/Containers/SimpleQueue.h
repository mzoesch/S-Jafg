// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * A Jafg implementation of a queue.
 * The queue is always thread-safe, unbounded and non-intrusive.
 * @remark This class is compatible with j objects but does currently not allow for Mpsc queues. If you need Mpsc
 *         queues, use TComplexQueue instead. If you need Mpsc queues inside j objects, use TComplexQueue with a
 *         pointer to it that is statically initialized with nullptr and at the objects' begin life allocated to avoid
 *         double orphans at the end of the object's life.
 */
template <typename T, EQueueKind::Type TKind, typename TSizeType>
class TSimpleQueue final
{
public:

    static_assert(TKind == EQueueKind::Spsc, "TSimpleQueue only supports Spsc queues for now.");

    using LSizeType = TSizeType;

    TSimpleQueue() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(TSimpleQueue)
    ~TSimpleQueue();

    /**
     * Removes the content from the tail of the queue.
     * @param OutContent The content of the dequeued item.
     * @return Whether the out content is meaningful.
     * @remark Consumer only.
     */
    FORCEINLINE bool Dequeue(T& OutContent);

    /**
     * Adds new content to the head of the queue.
     * @param InContent The content to add.
     * @remark Producer only.
     */
    FORCEINLINE void Enqueue(const T& InContent);
    FORCEINLINE void Enqueue(T&& InContent);

    /**
     * Removes the item from the tail of the queue.
     * @return Whether the queue was not empty and a node was successfully removed.
     * @remark Consumer only.
     */
    FORCEINLINE bool Pop();

    /**
     * Discards all items in the queue.
     * @remark Consumer only.
     */
    FORCEINLINE void Empty();

    /**
     * @return True if the queue was empty, false otherwise.
     * @remark Consumer only.
     */
    FORCEINLINE bool IsEmpty() const;

    /**
     * Peeks at the queue's tail item without removing it.
     * @param OutContent The content of the peeked item.
     * @return Whether the out content is meaningful.
     * @remark Consumer only.
     */
    FORCEINLINE bool Peek(T& OutContent) const;

    /**
     * Peeks at the queue's tail item without removing it.
     * @return Pointer to the item, or nullptr if queue is empty.
     * @remark Consumer only.
     */
    FORCEINLINE auto Peek() -> T*;
    FORCEINLINE auto Peek() const -> const T*;

    /**
     * Non-thread-safe num getter. For statistics, dev-checks and debugging purposes only.
     * @remark Consumer only.
     * @remark If you feel the need to check the number of items in the queue apart from the above reasons, you should
     *         really reevaluate if a queue is the right data structure for your use case.
     */
    FORCEINLINE LSizeType UnsafeSize() const;

    /**
     * Non-thread-safe contains check. For statistics, assertions and debugging purposes only.
     * @remark Consumer only.
     * @remark If you feel the need to check the existence of an item inside the queue apart from the above reasons,
     *         you should really reevaluate if a queue is the right data structure for your use case.
     */
    FORCEINLINE bool UnsafeContains(const T& InContent) const;

#if WITH_TESTS
public:
#else /* WITH_TESTS */
private:
#endif /* !WITH_TESTS */

    FORCEINLINE bool PopAlreadyLocked();

    struct TNode final
    {
        TNode* volatile NextNode = nullptr;
        T Content;

        FORCEINLINE TNode() { }
        FORCEINLINE TNode(LNullptrTy) { }
        FORCEINLINE TNode(const T& InContent) : Content(InContent) { }
        FORCEINLINE TNode(T&& InContent) : Content(std::move(InContent)) { }
        FORCEINLINE ~TNode() = default;
    };

    TNode* volatile   Head = nullptr;
    TNode*            Tail = nullptr;
    std::shared_mutex Mutex;
};

template <typename T, EQueueKind::Type TKind, typename TSizeType>
TSimpleQueue<T, TKind, TSizeType>::~TSimpleQueue()
{
    this->Mutex.lock();
    while (this->PopAlreadyLocked()) { }
    check( this->Tail == nullptr )
    check( this->Head == nullptr )

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::Dequeue(T& OutContent)
{
    if (this->Tail == nullptr)
    {
        return false;
    }

    OutContent = this->Tail->Content;
    ensure(this->Pop());

    return true;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
void TSimpleQueue<T, TKind, TSizeType>::Enqueue(const T& InContent)
{
    TNode* NewNode = new TNode(InContent);
    checkSlow( NewNode )

    static_assert(TKind == EQueueKind::Spsc, "Missing enqueue implementation for this queue kind.");

    std::unique_lock Lock(this->Mutex);
    if (this->Head)
    {
        TNode* OldHead = this->Head;
        this->Head = NewNode;
        std::atomic_thread_fence(std::memory_order_acq_rel);
        OldHead->NextNode = NewNode;
        check( this->Tail != nullptr )
    }
    else
    {
        this->Head = NewNode;
        this->Tail = NewNode;
        check( this->Tail->NextNode == nullptr )
    }

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
void TSimpleQueue<T, TKind, TSizeType>::Enqueue(T&& InContent)
{
    TNode* NewNode = new TNode(std::move(InContent));
    checkSlow( NewNode )

    static_assert(TKind == EQueueKind::Spsc, "Missing enqueue implementation for this queue kind.");

    std::unique_lock Lock(this->Mutex);
    if (this->Head)
    {
        TNode* OldHead = this->Head;
        this->Head = NewNode;
        std::atomic_thread_fence(std::memory_order_acq_rel);
        OldHead->NextNode = NewNode;
        check( this->Tail != nullptr )
    }
    else
    {
        this->Head = NewNode;
        this->Tail = NewNode;
        check( this->Tail->NextNode == nullptr )
    }

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::Pop()
{
    if (this->Tail == nullptr)
    {
        return false;
    }

    const TNode* Popped = this->Tail;

    this->Mutex.lock();
    this->Tail = this->Tail->NextNode;
    if (this->Tail == nullptr)
    {
        check( Popped == this->Head )
        this->Head = nullptr;
    }
    this->Mutex.unlock();

    delete Popped;

    return true;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
void TSimpleQueue<T, TKind, TSizeType>::Empty()
{
    while (this->Pop()) { }
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::IsEmpty() const
{
    return this->Tail == nullptr;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::Peek(T& OutContent) const
{
    if (this->Tail)
    {
        OutContent = this->Tail->Content;
        return true;
    }

    return false;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
T* TSimpleQueue<T, TKind, TSizeType>::Peek()
{
    if (this->Tail)
    {
        return &this->Tail->Content;
    }

    return nullptr;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
const T* TSimpleQueue<T, TKind, TSizeType>::Peek() const
{
    return const_cast<TSimpleQueue*>(this)->Peek();
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
typename TSimpleQueue<T, TKind, TSizeType>::LSizeType TSimpleQueue<T, TKind, TSizeType>::UnsafeSize() const
{
    if (this->Tail == nullptr)
    {
        return 0;
    }

    LSizeType Count = 0;

    for (TNode* Node = this->Tail; Node != nullptr; Node = Node->NextNode)
    {
        ++Count;
    }

    return Count;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::UnsafeContains(const T& InContent) const
{
    if (this->Tail == nullptr)
    {
        return false;
    }

    for (TNode* Node = this->Tail; Node != nullptr; Node = Node->NextNode)
    {
        if (Node->Content == InContent)
        {
            return true;
        }
    }

    return false;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TSimpleQueue<T, TKind, TSizeType>::PopAlreadyLocked()
{
    check( this->Mutex.try_lock() == false )
    if (this->Tail == nullptr)
    {
        return false;
    }

    const TNode* Popped = this->Tail;
    this->Tail = this->Tail->NextNode;
    if (this->Tail == nullptr)
    {
        check( Popped == this->Head )
        this->Head = nullptr;
    }

    delete Popped;

    return true;
}

} /* ~Namespace Jafg */
