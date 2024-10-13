// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/BuildTool/Info"
    "Jafg/Shared"
    "fmt"
    "slices"
)

func Launch(args []string) {
    Info.GProjectBuildInfo = new (Info.ProjectBuildInfo)
    Info.GProjectBuildInfo.Initialize(args)

    fmt.Println(fmt.Sprintf("Launching build tool for project %s ...", Info.GProjectBuildInfo.ProjectName))
    fmt.Println(fmt.Sprintf("Build Info: %s.", Info.GProjectBuildInfo.ToString()))

    if slices.Contains(args, "--pre-build") {
        LaunchPreBuildTasks(args)
    }
    if slices.Contains(args, "--post-build") {
        LaunchPostBuildTasks(args)
    }

    return
}

func LaunchPreBuildTasks(args []string) {
    Shared.CheckForDir(Shared.GeneratedHeadersDir)
    GenerateBuildHeaderFile()

    return
}

func LaunchPostBuildTasks(args []string) {
    fmt.Println("A")
    if Info.GProjectBuildInfo.Kind.IsLaunch() {
        fmt.Println("B")
        CopyContentFolder()
    }
    fmt.Println("C")
    if Info.GProjectBuildInfo.Kind.IsShared() {
        fmt.Println("D")
        CopySharedProjectBinaries()
    }

    return
}
