// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Core"
    "Jafg/Shared"
    "fmt"
    "slices"
    "strings"
)

//
// Launch is the entry point for the BuildTool
//
// Arguments - all arguments are mandatory:
//   - pre-build or post-build [Which build-step to execute]
//   - SLN=<str>               [The solution to build]
//   - MODULE=<str>            [The module to build]
//   - SYSTEM=<str>            [The system to build for]
//   - ARCH=<str>              [The architecture to build for]
//   - TARGET=<str>            [The target to build]
//   - PLATFORM=<str>          [The platform to build for]
//
func Launch(args []string) {
    GBuildTargetInfo = new(BuildTargetInfo)
    GBuildTargetInfo.Initialize(args)
    GBuildTargetInfo.PrettyPrint()

    if slices.Contains(args, "pre-build") {
        LaunchPreBuildTasks(args)
    } else if slices.Contains(args, "post-build") {
        LaunchPostBuildTasks(args)
    } else {
        panic(fmt.Sprintf("No build task specified. Args: %v", args))
    }

    GBuildTargetInfo = nil

    return
}

func LaunchPreBuildTasks(args []string) {
    /*
     * Absolute necessary directories.
     * The standard api for the platform filesystem does not allow, for whatever reason, to create a file
     * inside a directory that currently does not exist. So we just recursively create all potential points
     * of interest directories.
     */
    Shared.CheckRelativeDir(GBuildTargetInfo.GetRelativeModuleGhDir())
    Shared.CheckRelativeDir(GBuildTargetInfo.GetRelativeModuleGtDir())

    GJPacketContainer = new(JPacketContainer)
    defer func() {
        GJPacketContainer = nil
    }()

    GJPacketContainer.PullCache()
    ReflectModule()
    GJPacketContainer.PushCache()

    return
}

func LaunchPostBuildTasks(args []string) {
    if GBuildTargetInfo.GetTranslatedKind().IsLaunch() {
        CopyRelevantBinariesToLaunch()
    }

    return
}

func CopyRelevantBinariesToLaunch() {
    var tar *Core.Target = GBuildTargetInfo.GetTargetPointerChecked()
    var mod *Core.Module = GBuildTargetInfo.GetModulePointerChecked()

    var allDeps []*Core.Module
    mod.GetTransitiveAllDependenciesWithPrivate(tar, &allDeps)

    fmt.Println("Copying shared binaries and other dependencies to launch directory ...")

    var copied int = 0
    for idx, _ := range allDeps {
        var dMod *Core.Module = allDeps[idx]
        if dMod.Kind.IsShared() {
            if CopyBinaryToLaunch(fmt.Sprintf(
                "%s/%s/%s%s",
                GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
                dMod.GetFunctionalRelativeDir(),
                dMod.Name,
                GBuildTargetInfo.GetSharedLibExtension(),
            )) {
                copied++
            }

            if strings.Contains(GBuildTargetInfo.Target, "Shipping") == false {
                if CopyBinaryToLaunch(fmt.Sprintf(
                    "%s/%s/%s%s",
                    GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
                    dMod.GetFunctionalRelativeDir(),
                    dMod.Name,
                    GBuildTargetInfo.GetSharedLibDebugSymbolsExtension(),
                )) {
                    copied++
                }
            }
        }

        for _, bin := range allDeps[idx].NativeDependencies {
            if CopyBinaryToLaunch(bin) {
                copied++
            }
        }

        for _, bin := range allDeps[idx].AdditionalCopyFiles {
            if CopyBinaryToLaunch(bin) {
                copied++
            }
        }

        continue
    }

    fmt.Printf("Successfully copied %d files to launch directory.\n", copied)

    return
}

func CopyBinaryToLaunch(relSource string) bool {
    var lastSlash int = strings.LastIndex(relSource, "/")
    if lastSlash == -1 {
        panic(fmt.Sprintf("Path [%s] is not valid.", relSource))
    }
    var relTarget string = fmt.Sprintf("%s/%s", GBuildTargetInfo.GetRelativeBinaryDir(), relSource[lastSlash+1:])

    if Shared.CopyFileIfDifferent(Shared.ToAbsolutePath(relSource), Shared.ToAbsolutePath(relTarget), false) {
        fmt.Printf("Copied [%s] to [%s].\n", relSource, relTarget)
        return true
    }

    return false
}
