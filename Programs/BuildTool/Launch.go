// Copyright mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "slices"
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
    // Do something
}
