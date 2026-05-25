// Copyright mzoesch. All rights reserved.

#pragma once

//# Represents a handle to a delegate.
struct LDelegateHandle final
{
    constexpr LDelegateHandle() noexcept = delete;
    FORCEINLINE constexpr LDelegateHandle(std::nullptr_t) noexcept : Handle(0) { return; }
    FORCEINLINE constexpr LDelegateHandle(const LDelegateHandle& InOther) noexcept = default;
    FORCEINLINE constexpr LDelegateHandle(LDelegateHandle&& InOther) noexcept
    {
        this->Handle = InOther.Handle;
        InOther.Handle = 0;

        return;
    }
    FORCEINLINE constexpr LDelegateHandle& operator=(std::nullptr_t) noexcept { this->Handle = 0; return *this; }
    FORCEINLINE constexpr LDelegateHandle& operator=(const LDelegateHandle& InOther) noexcept = default;
    FORCEINLINE constexpr LDelegateHandle& operator=(LDelegateHandle&& InOther) noexcept
    {
        this->Handle = InOther.Handle;
        InOther.Handle = 0;
        return *this;
    }

    FORCEINLINE constexpr explicit LDelegateHandle(const u64 InHandle)
        : Handle(InHandle)
    {
        check( this->Handle != 0 )
        return;
    }

    FORCEINLINE constexpr void Reset() noexcept { this->Handle = 0; return; }
    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Handle != 0; }
    NODISCARD FORCEINLINE constexpr bool operator!() const noexcept { return !this->IsValid(); }

    FORCEINLINE constexpr u64 UnderlyingValue() const noexcept { return this->Handle; }

private:

    u64 Handle;
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

    FORCEINLINE LDelegateHandle Add(LDelegate&& Delegate) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, std::move(Delegate));
        return LDelegateHandle{ this->Cursor };
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TFunctor&& Functor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, std::forward<TFunctor>(Functor));
        return LDelegateHandle{ this->Cursor };
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TFunctor* Functor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, Functor);
        return LDelegateHandle{ this->Cursor };
    }

    template<typename TObj, typename TMemberFunctor> requires(std::is_invocable_r_v<bool, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE LDelegateHandle Emplace(TObj* Object, TMemberFunctor MemberFunctor) noexcept
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate{ Object, MemberFunctor });
        return LDelegateHandle{ this->Cursor };
    }

    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceStrong(TFunctor&& Functor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateStrong(std::forward<TFunctor>(Functor)));
        return LDelegateHandle{ this->Cursor };
    }
    template<typename TFunctor> requires(std::is_same_v<TFunctor, LDelegate> == false && std::is_invocable_r_v<bool, TFunctor, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceWeak(TFunctor* Functor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateWeak(Functor));
        return LDelegateHandle{ this->Cursor };
    }
    template<typename TObj, typename TMemberFunctor> requires(std::is_invocable_r_v<bool, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE LDelegateHandle EmplaceMember(TObj* Object, TMemberFunctor MemberFunctor)
    {
        this->Delegates.emplace_back(++this->Cursor, LDelegate::CreateMember(Object, MemberFunctor));
        return LDelegateHandle{ this->Cursor };
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
