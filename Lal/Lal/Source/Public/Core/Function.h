// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T>
class TFunction;

//#
//# A function object that can store any callable type.
//# Weak / strong references to lambdas, function pointers, and member functions are supported.
//#
template <typename TRet, typename... TParams>
class TFunction<TRet(TParams...)>
{
    template <typename T>
    friend class TFunction;

public:

    FORCEINLINE static constexpr u64 NumParams() noexcept { return sizeof... (TParams); }

private:

    struct LCallableBase;

    template <typename TFunctor> requires (std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    struct LCallableStrong;

    template <typename TFunctor> requires (std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    struct LCallableWeak;

    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    struct LCallableMember;

    friend LCallableBase;

    //#
    //# This function returns a copy of the base-callable type.
    //# If nullptr is returned, then the underlying callable type is not copyable.
    //#
    typedef void(*LCopyImplDelegate)(const LCallableBase* Base, TFunction* OutFunction);

public:

    typedef Smart::TUnique<LCallableBase> LImpl;

    FORCEINLINE constexpr TFunction() noexcept = default;
    FORCEINLINE constexpr TFunction(LNullptrTy) noexcept : Impl(nullptr) { }
    FORCEINLINE constexpr TFunction& operator=(LNullptrTy) noexcept { this->Impl.operator=(nullptr); return *this; }
    FORCEINLINE constexpr TFunction(TFunction&& Other) noexcept;
    FORCEINLINE constexpr TFunction& operator=(TFunction&& Other) noexcept;
    FORCEINLINE TFunction(const TFunction& Other) { this->CopyImpl(Other); }
    FORCEINLINE TFunction& operator=(const TFunction& Other) { this->CopyImpl(Other); return *this; }
    FORCEINLINE constexpr ~TFunction() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // Templated constructors for different callable types - might not be resolved by compiler.
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE constexpr TFunction(TFunctor&& Functor) noexcept
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableStrong<TFunctor>>(this, std::forward<TFunctor>(Functor));
    }
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE constexpr TFunction(TFunctor* Functor) noexcept
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableWeak<TFunctor>>(this, Functor);
    }
    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE constexpr TFunction(TObj* Object, TMemberFunctor MemberFunctor) noexcept
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableMember<TObj, TMemberFunctor>>(this, Object, MemberFunctor);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Explicit bind functions for different callable types.
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE void BindStrong(TFunctor&& Functor)
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableStrong<TFunctor>>(this, std::forward<TFunctor>(Functor));
    }
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE void BindWeak(TFunctor* Functor)
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableWeak<TFunctor>>(this, Functor);
    }
    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE void BindMember(TObj* Object, TMemberFunctor MemberFunctor)
    {
        this->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableMember<TObj, TMemberFunctor>>(this, Object, MemberFunctor);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Implicit static bind functions for different callable types.
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE static TFunction Create(TFunctor&& Functor)
    {
        return { std::forward<TFunctor>(Functor) };
    }
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE static TFunction Create(TFunctor* Functor)
    {
        return { std::forward<TFunctor>(Functor) };
    }
    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE static TFunction Create(TObj* Object, TMemberFunctor MemberFunctor)
    {
        return { Object, MemberFunctor };
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Explicit static bind functions for different callable types.
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE static TFunction CreateStrong(TFunctor&& Functor)
    {
        TFunction Function; Function.BindStrong(std::forward<TFunctor>(Functor));
        return Function;
    }
    template <typename TFunctor> requires (std::is_same_v<TFunctor, TFunction> == false && std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    FORCEINLINE static TFunction CreateWeak(TFunctor* Functor)
    {
        TFunction Function; Function.BindWeak(Functor);
        return Function;
    }
    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    FORCEINLINE static TFunction CreateMember(TObj* Object, TMemberFunctor MemberFunctor)
    {
        TFunction Function; Function.BindMember(Object, MemberFunctor);
        return Function;
    }

    FORCEINLINE TRet Invoke(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, decltype(this->Impl), TParams...>)
    {
        check( this->IsValid() )
        return this->Impl->Invoke(std::forward<TParams>(Params)...);
    }
    FORCEINLINE TRet operator()(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, decltype(this->Impl), TParams...>)
    {
        check( this->IsValid() )
        return this->Impl->Invoke(std::forward<TParams>(Params)...);
    }

    FORCEINLINE constexpr bool operator==(LNullptrTy) const noexcept { return this->IsValid() == false; }

    FORCEINLINE constexpr void Reset() noexcept { this->Impl.Reset(); this->CopyImplDelegate = nullptr; }
    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Impl.IsValid(); }
    FORCEINLINE constexpr bool IsCopyable() const noexcept { return this->IsValid() && this->CopyImplDelegate != nullptr; }

private:

    FORCEINLINE void CopyImpl(const TFunction& Other) noexcept;

    struct LCallableBase
    {
        FORCEINLINE virtual ~LCallableBase() = default;
        FORCEINLINE virtual TRet Invoke(TParams... Params) const = 0;
    };

    template <typename TFunctor> requires (std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    struct LCallableStrong final : public LCallableBase
    {
        TFunctor Inner;

        FORCEINLINE constexpr LCallableStrong(TFunction* Function, TFunctor&& Functor) noexcept
            : Inner(std::forward<TFunctor>(Functor))
        {
            check( Function )

            if constexpr (std::is_copy_constructible_v<TFunctor>)
            {
                Function->CopyImplDelegate = LCallableStrong<TFunctor>::Copy;
            }
            else
            {
                Function->CopyImplDelegate = nullptr;
            }

            return;
        }

        FORCEINLINE static constexpr void Copy(const LCallableBase* Base, TFunction* OutFunction) noexcept requires (std::is_copy_constructible_v<TFunctor>)
        {
            check( Base && OutFunction )
            const LCallableStrong* Strong { static_cast<const LCallableStrong*>(Base) };

            OutFunction->Impl = LImpl { new LCallableStrong<TFunctor>(OutFunction, Strong->Inner) };
            check( OutFunction->IsValid() )
            check( OutFunction->IsCopyable() )

            return;
        }

        FORCEINLINE virtual TRet Invoke(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, TFunctor, TParams...>) override
        {
            return this->Inner(std::forward<TParams>(Params)...);
        }

        private:

        FORCEINLINE constexpr LCallableStrong(TFunction* Function, const TFunctor& Functor) noexcept requires (std::is_copy_constructible_v<TFunctor>)
            : Inner(Functor)
        {
            check( Function )
            Function->CopyImplDelegate = LCallableStrong<TFunctor>::Copy;

            return;
        }
    };

    template <typename TFunctor> requires (std::is_invocable_r_v<TRet, TFunctor, TParams...>)
    struct LCallableWeak final : public LCallableBase
    {
        TFunctor* Inner { nullptr };

        FORCEINLINE constexpr LCallableWeak(TFunction* Function, TFunctor* Functor) noexcept
            : Inner(Functor)
        {
            check( this->Inner )

            check( Function )
            Function->CopyImplDelegate = LCallableWeak<TFunctor>::Copy;

            return;
        }

        FORCEINLINE static constexpr void Copy(const LCallableBase* Base, TFunction* OutFunction) noexcept
        {
            check( Base && OutFunction )
            const LCallableWeak* Weak { static_cast<const LCallableWeak*>(Base) };

            OutFunction->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableWeak<TFunctor>>(OutFunction, Weak->Inner);
            check( OutFunction->IsValid() )
            check( OutFunction->IsCopyable() )

            return;
        }

        FORCEINLINE TRet Invoke(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, TFunctor, TParams...>) override
        {
            check( this->Inner )
            return (*this->Inner)(std::forward<TParams>(Params)...);
        }
    };

    template <typename TObj, typename TMemberFunctor> requires (std::is_invocable_r_v<TRet, TMemberFunctor, TObj*, TParams...>)
    struct LCallableMember final : public LCallableBase
    {
        TObj* Object { nullptr };
        TMemberFunctor MemberFunctor { nullptr };

        FORCEINLINE constexpr LCallableMember(TFunction* Function, TObj* InObject, TMemberFunctor InMemberFunctor) noexcept
            : Object(InObject), MemberFunctor(InMemberFunctor)
        {
            check( this->Object && this->MemberFunctor )

            check( Function )
            Function->CopyImplDelegate = LCallableMember<TObj, TMemberFunctor>::Copy;

            return;
        }

        FORCEINLINE static constexpr void Copy(const LCallableBase* Base, TFunction* OutFunction) noexcept
        {
            check( Base && OutFunction )
            const LCallableMember* Member { static_cast<const LCallableMember*>(Base) };

            OutFunction->Impl = Smart::EmplaceUniqueOfType<LCallableBase, LCallableMember<TObj, TMemberFunctor>>(OutFunction, Member->Object, Member->MemberFunctor);
            check( OutFunction->IsValid() )
            check( OutFunction->IsCopyable() )

            return;
        }

        FORCEINLINE TRet Invoke(TParams... Params) const noexcept(std::is_nothrow_invocable_r_v<TRet, TObj, TMemberFunctor, TParams...>) override
        {
            check( this->Object && this->MemberFunctor )
            return (this->Object->*this->MemberFunctor)(std::forward<TParams>(Params)...);
        }
    };

    LImpl Impl;
    LCopyImplDelegate CopyImplDelegate { nullptr };
};

template <typename TRet, typename... TParams>
FORCEINLINE constexpr TFunction<TRet(TParams...)>::TFunction(TFunction&& Other) noexcept
    : Impl(std::move(Other.Impl)), CopyImplDelegate(std::move(Other.CopyImplDelegate))
{
    check( Other.Impl.IsValid() == false )
}

template <typename TRet, typename... TParams>
FORCEINLINE constexpr TFunction<TRet(TParams...)>& TFunction<TRet(TParams...)>::operator=(TFunction&& Other) noexcept
{
    this->Impl = std::move(Other.Impl);
    this->CopyImplDelegate = Other.CopyImplDelegate;

    check( Other.Impl.IsValid() == false )

    return *this;
}

template <typename TRet, typename... TParams>
FORCEINLINE void TFunction<TRet(TParams...)>::CopyImpl(const TFunction& Other) noexcept
{
    if (Other.IsValid() == false)
    {
        this->Reset();
        return;
    }

    jassert( Other.IsCopyable() )
    Other.CopyImplDelegate(Other.Impl.GetPointer(), this);

    return;
}

} /* ~Namespace Jafg */
