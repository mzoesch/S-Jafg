// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * This is a thread safe, unbounded, non-intrusive and locked queue.
 * Produces push back and the consumer pops from the front OR in the middle.
 * @remark This queue is not lock-free because non-lock-free queues will never be able to satisfy all data-structure
 *         requirements for J-Objects.
 */
template <typename T, typename TSizeType>
class TMpscQueue final
{
public:

    using LItemTy = T;
    using LSizeTy = TSizeType;

    FORCEINLINE TMpscQueue() = default;
    PROHIBIT_COPY(TMpscQueue)
    FORCEINLINE ~TMpscQueue() { this->Empty(); }

    /** @remark Producer only. */
    FORCEINLINE void Enqueue(const T& InContent) { this->EnqueueImpl(new TNode(InContent)); }
    FORCEINLINE void Enqueue(T&& InContent) { this->EnqueueImpl(new TNode(std::move(InContent))); }
    template <typename... TArgs>
    FORCEINLINE void EnqueueByEmplace(TArgs&&... Args) { this->EnqueueImpl(new TNode(std::forward<TArgs>(Args)...)); }

    /** @remark Consumer only. */
    FORCEINLINE bool Dequeue(T* OutContent);
    FORCEINLINE bool DequeueByMove(T* OutContent);
    /** Allows to deque the first item for which the comparison is true. */
    template <typename TOther>
    FORCEINLINE bool DequeueWithComparison(T* OutContent, const TOther& InContent);
    template <typename Predicate>
    FORCEINLINE bool DequeueWithPredicate(T* OutContent, const Predicate& InPredicate);
    template <typename TOther>
    FORCEINLINE bool DequeueByMoveWithComparison(T* OutContent, const TOther& InContent);
    template <typename Predicate>
    FORCEINLINE bool DequeueByMoveWithPredicate(T* OutContent, const Predicate& InPredicate);

    /** @remark Consumer only. */
    FORCEINLINE bool Pop();

    /** @remark Consumer only. */
    FORCEINLINE void Empty();
    FORCEINLINE bool IsEmpty() const;

    /**
     * Non-thread-safe num getter. For statistics, dev-checks and debugging purposes only.
     * @remark Consumer only.
     * @remark If you feel the need to check the number of items in the queue apart from the above reasons, you should
     *         really reevaluate if a queue is the right data structure for your use case.
     */
    FORCEINLINE LSizeTy UnsafeSize() const;

#if WITH_TESTS
public:
#else /* WITH_TESTS */
private:
#endif /* !WITH_TESTS */

    struct TNode;

    FORCEINLINE void EnqueueImpl(TNode* NewNode);

    struct TNode final
    {
        TNode* NextNode = nullptr;
        T      Content;

        FORCEINLINE TNode() = delete;
        FORCEINLINE TNode(const T& InContent) : Content(InContent) { }
        FORCEINLINE TNode(T&& InContent) : Content(std::move(InContent)) { }
        template <typename... TArgs>
        FORCEINLINE TNode(TArgs&&... Args) : Content(std::forward<TArgs>(Args)...) { }
        FORCEINLINE ~TNode() = default;
    };

    TNode*     Head = nullptr;
    TNode*     Tail = nullptr;
    std::mutex Mutex;
};

template <typename T, typename TSizeType>
bool TMpscQueue<T, TSizeType>::Dequeue(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    *OutContent = this->Tail->Content;
    ensure(this->Pop());

    return true;
}

template <typename T, typename TSizeType>
bool TMpscQueue<T, TSizeType>::DequeueByMove(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    *OutContent = std::move(this->Tail->Content);
    ensure(this->Pop());

    return true;
}

template <typename T, typename TSizeType>
template <typename TOther>
bool TMpscQueue<T, TSizeType>::DequeueWithComparison(T* OutContent, const TOther& InContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            if (PrevCursor == nullptr)
            {
                return this->Dequeue(OutContent);
            }

            *OutContent = Cursor->Content;
            std::unique_lock Lock(this->Mutex);
            PrevCursor->NextNode = Cursor->NextNode;
            if (this->Head == Cursor)
            {
                this->Head = PrevCursor->NextNode;
                if (this->Head == nullptr)
                {
                    this->Head = this->Tail;
                }
            }
            checkSlow( this->Head )
            Lock.unlock();
            delete Cursor;

            return true;
        }

        PrevCursor = Cursor;
        Cursor = Cursor->NextNode;

        continue;
    }

    return false;
}

template <typename T, typename TSizeType>
template <typename Predicate>
bool TMpscQueue<T, TSizeType>::DequeueWithPredicate(T* OutContent, const Predicate& InPredicate)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            if (PrevCursor == nullptr)
            {
                return this->Dequeue(OutContent);
            }

            *OutContent = Cursor->Content;
            std::unique_lock Lock(this->Mutex);
            PrevCursor->NextNode = Cursor->NextNode;
            if (this->Head == Cursor)
            {
                this->Head = PrevCursor->NextNode;
                if (this->Head == nullptr)
                {
                    this->Head = this->Tail;
                }
            }
            checkSlow( this->Head )
            Lock.unlock();
            delete Cursor;

            return true;
        }

        PrevCursor = Cursor;
        Cursor = Cursor->NextNode;

        continue;
    }

    return false;
}

template <typename T, typename TSizeType>
template <typename TOther>
bool TMpscQueue<T, TSizeType>::DequeueByMoveWithComparison(T* OutContent, const TOther& InContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            if (PrevCursor == nullptr)
            {
                return this->DequeueByMove(OutContent);
            }

            *OutContent = std::move(Cursor->Content);
            std::unique_lock Lock(this->Mutex);
            PrevCursor->NextNode = Cursor->NextNode;
            if (this->Head == Cursor)
            {
                this->Head = PrevCursor->NextNode;
                if (this->Head == nullptr)
                {
                    this->Head = this->Tail;
                }
            }
            checkSlow( this->Head )
            Lock.unlock();
            delete Cursor;

            return true;
        }

        PrevCursor = Cursor;
        Cursor = Cursor->NextNode;

        continue;
    }

    return false;
}

template <typename T, typename TSizeType>
template <typename Predicate>
bool TMpscQueue<T, TSizeType>::DequeueByMoveWithPredicate(T* OutContent, const Predicate& InPredicate)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            if (PrevCursor == nullptr)
            {
                return this->DequeueByMove(OutContent);
            }

            *OutContent = std::move(Cursor->Content);
            std::unique_lock Lock(this->Mutex);
            PrevCursor->NextNode = Cursor->NextNode;
            if (this->Head == Cursor)
            {
                this->Head = PrevCursor->NextNode;
                if (this->Head == nullptr)
                {
                    this->Head = this->Tail;
                }
            }
            checkSlow( this->Head )
            Lock.unlock();
            delete Cursor;

            return true;
        }

        PrevCursor = Cursor;
        Cursor = Cursor->NextNode;

        continue;
    }

    return false;
}

template <typename T, typename TSizeType>
bool TMpscQueue<T, TSizeType>::Pop()
{
    if (this->Tail == nullptr)
    {
        return false;
    }

    const TNode* Popped = this->Tail;

    std::unique_lock Lock(this->Mutex);
    this->Tail = this->Tail->NextNode;
    if (this->Tail == nullptr)
    {
        check( Popped == this->Head )
        this->Head = nullptr;
    }
    Lock.unlock();

    delete Popped;

    return true;
}

template <typename T, typename TSizeType>
void TMpscQueue<T, TSizeType>::Empty()
{
    while (this->Pop()) { }
}

template <typename T, typename TSizeType>
bool TMpscQueue<T, TSizeType>::IsEmpty() const
{
    return this->Tail == nullptr;
}

template <typename T, typename TSizeType>
typename TMpscQueue<T, TSizeType>::LSizeTy TMpscQueue<T, TSizeType>::UnsafeSize() const
{
    if (this->Tail == nullptr)
    {
        return 0;
    }

    LSizeTy Count = 0;

    for (TNode* Node = this->Tail; Node != nullptr; Node = Node->NextNode)
    {
        ++Count;
    }

    return Count;
}

template <typename T, typename TSizeType>
void TMpscQueue<T, TSizeType>::EnqueueImpl(TNode* NewNode)
{
    checkSlow( NewNode )

    std::unique_lock Lock(this->Mutex);
    if (this->Head)
    {
        TNode* OldHead = this->Head;
        this->Head = NewNode;
        OldHead->NextNode = NewNode;
    }
    else
    {
        this->Head = NewNode;
        this->Tail = NewNode;
        check( this->Tail->NextNode == nullptr )
    }

    return;
}

} /* ~Namespace Jafg */
