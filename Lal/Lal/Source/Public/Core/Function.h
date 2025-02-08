// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T>
class TFunction;

/**
 * A function object that can store any callable type.
 * Weak / strong references to lambdas, function pointers, and member functions are supported.
 */
template <typename RetTy, typename ... ParamsTy>
class TFunction<RetTy(ParamsTy...)>
{
    template <typename T>
    friend class TFunction;

public:

    FORCEINLINE static constexpr int32 NumParams() { return sizeof ... (ParamsTy); }

    using LRetTy    = RetTy;
    using LParamsTy = std::tuple<ParamsTy...>;

    struct LCallableBase;
    template <typename CallableTy>                 struct LStrongCallable;
    template <typename CallableTy>                 struct LWeakCallable;
    template <typename ObjTy, typename CallableTy> struct LMemberCallable;

    using LUniqueCallableTy = Smart::TUnique<LCallableBase>;

    FORCEINLINE TFunction() = default;
    FORCEINLINE TFunction(LNullptrTy) : Callable(nullptr) { }
    FORCEINLINE TFunction& operator=(LNullptrTy) { this->Reset(); return *this; }
    PROHIBIT_COPY(TFunction)
    FORCEINLINE TFunction(TFunction&& Other) noexcept
    {
        this->Reset();
        this->Callable = std::move(Other.Callable);
        Other.Callable = nullptr; /* Do not reset as it would orphan the memory. */
        return;
    }
    FORCEINLINE TFunction& operator=(TFunction&& Other) noexcept
    {
        this->Reset();
        this->Callable = std::move(Other.Callable);
        Other.Callable = nullptr; /* Do not reset as it would orphan the memory. */
        return *this;
    }
    FORCEINLINE ~TFunction() { this->Reset(); return; }

    template <typename CallableTy>
    FORCEINLINE TFunction(CallableTy&& InCallable)
    {
        this->Reset();

        if constexpr (std::is_same_v<std::decay_t<CallableTy>, TFunction>)
        {
            this->Callable = Smart::MakeUnique(const_cast<typename CallableTy::LUniqueCallableTy::StoredInnerTy>(
                InCallable.Callable.GetValuePtr()
            ));
            ::Jafg::Smart::Private::LMySmartHelper::RemoveNoOrphan(
                const_cast<typename CallableTy::LUniqueCallableTy&>(InCallable.Callable)
            ); /* Do not reset as it would orphan the memory. */
        }
        else if constexpr (std::is_invocable_v<CallableTy, ParamsTy...>)
        {
            this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LStrongCallable<CallableTy>>(std::forward<CallableTy>(InCallable));
        }
        else
        {
            UNREACHABLE_CONTROL_PATH_STATIC( std::is_same_v<std::decay_t<CallableTy> PRIVATE_JAFG_CORE_COMMA TFunction> )
        }
    }
    template <typename CallableTy>
    FORCEINLINE void BindStrong(CallableTy&& InCallable)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LStrongCallable<CallableTy>>(std::forward<CallableTy>(InCallable));
    }

    template <typename CallableTy>
    FORCEINLINE TFunction(CallableTy* InCallable)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LWeakCallable<CallableTy>>(InCallable);
    }
    template <typename CallableTy>
    FORCEINLINE void BindWeak(CallableTy* InCallable)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LWeakCallable<CallableTy>>(InCallable);
    }

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE TFunction(ObjTy* InObject, CallableTy InMember)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LMemberCallable<ObjTy, CallableTy>>(InObject, InMember);
    }
    template <typename ObjTy, typename CallableTy>
    FORCEINLINE void BindMember(ObjTy* InObject, CallableTy InMember)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LMemberCallable<ObjTy, CallableTy>>(InObject, InMember);
    }

    FORCEINLINE RetTy Invoke(ParamsTy... InParams) const { return this->operator()(std::forward<ParamsTy>(InParams)...); }
    FORCEINLINE RetTy operator()(ParamsTy... InParams) const
    {
        if (this->Callable == nullptr)
        {
            panic( "TFunction is not valid" )
            abort();
        }

        return this->Callable->Invoke(std::forward<ParamsTy>(InParams)...);
    }

    FORCEINLINE bool operator==(const LNullptrTy) const { return this->IsBound() == false; }
    FORCEINLINE bool operator!=(const LNullptrTy) const { return this->IsBound();          }

    FORCEINLINE void Reset() { this->Callable.Reset(); }

    /** Whether the callable is set. */
    FORCEINLINE bool IsBound() const { return this->Callable != nullptr; }

    /** Whether the callable is set and valid. */
    FORCEINLINE bool IsValid() const { return this->IsBound() && this->Callable->IsValid(); }
    FORCEINLINE void CheckValidCall() const { check( this->IsValid() ) return; }
    FORCEINLINE explicit operator bool() const { return this->IsBound(); }

private:

    struct LCallableBase
    {
        virtual ~LCallableBase() = default;
        virtual auto Invoke(ParamsTy... InParams) const -> RetTy = 0;
        virtual bool IsValid() const { return false; }
    };

    template <typename CallableTy>
    struct LStrongCallable final : public LCallableBase
    {
        CallableTy InnerCallable;

        FORCEINLINE LStrongCallable(CallableTy&& InCallable) : InnerCallable(std::move(InCallable)) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) const override
        {
            static_assert(
                std::is_invocable_v<CallableTy, ParamsTy...>,
                "Callable is not invocable with parameters. Was an invalid function signature provided?"
            );
            static_assert(
                std::is_same_v<std::invoke_result_t<CallableTy, ParamsTy...>, RetTy>,
                "Callable must return the correct type."
            );
            return this->InnerCallable(std::forward<ParamsTy>(InParams)...);
        }
        FORCEINLINE bool IsValid() const override { return true; }
    };

    template <typename CallableTy>
    struct LWeakCallable final : public LCallableBase
    {
        CallableTy* InnerCallable = nullptr;

        FORCEINLINE LWeakCallable(CallableTy* InCallable) : InnerCallable(InCallable) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) const override
        {
            if (this->InnerCallable)
            {
                return (*this->InnerCallable)(std::forward<ParamsTy>(InParams)...);
            }

            panic( "Attempt to invoke null callable" )
            abort();
        }
        FORCEINLINE bool IsValid() const override { return this->InnerCallable != nullptr; }
    };

    template <typename ObjTy, typename CallableTy>
    struct LMemberCallable final : public LCallableBase
    {
        ObjTy*     Object = nullptr;
        CallableTy Member = nullptr;

        FORCEINLINE LMemberCallable(ObjTy* InObject, CallableTy InMember) : Object(InObject), Member(InMember) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) const override
        {
            if (this->Object && this->Member)
            {
                return (this->Object->*Member)(std::forward<ParamsTy>(InParams)...);
            }

            panic( "Invalid member function call" )
            abort();
        }
        FORCEINLINE bool IsValid() const override { return this->Object != nullptr; }
    };

   LUniqueCallableTy Callable = nullptr;
};

} /* ~Namespace Jafg */
