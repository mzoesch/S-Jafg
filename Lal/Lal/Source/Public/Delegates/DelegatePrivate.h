// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename DelegateTy>
struct TDelegate;

/**
 * A delegate that can store a single function.
 */
template <typename RetTy, typename... ParamsTy>
struct TDelegate<RetTy(ParamsTy...)> final
{
    template <typename DelegateTy>
    friend struct TDelegate;

    template <bool Condition, typename RetVal>
    using TEnableIf = std::enable_if_t<Condition, RetVal>;

    using LRetValTy = RetTy;
    using LParamsTy = std::tuple<ParamsTy...>;

    using LFunctionSigTy = TFunction<RetTy(ParamsTy...)>;

    FORCEINLINE TDelegate() = default;
    FORCEINLINE TDelegate(LNullptrTy) : Delegate(nullptr) { return; }
    PROHIBIT_COPY(TDelegate)
    FORCEINLINE TDelegate(TDelegate&& InOther) noexcept : Delegate(std::move(InOther.Delegate))
    {
        check( InOther.Delegate.IsValid() == false );
    }
    FORCEINLINE TDelegate& operator=(TDelegate&& InOther) noexcept
    {
        this->Delegate = std::move(InOther.Delegate);
        check( InOther.Delegate.IsValid() == false );
        return *this;
    }
    FORCEINLINE ~TDelegate() { this->Unbind(); return; }

    FORCEINLINE LRetValTy Invoke(ParamsTy... InFuncParams) const
    {
        check( this->Delegate.IsValid() )
        return this->Delegate(std::forward<ParamsTy>(InFuncParams)...);
    }
    FORCEINLINE LRetValTy operator()(ParamsTy... InFuncParams) const { return this->Invoke(std::forward<ParamsTy>(InFuncParams)...); }

    template <typename LocalFuncRetValTy = LRetValTy, TEnableIf<std::is_void_v<LocalFuncRetValTy>, void>* = nullptr>
    UNUSED FORCEINLINE bool InvokeIfBound(ParamsTy... InFuncParams) const
    {
        if (this->Delegate.IsValid())
        {
            this->Delegate.Invoke(std::forward<ParamsTy>(InFuncParams)...);
            return true;
        }

        return false;
    }

    template <typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ParamsTy...>
    FORCEINLINE TDelegate(CallableTy&& InCallable)
    {
        this->Delegate.BindStrong(std::forward<CallableTy>(InCallable));
    }
    template <typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ParamsTy...>
    FORCEINLINE void BindStrong(CallableTy&& InCallable)
    {
        this->Delegate.BindStrong(std::forward<CallableTy>(InCallable));
    }

    template <typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ParamsTy...>
    FORCEINLINE TDelegate(CallableTy* InCallable)
    {
        this->Delegate.BindWeak(InCallable);
    }
    template <typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ParamsTy...>
    FORCEINLINE void BindWeak(CallableTy* InCallable)
    {
        this->Delegate.BindWeak(InCallable);
    }

    template <typename ObjTy, typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ObjTy*, ParamsTy...>
    FORCEINLINE TDelegate(ObjTy* InObject, CallableTy InMember)
    {
        this->Delegate.BindMember(InObject, InMember);
    }
    template <typename ObjTy, typename CallableTy> requires std::is_invocable_r_v<RetTy, CallableTy, ObjTy*, ParamsTy...>
    FORCEINLINE void BindMember(ObjTy* InObject, CallableTy InMember)
    {
        this->Delegate.BindMember(InObject, InMember);
    }

    FORCEINLINE TDelegate(const LFunctionSigTy& InFunction) = delete;
    FORCEINLINE TDelegate(LFunctionSigTy&& InFunction) { this->Delegate = std::move(InFunction); }
    FORCEINLINE void BindFunction(const LFunctionSigTy& InFunction) = delete;
    FORCEINLINE void BindFunction(LFunctionSigTy&& InFunction) { this->Delegate = std::move(InFunction); }

    FORCEINLINE bool IsBound() const { return this->Delegate.IsValid(); }
    FORCEINLINE void Unbind()        { this->Delegate.Reset();        }
    FORCEINLINE explicit operator bool() const { return this->IsBound(); }

    template <typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateStrongFunction(CallableTy&& InCallable)
    {
        return TFunction<RetTy(ParamsTy...)>(std::forward<CallableTy>(InCallable));
    }
    template <typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateFunction(CallableTy&& InCallable)
    {
        return TDelegate::CreateStrongFunction(std::forward<CallableTy>(InCallable));
    }

    template <typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateWeakFunction(CallableTy* InCallable)
    {
        return TFunction<RetTy(ParamsTy...)>(InCallable);
    }
    template <typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateFunction(CallableTy* InCallable)
    {
        return TDelegate::CreateWeakFunction(InCallable);
    }

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateMemberFunction(ObjTy* InObj, CallableTy InMember)
    {
        return TFunction<RetTy(ParamsTy...)>(InObj, InMember);
    }
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE static LFunctionSigTy CreateFunction(ObjTy* InObj, CallableTy InMember)
    {
        return TDelegate::CreateMemberFunction(InObj, InMember);
    }

    template <typename CallableTy>
    FORCEINLINE static TDelegate CreateStrongDelegate(CallableTy&& InCallable)
    {
        return TDelegate(std::forward<CallableTy>(InCallable));
    }
    template <typename CallableTy>
    FORCEINLINE static TDelegate CreateDelegate(CallableTy&& InCallable)
    {
        return TDelegate::CreateStrongDelegate(std::forward<CallableTy>(InCallable));
    }

    template <typename CallableTy>
    FORCEINLINE static TDelegate CreateWeakDelegate(CallableTy* InCallable)
    {
        return TDelegate(InCallable);
    }
    template <typename CallableTy>
    FORCEINLINE static TDelegate CreateDelegate(CallableTy* InCallable)
    {
        return TDelegate::CreateWeakDelegate(InCallable);
    }

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE static TDelegate CreateMemberDelegate(ObjTy* InObj, CallableTy InMember)
    {
        return TDelegate(InObj, InMember);
    }
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE static TDelegate CreateDelegate(ObjTy* InObj, CallableTy InMember)
    {
        return TDelegate::CreateMemberDelegate(InObj, InMember);
    }

private:

    LFunctionSigTy Delegate;
};

template <typename DelegateTy>
struct TMulticastDelegate;

template <typename RetTy, typename... ParamsTy>
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

    FORCEINLINE explicit LDelegateHandle(const u32 InHandle) : Handle(InHandle) { check( this->Handle != 0 ) return; }

    FORCEINLINE void Reset()         { this->Handle = 0; return; }
    FORCEINLINE bool IsValid() const { return this->Handle != 0; }

#if WITH_TESTS
    FORCEINLINE u32 GetHandle() const { return this->Handle; }
#endif /* WITH_TESTS */

private:

    u32 Handle;
};

#if LAL_WITH_LEGACY_LAL_ARRAY
    static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LDelegateHandle>);
#endif /* LAL_WITH_LEGACY_LAL_ARRAY */

/**
 * A delegate that can store multiple functions and broadcast to all of them.
 */
template <typename RetTy, typename... ParamsTy>
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

    FORCEINLINE bool Remove(LDelegateHandle* InDelegateHandle);
    FORCEINLINE i32  UnbindAll();

private:

    static constexpr u32 InvalidHandle { 0 };
    u32         HandleCount { 0 };

    // TODO: Use one array with structs or std::pair.
    TArray<u32> DelegatesHandles;
    TArray<TFunction<RetTy(ParamsTy...)>> Delegates;
};

template <typename RetTy, typename... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::Broadcast(ParamsTy... InFuncParams)
{
    for (TFunction<RetTy(ParamsTy...)>& Delegate : this->Delegates)
    {
        Delegate(std::forward<ParamsTy>(InFuncParams)...);
    }

    /**
     * Delegates must never remove their handle when boradcasting, so this is safe.
     */
    return this->Delegates.empty() == false;
}

template <typename RetTy, typename... ParamsTy>
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
        );

    this->Delegates.emplace_back(std::forward<CallableTy>(InCallable));
    this->DelegatesHandles.emplace_back(++HandleCount);

    checkSlow( this->DelegatesHandles.back() == this->HandleCount  )
    return LDelegateHandle{this->DelegatesHandles.back()};
}

template <typename RetTy, typename... ParamsTy>
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
        );

    this->Delegates.emplace_back(InCallable);
    this->DelegatesHandles.emplace_back(++HandleCount);

    checkSlow( this->DelegatesHandles.back() == this->HandleCount  )
    return LDelegateHandle{this->DelegatesHandles.back()};
}

template <typename RetTy, typename... ParamsTy>
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

    this->Delegates.emplace_back(InObj, InMember);
    this->DelegatesHandles.emplace_back(++HandleCount);

    checkSlow( this->DelegatesHandles.back() == this->HandleCount  )
    return LDelegateHandle{this->DelegatesHandles.back()};
}

template <typename RetTy, typename... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::IsStillBound(const LDelegateHandle& InDelegateHandle) const
{
    return algo::contains(this->DelegatesHandles, InDelegateHandle.Handle);
}

template <typename RetTy, typename... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::HasAny() const
{
    return this->DelegatesHandles.empty() == false;
}

template <typename RetTy, typename... ParamsTy>
bool TMulticastDelegate<RetTy(ParamsTy...)>::Remove(LDelegateHandle* InDelegateHandle)
{
    check( InDelegateHandle )

    check( this->Delegates.size() == this->DelegatesHandles.size() )

    auto Begin { this->DelegatesHandles.begin() };
    while (Begin != this->DelegatesHandles.end())
    {
        if (*Begin == InDelegateHandle->Handle)
        {
            this->Delegates.erase(this->Delegates.begin() + std::distance(this->DelegatesHandles.begin(), Begin));
            this->DelegatesHandles.erase(Begin);

            InDelegateHandle->Reset();

            checkSlow( this->Delegates.size() == this->DelegatesHandles.size() )

            return true;
        }

        ++Begin;

        continue;
    }

    return false;
}

template <typename RetTy, typename... ParamsTy>
i32 TMulticastDelegate<RetTy(ParamsTy...)>::UnbindAll()
{
    for (auto& Delegate : this->Delegates)
    {
        Delegate.Reset();
    }

    const i32 NumDelegates { static_cast<i32>(this->Delegates.size()) };

    algo::orphan(&this->Delegates);
    algo::orphan(&this->DelegatesHandles);

    /*
     * Do not reset handle count to avoid handle reuse.
     */

    return NumDelegates;
}

} /* ~Namespace Jafg */

#if LAL_WITH_LEGACY_LAL_ARRAY
    template <typename RetTy, typename... ParamsTy>
    struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<Jafg::TMulticastDelegate<RetTy(ParamsTy...)>> : Lal::TrueType { };

    template <typename RetTy, typename... ParamsTy>
    struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<Jafg::TFunction<RetTy(ParamsTy...)>> : Lal::TrueType { };

    static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<Jafg::TMulticastDelegate<int()>>);
    static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<Jafg::TFunction<int()>>);
#endif /* LAL_WITH_LEGACY_LAL_ARRAY */
