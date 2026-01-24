// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Core/Name.h"
#include "Foreign/PluginForward.h"

namespace Jafg
{

//#
//# A reflected record.
//#
class LCxxRecord
{
public:

    constexpr LCxxRecord() noexcept = delete;
    constexpr LCxxRecord(LString InFullyQualifiedName) noexcept
        : FullyQualifiedName(std::move(InFullyQualifiedName))
    {
        check( this->FullyQualifiedName.empty() == false )
        this->Name = MAKE_NAME(this->FullyQualifiedName);
        check( this->Name.IsSet() )

        return;
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxRecord)
    ~LCxxRecord() noexcept = default;

    FORCEINLINE constexpr LName GetName() const noexcept { return this->Name; }
    FORCEINLINE constexpr LString const& GetFullyQualifiedName() const noexcept { return this->FullyQualifiedName; }

    FORCEINLINE constexpr bool IsPluginHandleValid() const noexcept { return this->PluginHandle.IsValid(); }
    FORCEINLINE constexpr LLoadedPluginHandle GetPluginHandle() const noexcept { return this->PluginHandle; }

private:

    LName   Name;
    LString FullyQualifiedName;

    LLoadedPluginHandle PluginHandle;
};

} /* ~Namespace Jafg */
