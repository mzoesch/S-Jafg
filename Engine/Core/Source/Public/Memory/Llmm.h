// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_PLATFORM_NEW_OP_THROW_SPEC
    #define LAL_PLATFORM_NEW_OP_THROW_SPEC
#endif /* !LAL_PLATFORM_NEW_OP_THROW_SPEC */

struct LLlmm
{
    __attribute__((noinline)) CORE_API static void* Alloc(std::size_t, std::align_val_t) noexcept;
    __attribute__((noinline)) CORE_API static void  Free(void*) noexcept;
    __attribute__((noinline)) CORE_API static void* Realloc(void*, std::size_t, std::align_val_t) noexcept;
};

#define JAFG_LLMM_NEW_DEL_OPS_REPLACEMENTS \
    void* operator new  (std::size_t Size                                               ) LAL_PLATFORM_NEW_OP_THROW_SPEC { return LLlmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new[](std::size_t Size                                               ) LAL_PLATFORM_NEW_OP_THROW_SPEC { return LLlmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new  (std::size_t Size,                         std::nothrow_t const&) noexcept                       { return LLlmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new[](std::size_t Size,                         std::nothrow_t const&) noexcept                       { return LLlmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new  (std::size_t Size, std::align_val_t Align                       ) LAL_PLATFORM_NEW_OP_THROW_SPEC { return LLlmm::Alloc(Size, Align); } \
    void* operator new[](std::size_t Size, std::align_val_t Align                       ) LAL_PLATFORM_NEW_OP_THROW_SPEC { return LLlmm::Alloc(Size, Align); } \
    void* operator new  (std::size_t Size, std::align_val_t Align, std::nothrow_t const&) noexcept                       { return LLlmm::Alloc(Size, Align); } \
    void* operator new[](std::size_t Size, std::align_val_t Align, std::nothrow_t const&) noexcept                       { return LLlmm::Alloc(Size, Align); } \
    void  operator delete  (void* Ptr                                                            )          { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr                                                            )          { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,                                      std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,                                      std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t                                               )          { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t                                               )          { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t,                         std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t,                         std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,              std::align_val_t Align                       )          { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,              std::align_val_t Align                       )          { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,              std::align_val_t Align, std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,              std::align_val_t Align, std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t, std::align_val_t Align                       )          { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t, std::align_val_t Align                       )          { LLlmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t, std::align_val_t Align, std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t, std::align_val_t Align, std::nothrow_t const&) noexcept { LLlmm::Free(Ptr); }
