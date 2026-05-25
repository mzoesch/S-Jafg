// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

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

    FORCEINLINE constexpr std::size_t GetNumber() const noexcept { return this->Number; }
    FORCEINLINE constexpr std::size_t GetByteSize() const noexcept { return this->Number * sizeof(LDomain); }

    FORCEINLINE constexpr LDomain* data() noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain const* data() const noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain& operator*() noexcept { return this->Bulk; }
    FORCEINLINE constexpr LDomain const& operator*() const noexcept { return this->Bulk; }

    FORCEINLINE constexpr auto begin() noexcept { return this->Bulk; }
    FORCEINLINE constexpr auto begin() const noexcept { return this->Bulk; }
    FORCEINLINE constexpr auto end() noexcept { return this->Bulk + this->Number; }
    FORCEINLINE constexpr auto end() const noexcept { return this->Bulk + this->Number; }

    FORCEINLINE constexpr LDomain& operator[](std::size_t Idx) noexcept
    {
        check(this->Bulk)
        check(Idx < this->Number)
        return this->Bulk[Idx];
    }
    FORCEINLINE constexpr LDomain const& operator[](std::size_t Idx) const noexcept
    {
        check(this->Bulk)
        check(Idx < this->Number)
        return this->Bulk[Idx];
    }

    FORCEINLINE constexpr void Allocate(const std::size_t DomainNumber)
    {
        check( this->IsAllocated() == false )
        check( DomainNumber > 0 )

        this->Number = DomainNumber;
        this->Bulk = new LDomain[this->Number];

        return;
    }

    FORCEINLINE void AllocateZeroed(const std::size_t DomainNumber)
    {
        this->Allocate(DomainNumber);
        std::memset(this->Bulk, 0, this->GetByteSize());
        return;
    }

    FORCEINLINE void Serialize(LDomain const* InBulk, const std::size_t DomainNumber, const std::size_t InOffset = 0)
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

    std::size_t Number{};
    LDomain* Bulk{};
};

typedef TBulkData<u8>  LByteBulkData;
typedef TBulkData<f32> LfBulkData;
typedef TBulkData<f64> LdBulkData;

} /* ~Namespace Jafg */
