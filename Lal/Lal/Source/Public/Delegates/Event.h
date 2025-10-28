// Copyright mzoesch. All rights reserved.

#pragma once

//# An event is a #TFunction which can only be invoked by its owning class.
template<typename TOwner, typename TRet, typename... TParams>
class TEvent : public TFunction<TRet(TParams...)>
{
    friend TOwner;

    typedef TFunction<TRet(TParams...)> Super;

protected:

    FORCEINLINE bool InvokeIfBound(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, decltype(this->Impl), TParams...>)
        requires std::is_void_v<TRet>
    {
        return Super::InvokeIfBound(std::forward<TParams>(Params)...);
    }

    FORCEINLINE TRet Invoke(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, decltype(this->Impl), TParams...>)
    {
        return Super::Invoke(std::forward<TParams>(Params)...);
    }
    FORCEINLINE TRet operator()(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, decltype(this->Impl), TParams...>)
    {
        return Super::operator()(std::forward<TParams>(Params)...);
    }
};

template<typename TOwner, typename TSig>
class TMulticastEvent;

//# Like #TEvent but with the capabilities of #TMulticastDelegate.
template<typename TOwner, typename... TParams>
class TMulticastEvent<TOwner, void(TParams...)> : public TMulticastDelegate<void(TParams...)>
{
    friend TOwner;

    template<typename UOwner, typename TSig>
    friend class TMulticastEvent;

    typedef TMulticastDelegate<void(TParams...)> Super;

protected:

    FORCEINLINE bool Broadcast(TParams... Params)
    {
        return Super::Broadcast(std::forward<TParams>(Params)...);
    }
};
