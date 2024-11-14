// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/Finder.h"
#include "Platform/PlatformMisc.h"
#include "System/EnginePath.h"
#include <filesystem>
namespace Fs = std::filesystem;

namespace Jafg
{

LPath Finder::GetEngineRootDir()
{
    return PlatformMisc::GetEngineRootDir();
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
    /*
     * Currently not implemented, but as soon as we separate the engine with the actual content to JafgPlugin, we will
     * need this. But this is in a long distance future :).
     */
    return Finder::ResolvePathToRelativeModulePath(InEnginePath);
}

LPath Finder::ResolvePathToAbsolutePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences)
{
    LPath Out = Finder::GetEngineRootDir();
    Out /= Finder::ResolvePathToRelativeEnginePath(InEnginePath, InUserPreferences);
    return Out;
}

LPath Finder::ResolvePathToAbsolutePath(const EEnginePaths::Type& InEnginePath, const JUserPreferences& InUserPreferences)
{
    return Finder::ResolvePathToAbsolutePath(LEnginePath(InEnginePath), InUserPreferences);
}

TdhArray<LSimpleString> Finder::FindFiles(
    const LPath& InAbsolutePath,
    const bool bKeepExtension /* = false */,
    const LSimpleString& InFileExtension /* = ".*" */
)
{
    TdhArray<LSimpleString> Out;

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
                LStringLegacy Temp = LGenericPlatformTypes::Ws2S(p.path().filename().c_str());
                Out.Add(LSimpleString(Temp.c_str()));
            }
            else
            {
                LSimpleString SimpleTemp = LGenericPlatformTypes::Ws2S(p.path().stem()).c_str();
                Out.Add(std::move(SimpleTemp));
            }
        }
        else
        {
            if (p.path().extension() == InFileExtension.ToC())
            {
                if (bKeepExtension)
                {
                    LStringLegacy Temp = LGenericPlatformTypes::Ws2S(p.path().filename().c_str());
                    Out.Add(LSimpleString(Temp.c_str()));
                }
                else
                {
                    LSimpleString SimpleTemp = LGenericPlatformTypes::Ws2S(p.path().stem()).c_str();
                    Out.Add(std::move(SimpleTemp));
                }
            }
        }

        continue;
    }

    return Out;
}

TdhArray<LSimpleString> Finder::FindFiles(
    const EEnginePaths::Type InEnginePathTy,
    const JUserPreferences& InUserPreferences,
    const bool bKeepExtension /* = false */,
    const LSimpleString& InFileExtension /* = ".*"*/
)
{
    return Finder::FindFiles(
        Finder::ResolvePathToAbsolutePath(LEnginePath(InEnginePathTy), InUserPreferences),
        bKeepExtension,
        InFileExtension
    );
}

} /* ~Namespace Jafg */
