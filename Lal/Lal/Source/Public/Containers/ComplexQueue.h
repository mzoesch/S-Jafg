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

    FORCEINLINE void Empty();
    FORCEINLINE bool IsEmpty() const;

    FORCEINLINE SizeType GetSizeSlow();

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
        TNode* volatile NextNode { nullptr };
        T Content;

        FORCEINLINE TNode() = delete;
        FORCEINLINE TNode(const T& InContent) : Content(InContent) { }
        FORCEINLINE TNode(T&& InContent) : Content(std::move(InContent)) { }
        template <typename... TArgs>
        FORCEINLINE TNode(TArgs&&... Args) : Content(std::forward<TArgs>(Args)...) { }
        FORCEINLINE ~TNode() = default;
    };

    std::atomic<TNode*> Head { nullptr };
    std::atomic<TNode*> Tail { nullptr };
    std::mutex Mutex;
};

template <typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::Dequeue(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail.load() == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    if (TNode* LocalTail = this->Tail.load(); LocalTail)
    {
        *OutContent = LocalTail->Content;
        ensure( PopLockFree() );
        return true;
    }

    return false;
}

template <typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueByMove(T* OutContent)
{
    checkSlow( OutContent )

    if (this->Tail.load() == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    if (TNode* LocalTail = this->Tail.load(); LocalTail)
    {
        *OutContent = std::move(LocalTail->Content);
        ensure( PopLockFree() );
        return true;
    }

    return false;
}

template <typename T, typename TSizeType>
template <typename TOther>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::DequeueWithComparison(T* OutContent, const TOther& InContent)
{
    checkSlow( OutContent )

    if (this->Tail.load() == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail.load();
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            *OutContent = Cursor->Content;

            if (PrevCursor)
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head.store(PrevCursor);
                    PrevCursor->NextNode = nullptr;
                }
                else
                {
                    PrevCursor->NextNode = Cursor->NextNode;
                }
            }
            else
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head.load() == Cursor )
                    this->Head.store(nullptr);
                    this->Tail.store(nullptr);
                }
                else
                {
                    this->Tail.store(Cursor->NextNode);
                }
            }

            check( this->Tail.load() == nullptr ? this->Head.load() == nullptr : this->Head.load() != nullptr )

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

        if (this->Tail.load() == nullptr)
        {
            return false;
        }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail.load();
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            *OutContent = Cursor->Content;

            if (PrevCursor)
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head.store(PrevCursor);
                    PrevCursor->NextNode = nullptr;
                }
                else
                {
                    PrevCursor->NextNode = Cursor->NextNode;
                }
            }
            else
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head.load() == Cursor )
                    this->Head.store(nullptr);
                    this->Tail.store(nullptr);
                }
                else
                {
                    this->Tail.store(Cursor->NextNode);
                }
            }

            check( this->Tail.load() == nullptr ? this->Head.load() == nullptr : this->Head.load() != nullptr )

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

    if (this->Tail.load() == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail.load();
    while (Cursor != nullptr)
    {
        if (Cursor->Content == InContent)
        {
            *OutContent = std::move(Cursor->Content);

            if (PrevCursor)
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head.store(PrevCursor);
                    PrevCursor->NextNode = nullptr;
                }
                else
                {
                    PrevCursor->NextNode = Cursor->NextNode;
                }
            }
            else
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head.load() == Cursor )
                    this->Head.store(nullptr);
                    this->Tail.store(nullptr);
                }
                else
                {
                    this->Tail.store(Cursor->NextNode);
                }
            }

            check( this->Tail.load() == nullptr ? this->Head.load() == nullptr : this->Head.load() != nullptr )

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

        if (this->Tail.load() == nullptr)
        {
            return false;
        }

    std::unique_lock Lock(this->Mutex);

    TNode* PrevCursor = nullptr;
    TNode* Cursor     = this->Tail.load();
    while (Cursor != nullptr)
    {
        if (InPredicate(Cursor->Content))
        {
            *OutContent = std::move(Cursor->Content);

            if (PrevCursor)
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head == Cursor )
                    this->Head.store(PrevCursor);
                    PrevCursor->NextNode = nullptr;
                }
                else
                {
                    PrevCursor->NextNode = Cursor->NextNode;
                }
            }
            else
            {
                if (Cursor->NextNode == nullptr)
                {
                    check( this->Head.load() == Cursor )
                    this->Head.store(nullptr);
                    this->Tail.store(nullptr);
                }
                else
                {
                    this->Tail.store(Cursor->NextNode);
                }
            }

            check( this->Tail.load() == nullptr ? this->Head.load() == nullptr : this->Head.load() != nullptr )

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
    if (this->Tail.load() == nullptr)
    {
        return false;
    }

    std::unique_lock Lock(this->Mutex);
    return this->PopLockFree();
}

// template<typename T, typename TSizeType>
// FORCEINLINE T* TMpmcQueue<T, TSizeType>::Peek()
// {
//     std::shared_lock Lock(this->Mutex);
//     TNode* LocalTail = this->Tail.load();
//     return LocalTail ? &LocalTail->Content : nullptr;
// }
//
// template<typename T, typename TSizeType>
// FORCEINLINE const T* TMpmcQueue<T, TSizeType>::Peek() const
// {
//     std::shared_lock Lock(this->Mutex);
//     TNode* LocalTail = this->Tail.load();
//     return LocalTail ? &LocalTail->Content : nullptr;
// }

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
    return this->Tail.load() == nullptr;
}

template<typename T, typename TSizeType>
FORCEINLINE typename TMpmcQueue<T, TSizeType>::SizeType TMpmcQueue<T, TSizeType>::GetSizeSlow()
{
    if (this->Tail.load() == nullptr)
    {
        return 0;
    }

    SizeType Size;

    std::unique_lock Lock(this->Mutex);

    for (TNode* Node = this->Tail.load(); Node != nullptr; Node = Node->NextNode)
    {
        ++Size;
    }

    return Size;
}

template <typename T, typename TSizeType>
FORCEINLINE void TMpmcQueue<T, TSizeType>::EnqueueImpl(TNode* NewNode)
{
    checkSlow( NewNode )

    std::unique_lock Lock(this->Mutex);
    if (TNode* OldHead = this->Head.load(); OldHead)
    {
        this->Head.store(NewNode);
        OldHead->NextNode = NewNode;
    }
    else
    {
        check( this->Tail.load() == nullptr )
        this->Head.store(NewNode);
        this->Tail.store(NewNode);
        check( this->Tail.load()->NextNode == nullptr )
    }

    return;
}

template<typename T, typename TSizeType>
FORCEINLINE bool TMpmcQueue<T, TSizeType>::PopLockFree()
{
    const TNode* Popped = this->Tail.load();

    if (Popped == nullptr)
    {
        check( this->Head.load() == nullptr )
        return false;
    }

    this->Tail.store(Popped->NextNode);
    if (this->Tail.load() == nullptr)
    {
        check( Popped == this->Head.load() )
        this->Head.store(nullptr);
    }

    delete Popped;

    return true;
}

} /* ~Namespace Jafg */
