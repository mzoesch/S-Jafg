// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

namespace Detail
{

ENGINE_API void FreeDeviceAllocation(vk::Buffer Handle, rhi::device_allocation Allocation) noexcept;
ENGINE_API void FreeDeviceAllocation(vk::Image Handle, rhi::device_allocation Allocation) noexcept;

} /* ~Namespace Jafg::Detail */

//# A generic buffer located on the device.
template<typename T>
struct TGenericDeviceBuffer
{
    static_assert(sizeof(T) == POINTER_BYTE_SIZE);

    constexpr TGenericDeviceBuffer() noexcept : Buffer{nullptr}, Allocation{nullptr} {}
    constexpr TGenericDeviceBuffer(T InBuffer, rhi::device_allocation InAllocation) noexcept : Buffer{InBuffer}, Allocation{InAllocation} {}
    PROHIBIT_COPY(TGenericDeviceBuffer)
    constexpr TGenericDeviceBuffer(TGenericDeviceBuffer&& Other) noexcept : Buffer{ Other.Buffer }, Allocation{ Other.Allocation }
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
    }
    TGenericDeviceBuffer& operator=(TGenericDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);

        this->Buffer = Other.Buffer;
        this->Allocation = Other.Allocation;

        Other.Buffer = nullptr;
        Other.Allocation = nullptr;

        return *this;
    }
    ~TGenericDeviceBuffer() noexcept { Detail::FreeDeviceAllocation(this->Buffer, this->Allocation); }

    FORCEINLINE constexpr T GetBuffer() const noexcept { return this->Buffer; }
    FORCEINLINE constexpr T operator*() const noexcept { return this->Buffer; }
    FORCEINLINE constexpr rhi::device_allocation GetAllocation() const noexcept { return this->Allocation; }

    constexpr inline void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

    inline void Free() noexcept
    {
        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

private:

    T Buffer;
    rhi::device_allocation Allocation;
};

typedef TGenericDeviceBuffer<vk::Buffer> LDeviceBuffer;
typedef TGenericDeviceBuffer<vk::Image> LDeviceImage;

//# A device buffer with detailed allocation info.
struct LDetailedDeviceBuffer final : private LDeviceBuffer
{
    constexpr LDetailedDeviceBuffer() noexcept
        : LDeviceBuffer{}, Info{} {}
    constexpr LDetailedDeviceBuffer(vk::Buffer InBuffer, rhi::device_allocation InAllocation, rhi::device_allocation_info&& InInfo) noexcept
        : LDeviceBuffer{ InBuffer, InAllocation }, Info{ std::move(InInfo) } {}
    PROHIBIT_COPY(LDetailedDeviceBuffer)
    constexpr LDetailedDeviceBuffer(LDetailedDeviceBuffer&& Other) noexcept
        : LDeviceBuffer{ std::move(Other) }, Info{ Other.Info }
    {
        Other.Info = {};
    }
    LDetailedDeviceBuffer& operator=(LDetailedDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        LDeviceBuffer::operator=(std::move(Other));
        this->Info = Other.Info;
        Other.Info = {};

        return *this;
    }

    FORCEINLINE constexpr vk::Buffer GetBuffer() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr vk::Buffer operator*() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr rhi::device_allocation GetAllocation() const noexcept { return LDeviceBuffer::GetAllocation(); }
    FORCEINLINE constexpr rhi::device_allocation_info const& GetAllocationInfo() const noexcept { return this->Info; }

    constexpr inline void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Info = {};
    }

    inline void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Info = {};
    }

private:

    rhi::device_allocation_info Info;
};

//# A device buffer that is mapped to host visible memory.
struct LMappedDeviceBuffer final : private LDeviceBuffer
{
    constexpr LMappedDeviceBuffer() noexcept
        : LDeviceBuffer{}, Data{nullptr} {}
    constexpr LMappedDeviceBuffer(vk::Buffer InBuffer, rhi::device_allocation InAllocation, void* InData) noexcept
        : LDeviceBuffer{ InBuffer, InAllocation }, Data{ InData } {}
    PROHIBIT_COPY(LMappedDeviceBuffer)
    constexpr LMappedDeviceBuffer(LMappedDeviceBuffer&& Other) noexcept
        : LDeviceBuffer{ std::move(Other) }, Data{ Other.Data }
    {
        Other.Data = nullptr;
    }
    LMappedDeviceBuffer& operator=(LMappedDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        LDeviceBuffer::operator=(std::move(Other));
        this->Data = Other.Data;
        Other.Data = nullptr;

        return *this;
    }

    FORCEINLINE constexpr vk::Buffer GetBuffer() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr vk::Buffer operator*() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr rhi::device_allocation GetAllocation() const noexcept { return LDeviceBuffer::GetAllocation(); }
    FORCEINLINE constexpr void* GetData() const noexcept { return this->Data; }

    constexpr inline void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Data = nullptr;
    }

    inline void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Data = nullptr;
    }

private:

    void* Data;
};

//# A device buffer holing vertices and indices.
struct LDeviceIndexVertexBuffer
{
    //# TODO Make this struct that can hold to buffers in one single vk::buffer? for better cache locality?
    // https://developer.nvidia.com/vulkan-memory-management
};

} /* ~Namespace Jafg */
