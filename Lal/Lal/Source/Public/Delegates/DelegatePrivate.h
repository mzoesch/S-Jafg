// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename DelegateTy>
struct TDelegate;

template <typename RetTy, typename ... ParamsTy>
struct TDelegate<RetTy(ParamsTy...)> final
{
    template <typename DelegateTy>
    friend struct TDelegate;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    typedef RetTy RetValTy;

    FORCEINLINE auto Execute(ParamsTy ... InFuncParams) -> RetValTy
    {
        check( this->Delegate.IsSet() )
        return this->Delegate(std::forward<ParamsTy>(InFuncParams)...);
    }

    template <typename LocalFuncRetValTy = RetValTy, TEnableIf<std::is_void_v<LocalFuncRetValTy>, void>* = nullptr>
    FORCEINLINE auto ExecuteIfBound(ParamsTy ... InFuncParams) -> bool
    {
        if (this->Delegate.IsSet())
        {
            this->Delegate(std::forward<ParamsTy>(InFuncParams)...);
            return true;
        }

        return false;
    }

    template <typename InOtherFunctorTy>
    FORCEINLINE void Bind(InOtherFunctorTy&& InOtherFunctor)
    {
        this->Delegate = std::forward<InOtherFunctorTy>(InOtherFunctor);
    }

    template <typename InMemberFunc, typename InObj>
    FORCEINLINE void BindMember(InMemberFunc InMemberFuncPtr, InObj InObjPtr)
    {
        this->Delegate.BindMember(InMemberFuncPtr, InObjPtr);
    }

    FORCEINLINE bool IsBound() const { return this->Delegate.IsSet(); }
    FORCEINLINE void Unbind()        { this->Delegate.Reset();        }

private:

    TFunction<RetTy(ParamsTy...)> Delegate;
};

template <typename DelegateTy>
struct TMulticastDelegate;

template <typename RetTy, typename ... ParamsTy>
struct TMulticastDelegate<RetTy(ParamsTy...)>;

/**
 * Represents a handle to a delegate.
 * Store this handle somewhere if you need to unbind the delegate later without removing all other delegates.
 */
struct LDelegateHandle final
{
    template <typename DelegateTy>
    friend struct TMulticastDelegate;

    LDelegateHandle() = delete;
    FORCEINLINE LDelegateHandle(LNullptrTy) : Handle(nullptr) { return; }
    FORCEINLINE LDelegateHandle(const LDelegateHandle& InOther) = default;
    FORCEINLINE LDelegateHandle(LDelegateHandle&& InOther) = default;
    FORCEINLINE LDelegateHandle& operator=(LNullptrTy) { this->Handle = nullptr; return *this; }
    FORCEINLINE LDelegateHandle& operator=(const LDelegateHandle& InOther) = default;
    FORCEINLINE LDelegateHandle& operator=(LDelegateHandle&& InOther) = default;

    FORCEINLINE explicit LDelegateHandle(void* InHandle) : Handle(InHandle) { check( this->Handle != nullptr ) return; }

    FORCEINLINE auto Reset()         -> void { this->Handle = nullptr; return; }
    FORCEINLINE auto IsValid() const -> bool { return this->Handle != nullptr; }

#if IN_DEBUG
    /** For unit tests. */
    FORCEINLINE void* GetHandle() const { return this->Handle; }
#endif /* IN_DEBUG */

private:

    void* Handle = nullptr;
};

template <typename RetTy, typename ... ParamsTy>
struct TMulticastDelegate<RetTy(ParamsTy...)> final
{
    template <typename DelegateTy>
    friend struct TMulticastDelegate;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    typedef RetTy RetValTy;

    FORCEINLINE auto Broadcast(ParamsTy... InFuncParams) -> void;

    template <typename InOtherFunctorTy>
    FORCEINLINE auto Add(InOtherFunctorTy&& InOtherFunctor) -> LDelegateHandle;
    template <typename InMemberFunc, typename InObj>
    FORCEINLINE auto AddMember(InMemberFunc InMemberFuncPtr, InObj InObjPtr) -> LDelegateHandle;

    FORCEINLINE auto Remove(LDelegateHandle& InDelegateHandle) -> bool;

    FORCEINLINE auto IsStillBound(const LDelegateHandle& InDelegateHandle) const -> bool;
    FORCEINLINE auto HasAny() const -> bool ;
    FORCEINLINE auto UnbindAll() -> int32;

private:

    TdhArray<TFunction<RetTy(ParamsTy...)>*> Delegates;
};

template <typename RetTy, typename ... ParamsTy>
void TMulticastDelegate<RetTy(ParamsTy...)>::Broadcast(ParamsTy... InFuncParams)
{
    for (auto& Delegate : this->Delegates)
    {
        (*Delegate)(std::forward<ParamsTy>(InFuncParams)...);
    }

    return;
}

template <typename RetTy, typename ... ParamsTy>
template <typename InOtherFunctorTy>
LDelegateHandle TMulticastDelegate<RetTy(ParamsTy...)>::Add(InOtherFunctorTy&& InOtherFunctor)
{
    this->Delegates.Add(new TFunction<RetTy(ParamsTy...)>(std::forward<InOtherFunctorTy>(InOtherFunctor)));
    void** Delegate = reinterpret_cast<void**>(this->Delegates.Peek());
    checkSlow( Delegate != nullptr )

    return LDelegateHandle(*Delegate);
}

template <typename RetTy, typename ... ParamsTy>
template <typename InMemberFunc, typename InObj>
LDelegateHandle TMulticastDelegate<RetTy(ParamsTy...)>::AddMember(InMemberFunc InMemberFuncPtr, InObj InObjPtr)
{
    this->Delegates.Add(new TFunction<RetTy(ParamsTy...)>());
    (*this->Delegates.Peek())->BindMember(InMemberFuncPtr, InObjPtr);
    void** Delegate = reinterpret_cast<void**>(this->Delegates.Peek());
    checkSlow( Delegate != nullptr )

    return LDelegateHandle(*Delegate);
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::Remove(LDelegateHandle& InDelegateHandle)
{
    for (int32 I = 0; I < this->Delegates.GetSize(); ++I)
    {
        if (this->Delegates[I] == InDelegateHandle.Handle)
        {
            this->Delegates[I]->Reset();
            delete this->Delegates[I];
            this->Delegates.RemoveAt(I);

            InDelegateHandle.Reset();

            return true;
        }

        continue;
    }

    return false;
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::IsStillBound(const LDelegateHandle& InDelegateHandle) const
{
    return this->Delegates.Contains(InDelegateHandle.Handle);
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::HasAny() const
{
    return this->Delegates.IsEmpty() == false;
}

template <typename RetTy, typename ... ParamsTy>
int32 TMulticastDelegate<RetTy(ParamsTy...)>::UnbindAll()
{
    for (auto& Delegate : this->Delegates)
    {
        Delegate->Reset();
        delete Delegate;
    }

    const int32 NumDelegates = this->Delegates.GetSize();

    this->Delegates.Empty();

    return NumDelegates;
}

} /* ~Namespace Jafg */
