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
class TMpmcQueue final
{
public:

    using Item     = T;
    using SizeType = TSizeType;

    FORCEINLINE TMpmcQueue() = default;
    PROHIBIT_COPY(TMpmcQueue)
    FORCEINLINE ~TMpmcQueue() { this->Empty(); }

    FORCEINLINE void Enqueue(const T& InContent) { this->EnqueueImpl(new TNode(InContent)); }
    FORCEINLINE void Enqueue(T&& InContent) { this->EnqueueImpl(new TNode(std::move(InContent))); }
    template <typename... TArgs>
    FORCEINLINE void EnqueueByEmplace(TArgs&&... Args) { this->EnqueueImpl(new TNode(std::forward<TArgs>(Args)...)); }

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

    FORCEINLINE bool Pop();

    /**
     * Peeks at the queue's tail item without removing it.
     * @return Pointer to the item, or nullptr if queue is empty.
     */
    FORCEINLINE       T* Peek();
    FORCEINLINE const T* Peek() const;

    FORCEINLINE void Empty();
    FORCEINLINE bool IsEmpty() const;

#if WITH_TESTS
    FORCEINLINE SizeType UnsafeSize() const;
#endif /* WITH_TESTS */

#if WITH_TESTS
public:
#else /* WITH_TESTS */
private:
#endif /* !WITH_TESTS */

    struct TNode;

    FORCEINLINE void EnqueueImpl(TNode* NewNode);
    FORCEINLINE bool PopLockFree();

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
FORCEINLINE bool TMpmcQueue<T, TSizeType>::Dequeue(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);
    *OutContent = this->Tail->Content;
    ensure(this->PopLockFree());

    return true;
}

template <typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueByMove(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);
    *OutContent = std::move(this->Tail->Content);
    ensure(this->PopLockFree());

    return true;
}

template <typename T, typename TSizeType>
template <typename TOther>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueWithComparison(T* OutContent, const TOther& InContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            *OutContent = Cursor->Content;

            if (PrevCursor)
            {
                PrevCursor->NextNode = Cursor->NextNode;
                if (PrevCursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = PrevCursor;
                }
            }
            else
            {
                this->Tail = Cursor->NextNode;
                if (this->Tail == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = nullptr;
                }
            }

            check( this->Tail == nullptr ? this->Head == nullptr : this->Head != nullptr )

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
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueWithPredicate(T* OutContent, const Predicate& InPredicate)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            *OutContent = Cursor->Content;

            if (PrevCursor)
            {
                PrevCursor->NextNode = Cursor->NextNode;
                if (PrevCursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = PrevCursor;
                }
            }
            else
            {
                this->Tail = Cursor->NextNode;
                if (this->Tail == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = nullptr;
                }
            }

            check( this->Tail == nullptr ? this->Head == nullptr : this->Head != nullptr )

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
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueByMoveWithComparison(T* OutContent, const TOther& InContent)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            *OutContent = std::move(Cursor->Content);

            if (PrevCursor)
            {
                PrevCursor->NextNode = Cursor->NextNode;
                if (PrevCursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = PrevCursor;
                }
            }
            else
            {
                this->Tail = Cursor->NextNode;
                if (this->Tail == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = nullptr;
                }
            }

            check( this->Tail == nullptr ? this->Head == nullptr : this->Head != nullptr )

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
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueByMoveWithPredicate(T* OutContent, const Predicate& InPredicate)
{
    checkSlow( OutContent )

    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail;
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            *OutContent = std::move(Cursor->Content);

            if (PrevCursor)
            {
                PrevCursor->NextNode = Cursor->NextNode;
                if (PrevCursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = PrevCursor;
                }
            }
            else
            {
                this->Tail = Cursor->NextNode;
                if (this->Tail == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head = nullptr;
                }
            }

            check( this->Tail == nullptr ? this->Head == nullptr : this->Head != nullptr )

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
FORCEINLINE bool TMpmcQueue<T, TSizeType>::Pop()
{
    if (this->Tail == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);
    return this->PopLockFree();
}

template<typename T, typename TSizeType>
FORCEINLINE T* TMpmcQueue<T, TSizeType>::Peek()
{
    std::shared_lock Lock(this->Mutex);
    return this->Tail ? &this->Tail->Content : nullptr;
}

template<typename T, typename TSizeType>
FORCEINLINE const T* TMpmcQueue<T, TSizeType>::Peek() const
{
    std::shared_lock Lock(this->Mutex);
    return this->Tail ? &this->Tail->Content : nullptr;
}

template <typename T, typename TSizeType>
FORCEINLINE void TMpmcQueue<T, TSizeType>::Empty()
{
    std::unique_lock Lock(this->Mutex);
    while (this->PopLockFree()) { }
    return;
}

template <typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::IsEmpty() const
{
    return this->Tail == nullptr;
}

#if WITH_TESTS
template <typename T, typename TSizeType>
FORCEINLINE typename TMpmcQueue<T, TSizeType>::SizeType TMpmcQueue<T, TSizeType>::UnsafeSize() const
{
    if (this->Tail == nullptr)
    {
        return 0;
    }

    SizeType Count = 0;

    for (TNode* Node = this->Tail; Node != nullptr; Node = Node->NextNode)
    {
        ++Count;
    }

    return Count;
}
#endif /* WITH_TESTS */

template <typename T, typename TSizeType>
FORCEINLINE void TMpmcQueue<T, TSizeType>::EnqueueImpl(TNode* NewNode)
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

template<typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::PopLockFree()
{
    if ( this->Tail == nullptr )
    {
        check( this->Head == nullptr )
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
