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
//   - PLATFORM=<str>          [The platform to build for]
//   - ARCH=<str>              [The architecture to build for]
//   - TARGET=<str>            [The target to build]
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
    if GBuildTargetInfo.GetTranslatedKind().IsShared() {
        CopyRelevantBinariesToAllLaunchers()
    } else if GBuildTargetInfo.GetTranslatedKind().IsLaunch() {
        CopyRelevantNativeBinariesToLaunch()
    }

    return
}

func CopyRelevantBinariesToAllLaunchers() {
    var tar *Core.Target = GBuildTargetInfo.GetTargetPointerChecked()
    var mod *Core.Module = GBuildTargetInfo.GetModulePointerChecked()

    if mod.Kind.IsShared() == false {
        panic(fmt.Sprintf("Module [%s] is not shared.", mod.Name))
    }

    var allDeps []*Core.Module
    mod.GetTransitiveAllDependenciesWithPrivate(tar, &allDeps)

    var sharedBin string = fmt.Sprintf(
        "%s/%s/%s%s",
        GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
        mod.GetFunctionalRelativeDir(),
        mod.Name,
        GBuildTargetInfo.GetSharedLibExtension(),
    )
    var sharedBinLib string = fmt.Sprintf(
        "%s/%s/lib%s%s",
        GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
        mod.GetFunctionalRelativeDir(),
        mod.Name,
        GBuildTargetInfo.GetSharedLibExtension(),
    )

    var sharedDebugDatabase string = fmt.Sprintf(
        "%s/%s/%s%s",
        GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
        mod.GetFunctionalRelativeDir(),
        mod.Name,
        GBuildTargetInfo.GetSharedLibDebugSymbolsExtension(),
    )

    fmt.Println("Copying this binary to launch directories ...")

    var copied int = 0
    for idx, _ := range tar.Modules {
        if tar.Modules[idx].Kind.IsLaunch() == false {
            continue
        }

        var launchMod *Core.Module = &tar.Modules[idx]
        var targetDir string = fmt.Sprintf(
            "%s/%s",
            GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
            launchMod.GetFunctionalRelativeDir(),
        )

        if Shared.DoesRelativeFileExist(sharedBin) {
            if Shared.CopyToDirIfDifferent(sharedBin, targetDir) {
                copied++
            }
        } else {
            if Shared.CopyToDirIfDifferent(sharedBinLib, targetDir) {
                copied++
            }
        }

        if strings.Contains(GBuildTargetInfo.Target, "Shipping") == false {
            if Shared.DoesRelativeFileExist(sharedDebugDatabase) {
                if Shared.CopyToDirIfDifferent(sharedDebugDatabase, targetDir) {
                    copied++
                }
            }
        }

        for idxD, _ := range allDeps {
            for _, bin := range allDeps[idxD].NativeDependencies {
                if Shared.CopyToDirIfDifferent(bin, targetDir) {
                    copied++
                }
            }
            for _, bin := range allDeps[idxD].AdditionalCopyFiles {
                if Shared.CopyToDirIfDifferent(bin, targetDir) {
                    copied++
                }
            }
            continue
        }

        continue
    }

    fmt.Printf("Successfully copied %d files to launch directory.\n", copied)

    return
}

func CopyRelevantNativeBinariesToLaunch() {
    var tar *Core.Target = GBuildTargetInfo.GetTargetPointerChecked()
    var mod *Core.Module = GBuildTargetInfo.GetModulePointerChecked()

    var allDeps []*Core.Module
    mod.GetTransitiveAllDependenciesWithPrivate(tar, &allDeps)

    fmt.Println("Copying shared binaries and other dependencies to launch directory ...")

    var relTarget string = GBuildTargetInfo.GetRelativeBinaryDir()

    var copied int = 0
    for idx, _ := range allDeps {
        var dMod *Core.Module = allDeps[idx]
        if dMod.Kind.IsShared() {
            source := fmt.Sprintf(
                "%s/%s/%s%s",
                GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
                dMod.GetFunctionalRelativeDir(),
                dMod.Name,
                GBuildTargetInfo.GetSharedLibExtension(),
            )
            if Shared.DoesRelativeFileExist(source) {
                if Shared.CopyToDirIfDifferent(source, relTarget) {
                    copied++
                }
            }

            if strings.Contains(GBuildTargetInfo.Target, "Shipping") == false {
                source := fmt.Sprintf(
                    "%s/%s/%s%s",
                    GBuildTargetInfo.GetRelativeBinaryDirNoModules(),
                    dMod.GetFunctionalRelativeDir(),
                    dMod.Name,
                    GBuildTargetInfo.GetSharedLibDebugSymbolsExtension(),
                )
                if Shared.DoesRelativeFileExist(source) {
                    if Shared.CopyToDirIfDifferent(source, relTarget) {
                        copied++
                    }
                }
            }
        }

        for _, bin := range allDeps[idx].NativeDependencies {
            if Shared.CopyToDirIfDifferent(bin, relTarget) {
                copied++
            }
        }

        for _, bin := range allDeps[idx].AdditionalCopyFiles {
            if Shared.CopyToDirIfDifferent(bin, relTarget) {
                copied++
            }
        }

        continue
    }

    fmt.Printf("Successfully copied %d files to launch directory.\n", copied)

    return
}
