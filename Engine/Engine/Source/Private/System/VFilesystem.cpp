// Copyright mzoesch. All rights reserved.

#if WITH_VIRTUAL_FILESYSTEM

#include "System/VFilesystem.h"
#include "Engine/ObjectBaseUtility.h"
#include "System/EnginePath.h"
#include "System/Paths.h"
#include "User/UserPreferences.h"

namespace Jafg
{

ENGINE_API LVirtualFileSystem* GVirtualFileSystem;

} /* ~Namespace Jafg */

namespace Jafg::Private
{

struct LVirtualFile;
TdhArray<LVirtualFile>& GetVirtualFiles();

struct LVirtualFile final
{
    LVirtualFile(const LPath& RelativeContentPath, const uint8* InFileContents, const uint64 InFileSize);
    PROHIBIT_COPY(LVirtualFile)
    DEFAULT_MOVE(LVirtualFile)
    ~LVirtualFile() = default;

    FORCEINLINE auto GetFileSize() const -> LuBigSizeTy { return this->FileSize; }
    FORCEINLINE auto GetRelativeContentPath() const -> const LPath& { return this->RelativeContentPath; }
    FORCEINLINE auto GetFileName() const -> LSimpleString { return this->RelativeContentPath.GetBase(); }
    FORCEINLINE auto GetBulk() const -> const uint8* { return this->FileContents; }

private:

    uint64       FileSize;
    LPath        RelativeContentPath;
    const uint8* FileContents;
};

LVirtualFile::LVirtualFile(const LPath& RelativeContentPath, const uint8* InFileContents, const uint64 InFileSize)
{
    this->FileSize            = InFileSize;
    this->RelativeContentPath = RelativeContentPath;
    this->FileContents        = InFileContents;

    GetVirtualFiles().Add(std::move(*this));

    return;
}

TdhArray<LVirtualFile>& GetVirtualFiles()
{
    static TdhArray<LVirtualFile> VirtualFiles = { };
    return VirtualFiles;
}

} /* ~Namespace Jafg::Private */

#include "VFilesystem.generated.h"

namespace Jafg::VirtualFileSystem
{

ENGINE_API LVirtualFileSystem* GVirtualFileSystem = nullptr;

} /* ~Namespace Jafg::VirtualFileSystem */

Jafg::LVirtualFileSystem::LVirtualFileSystem()
{
    if (GVirtualFileSystem)
    {
        panic( "Global singleton already exists." )
        return;
    }

    GVirtualFileSystem = this;

    return;
}

Jafg::LVirtualFileSystem::~LVirtualFileSystem()
{
    if (ensure(GVirtualFileSystem))
    {
        check( this == GVirtualFileSystem )
        GVirtualFileSystem = nullptr;
    }

    return;
}

int32 Jafg::LVirtualFileSystem::GetTotalEmbeddedFileCount() const
{
    return Private::GetVirtualFiles().GetSize();
}

bool Jafg::LVirtualFileSystem::DoesFileExist(const LEnginePath& InEnginePath) const
{
    const LPath RelPath = InEnginePath.ResolveRelativePath();

    for (const Private::LVirtualFile& File : Private::GetVirtualFiles())
    {
        if (File.GetRelativeContentPath() == RelPath)
        {
            return true;
        }

        continue;
    }

    return false;
}

Jafg::LString Jafg::LVirtualFileSystem::ReadFileAsString(const LEnginePath& InEnginePath) const
{
    const Private::LVirtualFile* File = this->GetPanickedVirtualFileHandle(InEnginePath);
    const LStringLegacy OutLegacy = LStringLegacy{reinterpret_cast<const char*>(File->GetBulk()), static_cast<uint32>(File->GetFileSize()) };
    LString Out = OutLegacy.c_str();
    return Out;
}

LStringLegacy Jafg::LVirtualFileSystem::ReadFileAsStringLegacy(const LEnginePath& InEnginePath) const
{
    const Private::LVirtualFile* File = this->GetPanickedVirtualFileHandle(InEnginePath);
    return { reinterpret_cast<const char*>(File->GetBulk()), static_cast<uint32>(File->GetFileSize()) };
}

void Jafg::LVirtualFileSystem::ReadFileAsBytes(const LEnginePath& InEnginePath, const uint8*& OutBuffer, uint64& OutBufferOverflowGuard) const
{
    const Private::LVirtualFile* File = this->GetPanickedVirtualFileHandle(InEnginePath);

    OutBuffer              = File->GetBulk();
    OutBufferOverflowGuard = File->GetFileSize();

    check( OutBuffer != nullptr )
    check( OutBufferOverflowGuard > 0 )

    return;
}

Jafg::TdhArray<uint8> Jafg::LVirtualFileSystem::ReadFileAsJafgByteArray(const LEnginePath& InEnginePath) const
{
    const Private::LVirtualFile* File = this->GetPanickedVirtualFileHandle(InEnginePath);

    TdhArray<uint8> Out;
    Out.Reserve(File->GetFileSize());

    /*
     * Stupid solution. We can memcpy this shit. But we should implement a generalized
     * solution for that in the array class itself.
     */
    for (LuBigSizeTy i = 0; i < File->GetFileSize(); ++i)
    {
        Out.Emplace(File->GetBulk()[i]);
    }

    return Out;
}

Jafg::TdhArray<Jafg::LSimpleString> Jafg::LVirtualFileSystem::FindFiles(
    const EEnginePaths::Type InEnginePathTy,
    const JUserPreferences& InUserPreferences,
    const bool bKeepExtension /* = false */,
    const LSimpleString& InFileExtension /* = ".*"*/
)
{
    TdhArray<LSimpleString> Out;
    const LPath RelPath = LEnginePath(InEnginePathTy).ResolveRelativeEnginePath(InUserPreferences);

    for (const Private::LVirtualFile& File : Private::GetVirtualFiles())
    {
        if (File.GetRelativeContentPath().GetPath().StartsWith(RelPath.GetPath()) == false)
        {
            continue;
        }

        if (File.GetRelativeContentPath().GetPath().Count('/') != RelPath.GetPath().Count('/') + 1)
        {
            continue;
        }

        if (InFileExtension == ".*")
        {
            if (bKeepExtension)
            {
                Out.Add(File.GetFileName());
            }
            else
            {
                LSimpleString Name = File.GetFileName();
                Paths::GetStemInline(Name);
                Out.Add(std::move(Name));
            }
        }
        else
        {
            if (File.GetRelativeContentPath().GetPath().EndsWith(InFileExtension))
            {
                if (bKeepExtension)
                {
                    Out.Add(File.GetFileName());
                }
                else
                {
                    LSimpleString Name = File.GetFileName();
                    Paths::GetStemInline(Name);
                    Out.Add(std::move(Name));
                }
            }
        }

        continue;
    }

    return Out;
}

Jafg::Private::LVirtualFile* Jafg::LVirtualFileSystem::GetVirtualFileHandle(const LEnginePath& InEnginePath) const
{
    const LPath RelPath = InEnginePath.ResolveRelativeEnginePath(*GetDefault<JUserPreferences>());

    for (Private::LVirtualFile& File : Private::GetVirtualFiles())
    {
        if (File.GetRelativeContentPath() == RelPath)
        {
            return &File;
        }

        continue;
    }

    return nullptr;
}

Jafg::Private::LVirtualFile* Jafg::LVirtualFileSystem::GetCheckedVirtualFileHandle(const LEnginePath& InEnginePath) const
{
    Private::LVirtualFile* File = this->GetVirtualFileHandle(InEnginePath);
    check( File )
    return File;
}

Jafg::Private::LVirtualFile* Jafg::LVirtualFileSystem::GetPanickedVirtualFileHandle(const LEnginePath& InEnginePath) const
{
    Private::LVirtualFile* File = this->GetVirtualFileHandle(InEnginePath);

    if (File == nullptr)
    {
        panicMsgf(
            "File not found in virtual filesystem. Faulty path: {}.",
            InEnginePath.GetRelativeUnresolvedPath()
        )
    }

    return File;
}

#endif /* WITH_VIRTUAL_FILESYSTEM */
