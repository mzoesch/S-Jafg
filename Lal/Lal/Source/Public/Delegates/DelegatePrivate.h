// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename DelegateTy>
struct TDelegate;

/**
 * A delegate that can store a single function.
 */
template <typename RetTy, typename ... ParamsTy>
struct TDelegate<RetTy(ParamsTy...)> final
{
    template <typename DelegateTy>
    friend struct TDelegate;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    using LRetValTy = RetTy;
    using LParamsTy = std::tuple<ParamsTy...>;

    FORCEINLINE TDelegate() = default;
    FORCEINLINE TDelegate(LNullptrTy) : Delegate(nullptr) { return; }

    FORCEINLINE LRetValTy Execute(ParamsTy ... InFuncParams)
    {
        check( this->Delegate.IsBound() )
        return this->Delegate(std::forward<ParamsTy>(InFuncParams)...);
    }
    FORCEINLINE LRetValTy operator()(ParamsTy ... InFuncParams) { return this->Execute(std::forward<ParamsTy>(InFuncParams)...); }

    template <typename LocalFuncRetValTy = LRetValTy, TEnableIf<std::is_void_v<LocalFuncRetValTy>, void>* = nullptr>
    FORCEINLINE bool ExecuteIfBound(ParamsTy ... InFuncParams)
    {
        if (this->Delegate.IsBound())
        {
            this->Delegate(std::forward<ParamsTy>(InFuncParams)...);
            return true;
        }

        return false;
    }

    template <typename CallableTy>
    FORCEINLINE TDelegate(CallableTy&& InCallable)
    {
        this->Delegate.BindStrong(std::forward<CallableTy>(InCallable));
    }
    template <typename CallableTy>
    FORCEINLINE void BindStrong(CallableTy&& InCallable)
    {
        this->Delegate.BindStrong(std::forward<CallableTy>(InCallable));
    }

    template <typename CallableTy>
    FORCEINLINE TDelegate(CallableTy* InCallable)
    {
        this->Delegate.BindWeak(InCallable);
    }
    template <typename CallableTy>
    FORCEINLINE void BindWeak(CallableTy* InCallable)
    {
        this->Delegate.BindWeak(InCallable);
    }

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE TDelegate(ObjTy* InObject, CallableTy InMember)
    {
        this->Delegate.BindMember(InObject, InMember);
    }
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE void BindMember(ObjTy* InObject, CallableTy InMember)
    {
        this->Delegate.BindMember(InObject, InMember);
    }

    FORCEINLINE TDelegate(const TFunction<RetTy(ParamsTy...)>& InFunction) = delete;
    FORCEINLINE TDelegate(TFunction<RetTy(ParamsTy...)>&& InFunction) { this->Delegate = std::move(InFunction); }
    FORCEINLINE void BindFunction(const TFunction<RetTy(ParamsTy...)>& InFunction) = delete;
    FORCEINLINE void BindFunction(TFunction<RetTy(ParamsTy...)>&& InFunction) { this->Delegate = std::move(InFunction); }

    FORCEINLINE bool IsBound() const { return this->Delegate.IsBound(); }
    FORCEINLINE void Unbind()        { this->Delegate.Reset();        }
    FORCEINLINE explicit operator bool() const { return this->IsBound(); }

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
    FORCEINLINE LDelegateHandle(LNullptrTy) : Handle(0) { return; }
    FORCEINLINE LDelegateHandle(const LDelegateHandle& InOther) = default;
    FORCEINLINE LDelegateHandle(LDelegateHandle&& InOther) = default;
    FORCEINLINE LDelegateHandle& operator=(LNullptrTy) { this->Handle = 0; return *this; }
    FORCEINLINE LDelegateHandle& operator=(const LDelegateHandle& InOther) = default;
    FORCEINLINE LDelegateHandle& operator=(LDelegateHandle&& InOther) = default;

    FORCEINLINE explicit LDelegateHandle(const uint32 InHandle) : Handle(InHandle) { check( this->Handle != 0 ) return; }

    FORCEINLINE void Reset()         { this->Handle = 0; return; }
    FORCEINLINE bool IsValid() const { return this->Handle != 0; }

#if WITH_TESTS
    FORCEINLINE uint32 GetHandle() const { return this->Handle; }
#endif /* WITH_TESTS */

private:

    uint32 Handle;
};

/**
 * A delegate that can store multiple functions and broadcast to all of them.
 */
template <typename RetTy, typename ... ParamsTy>
struct TMulticastDelegate<RetTy(ParamsTy...)> final
{
    static_assert(std::is_same_v<RetTy, void>);

    template <typename DelegateTy>
    friend struct TMulticastDelegate;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    using LRetValTy = RetTy;
    using LParamsTy = std::tuple<ParamsTy...>;

    /** @return True if at least one delegate was called. */
    FORCEINLINE bool Broadcast(ParamsTy... InFuncParams);

    template <typename CallableTy>
    FORCEINLINE LDelegateHandle AddStrong(CallableTy&& InCallable);
    template <typename CallableTy>
    FORCEINLINE LDelegateHandle AddWeak(CallableTy* InCallable);
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LDelegateHandle AddMember(ObjTy* InObj, CallableTy InMember);

    template <typename CallableTy>
    FORCEINLINE LDelegateHandle Add(CallableTy&& InCallable) { return this->AddStrong(std::forward<CallableTy>(InCallable)); }
    template <typename CallableTy>
    FORCEINLINE LDelegateHandle Add(CallableTy* InCallable) { return this->AddWeak(InCallable); }
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LDelegateHandle Add(ObjTy* InObj, CallableTy InMember) { return this->AddMember(InObj, InMember); }

    FORCEINLINE bool IsStillBound(const LDelegateHandle& InDelegateHandle) const;
    FORCEINLINE bool HasAny() const;

    FORCEINLINE bool  Remove(LDelegateHandle& InDelegateHandle);
    FORCEINLINE int32 UnbindAll();

private:

    static constexpr uint32 InvalidHandle = 0;
    uint32           HandleCount = 0;
    TdhArray<uint32> DelegatesHandles;
    TdhArray<TFunction<RetTy(ParamsTy...)>> Delegates;
};

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::Broadcast(ParamsTy... InFuncParams)
{
    for (TFunction<RetTy(ParamsTy...)>& Delegate : this->Delegates)
    {
        Delegate(std::forward<ParamsTy>(InFuncParams)...);
    }

    /**
     * Delegates must never remove their handle when boradcasting, so this is safe.
     */
    return this->Delegates.IsEmpty() == false;
}

template <typename RetTy, typename ... ParamsTy>
template <typename CallableTy>
LDelegateHandle TMulticastDelegate<RetTy(ParamsTy...)>::AddStrong(CallableTy&& InCallable)
{
    static_assert(
        std::is_invocable_v<CallableTy, ParamsTy...>,
        "Callable is not invocable with parameters. Was an invalid function signature provided?"
    );
    static_assert(
        std::is_same_v<std::invoke_result_t<CallableTy, ParamsTy...>, void>,
        "Callable must return void."
    );    this->Delegates.Emplace(std::forward<CallableTy>(InCallable));
    this->DelegatesHandles.Emplace(++HandleCount);
    checkSlow( *this->DelegatesHandles.Peek() == this->HandleCount  )
    return LDelegateHandle(*this->DelegatesHandles.Peek());
}

template <typename RetTy, typename ... ParamsTy>
template <typename CallableTy>
LDelegateHandle TMulticastDelegate<RetTy(ParamsTy...)>::AddWeak(CallableTy* InCallable)
{
    static_assert(
        std::is_invocable_v<CallableTy, ParamsTy...>,
        "Callable is not invocable with parameters. Was an invalid function signature provided?"
    );
    static_assert(
        std::is_same_v<std::invoke_result_t<CallableTy, ParamsTy...>, void>,
        "Callable must return void."
    );    this->Delegates.Emplace(InCallable);
    this->DelegatesHandles.Emplace(++HandleCount);
    checkSlow( *this->DelegatesHandles.Peek() == this->HandleCount  )
    return LDelegateHandle(*this->DelegatesHandles.Peek());
}

template <typename RetTy, typename ... ParamsTy>
template <typename ObjTy, typename CallableTy>
LDelegateHandle TMulticastDelegate<RetTy(ParamsTy...)>::AddMember(ObjTy* InObj, CallableTy InMember)
{
    static_assert(
        std::is_invocable_v<CallableTy, ObjTy*, ParamsTy...>,
        "Callable is not invocable with object and parameters. Was an invalid function signature provided?"
    );
    static_assert(
        std::is_same_v<std::invoke_result_t<CallableTy, ObjTy*, ParamsTy...>, void>,
        "Callable must return void."
    );
    this->Delegates.Emplace(InObj, InMember);
    this->DelegatesHandles.Emplace(++HandleCount);
    checkSlow( *this->DelegatesHandles.Peek() == this->HandleCount  )
    return LDelegateHandle(*this->DelegatesHandles.Peek());
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::Remove(LDelegateHandle& InDelegateHandle)
{
    for (int32 Idx = 0; Idx < this->DelegatesHandles.GetSize(); ++Idx)
    {
        if (this->DelegatesHandles[Idx] == InDelegateHandle.Handle)
        {
            this->Delegates.RemoveAt(Idx);
            this->DelegatesHandles.RemoveAt(Idx);

            InDelegateHandle.Reset();

            checkSlow( this->Delegates.GetSize() == this->DelegatesHandles.GetSize() )

            return true;
        }

        continue;
    }

    return false;
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::IsStillBound(const LDelegateHandle& InDelegateHandle) const
{
    return this->DelegatesHandles.Contains(InDelegateHandle.Handle);
}

template <typename RetTy, typename ... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::HasAny() const
{
    return this->DelegatesHandles.IsEmpty() == false;
}

template <typename RetTy, typename ... ParamsTy>
int32 TMulticastDelegate<RetTy(ParamsTy...)>::UnbindAll()
{
    for (TFunction<RetTy(ParamsTy...)>& Delegate : this->Delegates)
    {
        Delegate.Reset();
    }

    const int32 NumDelegates = this->Delegates.GetSize();

    this->Delegates.Empty();
    this->DelegatesHandles.Empty();

    /*
     * Do not reset handle count to avoid handle reuse.
     */

    return NumDelegates;
}

} /* ~Namespace Jafg */
