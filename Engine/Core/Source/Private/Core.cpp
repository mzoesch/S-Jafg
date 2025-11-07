// Copyright mzoesch. All rights reserved.

#include <vector>
#include <string>
#include <mutex>
#include "Specs.h"
#include "RegisteredProgramArguments.h"
#include "Memory/Llmm.h"

namespace JafgCore
{

CORE_API bool bGAlreadyCrashed{ false };
CORE_API bool bGSuppressCrashDialog{ false };
CORE_API std::vector<Lal::LProgramParameter*> GRegisteredProgramArguments;

CORE_API std::mutex GLongLiquidLogsMutex;
/* This still exists even if LAL_SAVE_LOGS_IN_MEMORY but how would we access that macro here?? */
CORE_API std::vector<std::tuple<std::string, std::string>> GLongLiquidLogs;

} /* ~Namespace JafgCore */

void* LLlmm::Alloc(std::size_t Size, std::align_val_t) noexcept
{
    return std::malloc(Size);
}

void LLlmm::Free(void* Ptr) noexcept
{
    std::free(Ptr);
}

void* LLlmm::Realloc(void* Ptr, std::size_t NewSize, std::align_val_t) noexcept
{
    return std::realloc(Ptr, NewSize);
}
