// Copyright mzoesch. All rights reserved.

#pragma once

//# Represents a handle to a delegate.
struct LDelegateHandle
{
    typedef std::size_t value_type;

    inline static constexpr value_type InvalidHandle{};

    FORCEINLINE constexpr LDelegateHandle() noexcept = default;
    DEFAULT_REALLOC_OF_ANY_FORM(LDelegateHandle)
    FORCEINLINE constexpr ~LDelegateHandle() noexcept = default;

    FORCEINLINE constexpr void Reset() noexcept { this->Handle = {}; }
    FORCEINLINE constexpr bool IsValid() const noexcept { return *this->Handle != InvalidHandle; }
    FORCEINLINE constexpr operator bool() const noexcept { return this->IsValid(); }

    FORCEINLINE static LDelegateHandle From(value_type Handle) noexcept
    {
        LDelegateHandle Result;
        Result.Handle.Value = Handle;
        return Result;
    }

    FORCEINLINE constexpr value_type UnderlyingValue() const noexcept { return *this->Handle; }

protected:

    algo::exchange_storage<value_type, InvalidHandle> Handle;
};

//# Automatic unbinding delegate handle on destruction.
template<typename TDelegate>
struct TRaiiDelegateHandle : private LDelegateHandle
{
    FORCEINLINE constexpr TRaiiDelegateHandle(TDelegate& Delegate) noexcept : LDelegateHandle{}, Delegate{Delegate} {}
    PROHIBIT_COPY(TRaiiDelegateHandle)
    DEFAULT_MOVE(TRaiiDelegateHandle)
    FORCEINLINE constexpr ~TRaiiDelegateHandle() noexcept { this->Unbind(); }

    FORCEINLINE constexpr void Reset() noexcept { this->Handle = {}; }
    FORCEINLINE constexpr void Unbind() noexcept
    {
        if (this->IsValid())
        {
            this->Delegate.get().Remove(&**this);
        }
    }
    FORCEINLINE constexpr bool IsValid() const noexcept { return *this->Handle != InvalidHandle; }
    FORCEINLINE constexpr operator bool() const noexcept { return this->IsValid(); }

    FORCEINLINE LDelegateHandle& operator*() noexcept { return static_cast<LDelegateHandle&>(*this); }
    FORCEINLINE LDelegateHandle const& operator*() const noexcept { return static_cast<LDelegateHandle const&>(*this); }

    FORCEINLINE static TRaiiDelegateHandle From(TDelegate& Delegate, LDelegateHandle const& Handle) noexcept
    {
        TRaiiDelegateHandle Result{Delegate};
        Result.Handle.Value = Handle.UnderlyingValue();
        return Result;
    }

    template<typename T>
    FORCEINLINE static TRaiiDelegateHandle Make(TDelegate& Delegate, T Functor) noexcept
        requires(TDelegate::value_type::template is_valid_functor_v<T>)
    {
        return From(Delegate, Delegate.Emplace(std::move(Functor)));
    }

    FORCEINLINE constexpr value_type UnderlyingValue() const noexcept { return *this->Handle; }

private:

    std::reference_wrapper<TDelegate> Delegate;
};

template<typename TSig>
class TMulticastDelegate;

//# Like #TFunction, but can store multiple. If a delegate returns true, it will be removed from the multicast.
template<typename... TParams>
class TMulticastDelegate<bool(TParams...)>
{
    template<typename TSgi>
    friend class TMulticastDelegate;

public:

    typedef TFunction<bool(TParams...)> LDelegate;
    typedef TFunction<bool(TParams...)> value_type;

    FORCEINLINE LDelegateHandle Add(LDelegate&& Delegate) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, std::move(Delegate));
        return LDelegateHandle::From(this->Cursor);
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TFunctor&& Functor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, std::forward<TFunctor>(Functor));
        return LDelegateHandle::From(this->Cursor);
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TFunctor* Functor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, Functor);
        return LDelegateHandle::From(this->Cursor);
    }

    template<typename TObj, typename TMemberFunctor> requires(std::is_invocable_r_v<bool, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TObj* Object, TMemberFunctor MemberFunctor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate{ Object, MemberFunctor });
        return LDelegateHandle::From(this->Cursor);
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceStrong(TFunctor&& Functor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateStrong(std::forward<TFunctor>(Functor)));
        return LDelegateHandle::From(this->Cursor);
    }
    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceWeak(TFunctor* Functor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateWeak(Functor));
        return LDelegateHandle::From(this->Cursor);
    }
    template<typename TObj, typename TMemberFunctor> requires(std::is_invocable_r_v<bool, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceMember(TObj* Object, TMemberFunctor MemberFunctor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateMember(Object, MemberFunctor));
        return LDelegateHandle::From(this->Cursor);
    }

    //# @return True, if at least one delegate was called.
    FORCEINLINE bool Broadcast(TParams... Params)
    {
        bool bResult{};
        for (auto It{this->Delegates.begin()}; It != this->Delegates.end();)
        {
            bResult = true;
            if (It->second(Params...))
            {
                It = this->Delegates.erase(It);
            }
            else
            {
                ++It;
            }
        }
        return bResult;
    }

    FORCEINLINE bool HasAny() const noexcept
    {
        return this->Delegates.empty() == false;
    }

    FORCEINLINE bool IsStillBound(LDelegateHandle const& Handle) noexcept
    {
        return algo::contains(this->Delegates, Handle.UnderlyingValue(), [](auto const& Pair) { return Pair.first; });
    }

    //# @return True, if delegate was found and removed.
    FORCEINLINE bool Remove(LDelegateHandle* Handle) noexcept
    {
        check(Handle)

        if (auto It{algo::find(this->Delegates, Handle->UnderlyingValue(), [](auto const& Pair) { return Pair.first; })};
            It != this->Delegates.end())
        {
            this->Delegates.erase(It);
            Handle->Reset();
            return true;
        }

        return false;
    }

    FORCEINLINE u64 UnbindAll() noexcept
    {
        const u64 Bound{ static_cast<u64>(this->Delegates.size()) };
        algo::orphan(&this->Delegates);
        return Bound;
    }

private:

    inline static constexpr u64 InvalidHandle{};

    TArray<std::pair<u64, LDelegate>> Delegates;
    u64 Cursor{ InvalidHandle };
};
