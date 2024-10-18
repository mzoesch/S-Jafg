// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "slices"
)

func Launch(args []string) {
    var myInfo *BuildInfo = new(BuildInfo)
    myInfo.Initialize(args)

    fmt.Println(fmt.Sprintf("Launching build tool for module [%s] ...", GBuildInfo.ModuleName))
    fmt.Println(fmt.Sprintf("Build configuration: [%s].", GBuildInfo.ToString()))

    if slices.Contains(args, "--pre-build") {
        LaunchPreBuildTasks(args)
    } else if slices.Contains(args, "--post-build") {
        LaunchPostBuildTasks(args)
    } else {
        panic("No build task specified.")
    }

    return
}

func LaunchPreBuildTasks(args []string) {
    Shared.CheckRelativeDir(Shared.GeneratedHeadersDir)

    GenerateWorkspaceWideBuildHeaderFile()

    return
}

func LaunchPostBuildTasks(args []string) {
    if GBuildInfo.Kind.IsLaunch() {
        GBuildInfo.CopyWorkspaceContentToLaunch()
    }

    if GBuildInfo.Kind.IsShared() {
        GBuildInfo.CopyModuleBinariesToLaunch()
    }

    return
}
