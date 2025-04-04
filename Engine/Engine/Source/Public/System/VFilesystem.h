// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !WITH_VIRTUAL_FILESYSTEM && !PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION
    #error "This file should only be included when WITH_VIRTUAL_FILESYSTEM is defined."
#endif /* !WITH_VIRTUAL_FILESYSTEM && !PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION */

#include "System/SystemForward.h"

PRAGMA_FOR_JAFG_BUILD_TOOL("MakeVirtualFilesystem")

namespace Jafg
{

class JUserPreferences;
class LVirtualFileSystem;

namespace Private
{

struct LVirtualFile;

#if PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION || PRIVATE_JAFG_DEFINE_VIRTUAL_FILE
    struct LVirtualFile final
    {
        ENGINE_API LVirtualFile(const LPath& RelativeContentPath, const u8* InFileContents, const u64 InFileSize);
        PROHIBIT_COPY(LVirtualFile)
        DEFAULT_MOVE(LVirtualFile)
        ~LVirtualFile() = default;

        FORCEINLINE auto GetFileSize() const -> LuBigSizeTy { return this->FileSize; }
        FORCEINLINE auto GetRelativeContentPath() const -> const LPath& { return this->RelativeContentPath; }
        FORCEINLINE auto GetFileName() const -> LString { return this->RelativeContentPath.GetBase(); }
        FORCEINLINE auto GetBulk() const -> const u8* { return this->FileContents; }

    private:

        u64       FileSize;
        LPath        RelativeContentPath;
        const u8* FileContents;
    };
#endif /* PRIVATE_JAFG_INCLUDED_FROM_GENERATED_TRANSLATION || PRIVATE_JAFG_DEFINE_VIRTUAL_FILE */

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

    ENGINE_API i32 GetTotalEmbeddedFileCount() const;

    ENGINE_API bool DoesFileExist(const LEnginePath& InEnginePath) const;
    ENGINE_API auto ReadFileAsString(const LEnginePath& InEnginePath) const -> LString;
    ENGINE_API auto ReadFileAsStringLegacy(const LEnginePath& InEnginePath) const -> LStringLegacy;
    //#
    //# Modifying the raw data is in no case recommended. Better make a copy of it,
    //# or even better use the save jafg array type.
    //#
    ENGINE_API void ReadFileAsBytes(const LEnginePath& InEnginePath, const u8*& OutBuffer, u64& OutBufferOverflowGuard) const;
    ENGINE_API auto ReadFileAsJafgByteArray(const LEnginePath& InEnginePath) const -> TArray<u8>;

    ENGINE_API auto FindFiles(
        const EEnginePaths::Type InEnginePathTy,
        const JUserPreferences& InUserPreferences,
        const bool bKeepExtension = false,
        const LString& InFileExtension = ".*"
    ) -> TArray<LString>;

private:

    Private::LVirtualFile* GetVirtualFileHandle(const LEnginePath& InEnginePath) const;
    Private::LVirtualFile* GetCheckedVirtualFileHandle(const LEnginePath& InEnginePath) const;
    Private::LVirtualFile* GetPanickedVirtualFileHandle(const LEnginePath& InEnginePath) const;
};

} /* ~Namespace Jafg */
