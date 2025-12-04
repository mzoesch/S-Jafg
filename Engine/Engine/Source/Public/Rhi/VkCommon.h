// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/RhiVendorInclude.h"

namespace Jafg
{

//# Frees memory allocated by vma.
ENGINE_API void FreeVmaAllocation(VkBuffer Buffer, VmaAllocation Allocation);
ENGINE_API void FreeVmaImage(VkImage Image, VmaAllocation Allocation);

//# Raii wrapper for vma allocations.
struct LVmaBuffer
{
    constexpr LVmaBuffer() noexcept : Buffer{nullptr} , Allocation{nullptr} {}
    constexpr LVmaBuffer(vk::Buffer InBuffer, VmaAllocation InAllocation) noexcept : Buffer{InBuffer} , Allocation{InAllocation} {}
    PROHIBIT_COPY(LVmaBuffer)
    constexpr LVmaBuffer(LVmaBuffer&& Other) noexcept
        : Buffer{ Other.Buffer }
        , Allocation{ Other.Allocation }
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
    }
    LVmaBuffer& operator=(LVmaBuffer&& Other) noexcept
    {
        if (this != &Other)
        {
            Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);

            this->Buffer = Other.Buffer;
            this->Allocation = Other.Allocation;

            Other.Buffer = nullptr;
            Other.Allocation = nullptr;
        }

        return *this;
    }
    ~LVmaBuffer() noexcept { Jafg::FreeVmaAllocation(this->Buffer, this->Allocation); }

    void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;

        return;
    }

    void Free() noexcept
    {
        Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;

        return;
    }

    vk::Buffer Buffer;
    VmaAllocation Allocation;
};

//# Raii wrapper for vma allocation with additional info.
struct LVmaDetailedBuffer
{
    constexpr LVmaDetailedBuffer() noexcept
        : Buffer{nullptr} , Allocation{nullptr}, Info{} { check( this->Info.pMappedData == nullptr ) }
    constexpr LVmaDetailedBuffer(vk::Buffer InBuffer, VmaAllocation InAllocation, VmaAllocationInfo&& InInfo) noexcept
        : Buffer{InBuffer} , Allocation{InAllocation}, Info(std::move(InInfo)) { check( InInfo.pMappedData == nullptr ) }
    PROHIBIT_COPY(LVmaDetailedBuffer)
    constexpr LVmaDetailedBuffer(LVmaDetailedBuffer&& Other) noexcept
        : Buffer{ Other.Buffer }, Allocation{ Other.Allocation }, Info(std::move(Other.Info))
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
        Other.Info = {};
        check( Other.Info.pMappedData == nullptr )
    }
    LVmaDetailedBuffer& operator=(LVmaDetailedBuffer&& Other) noexcept
    {
        if (this != &Other)
        {
            Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);

            this->Buffer = Other.Buffer;
            this->Allocation = Other.Allocation;
            this->Info = std::move(Other.Info);

            Other.Buffer = nullptr;
            Other.Allocation = nullptr;
            check( Other.Info.pMappedData == nullptr )
        }

        return *this;
    }
    ~LVmaDetailedBuffer() noexcept { Jafg::FreeVmaAllocation(this->Buffer, this->Allocation); }

    void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;
        this->Info = {};

        return;
    }

    void Free() noexcept
    {
        Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;
        this->Info = {};
        check( this->Info.pMappedData == nullptr )

        return;
    }

    vk::Buffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationInfo Info;
};

//# Raii wrapper for a mapped vma allocation.
struct LVmaMappedBuffer
{
    constexpr LVmaMappedBuffer() noexcept
        : Buffer{nullptr} , Allocation{nullptr}, Data{nullptr} {}
    constexpr LVmaMappedBuffer(vk::Buffer InBuffer, VmaAllocation InAllocation, void* InData) noexcept
        : Buffer{InBuffer} , Allocation{InAllocation}, Data{InData} {}
    PROHIBIT_COPY(LVmaMappedBuffer)
    constexpr LVmaMappedBuffer(LVmaMappedBuffer&& Other) noexcept
        : Buffer{ Other.Buffer }, Allocation{ Other.Allocation }, Data{ Other.Data }
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
        Other.Data = nullptr;
    }
    LVmaMappedBuffer& operator=(LVmaMappedBuffer&& Other) noexcept
    {
        if (this != &Other)
        {
            Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);

            this->Buffer = Other.Buffer;
            this->Allocation = Other.Allocation;
            this->Data = Other.Data;

            Other.Buffer = nullptr;
            Other.Allocation = nullptr;
            Other.Data = nullptr;
        }

        return *this;
    }
    ~LVmaMappedBuffer() noexcept { Jafg::FreeVmaAllocation(this->Buffer, this->Allocation); }

    void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;
        this->Data = nullptr;

        return;
    }

    void Free() noexcept
    {
        Jafg::FreeVmaAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;
        this->Data = nullptr;

        return;
    }

    vk::Buffer Buffer;
    VmaAllocation Allocation;
    void* Data;
};

//#
//# Struct that holds both a vertex and an index buffer.
//#
struct LVmaIndexVertexBuffer
{
    //# TODO Make this struct that can hold to buffers in one single vk::buffer? for better cache locality?
    // https://developer.nvidia.com/vulkan-memory-management
};

//# Raii wrapper for a device local image with vma allocation.
struct LVmaImage
{
    constexpr LVmaImage() noexcept
        : Image{nullptr} , Allocation{nullptr} {}
    constexpr LVmaImage(vk::Image InImage, VmaAllocation InAllocation) noexcept
        : Image{InImage} , Allocation{InAllocation} {}
    PROHIBIT_COPY(LVmaImage)
    constexpr LVmaImage(LVmaImage&& Other) noexcept
        : Image{ Other.Image }, Allocation{ Other.Allocation }
    {
        Other.Image = nullptr;
        Other.Allocation = nullptr;
    }
    LVmaImage& operator=(LVmaImage&& Other) noexcept
    {
        if (this != &Other)
        {
            Jafg::FreeVmaImage(this->Image, this->Allocation);

            this->Image = Other.Image;
            this->Allocation = Other.Allocation;

            Other.Image = nullptr;
            Other.Allocation = nullptr;
        }

        return *this;
    }
    ~LVmaImage() noexcept { Jafg::FreeVmaImage(this->Image, this->Allocation); }

    void Release() noexcept
    {
        this->Image = nullptr;
        this->Allocation = nullptr;

        return;
    }

    void Free() noexcept
    {
        Jafg::FreeVmaImage(this->Image, this->Allocation);
        this->Image = nullptr;
        this->Allocation = nullptr;

        return;
    }

    vk::Image Image;
    VmaAllocation Allocation;
};

} /* ~Namespace Jafg */
