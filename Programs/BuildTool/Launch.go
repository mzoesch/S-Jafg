// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/BuildTool/Info"
    "Jafg/Shared"
    "slices"
)

func Launch(args []string) {
    Info.GProjectBuildInfo = new (Info.ProjectBuildInfo)
    Info.GProjectBuildInfo.Initialize(args)

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
    return
}
