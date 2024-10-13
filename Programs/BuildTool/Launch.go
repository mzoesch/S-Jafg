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
    if Info.GProjectBuildInfo.Kind.IsLaunch() {
        CopyContentFolder()
        CopySharedProjectBinaries()
    }

    return
}
