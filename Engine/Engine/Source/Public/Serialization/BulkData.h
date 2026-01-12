// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

template<typename TDomain>
class TBulkData final
{
public:

    using LDomain = TDomain;

    FORCEINLINE constexpr TBulkData() noexcept = default;
    PROHIBIT_COPY(TBulkData)
    inline constexpr TBulkData(TBulkData&& Other) noexcept
    {
        this->Number = Other.Number;
        this->Bulk = Other.Bulk;

        Other.Number = 0;
        Other.Bulk = nullptr;

        return;
    }
    inline constexpr TBulkData& operator=(TBulkData&& Rhs) noexcept
    {
        if (this != &Rhs)
        {
            if (this->IsAllocated())
            {
                this->Free();
            }

            this->Number = Rhs.Number;
            this->Bulk = Rhs.Bulk;
            Rhs.Number = 0;
            Rhs.Bulk = nullptr;
        }

        return *this;
    }
    inline constexpr ~TBulkData() noexcept
    {
        if (this->Bulk)
        {
            delete[] this->Bulk;
        }

        return;
    }

    FORCEINLINE constexpr bool IsAllocated() const noexcept { return this->Bulk != nullptr; }

    FORCEINLINE constexpr LSize GetNumber() const noexcept { return this->Number; }
    FORCEINLINE constexpr LSize GetByteSize() const noexcept { return this->Number * sizeof(LDomain); }

    FORCEINLINE constexpr LDomain* data() noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain const* data() const noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain& operator*() noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain const& operator*() const noexcept { return this->Bulk; }

    FORCEINLINE constexpr auto begin() noexcept { return this->Bulk; }
    FORCEINLINE constexpr auto begin() const noexcept { return this->Bulk; }
    FORCEINLINE constexpr auto end() noexcept { return this->Bulk + this->Number; }
    FORCEINLINE constexpr auto end() const noexcept { return this->Bulk + this->Number; }

    FORCEINLINE constexpr void Allocate(const LSize DomainNumber)
    {
        check( this->IsAllocated() == false )
        check( DomainNumber > 0 )

        this->Number = DomainNumber;
        this->Bulk = new LDomain[this->Number];

        return;
    }

    FORCEINLINE void AllocateZeroed(const LSize DomainNumber)
    {
        this->Allocate(DomainNumber);
        std::memset(this->Bulk, 0, this->GetByteSize());
        return;
    }

    FORCEINLINE void Serialize(LDomain const* InBulk, const LSize DomainNumber, const LSize InOffset = 0)
    {
        check( this->IsAllocated() == false )
        check( InBulk && DomainNumber > 0 )

        this->Allocate(DomainNumber);
        std::memcpy(this->Bulk, InBulk + InOffset, this->GetByteSize());

        return;

    }

    inline constexpr void Free() noexcept
    {
        check( this->Bulk )

        delete[] this->Bulk;
        this->Number = {};
        this->Bulk = {};

        check( this->Number == 0 && this->Bulk == nullptr )

        return;
    }

private:

    LSize Number{};
    LDomain* Bulk{};
};

typedef TBulkData<u8>  LByteBulkData;
typedef TBulkData<f32> LfBulkData;
typedef TBulkData<f64> LdBulkData;

} /* ~Namespace Jafg */
