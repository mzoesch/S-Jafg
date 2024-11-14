// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

template <typename InDomainTy>
class LBulkData
{
public:

    using LBulkDomainTy = InDomainTy;

    FORCEINLINE LBulkData() = default;
    PROHIBIT_COPY(LBulkData)
    FORCEINLINE LBulkData(LBulkData&& InOther) noexcept;
    FORCEINLINE LBulkData& operator=(LBulkData&& InOther) noexcept;
    FORCEINLINE ~LBulkData();

    FORCEINLINE bool IsAllocated() const { return this->Bulk != nullptr; }

    FORCEINLINE auto GetNum() const -> int32 { return this->Num; }
    FORCEINLINE auto GetByteSize() const -> int32 { return this->Num * sizeof(LBulkDomainTy); }
    FORCEINLINE auto GetBulk()       ->       LBulkDomainTy* { return this->Bulk; }
    FORCEINLINE auto GetBulk() const -> const LBulkDomainTy* { return this->Bulk; }

    inline void AllocateBulk(const int32 InNumberOfDomains, const bool bZeroed = false);
    inline void Serialize(const LBulkDomainTy* const InBulk, const int32 InNumberOfDomains, const int32 InOffset = 0);
    inline void FreeBulk();

private:

    /** The number of domains. */
    int32          Num  = 0;
    LBulkDomainTy* Bulk = nullptr;
};

template <typename InDomainTy>
LBulkData<InDomainTy>::LBulkData(LBulkData&& InOther) noexcept
{
    this->Num = InOther.Num;
    this->Bulk = InOther.Bulk;
    InOther.Num = 0;
    InOther.Bulk = nullptr;

    return;
}

template <typename InDomainTy>
LBulkData<InDomainTy>& LBulkData<InDomainTy>::operator=(LBulkData&& InOther) noexcept
{
    if (this != &InOther)
    {
        this->Num = InOther.Num;
        this->Bulk = InOther.Bulk;
        InOther.Num = 0;
        InOther.Bulk = nullptr;
    }

    return *this;
}

template <typename InDomainTy>
LBulkData<InDomainTy>::~LBulkData()
{
    if (this->IsAllocated())
    {
        this->FreeBulk();
    }

    return;
}

template <typename InDomainTy>
void LBulkData<InDomainTy>::AllocateBulk(const int32 InNumberOfDomains, const bool bZeroed /* = false */)
{
    if (this->Bulk)
    {
        panic( "Tried to allocate bulk data that has bulk storage already." )
        return;
    }

    jassert( InNumberOfDomains > 0 )

    this->Num = InNumberOfDomains;
    this->Bulk = new LBulkDomainTy[InNumberOfDomains];

    if (bZeroed)
    {
        ::memset(this->Bulk, 0, this->GetByteSize());
    }

    return;
}

template <typename InDomainTy>
void LBulkData<InDomainTy>::Serialize(const LBulkDomainTy* InBulk, const int32 InNumberOfDomains, const int32 InOffset /* = 0 */)
{
    if (this->Bulk)
    {
        panic( "Tried to serialize bulk data that has bulk storage already." )
        return;
    }

    jassert( InNumberOfDomains > 0 )

    this->Num = InNumberOfDomains;

    InBulk += InOffset;
    this->Bulk = new LBulkDomainTy[InNumberOfDomains];

    ::memcpy(this->Bulk, InBulk, this->GetByteSize());

    return;
}

template <typename InDomainTy>
void LBulkData<InDomainTy>::FreeBulk()
{
    check( this->Bulk )

    if (this->Bulk)
    {
        delete[] this->Bulk;
        this->Bulk = nullptr;
        this->Num = 0;
    }

    check( this->Num == 0 )

    return;
}

using LByteBulkData = LBulkData<uint8>;
using LfBulkData    = LBulkData<float>;
using LdBulkData    = LBulkData<double>;

} /* ~Namespace Jafg */
