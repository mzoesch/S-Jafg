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

    FORCEINLINE TFunction() = default;
    FORCEINLINE ~TFunction() { this->Reset(); return; }

    FORCEINLINE TFunction(LNullptrTy) : Callable(nullptr) { }

    template <typename CallableTy>
    FORCEINLINE TFunction(CallableTy&& InCallable)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LStrongCallable<std::decay_t<CallableTy>>>(std::forward<CallableTy>(InCallable));
    }
    template <typename CallableTy>
    FORCEINLINE void BindStrong(CallableTy&& InCallable)
    {
        this->Reset();
        this->Callable = Smart::EmplaceUniqueOfType<LCallableBase, LStrongCallable<std::decay_t<CallableTy>>>(std::forward<CallableTy>(InCallable));
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

    FORCEINLINE TFunction& operator=(const LNullptrTy)
    {
        this->Reset();
        return *this;
    }
    FORCEINLINE TFunction& operator=(TFunction&& Other) noexcept
    {
        this->Reset();
        this->Callable = std::move(Other.Callable);
        Other.Callable = nullptr; /* Do not reset as it would orphan the memory. */
        return *this;
    }

    FORCEINLINE TFunction& operator=(const TFunction& Other) noexcept = delete;

    FORCEINLINE RetTy Call(ParamsTy... InParams) { return this->operator()(std::forward<ParamsTy>(InParams)...); }
    FORCEINLINE RetTy operator()(ParamsTy... InParams)
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
        virtual RetTy Invoke(ParamsTy... InParams) = 0;
        virtual bool  IsValid() const { return false; }
    };

    template <typename CallableTy>
    struct LStrongCallable final : public LCallableBase
    {
        CallableTy Callable;

        FORCEINLINE LStrongCallable(CallableTy&& InCallable) : Callable(std::move(InCallable)) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) override { return this->Callable(std::forward<ParamsTy>(InParams)...); }
        FORCEINLINE bool  IsValid() const override { return true; }
    };

    template <typename CallableTy>
    struct LWeakCallable final : public LCallableBase
    {
        CallableTy* Callable = nullptr;

        FORCEINLINE LWeakCallable(CallableTy* InCallable) : Callable(InCallable) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) override
        {
            if (Callable)
            {
                return (*Callable)(std::forward<ParamsTy>(InParams)...);
            }

            panic( "Attempt to invoke null callable" )
            abort();
        }
        FORCEINLINE bool IsValid() const override { return Callable != nullptr; }
    };

    template <typename ObjTy, typename CallableTy>
    struct LMemberCallable final : public LCallableBase
    {
        ObjTy*     Object = nullptr;
        CallableTy Member = nullptr;

        FORCEINLINE LMemberCallable(ObjTy* InObject, CallableTy InMember) : Object(InObject), Member(InMember) { }

        FORCEINLINE RetTy Invoke(ParamsTy... InParams) override
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

    Smart::TUnique<LCallableBase> Callable = nullptr;
};

} /* ~Namespace Jafg */
