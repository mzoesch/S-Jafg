// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace rhi
{

namespace detail
{

ENGINE_API void free_device_allocation(vk::Buffer Handle, device_allocation Allocation) noexcept;
ENGINE_API void free_device_allocation(vk::Image Handle, device_allocation Allocation) noexcept;

} /* ~Namespace detail */

//# A generic buffer located on the device.
template<typename T>
struct generic_device_buffer
{
    static_assert(sizeof(T) == algo::ptr_size);

    constexpr generic_device_buffer() noexcept : buffer{nullptr}, allocation{nullptr} {}
    constexpr generic_device_buffer(T InBuffer, device_allocation InAllocation) noexcept : buffer{InBuffer}, allocation{InAllocation} {}
    PROHIBIT_COPY(generic_device_buffer)
    constexpr generic_device_buffer(generic_device_buffer&& Other) noexcept
        : buffer{std::exchange(Other.buffer, nullptr)}, allocation{std::exchange(Other.allocation, nullptr)} {}
    generic_device_buffer& operator=(generic_device_buffer&& Rhs) noexcept
    {
        check(this != &Rhs)
        detail::free_device_allocation(this->buffer, this->allocation);
        this->buffer = std::exchange(Rhs.buffer, nullptr);
        this->allocation = std::exchange(Rhs.allocation, nullptr);
        return *this;
    }
    ~generic_device_buffer() noexcept { detail::free_device_allocation(this->buffer, this->allocation); }

    NODISCARD FORCEINLINE constexpr T operator*() const noexcept { return this->buffer; }
    NODISCARD FORCEINLINE constexpr T* operator->() noexcept { return &this->buffer; }
    NODISCARD FORCEINLINE constexpr T const* operator->() const noexcept { return &this->buffer; }
    NODISCARD FORCEINLINE constexpr T get_buffer() const noexcept { return this->buffer; }
    NODISCARD FORCEINLINE constexpr device_allocation get_allocation() const noexcept { return this->allocation; }

    constexpr void release() noexcept
    {
        this->buffer = nullptr;
        this->allocation = nullptr;
    }

    void free() noexcept
    {
        detail::free_device_allocation(this->buffer, this->allocation);
        this->buffer = nullptr;
        this->allocation = nullptr;
    }

private:

    T buffer;
    device_allocation allocation;
};

typedef generic_device_buffer<vk::Buffer> device_buffer;
typedef generic_device_buffer<vk::Image> device_image;

//# A device buffer with detailed allocation info.
struct detailed_device_buffer final : private device_buffer
{
    constexpr detailed_device_buffer() noexcept
        : device_buffer{}, info{} {}
    constexpr detailed_device_buffer(vk::Buffer Buffer, device_allocation Allocation, device_allocation_info Info) noexcept
        : device_buffer{Buffer, Allocation}, info{Info} {}
    PROHIBIT_COPY(detailed_device_buffer)
    constexpr detailed_device_buffer(detailed_device_buffer&& Other) noexcept
        : device_buffer{std::move(Other)}, info{std::exchange(Other.info, {})} {}
    detailed_device_buffer& operator=(detailed_device_buffer&& Rhs) noexcept
    {
        check(this != &Rhs)
        this->info = std::exchange(Rhs.info, {});
        device_buffer::operator=(std::move(Rhs));
        return *this;
    }

    NODISCARD FORCEINLINE constexpr vk::Buffer operator*() const noexcept { return device_buffer::operator*(); }
    NODISCARD FORCEINLINE constexpr vk::Buffer get_buffer() const noexcept { return device_buffer::get_buffer(); }
    NODISCARD FORCEINLINE constexpr device_allocation get_allocation() const noexcept { return device_buffer::get_allocation(); }
    NODISCARD FORCEINLINE constexpr device_allocation_info const& get_allocation_info() const noexcept { return this->info; }

    constexpr void release() noexcept
    {
        device_buffer::release();
        this->info = {};
    }

    void free() noexcept
    {
        device_buffer::free();
        this->info = {};
    }

private:

    device_allocation_info info;
};

//# A device buffer that is mapped to host visible memory.
struct mapped_device_buffer final : private device_buffer
{
    constexpr mapped_device_buffer() noexcept
        : device_buffer{}, m_data{nullptr} {}
    constexpr mapped_device_buffer(vk::Buffer Buffer, device_allocation Allocation, void* Data) noexcept
        : device_buffer{Buffer, Allocation}, m_data{Data} {}
    PROHIBIT_COPY(mapped_device_buffer)
    constexpr mapped_device_buffer(mapped_device_buffer&& Other) noexcept
        : device_buffer{std::move(Other)}, m_data{std::exchange(Other.m_data, nullptr)} {}
    mapped_device_buffer& operator=(mapped_device_buffer&& Other) noexcept
    {
        check(this != &Other)
        device_buffer::operator=(std::move(Other));
        this->m_data = std::exchange(Other.m_data, nullptr);
        return *this;
    }

    NODISCARD FORCEINLINE constexpr vk::Buffer operator*() const noexcept { return device_buffer::operator*(); }
    NODISCARD FORCEINLINE constexpr vk::Buffer get_buffer() const noexcept { return device_buffer::get_buffer(); }
    NODISCARD FORCEINLINE constexpr device_allocation get_allocation() const noexcept { return device_buffer::get_allocation(); }
    NODISCARD FORCEINLINE constexpr void* data() const noexcept { return this->m_data; }

    constexpr void release() noexcept
    {
        device_buffer::release();
        this->m_data = nullptr;
    }

    void free() noexcept
    {
        device_buffer::free();
        this->m_data = nullptr;
    }

private:

    void* m_data;
};

//# A device buffer holing vertices and indices.
struct LDeviceIndexVertexBuffer
{
    //# TODO Make this struct that can hold to buffers in one single vk::buffer? for better cache locality?
    // https://developer.nvidia.com/vulkan-memory-management
};

} /* ~Namespace rhi */
