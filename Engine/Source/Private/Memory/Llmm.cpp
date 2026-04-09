// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"

JAFG_LLMM_NEW_DEL_OPS_REPLACEMENTS

namespace Llmm
{

void* Alloc(std::size_t Size, std::align_val_t) noexcept
{
    return std::malloc(Size);
}

void Free(void* Ptr) noexcept
{
    std::free(Ptr);
}

void* Realloc(void* Ptr, std::size_t NewSize, std::align_val_t) noexcept
{
    return std::realloc(Ptr, NewSize);
}

} /* ~Namespace Llmm */
