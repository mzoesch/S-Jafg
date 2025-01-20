// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !WITH_VIRTUAL_FILESYSTEM
    #error "This file should only be included when WITH_VIRTUAL_FILESYSTEM is defined."
#endif /* !WITH_VIRTUAL_FILESYSTEM */

#include "System/SystemForward.h"

PRAGMA_FOR_JAFG_BUILD_TOOL("MakeVirtualFilesystem")

namespace Jafg
{

class JUserPreferences;
class LVirtualFileSystem;

namespace Private
{

struct LVirtualFile;

} /* ~Namespace Private */

//#
//# Singleton class that manages the virtual filesystem.
//#
extern ENGINE_API LVirtualFileSystem* GVirtualFileSystem;

class LVirtualFileSystem final
{
public:

    ENGINE_API LVirtualFileSystem();
    PROHIBIT_REALLOC_OF_ANY_FORM(LVirtualFileSystem)
    ENGINE_API ~LVirtualFileSystem();

    ENGINE_API auto GetTotalEmbeddedFileCount() const -> int32;

    ENGINE_API auto DoesFileExist(const LEnginePath& InEnginePath) const -> bool;
    ENGINE_API auto ReadFileAsString(const LEnginePath& InEnginePath) const -> LStringLegacy;
    //#
    //# Modifying the raw data is in no case recommended. Better make a copy of it,
    //# or even better use the save jafg array type.
    //#
    ENGINE_API auto ReadFileAsBytes(const LEnginePath& InEnginePath, const uint8*& OutBuffer, uint64& OutBufferOverflowGuard) const -> void;
    ENGINE_API auto ReadFileAsJafgByteArray(const LEnginePath& InEnginePath) const -> TdhArray<uint8>;

    ENGINE_API auto FindFiles(
        const EEnginePaths::Type InEnginePathTy,
        const JUserPreferences& InUserPreferences,
        const bool bKeepExtension = false,
        const LSimpleString& InFileExtension = ".*"
    ) -> TdhArray<LSimpleString>;

private:

    Private::LVirtualFile* GetVirtualFileHandle(const LEnginePath& InEnginePath) const;
    Private::LVirtualFile* GetCheckedVirtualFileHandle(const LEnginePath& InEnginePath) const;
    Private::LVirtualFile* GetPanickedVirtualFileHandle(const LEnginePath& InEnginePath) const;
};

} /* ~Namespace Jafg */
