// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/RhiVendorInclude.h"

namespace Jafg
{

//#
//# Raii wrapper for vma allocations.
//#
struct LVmaBuffer
{
    constexpr LVmaBuffer() noexcept : Buffer{nullptr} , Allocation{nullptr} { }
    constexpr LVmaBuffer(vk::Buffer InBuffer, VmaAllocation InAllocation) noexcept : Buffer{InBuffer} , Allocation{InAllocation} { }
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
            this->Buffer = Other.Buffer;
            this->Allocation = Other.Allocation;

            Other.Buffer = nullptr;
            Other.Allocation = nullptr;
        }

        return *this;
    }
    ~LVmaBuffer() noexcept { this->FreeImpl(); }

    void Free() noexcept
    {
        this->FreeImpl();
        this->Buffer = nullptr;
        this->Allocation = nullptr;

        return;
    }

    vk::Buffer Buffer;
    VmaAllocation Allocation;

private:

    ENGINE_API void FreeImpl() noexcept;
};

//#
//# Struct that holds both a vertex and an index buffer.
//#
struct LVmaIndexVertexBuffer
{
    //# TODO Make this struct that can hold to buffers in one single vk::buffer? for better cache locality?
    // https://developer.nvidia.com/vulkan-memory-management
};

} /* ~Namespace Jafg */
