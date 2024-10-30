// Copyright mzoesch. All rights reserved.

#pragma once

#include <functional>

namespace Jafg
{

namespace Private
{

template <bool bAllowNull>
struct TFunctionStoragePolicy
{
    FORCEINLINE static constexpr bool CanBeNull() { return bAllowNull; }
};

template <typename StoragePolicy, typename FunctorTy>
struct TFunctionBase;

template <typename StoragePolicy, typename RetTy, typename ... ParamsTy>
struct TFunctionBase<StoragePolicy, RetTy (ParamsTy ...)>
{
    template <typename OtherStoragePolicy, typename OtherFunctorTy>
    friend struct TFunctionBase;

    FORCEINLINE static constexpr auto NumParams() -> int32 { return sizeof ... (ParamsTy); }

    TFunctionBase() = default;
    ~TFunctionBase() = default;

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunctionBase(InOtherFunctorTy&  Other) noexcept : Functor(Other) { }
    template <typename InOtherFunctorTy>
    FORCEINLINE auto operator=(InOtherFunctorTy& Other) noexcept -> TFunctionBase&
    {
        this->Functor = Other;
        return *this;
    }

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunctionBase(const InOtherFunctorTy&  Other) noexcept : Functor(Other) { }
    template <typename InOtherFunctorTy>
    FORCEINLINE auto operator=(const InOtherFunctorTy& Other) noexcept -> TFunctionBase&
    {
        this->Functor = Other;
        return *this;
    }

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunctionBase(InOtherFunctorTy&& Other) noexcept
    {
        this->Functor = std::forward<InOtherFunctorTy>(Other);
        return;
    }
    template <typename InOtherFunctorTy>
    FORCEINLINE TFunctionBase& operator=(InOtherFunctorTy&& Other) noexcept
    {
        this->Functor = std::forward<InOtherFunctorTy>(Other);
        return *this;
    }

    FORCEINLINE auto Call(ParamsTy... Params)       -> RetTy { return this->Functor(Params ...); }
    FORCEINLINE auto operator()(ParamsTy... Params) -> RetTy
    {
#if DO_CHECKS
        this->CheckForValidCall();
#endif /* DO_CHECKS */

        return this->Functor(Params ...);
    }

    FORCEINLINE auto operator==(LNullptrTy) const -> bool { return this->IsSet() == false; }
    FORCEINLINE auto operator!=(LNullptrTy) const -> bool { return this->IsSet();          }

    FORCEINLINE void CheckForValidCall() const { check( IsSet() ) return; }
    FORCEINLINE bool IsSet() const
    {
        if constexpr (StoragePolicy::CanBeNull() == false)
        {
            if (this->Functor == nullptr)
            {
                panic( "Found nullptr to function reference but nullptr is prohibited." )
            }
        }

        return this->Functor != nullptr;
    }

    FORCEINLINE void Reset()
    {
        if constexpr (StoragePolicy::CanBeNull() == false)
        {
            panic( "Cannot reset a function reference that cannot be null." )
        }

        this->Functor = nullptr;

        return;
    }

protected:

    /** The functor to operate on. */
    /*
     * Currently is this whole thing just a wrapper around the std library's std::function. But we should
     * implement our own implementation just for fun.
     */
    std::function<RetTy(ParamsTy& ...)> Functor = nullptr;
};

} /* ~Namespace Private */

/**
 * A struct that stores a function.
 */
template <typename FunctorTy>
struct TFunction final : public Private::TFunctionBase<Private::TFunctionStoragePolicy<true>, FunctorTy>
{
private:

    using Super = Private::TFunctionBase<Private::TFunctionStoragePolicy<true>, FunctorTy>;

public:

    TFunction(LNullptrTy = nullptr) { }

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunction(InOtherFunctorTy& Other) noexcept : Super(Other) { }
    FORCEINLINE TFunction& operator=(TFunction& Other) noexcept
    {
        this->Functor = Other.Functor;
        return *this;
    }

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunction(const InOtherFunctorTy& Other) noexcept : Super(Other) { }
    FORCEINLINE TFunction& operator=(const TFunction& Other) noexcept
    {
        this->Functor = Other.Functor;
        return *this;
    }

    template <typename InOtherFunctorTy>
    FORCEINLINE TFunction(InOtherFunctorTy&& Other) noexcept : Super(std::forward<InOtherFunctorTy>(Other)) { }
    FORCEINLINE TFunction& operator=(TFunction&& Other) noexcept
    {
        this->Functor = Other.Functor;
        Other.Reset();
        return *this;
    }

    template <typename InMemberFunc, typename InObj>
    FORCEINLINE void BindMember(InMemberFunc InMemberFuncPtr, InObj InObjPtr)
    {
        if constexpr (Super::NumParams() == 0)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr);
        }
        else if constexpr (Super::NumParams() == 1)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1
            );
        }
        else if constexpr (Super::NumParams() == 2)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2
            );
        }
        else if constexpr (Super::NumParams() == 3)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
            );
        }
        else if constexpr (Super::NumParams() == 4)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
            );
        }
        else if constexpr (Super::NumParams() == 5)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5
            );
        }
        else if constexpr (Super::NumParams() == 6)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6
            );
        }
        else if constexpr (Super::NumParams() == 7)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7
            );
        }
        else if constexpr (Super::NumParams() == 8)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8
            );
        }
        else if constexpr (Super::NumParams() == 9)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9
            );
        }
        else if constexpr (Super::NumParams() == 10)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10
            );
        }
        else if constexpr (Super::NumParams() == 11)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11
            );
        }
        else if constexpr (Super::NumParams() == 12)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12
            );
        }
        else if constexpr (Super::NumParams() == 13)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13
            );
        }
        else if constexpr (Super::NumParams() == 14)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14
            );
        }
        else if constexpr (Super::NumParams() == 15)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15
            );
        }
        else if constexpr (Super::NumParams() == 16)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16
            );
        }
        else if constexpr (Super::NumParams() == 17)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
                std::placeholders::_17
            );
        }
        else if constexpr (Super::NumParams() == 18)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
                std::placeholders::_17, std::placeholders::_18
            );
        }
        else if constexpr (Super::NumParams() == 19)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
                std::placeholders::_17, std::placeholders::_18, std::placeholders::_19
            );
        }
        else if constexpr (Super::NumParams() == 20)
        {
            this->Functor = std::bind(InMemberFuncPtr, InObjPtr,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
                std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
                std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
                std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, std::placeholders::_20
            );
        }
        else
        {
            panic( "Too many parameters for function binding. Support for more than 20 parameters is not implemented." )
        }

        return;
    }

    ~TFunction() = default;
};

/**
 * A struct that stores a function that is unique and can therefore only be moved.
 */
template <typename FunctorTy>
struct TUniqueFunction final : public Private::TFunctionBase<Private::TFunctionStoragePolicy<true>, FunctorTy>
{
private:

    using Super = Private::TFunctionBase<Private::TFunctionStoragePolicy<true>, FunctorTy>;

public:

    FORCEINLINE TUniqueFunction(LNullptrTy = nullptr)
    {
        this->Functor = nullptr;
        return;
    }

    PROHIBIT_COPY(TUniqueFunction)

    template <typename InOtherFunctorTy>
    FORCEINLINE TUniqueFunction(InOtherFunctorTy&& Other) noexcept : Super(std::forward<InOtherFunctorTy>(Other)) { }
    FORCEINLINE TUniqueFunction& operator=(TUniqueFunction&& Other) noexcept
    {
        this->Functor = Other.Functor;
        Other.Reset();
        return *this;
    }

    ~TUniqueFunction() = default;
};

} /* ~Namespace Jafg */
