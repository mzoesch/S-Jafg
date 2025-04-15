// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/Finder.h"
#include "Platform/PlatformMisc.h"
#include "System/EnginePath.h"
#include "System/Paths.h"
#if WITH_VIRTUAL_FILESYSTEM
    #include "System/VFilesystem.h"
#else /* WITH_VIRTUAL_FILESYSTEM */
    #include "User/UserPreferences.h"
    #include "Engine/ObjectBaseUtility.h"
    #include <filesystem>
    #include <fstream>
    namespace Fs = std::filesystem;
#endif /* !WITH_VIRTUAL_FILESYSTEM */

namespace Jafg
{

LPath Finder::GetEngineRootDir()
{
    return PlatformMisc::GetEngineRootDir();
}

LPath Finder::GetSavedDir()
{
    LPath Out = GetEngineRootDir();
    Out /= "Saved";
    return Out;
}

LPath Finder::GetUserPreferencesFile()
{
    LPath Out = GetSavedDir();
    Out /= "MyPreferences.cfg";
    return Out;
}

LString Finder::ReadFile(const LEnginePath& InEnginePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    return GVirtualFileSystem->ReadFileAsString(InEnginePath);
#else /* WITH_VIRTUAL_FILESYSTEM */
    return Paths::ReadFile(InEnginePath.ResolveAbsolutePath(*GetDefault<JUserPreferences>()));
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Finder::ReadFileAsBinary(const LEnginePath& InEnginePath, const u8*& OutBuffer, u64& OutBufferOverflowGuard)
{
    check( OutBuffer == nullptr )
#if WITH_VIRTUAL_FILESYSTEM
    GVirtualFileSystem->ReadFileAsBytes(InEnginePath, OutBuffer, OutBufferOverflowGuard);
#else /* WITH_VIRTUAL_FILESYSTEM */
    std::ifstream File(
        InEnginePath.ResolveAbsolutePath(*GetDefault<JUserPreferences>()).GetPath().ToC(),
        std::ios::binary | std::ios::ate
    );

    if (File.fail())
    {
        panicMsgf("Failed to open file: [{}].", InEnginePath.GetRelativeUnresolvedPath().GetPath())
        return;
    }

    OutBufferOverflowGuard = File.tellg();
    jassert( OutBufferOverflowGuard > 0 )
    File.seekg(0, std::ios::beg);

    OutBuffer = new u8[OutBufferOverflowGuard];
    jassert( OutBuffer )

    if (File.read(reinterpret_cast<char*>(const_cast<u8*>(OutBuffer)), static_cast<std::streamsize>(OutBufferOverflowGuard)).fail())
    {
        File.close();
        panicMsgf("Failed to read file: [{}].", InEnginePath.GetRelativeUnresolvedPath().GetPath())
        return;
    }

    File.close();

    return;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Finder::FreeReadFileBinaryBuffer(const u8*& InBuffer)
{
    check( InBuffer != nullptr )
#if !WITH_VIRTUAL_FILESYSTEM
    delete InBuffer;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
    InBuffer = nullptr;
    return;
}

bool Finder::DoesFileExists(const LEnginePath& InEnginePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    return GVirtualFileSystem->DoesFileExist(InEnginePath);
#else /* WITH_VIRTUAL_FILESYSTEM */
    return Paths::DoesFileExist(InEnginePath.ResolveAbsolutePath(*GetDefault<JUserPreferences>()));
#endif /* WITH_VIRTUAL_FILESYSTEM */
}

bool Finder::DoesFileExistsChecked(const LEnginePath& InEnginePath)
{
    const bool bOut = Finder::DoesFileExists(InEnginePath);
    check(bOut)
    return bOut;
}

bool Finder::DoesFileExistsPanicked(const LEnginePath& InEnginePath)
{
    const bool bOut = Finder::DoesFileExists(InEnginePath);
    if (bOut == false)
    {
        panicMsgf("File does not exist: {}", InEnginePath.GetRelativeUnresolvedPath().GetPath())
    }
    return bOut;
}

void Finder::EnsureFile(const LEnginePath& InEnginePath)
{
    if (Finder::DoesFileExists(InEnginePath))
    {
        return;
    }
#if WITH_VIRTUAL_FILESYSTEM
    panicMsgf( "File does not exist: {}.", InEnginePath.GetRelativeUnresolvedPath().GetPath() )
#else /* WITH_VIRTUAL_FILESYSTEM */
    Paths::CreateFileSlow(InEnginePath.ResolveAbsolutePath(*GetDefault<JUserPreferences>()));
#endif /* !WITH_VIRTUAL_FILESYSTEM */
    return;
}

void Finder::CheckFile(const LEnginePath& InEnginePath)
{
    if (Finder::DoesFileExists(InEnginePath))
    {
        return;
    }

    panicMsgf( "No such file: {}.", InEnginePath.GetRelativeUnresolvedPath().GetPath() )
}

LPath Finder::ResolvePathToRelativeModulePath(const LEnginePath& InEnginePath)
{
    if (InEnginePath.GetPathTy() == EEnginePaths::CustomEngine)
    {
        panicMsgf(
            "Cannot resolve path to relative module path. Engine path type is CustomEngine. Faulty path: {}.",
            InEnginePath.GetRelativeUnresolvedPath().GetPath()
        )
        return { };
    }

    LPath Out = LexToString(InEnginePath.GetPathTy());
    return Out /= InEnginePath.GetRelativeUnresolvedPath();
}

LPath Finder::ResolvePathToRelativeEnginePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences)
{
    if (InEnginePath.GetPathTy() == EEnginePaths::CustomEngine)
    {
        return InEnginePath.GetRelativeUnresolvedPath();
    }

    return Finder::ResolvePathToRelativeModulePath(InEnginePath);
}

LPath Finder::ResolvePathToAbsolutePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences)
{
    if (InEnginePath.GetPathTy() == EEnginePaths::None)
    {
        return InEnginePath.GetRelativeUnresolvedPath();
    }

    LPath Out = Finder::GetEngineRootDir();
    Out /= Finder::ResolvePathToRelativeEnginePath(InEnginePath, InUserPreferences);
    return Out;
}

LPath Finder::ResolvePathToAbsolutePath(const EEnginePaths::Type& InEnginePath, const JUserPreferences& InUserPreferences)
{
    return Finder::ResolvePathToAbsolutePath(LEnginePath(InEnginePath), InUserPreferences);
}

TArray<LString> Finder::FindFiles(
    const LPath& InAbsolutePath,
    const bool bKeepExtension /* = false */,
    const LString& InFileExtension /* = ".*" */
)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return { };
#else /* WITH_VIRTUAL_FILESYSTEM */
    TArray<LString> Out;

    if (Fs::exists(InAbsolutePath.GetPath().ToC()) == false)
    {
        panicMsgf("Path does not exist: {}", InAbsolutePath.GetPath().ToC())
        return Out;
    }

    for (auto &p : Fs::directory_iterator(InAbsolutePath.GetPath().ToC()))
    {
        if (p.is_directory())
        {
            continue;
        }

        if (InFileExtension == ".*")
        {
            if (bKeepExtension)
            {
#if PLATFORM_WINDOWS && UNICODE
                LStringLegacy Temp = LPlatformTypes::Ws2S(p.path().filename().c_str());
                Out.Add(LSimpleString(Temp.c_str()));
#else /* PLATFORM_WINDOWS && UNICODE */
                Out.Add(p.path().filename().c_str());
#endif /* !(PLATFORM_WINDOWS && UNICODE) */
            }
            else
            {
#if PLATFORM_WINDOWS && UNICODE
                LSimpleString SimpleTemp = LPlatformTypes::Ws2S(p.path().stem()).c_str();
                Out.Add(std::move(SimpleTemp));
#else /* PLATFORM_WINDOWS && UNICODE */
                Out.Add(p.path().stem().c_str());
#endif /* !(PLATFORM_WINDOWS && UNICODE) */
            }
        }
        else
        {
            if (p.path().extension() == InFileExtension.ToC())
            {
                if (bKeepExtension)
                {
#if PLATFORM_WINDOWS && UNICODE
                    LStringLegacy Temp = LPlatformTypes::Ws2S(p.path().filename().c_str());
                    Out.Add(LSimpleString(Temp.c_str()));
#else /* PLATFORM_WINDOWS && UNICODE */
                    Out.Add(p.path().filename().c_str());
#endif /* !(PLATFORM_WINDOWS && UNICODE) */
                }
                else
                {
#if PLATFORM_WINDOWS && UNICODE
                    LSimpleString SimpleTemp = LPlatformTypes::Ws2S(p.path().stem()).c_str();
                    Out.Add(std::move(SimpleTemp));
#else /* PLATFORM_WINDOWS && UNICODE */
                    Out.Add(p.path().stem().c_str());
#endif /* !(PLATFORM_WINDOWS && UNICODE) */
                }
            }
        }

        continue;
    }

    return Out;
#endif /* WITH_VIRTUAL_FILESYSTEM */
}

TArray<LString> Finder::FindFiles(
    const EEnginePaths::Type InEnginePathTy,
    const JUserPreferences& InUserPreferences,
    const bool bKeepExtension /* = false */,
    const LString& InFileExtension /* = ".*"*/
)
{
#if WITH_VIRTUAL_FILESYSTEM
    return GVirtualFileSystem->FindFiles(
        InEnginePathTy,
        InUserPreferences,
        bKeepExtension,
        InFileExtension
    );
#else /* WITH_VIRTUAL_FILESYSTEM */
    return Finder::FindFiles(
        Finder::ResolvePathToAbsolutePath(LEnginePath(InEnginePathTy), InUserPreferences),
        bKeepExtension,
        InFileExtension
    );
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

} /* ~Namespace Jafg */
