// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef JAFG_PLATFORM_NEW_OP_THROW_SPEC
    #define JAFG_PLATFORM_NEW_OP_THROW_SPEC
#endif /* !JAFG_PLATFORM_NEW_OP_THROW_SPEC */

namespace Llmm
{

ENGINE_API void* Alloc(std::size_t, std::align_val_t) noexcept;
ENGINE_API void  Free(void*) noexcept;
ENGINE_API void* Realloc(void*, std::size_t, std::align_val_t) noexcept;

} /* ~Namespace Llmm */

#define JAFG_LLMM_NEW_DEL_OPS_REPLACEMENTS \
    void* operator new  (std::size_t Size                                               ) JAFG_PLATFORM_NEW_OP_THROW_SPEC { return Llmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new[](std::size_t Size                                               ) JAFG_PLATFORM_NEW_OP_THROW_SPEC { return Llmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new  (std::size_t Size,                         std::nothrow_t const&) noexcept                        { return Llmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new[](std::size_t Size,                         std::nothrow_t const&) noexcept                        { return Llmm::Alloc(Size, std::align_val_t{ alignof(std::max_align_t) }); } \
    void* operator new  (std::size_t Size, std::align_val_t Align                       ) JAFG_PLATFORM_NEW_OP_THROW_SPEC { return Llmm::Alloc(Size, Align); } \
    void* operator new[](std::size_t Size, std::align_val_t Align                       ) JAFG_PLATFORM_NEW_OP_THROW_SPEC { return Llmm::Alloc(Size, Align); } \
    void* operator new  (std::size_t Size, std::align_val_t Align, std::nothrow_t const&) noexcept                        { return Llmm::Alloc(Size, Align); } \
    void* operator new[](std::size_t Size, std::align_val_t Align, std::nothrow_t const&) noexcept                        { return Llmm::Alloc(Size, Align); } \
    void  operator delete  (void* Ptr                                                            )          { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr                                                            )          { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,                                      std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,                                      std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t                                               )          { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t                                               )          { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t,                         std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t,                         std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,              std::align_val_t Align                       )          { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,              std::align_val_t Align                       )          { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr,              std::align_val_t Align, std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr,              std::align_val_t Align, std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t, std::align_val_t Align                       )          { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t, std::align_val_t Align                       )          { Llmm::Free(Ptr); } \
    void  operator delete  (void* Ptr, std::size_t, std::align_val_t Align, std::nothrow_t const&) noexcept { Llmm::Free(Ptr); } \
    void  operator delete[](void* Ptr, std::size_t, std::align_val_t Align, std::nothrow_t const&) noexcept { Llmm::Free(Ptr); }
