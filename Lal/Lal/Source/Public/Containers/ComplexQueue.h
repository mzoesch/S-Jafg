// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * A Jafg implementation of a queue.
 * The queue is always thread-safe, unbounded and non-intrusive. This class is complex as it requires move and
 * copy semantics to function correctly.
 * @remark This class is not compatible with j objects. If you need to attach queues inside a j object, use
 *         TSimpleQueue instead.
 */
template <typename T, EQueueKind::Type TKind, typename TSizeType>
class TComplexQueue final
{
public:

    static_assert(
        TKind == EQueueKind::Spsc || TKind == EQueueKind::Mpsc,
        "TComplexQueue only supports Spsc and Mpsc queues for now."
    );

    using LSizeType = TSizeType;

    TComplexQueue() { this->Head = new TNode(); this->Tail = this->Head; }
    PROHIBIT_COPY(TComplexQueue)
    DEFAULT_MOVE(TComplexQueue)
    ~TComplexQueue();

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
    FORCEINLINE void Empty() { while (this->Pop()) { } }

    /**
     * @return True if the queue was empty, false otherwise.
     * @remark Consumer only.
     */
    FORCEINLINE bool IsEmpty() const { return this->Tail->NextNode == nullptr; }

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

    struct TNode final
    {
        TNode* volatile NextNode;
        T Content;
        FORCEINLINE TNode() : NextNode(nullptr) { }
        FORCEINLINE TNode(LNullptrTy) : NextNode(nullptr) { }
        FORCEINLINE TNode(const T& InContent) : NextNode(nullptr), Content(InContent) { }
        FORCEINLINE TNode(T&& InContent) : NextNode(nullptr), Content(std::move(InContent)) { }
        ~TNode() = default;
    };

    TNode* volatile Head;
    TNode*          Tail;
};

template <typename T, EQueueKind::Type TKind, typename TSizeType>
TComplexQueue<T, TKind, TSizeType>::~TComplexQueue()
{
    while (this->Tail != nullptr)
    {
        const TNode* Node = this->Tail;
        this->Tail = this->Tail->NextNode;

        delete Node;
    }

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TComplexQueue<T, TKind, TSizeType>::Dequeue(T& OutContent)
{
    TNode* Popped = this->Tail->NextNode;

    if (Popped == nullptr)
    {
        return false;
    }

    OutContent = std::move(Popped->Content);

    const TNode* OldTail = this->Tail;
    this->Tail = Popped;
    this->Tail->Content = T();
    delete OldTail;

    return true;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
void TComplexQueue<T, TKind, TSizeType>::Enqueue(const T& InContent)
{
    TNode* NewNode = new TNode(InContent);
    checkSlow( NewNode )

    if constexpr (TKind == EQueueKind::Spsc)
    {
        TNode* OldHead = this->Head;
        this->Head = NewNode;
        std::atomic_thread_fence(std::memory_order_acq_rel);
        OldHead->NextNode = NewNode;
    }
    else
    {
        unimplemented()
    }

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
void TComplexQueue<T, TKind, TSizeType>::Enqueue(T&& InContent)
{
    TNode* NewNode = new TNode(std::move(InContent));
    checkSlow( NewNode )

    if constexpr (TKind == EQueueKind::Spsc)
    {
        TNode* OldHead = this->Head;
        this->Head = NewNode;
        std::atomic_thread_fence(std::memory_order_acq_rel);
        OldHead->NextNode = NewNode;
    }
    else
    {
        unimplemented()
    }

    return;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TComplexQueue<T, TKind, TSizeType>::Pop()
{
    TNode* Popped = this->Tail->NextNode;

    if (Popped == nullptr)
    {
        return false;
    }

    const TNode* OldTail = this->Tail;
    this->Tail = Popped;
    this->Tail->Content = T();
    delete OldTail;

    return true;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TComplexQueue<T, TKind, TSizeType>::Peek(T& OutContent) const
{
    if (this->Tail->NextNode == nullptr)
    {
        return false;
    }

    OutContent = this->Tail->NextNode->Content;

    return true;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
T* TComplexQueue<T, TKind, TSizeType>::Peek()
{
    if (this->Tail->NextNode == nullptr)
    {
        return nullptr;
    }

    return &this->Tail->NextNode->Content;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
const T* TComplexQueue<T, TKind, TSizeType>::Peek() const
{
    return const_cast<TComplexQueue*>(this)->Peek();
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
typename TComplexQueue<T, TKind, TSizeType>::LSizeType TComplexQueue<T, TKind, TSizeType>::UnsafeSize() const
{
    LSizeType Count = 0;

    for (TNode* Node = this->Tail->NextNode; Node != nullptr; Node = Node->NextNode)
    {
        ++Count;
    }

    return Count;
}

template <typename T, EQueueKind::Type TKind, typename TSizeType>
bool TComplexQueue<T, TKind, TSizeType>::UnsafeContains(const T& InContent) const
{
    for (TNode* Node = this->Tail->NextNode; Node != nullptr; Node = Node->NextNode)
    {
        if (Node->Content == InContent)
        {
            return true;
        }
    }

    return false;
}

} /* ~Namespace Jafg */
