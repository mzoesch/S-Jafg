// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/Paths.h"
#include <filesystem>
#if !PLATFORM_WASM
    #include <fstream>
#endif /* !PLATFORM_WASM */

namespace Jafg
{
LStringLegacy Paths::ReadFile(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    panicMsgf( "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath() )
    return { };
#else /* WITH_VIRTUAL_FILESYSTEM */
    const std::ifstream File(InAbsolutePath.GetPath().ToC(), std::ios::in | std::ios::binary);

    if (!File)
    {
        panicMsgf( "Failed to open file: {}.", InAbsolutePath.GetPath() )
        return { };
    }

    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str();
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LSimpleString Paths::GetStem(const LSimpleString& InFileName)
{
    const int32 LastDot = InFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return InFileName;
    }

    return InFileName.Cut(LastDot);
}

void Paths::GetStemInline(LSimpleString& InOutFileName)
{
    const int32 LastDot = InOutFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return;
    }

    InOutFileName.InlineCut(LastDot);

    return;
}

bool Paths::DoesFileExist(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return false;
#else /* WITH_VIRTUAL_FILESYSTEM */
    return std::filesystem::exists(InAbsolutePath.GetPath().ToC());
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

} /* ~Namespace Jafg::Paths */
